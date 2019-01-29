#include <SDL.h>
#include <mach-o/dyld.h>
#include <dlfcn.h>
#include <cstdio>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <x86intrin.h>
#include "emeaka.h"
#include "osx_emeaka.h"

global_variable bool IsRunning;
global_variable uint64_t Frequency;
internal uint32_t OSXGetWindowRefreshRate(SDL_Window *window)
{
    SDL_DisplayMode mode;
    int displayIndex = SDL_GetWindowDisplayIndex(window);
    uint32_t defaultRefreshRate = 60;
    if (SDL_GetDesktopDisplayMode(displayIndex, &mode) != 0)
    {
        return defaultRefreshRate;
    }

    if (mode.refresh_rate == 0)
    {
        return defaultRefreshRate;
    }

    return (uint32_t)mode.refresh_rate;
}

internal void OSXPathForFileRelativeToExecutable(char *path, uint32_t pathSize, const char *filename)
{
    if (_NSGetExecutablePath(path, &pathSize) == 0)
    {
        char *lastSlash = strrchr(path, '/');
        lastSlash += 1;
        *lastSlash = '\0';
        strcat(path, filename);

        return;
    }
    else
    {
        return;
    }
}
internal void OSXSetupDynamicGameFilename(OSXDynamicGame *game)
{
    OSXPathForFileRelativeToExecutable((char *)&(game->DynamicLibPath), 1024, "emeaka_game.dylib");
}

internal void OSXSetupDynamicGameStruct(OSXDynamicGame *game)
{
    game->FileUpdateTime = 0;
    game->DynamicLibrary = 0;
    game->APIFunctions.UpdateAndRender = (GameUpdateAndRenderType *)0;
    game->APIFunctions.GetSoundSamples = (GameGetSoundSamplesType *)0;
    game->APIFunctions.ClearBitmap = (GameClearBitmapType *)0;
    game->APIFunctions.DrawPixel = (GameDrawPixelType *)0;
    game->APIFunctions.DrawRect = (GameDrawRectType *)0;
    game->APIFunctions.DrawCircle = (GameDrawCircleType *)0;
    game->APIFunctions.DrawLine = (GameDrawLineType *)0;
    game->APIFunctions.DrawChar = (GameDrawCharType *)0;
    game->APIFunctions.DrawText = (GameDrawTextType *)0;
    game->IsValid = false;
}

internal time_t OSXDynamicGameUpdateTime(OSXDynamicGame *game)
{
    time_t lastWriteTime = 0;
    struct stat fileStat = {};
    //this doesn't actually work...
    if (!stat(game->DynamicLibPath, &fileStat))
    {

        lastWriteTime = fileStat.st_mtimespec.tv_sec;
        // printf("Read Update Time: %ld\n", lastWriteTime);
    }
    return lastWriteTime;
}

internal bool OSXDynamicGameNeedsReImport(OSXDynamicGame *game)
{
    time_t updateTime = OSXDynamicGameUpdateTime(game);
    if (updateTime != game->FileUpdateTime)
    {
        return true;
    }
    return false;
}

internal void OSXLoadGame(OSXDynamicGame *game)
{
    game->IsValid = false;
    char dylibPath[1024];
    OSXPathForFileRelativeToExecutable((char *)&dylibPath, 1024, "emeaka_game.dylib");
    void *gameDylib = dlopen(game->DynamicLibPath, RTLD_LAZY | RTLD_GLOBAL);
    if (gameDylib)
    {
        game->DynamicLibrary = gameDylib;

        game->APIFunctions.UpdateAndRender = (GameUpdateAndRenderType *)dlsym(gameDylib, "GameUpdateAndRender");
        game->APIFunctions.GetSoundSamples = (GameGetSoundSamplesType *)dlsym(gameDylib, "GameGetSoundSamples");
        game->APIFunctions.ClearBitmap = (GameClearBitmapType *)dlsym(gameDylib, "ClearBitmap");
        game->APIFunctions.DrawPixel = (GameDrawPixelType *)dlsym(gameDylib, "DrawPixel");
        game->APIFunctions.DrawRect = (GameDrawRectType *)dlsym(gameDylib, "DrawRect");
        game->APIFunctions.DrawCircle = (GameDrawCircleType *)dlsym(gameDylib, "DrawCircle");
        game->APIFunctions.DrawLine = (GameDrawLineType *)dlsym(gameDylib, "DrawLine");
        game->APIFunctions.DrawChar = (GameDrawCharType *)dlsym(gameDylib, "DrawChar");
        game->APIFunctions.DrawText = (GameDrawTextType *)dlsym(gameDylib, "DrawText");

        if (game->APIFunctions.UpdateAndRender &&
            game->APIFunctions.GetSoundSamples &&
            game->APIFunctions.ClearBitmap &&
            game->APIFunctions.DrawPixel &&
            game->APIFunctions.DrawRect &&
            game->APIFunctions.DrawCircle &&
            game->APIFunctions.DrawLine &&
            game->APIFunctions.DrawChar &&
            game->APIFunctions.DrawText)
        {
            game->IsValid = true;
            game->FileUpdateTime = OSXDynamicGameUpdateTime(game);
        }
    }

    if (!game->IsValid)
    {
        printf("Failed to import dynamic game\n.");
        OSXSetupDynamicGameStruct(game);
    }
}

internal void OSXUnloadGame(OSXDynamicGame *game)
{
    if (game->DynamicLibrary)
    {
        dlclose(game->DynamicLibrary);
    }
    OSXSetupDynamicGameStruct(game);
}
#if EMEAKA_INTERNAL 
PLATFORM_READ_ENTIRE_FILE(OSXDebugReadEntireFile)
{
    //threadcontext, filename
    DebugFileResult result;
    result.FileSize = 0;
    result.Contents = nullptr;
    SDL_RWops *file = SDL_RWFromFile(filename, "rb");
    if (file != NULL)
    {
        result.FileSize = SDL_RWsize(file);
        result.Contents = malloc(result.FileSize);
        size_t read_total = 0, last_read = 1;
        void *buf = result.Contents;
        while(read_total < result.FileSize && last_read != 0)
        {
            last_read = SDL_RWread(file, buf, 1, result.FileSize - read_total);
            read_total += last_read;
            buf = (void *)((char *)buf + last_read);
        }
        SDL_RWclose(file);
        if(read_total != result.FileSize)
        {
            printf("FUCKED READ");
            free(result.Contents);
            result.Contents = NULL;
            result.FileSize = 0;
        }
    }
    return result;
    
} 

PLATFORM_FREE_FILE_MEMORY(OSXDebugFreeFileMemory) 
{
    free(memory);
    //threadcontext, memory?
}
PLATFORM_WRITE_ENTIRE_FILE(OSXDebugWriteEntireFile)
{
    //threadContext, filename, memorySize, memory;
    SDL_RWops *file = SDL_RWFromFile(filename,"wb");
    if(file != NULL)
    {
        if(SDL_RWwrite(file, memory, 1, memorySize) == memorySize)
        {
            SDL_RWclose(file);
            return true;
        } 
    }
    SDL_RWclose(file);
    return false;
}
#endif
internal void OSXResizeTexture(OSXOffscreenBuffer *offscreenBuffer, SDL_Renderer *renderer, int width, int height)
{
    if (offscreenBuffer->Memory)
    {
        munmap(offscreenBuffer->Memory, offscreenBuffer->Width * offscreenBuffer->Height * offscreenBuffer->BytesPerPixel);
    }

    if (offscreenBuffer->Texture)
    {
        SDL_DestroyTexture(offscreenBuffer->Texture);
    }

    offscreenBuffer->Texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    offscreenBuffer->Width = width;
    offscreenBuffer->Height = height;
    offscreenBuffer->BytesPerPixel = 4;
    offscreenBuffer->Pitch = width * offscreenBuffer->BytesPerPixel;
    offscreenBuffer->Memory = mmap(0, width * height * offscreenBuffer->BytesPerPixel, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
}

internal void OSXUpdateWindow(SDL_Window *window, SDL_Renderer *renderer, OSXOffscreenBuffer *offscreenBuffer)
{
    SDL_UpdateTexture(offscreenBuffer->Texture, 0, offscreenBuffer->Memory, offscreenBuffer->Pitch);
    SDL_RenderCopy(renderer, offscreenBuffer->Texture, 0, 0);
    SDL_RenderPresent(renderer);
}

internal bool OSXGetExecutableDirectory(char *exePath, uint32_t exePathSize)
{
    if (_NSGetExecutablePath(exePath, &exePathSize) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

internal void OSXUpdateMouse(SDL_Window *window, GameInputBuffer *inputBuffer, GameInputBuffer *lastInputBuffer)
{
    int x, y;
    uint32_t buttons = SDL_GetMouseState(&x, &y);
    inputBuffer->MouseInput.LeftButton.IsDown = (SDL_BUTTON(SDL_BUTTON_LEFT) & buttons) != 0;
    inputBuffer->MouseInput.RightButton.IsDown = (SDL_BUTTON(SDL_BUTTON_RIGHT) & buttons ) != 0;
    inputBuffer->MouseInput.MiddleButton.IsDown = (SDL_BUTTON(SDL_BUTTON_MIDDLE) & buttons ) != 0;
    
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    inputBuffer->MouseInput.MouseLocationX = (float)x/(float)w;
    inputBuffer->MouseInput.MouseLocationY = 1.0f-(float)y/(float)h;
}

float S16ToFloatWithDeadzone(int16_t raw_value, int16_t deadzone)
{
    float ret = 0.0f;
    if (raw_value > deadzone)
    {
        raw_value -= deadzone;
        ret = (float)raw_value / (float)(0x7FFF - deadzone);
    }
    else if (raw_value < -deadzone)
    {
        raw_value += deadzone;
        ret = (float)raw_value / (float)(32768 - deadzone);
    }
    return ret;
}

internal void OSXUpdateControllers(GameInputBuffer *inputBuffer, GameInputBuffer *lastInputBuffer, SDL_GameController *controller, size_t idx)
{
    const int DEADZONE = 8000;
    if (controller && SDL_GameControllerGetAttached(controller))
    {
        inputBuffer->ControllerInput[idx].Connected = true;
        inputBuffer->ControllerInput[idx].UpButton.IsDown = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP);

        int32_t x_value_left = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
        int32_t y_value_left = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
        int32_t x_value_right = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
        int32_t y_value_right = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY);

        inputBuffer->ControllerInput[idx].LeftStick.AverageX = S16ToFloatWithDeadzone(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX), DEADZONE);
        inputBuffer->ControllerInput[idx].LeftStick.AverageY = S16ToFloatWithDeadzone(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY), DEADZONE);
        inputBuffer->ControllerInput[idx].RightStick.AverageX = S16ToFloatWithDeadzone(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX), DEADZONE);
        inputBuffer->ControllerInput[idx].RightStick.AverageY = S16ToFloatWithDeadzone(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY), DEADZONE);

        inputBuffer->ControllerInput[idx].AButton.IsDown = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A);
        if(inputBuffer->ControllerInput[idx].AButton.IsDown != lastInputBuffer->ControllerInput[idx].AButton.IsDown)
        {
            inputBuffer->ControllerInput[idx].AButton.HalfTransitions = 1;
        }
    }
    else
    {
        inputBuffer->ControllerInput[idx].Connected = false;
    }
}

internal void OSXQueueAudio(OSXAudioBuffer *audioBuffer, GameSoundBuffer *gameSound, size_t byteToStartWriteAt)
{
    size_t bytesToWrite = gameSound->SampleCount * 4; //Bytes per Sample * channels
    size_t region1Size = bytesToWrite;
    size_t region2Size = 0;

    if (byteToStartWriteAt + bytesToWrite > audioBuffer->RingBufferLength)
    {
        region1Size = audioBuffer->RingBufferLength - byteToStartWriteAt;
        region2Size = bytesToWrite - region1Size;
    }

    memcpy((void *)((size_t)audioBuffer->RingBuffer + byteToStartWriteAt), gameSound->SampleBuffer, region1Size);
    memcpy(audioBuffer->RingBuffer, (void *)((size_t)gameSound->SampleBuffer + region1Size), region2Size);
    audioBuffer->ApplicationNextWritePtr += bytesToWrite;
}

internal void OSXAudioCallback(void *userData, uint8_t *buffer, int bufferLen)
{
    OSXAudioBuffer *audioBuffer = (OSXAudioBuffer *)userData;
    size_t region1Size = bufferLen;
    size_t region2Size = 0;

    size_t currentPlayCursor = audioBuffer->PlayCursor % audioBuffer->RingBufferLength;

    if (currentPlayCursor + region1Size > audioBuffer->RingBufferLength)
    {
        region1Size = audioBuffer->RingBufferLength - currentPlayCursor;
        region2Size = bufferLen - region1Size;
    }
    memcpy((void *)buffer, (void *)((size_t)audioBuffer->RingBuffer + currentPlayCursor), region1Size);
    memcpy((void *)((size_t)buffer + region1Size), audioBuffer->RingBuffer, region2Size);

    audioBuffer->PlayCursor = audioBuffer->PlayCursor + bufferLen;
    audioBuffer->WriteCursor = audioBuffer->PlayCursor + bufferLen;
}

internal void OSXInitializeSound(OSXAudioBuffer *audioBuffer)
{
    SDL_AudioSpec receivedAudioSpec = {};
    audioBuffer->AudioSpec.freq = 48000;
    audioBuffer->AudioSpec.format = AUDIO_S16;
    audioBuffer->AudioSpec.channels = 2;
    audioBuffer->AudioSpec.userdata = (void *)audioBuffer;
    audioBuffer->AudioSpec.samples = 512; //assume we want to grab information at least as fast as our frame rate, maybe more...
    audioBuffer->AudioSpec.callback = OSXAudioCallback;
    audioBuffer->AudioDevice = SDL_OpenAudioDevice(NULL, 0, &audioBuffer->AudioSpec, &receivedAudioSpec, 0);
    if (audioBuffer->AudioDevice == 0 || receivedAudioSpec.format != AUDIO_S16)
    {
        printf("[ERROR] Failed to open Audio Device: %s\n", SDL_GetError());
        SDL_CloseAudio();
    }
    else
    {
        audioBuffer->AudioSpec = receivedAudioSpec;
        printf("[INFO] Opened Audio Device : %d\n", audioBuffer->AudioDevice);
        printf("[INFO] Size: %d\n", audioBuffer->AudioSpec.size);
        printf("[INFO] Format: %d\n", audioBuffer->AudioSpec.format);
        printf("[INFO] Samples: %d\n", audioBuffer->AudioSpec.samples);
        printf("[INFO] Playing...\n");
    }
}
internal void OSXSwapInputBuffers(GameInputBuffer **current, GameInputBuffer **old)
{
    GameInputBuffer *storage = *old;
    *old = *current;
    *current = storage;
    for(size_t i = 0; i < KeyCode::KEY_TABLE_SIZE; ++i)
    {
        (*current)->KeyboardInput.Key[i].HalfTransitions = 0;
        (*current)->KeyboardInput.Key[i].IsDown = (*old)->KeyboardInput.Key[i].IsDown;
    }

    (*current)->MouseInput.LeftButton.IsDown = (*old)->MouseInput.LeftButton.IsDown;
    (*current)->MouseInput.RightButton.IsDown = (*old)->MouseInput.RightButton.IsDown;
    (*current)->MouseInput.MiddleButton.IsDown = (*old)->MouseInput.MiddleButton.IsDown;
    (*current)->MouseInput.LeftButton.HalfTransitions = 0;
    (*current)->MouseInput.RightButton.HalfTransitions = 0;
    (*current)->MouseInput.MiddleButton.HalfTransitions = 0;

}

bool OSXGetKeycode(SDL_Scancode scancode, KeyCode& emeaka_code)
{
    switch(scancode)
    {
    case SDL_SCANCODE_A: emeaka_code = KeyCode::A; return true;
    case SDL_SCANCODE_B: emeaka_code = KeyCode::B; return true;
    case SDL_SCANCODE_C: emeaka_code = KeyCode::C; return true;
    case SDL_SCANCODE_D: emeaka_code = KeyCode::D; return true;
    case SDL_SCANCODE_E: emeaka_code = KeyCode::E; return true;
    case SDL_SCANCODE_F: emeaka_code = KeyCode::F; return true;
    case SDL_SCANCODE_G: emeaka_code = KeyCode::G; return true;
    case SDL_SCANCODE_H: emeaka_code = KeyCode::H; return true;
    case SDL_SCANCODE_I: emeaka_code = KeyCode::I; return true;
    case SDL_SCANCODE_J: emeaka_code = KeyCode::J; return true;
    case SDL_SCANCODE_K: emeaka_code = KeyCode::K; return true;
    case SDL_SCANCODE_L: emeaka_code = KeyCode::L; return true;
    case SDL_SCANCODE_M: emeaka_code = KeyCode::M; return true;
    case SDL_SCANCODE_N: emeaka_code = KeyCode::N; return true;
    case SDL_SCANCODE_O: emeaka_code = KeyCode::O; return true;
    case SDL_SCANCODE_P: emeaka_code = KeyCode::P; return true;
    case SDL_SCANCODE_Q: emeaka_code = KeyCode::Q; return true;
    case SDL_SCANCODE_R: emeaka_code = KeyCode::R; return true;
    case SDL_SCANCODE_S: emeaka_code = KeyCode::S; return true;
    case SDL_SCANCODE_T: emeaka_code = KeyCode::T; return true;
    case SDL_SCANCODE_U: emeaka_code = KeyCode::U; return true;
    case SDL_SCANCODE_V: emeaka_code = KeyCode::V; return true;
    case SDL_SCANCODE_W: emeaka_code = KeyCode::W; return true;
    case SDL_SCANCODE_X: emeaka_code = KeyCode::X; return true;
    case SDL_SCANCODE_Y: emeaka_code = KeyCode::Y; return true;
    case SDL_SCANCODE_Z: emeaka_code = KeyCode::Z; return true;

    case SDL_SCANCODE_1: emeaka_code = KeyCode::K1; return true;
    case SDL_SCANCODE_2: emeaka_code = KeyCode::K2; return true;
    case SDL_SCANCODE_3: emeaka_code = KeyCode::K3; return true;
    case SDL_SCANCODE_4: emeaka_code = KeyCode::K4; return true;
    case SDL_SCANCODE_5: emeaka_code = KeyCode::K5; return true;
    case SDL_SCANCODE_6: emeaka_code = KeyCode::K6; return true;
    case SDL_SCANCODE_7: emeaka_code = KeyCode::K7; return true;
    case SDL_SCANCODE_8: emeaka_code = KeyCode::K8; return true;
    case SDL_SCANCODE_9: emeaka_code = KeyCode::K9; return true;
    case SDL_SCANCODE_0: emeaka_code = KeyCode::K0; return true;

    case SDL_SCANCODE_F1: emeaka_code = KeyCode::F1; return true;
    case SDL_SCANCODE_F2: emeaka_code = KeyCode::F2; return true;
    case SDL_SCANCODE_F3: emeaka_code = KeyCode::F3; return true;
    case SDL_SCANCODE_F4: emeaka_code = KeyCode::F4; return true;
    case SDL_SCANCODE_F5: emeaka_code = KeyCode::F5; return true;
    case SDL_SCANCODE_F6: emeaka_code = KeyCode::F6; return true;
    case SDL_SCANCODE_F7: emeaka_code = KeyCode::F7; return true;
    case SDL_SCANCODE_F8: emeaka_code = KeyCode::F8; return true;
    case SDL_SCANCODE_F9: emeaka_code = KeyCode::F9; return true;
    case SDL_SCANCODE_F10: emeaka_code = KeyCode::F10; return true;
    case SDL_SCANCODE_F11: emeaka_code = KeyCode::F11; return true;
    case SDL_SCANCODE_F12: emeaka_code = KeyCode::F12; return true;

    case SDL_SCANCODE_KP_1: emeaka_code = KeyCode::N1; return true;
    case SDL_SCANCODE_KP_2: emeaka_code = KeyCode::N2; return true;
    case SDL_SCANCODE_KP_3: emeaka_code = KeyCode::N3; return true;
    case SDL_SCANCODE_KP_4: emeaka_code = KeyCode::N4; return true;
    case SDL_SCANCODE_KP_5: emeaka_code = KeyCode::N5; return true;
    case SDL_SCANCODE_KP_6: emeaka_code = KeyCode::N6; return true;
    case SDL_SCANCODE_KP_7: emeaka_code = KeyCode::N7; return true;
    case SDL_SCANCODE_KP_8: emeaka_code = KeyCode::N8; return true;
    case SDL_SCANCODE_KP_9: emeaka_code = KeyCode::N9; return true;
    case SDL_SCANCODE_KP_0: emeaka_code = KeyCode::N0; return true;

    case SDL_SCANCODE_RETURN: emeaka_code = KeyCode::RETURN; return true;
    case SDL_SCANCODE_ESCAPE: emeaka_code = KeyCode::ESC; return true;
    case SDL_SCANCODE_BACKSPACE: emeaka_code = KeyCode::BACKSPACE; return true;
    case SDL_SCANCODE_TAB: emeaka_code = KeyCode::TAB; return true;
    case SDL_SCANCODE_SPACE: emeaka_code = KeyCode::SPACE; return true;

    case SDL_SCANCODE_LEFTBRACKET: emeaka_code = KeyCode::LEFTBRACKET; return true;
    case SDL_SCANCODE_RIGHTBRACKET: emeaka_code = KeyCode::RIGHTBRACKET; return true;
    case SDL_SCANCODE_SEMICOLON: emeaka_code = KeyCode::SEMICOLON; return true;
    case SDL_SCANCODE_APOSTROPHE: emeaka_code = KeyCode::APOSTROPHE; return true;
    case SDL_SCANCODE_COMMA: emeaka_code = KeyCode::COMMA; return true;
    case SDL_SCANCODE_PERIOD: emeaka_code = KeyCode::PERIOD; return true;
    case SDL_SCANCODE_SLASH: emeaka_code = KeyCode::SLASH; return true;
    case SDL_SCANCODE_BACKSLASH: emeaka_code = KeyCode::BACKSLASH; return true;
    case SDL_SCANCODE_MINUS: emeaka_code = KeyCode::KMINUS; return true;
    case SDL_SCANCODE_EQUALS: emeaka_code = KeyCode::KEQUAL; return true;
    case SDL_SCANCODE_GRAVE: emeaka_code = KeyCode::GRAVE; return true;

    case SDL_SCANCODE_KP_PLUS: emeaka_code = KeyCode::NPLUS; return true;
    case SDL_SCANCODE_KP_MINUS: emeaka_code = KeyCode::NMINUS; return true;
    case SDL_SCANCODE_KP_ENTER: emeaka_code = KeyCode::NENTER; return true;
    case SDL_SCANCODE_KP_EQUALS : emeaka_code = KeyCode::NEQUAL; return true;
    case SDL_SCANCODE_KP_DIVIDE: emeaka_code = KeyCode::NDIVIDE; return true;
    case SDL_SCANCODE_KP_MULTIPLY: emeaka_code = KeyCode::NMULTIPLY; return true;
    case SDL_SCANCODE_KP_PERIOD: emeaka_code = KeyCode::NPERIOD; return true;

    case SDL_SCANCODE_PAGEDOWN: emeaka_code = KeyCode::PGDOWN; return true;
    case SDL_SCANCODE_PAGEUP: emeaka_code = KeyCode::PGUP; return true;
    case SDL_SCANCODE_HOME: emeaka_code = KeyCode::HOME; return true;
    case SDL_SCANCODE_END: emeaka_code = KeyCode::END; return true;
    case SDL_SCANCODE_DELETE: emeaka_code = KeyCode::DELETE; return true;
    case SDL_SCANCODE_UP: emeaka_code = KeyCode::UP; return true;
    case SDL_SCANCODE_DOWN: emeaka_code = KeyCode::DOWN; return true;
    case SDL_SCANCODE_LEFT: emeaka_code = KeyCode::LEFT; return true;
    case SDL_SCANCODE_RIGHT: emeaka_code = KeyCode::RIGHT; return true;
    case SDL_SCANCODE_PAUSE: emeaka_code = KeyCode::PAUSE; return true;
    case SDL_SCANCODE_INSERT: emeaka_code = KeyCode::INSERT; return true;
    
    case SDL_SCANCODE_LCTRL: emeaka_code = KeyCode::LCTRL; return true;
    case SDL_SCANCODE_LSHIFT: emeaka_code = KeyCode::LSHIFT; return true;
    case SDL_SCANCODE_LALT: emeaka_code = KeyCode::LALT; return true;
    case SDL_SCANCODE_LGUI: emeaka_code = KeyCode::LGUI; return true;
    case SDL_SCANCODE_RCTRL: emeaka_code = KeyCode::RCTRL; return true;
    case SDL_SCANCODE_RSHIFT: emeaka_code = KeyCode::RSHIFT; return true;
    case SDL_SCANCODE_RALT: emeaka_code = KeyCode::RALT; return true;
    case SDL_SCANCODE_RGUI: emeaka_code = KeyCode::RGUI; return true;
    default: 
        return false;
    } 
    return false;
}

internal void OSXHandleEvent(SDL_Event *event, OSXOffscreenBuffer *offscreenBuffer, GameInputBuffer *inputBuffer, GameInputBuffer *lastInputBuffer)
{

    switch (event->type)
    {
    case SDL_QUIT:
    {
        IsRunning = false;
    }
    break;

    case SDL_KEYDOWN:
    case SDL_KEYUP:
    {
        KeyCode emeaka_code;
        if(OSXGetKeycode(event->key.keysym.scancode, emeaka_code))
        {
            printf("EVENT %d\n",(int32_t)event->key.keysym.scancode);
            inputBuffer->KeyboardInput.Key[emeaka_code].IsDown = event->type == SDL_KEYDOWN;
            inputBuffer->KeyboardInput.Key[emeaka_code].HalfTransitions = 1;
        }

    }
    break;

    case SDL_WINDOWEVENT:
    {
        switch (event->window.event)
        {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
        {
            SDL_Window *window = SDL_GetWindowFromID(event->window.windowID);
            SDL_Renderer *renderer = SDL_GetRenderer(window);
            OSXResizeTexture(offscreenBuffer, renderer, event->window.data1, event->window.data2);
        }
        break;

        case SDL_WINDOWEVENT_EXPOSED:
        {
            SDL_Window *window = SDL_GetWindowFromID(event->window.windowID);
            SDL_Renderer *renderer = SDL_GetRenderer(window);
            OSXUpdateWindow(window, renderer, offscreenBuffer);
        }
        break;
        }
    }
    break;
    }
}

internal float OSXGetSecondsElapsed(uint64_t startTime, uint64_t endTime)
{
    return (float)(endTime - startTime) / (float)(Frequency);
}

internal uint64_t OSXGetCycleCounter()
{
    return _rdtsc();
}

int main(int argc, char **argv)
{
    Frequency = SDL_GetPerformanceFrequency();
    uint64_t gameStartupTime = SDL_GetPerformanceCounter();

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);
    SDL_Window *window = SDL_CreateWindow("EMEAKA",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          960,
                                          540,
                                          SDL_WINDOW_RESIZABLE);

    if (window)
    {
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

        if (renderer)
        {
            //game is going to run, let's get everything ready...
            GameInputBuffer *currentInputBuffer;
            GameInputBuffer *lastInputBuffer;

            GameInputBuffer inputBuffer[2] = {};

            currentInputBuffer = &inputBuffer[0];
            lastInputBuffer = &inputBuffer[1];

            GameSoundBuffer gameSoundBuffer = {};

            OSXOffscreenBuffer offscreenBuffer = {};
            OSXAudioBuffer audioBuffer = {};
            GameClocks gameClocks = {};
            GameMemory gameMemory = {};
            ThreadContext threadContext = {};
            gameClocks.UpdateDT = 1.f / 30.f;

            gameMemory.PermanentStorageSize = MegaBytes(256);
            gameMemory.TransientStorageSize = GigaBytes(2);
            size_t totalMemorySize = gameMemory.PermanentStorageSize + gameMemory.TransientStorageSize;
            gameMemory.PermanentStorage = mmap((void *)TeraBytes(2), totalMemorySize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
            gameMemory.TransientStorage = (void *)((size_t)gameMemory.PermanentStorage + gameMemory.PermanentStorageSize);
            gameMemory.PlatformFunctions.PlatformFreeFileMemory = OSXDebugFreeFileMemory;
            gameMemory.PlatformFunctions.PlatformReadEntireFile = OSXDebugReadEntireFile;
            gameMemory.PlatformFunctions.PlatformWriteEntireFile = OSXDebugWriteEntireFile;

            OSXResizeTexture(&offscreenBuffer, renderer, 960, 540);
            OSXInitializeSound(&audioBuffer);

            //2 seconds of audio
            gameSoundBuffer.SampleBuffer = (int16_t *)mmap(0, audioBuffer.AudioSpec.freq * 2 * 2 * audioBuffer.AudioSpec.channels, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
            gameSoundBuffer.SamplesPerSecond = audioBuffer.AudioSpec.freq;

            //setup the ring buffer as well// 1 second... check this.
            audioBuffer.RingBufferLength = audioBuffer.AudioSpec.freq * sizeof(int16_t) * audioBuffer.AudioSpec.channels;
            audioBuffer.RingBuffer = (int16_t *)mmap(0, audioBuffer.RingBufferLength, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
            audioBuffer.PlayCursor = audioBuffer.WriteCursor = 0;
            audioBuffer.ApplicationNextWritePtr = 0;

            OSXDynamicGame dynamicGame = {};
            OSXSetupDynamicGameFilename(&dynamicGame);
            OSXSetupDynamicGameStruct(&dynamicGame);
            OSXLoadGame(&dynamicGame);

#define MAX_CONTROLLERS (4)
            SDL_GameController *controllerHandles[MAX_CONTROLLERS] = {};
            int maxJoysticks = SDL_NumJoysticks();
            int controllerIndex = 0;
            for (int i = 0; i < maxJoysticks; ++i)
            {
                if (!SDL_IsGameController(i))
                {
                    continue;
                }
                if (i >= MAX_CONTROLLERS)
                {
                    break;
                }
                controllerHandles[controllerIndex] = SDL_GameControllerOpen(i);
                controllerIndex++;
            }

            int gameUpdateHz = 30;
            float targetFrameTime = 1.f / (float)gameUpdateHz;

            IsRunning = true;

            gameClocks.UpdateDT = targetFrameTime;

            //setup timing
            uint64_t lastCounter = SDL_GetPerformanceCounter();
            uint64_t lastCycleCounter = OSXGetCycleCounter();
            float lastElapsedTime = 0.f;
            float lastFPS = 0.f;
            uint64_t lastElapsedCycles;

            while (IsRunning)
            {

                //if we need to reload the game, let's do it...
                if (OSXDynamicGameNeedsReImport(&dynamicGame))
                {
                    printf("Need game update");
                    OSXUnloadGame(&dynamicGame);
                    OSXLoadGame(&dynamicGame);
                }

                SDL_Event ev;
                while (SDL_PollEvent(&ev))
                {
                    OSXHandleEvent(&ev, &offscreenBuffer, currentInputBuffer, lastInputBuffer);
                }
                OSXUpdateMouse(window, currentInputBuffer, lastInputBuffer);
                for (size_t i = 0; i < MAX_CONTROLLERS; ++i)
                {
                    if (controllerHandles[i])
                    {
                        OSXUpdateControllers(currentInputBuffer, lastInputBuffer, controllerHandles[i], i);
                    }
                }

                dynamicGame.APIFunctions.UpdateAndRender(&threadContext, &gameMemory, (GameOffscreenBuffer *)&offscreenBuffer, currentInputBuffer, &gameClocks);

                float megaCycles = (float)lastElapsedCycles / (float)1000000.f;
                char fpsString[128];
                snprintf(fpsString, 128, "FPS: %0.1f UpdateTime: %0.2fms LastCycles: %0.2f MCycles", lastFPS, 1000.f * lastElapsedTime, megaCycles);
                dynamicGame.APIFunctions.DrawText((GameOffscreenBuffer *)&offscreenBuffer, 16, 100, fpsString, 1.f, 1.f, 1.f, true);

                float preAudioFrameTime = OSXGetSecondsElapsed(lastCounter, SDL_GetPerformanceCounter());
                SDL_LockAudioDevice(audioBuffer.AudioDevice);

                size_t byteToLock = (audioBuffer.ApplicationNextWritePtr) % audioBuffer.RingBufferLength;
                float remainingFrameTime = targetFrameTime - preAudioFrameTime;
                float audioBufferToFill = remainingFrameTime + targetFrameTime; //buffer through one whole frame
                if (audioBufferToFill > (2.0 * targetFrameTime) || audioBufferToFill < 0.f)
                {
                    audioBufferToFill = 2.0f * targetFrameTime;
                }
                int32_t samplesRequired = (int32_t)((float)audioBuffer.AudioSpec.freq * audioBufferToFill);
                int32_t bytesRequired = samplesRequired * 4;
                size_t targetCursor = audioBuffer.PlayCursor + bytesRequired;
                if (targetCursor > audioBuffer.ApplicationNextWritePtr)
                {
                    size_t bytesToWrite = targetCursor - audioBuffer.ApplicationNextWritePtr;
                    gameSoundBuffer.SampleCount = bytesToWrite >> 2;
                    dynamicGame.APIFunctions.GetSoundSamples(&threadContext, &gameMemory, &gameSoundBuffer);
                    OSXQueueAudio(&audioBuffer, &gameSoundBuffer, byteToLock);
                }
                SDL_UnlockAudioDevice(audioBuffer.AudioDevice);

                if (audioBuffer.IsPlaying == false)
                {
                    SDL_PauseAudioDevice(audioBuffer.AudioDevice, 0);
                    audioBuffer.IsPlaying = true;
                }

                if (OSXGetSecondsElapsed(lastCounter, SDL_GetPerformanceCounter()) < targetFrameTime)
                {
                    uint32_t timeToSleep = ((targetFrameTime - OSXGetSecondsElapsed(lastCounter, SDL_GetPerformanceCounter())) * 1000) - 1;
                    SDL_Delay(timeToSleep);
                    //Assert(OSXGetSecondsElapsed(lastCounter, SDL_GetPerformanceCounter())<targetFrameTime, "FRAME RATE");
                    while (OSXGetSecondsElapsed(lastCounter, SDL_GetPerformanceCounter()) < targetFrameTime)
                    {
                    }
                }
                uint64_t endCounter = SDL_GetPerformanceCounter();
                uint64_t endCycleCounter = OSXGetCycleCounter();
                OSXUpdateWindow(window, renderer, &offscreenBuffer);
                OSXSwapInputBuffers(&currentInputBuffer, &lastInputBuffer);

                lastElapsedTime = OSXGetSecondsElapsed(lastCounter, endCounter);
                lastFPS = 1.f / lastElapsedTime;
                lastElapsedCycles = endCycleCounter - lastCycleCounter;
                lastCounter = endCounter;
                lastCycleCounter = endCycleCounter;
            }
        }
    }
    SDL_Quit();
    return 0;
}