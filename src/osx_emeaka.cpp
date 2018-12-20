#include <SDL.h>
#include <mach-o/dyld.h>
#include <dlfcn.h>
#include <cstdio>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "emeaka.h"
#include "osx_emeaka.h"

global_variable bool IsRunning;



internal void OSXPathForFileRelativeToExecutable(char *path, uint32_t pathSize, const char *filename)
{   
    if(_NSGetExecutablePath(path,&pathSize) == 0)
    {
        printf("Path Size: %d\n",pathSize);
        char *lastSlash = strrchr(path,'/');
        lastSlash+=1;
        *lastSlash = '\0';
        strcat(path,filename);
        
        return;
    }
    else{
        return;
    }
}

internal void OSXSetupDynamicGameStruct(OSXDynamicGame *game)
{
    OSXPathForFileRelativeToExecutable((char *)&(game->DynamicLibPath),1024,"emeaka_game.dylib");
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
    struct stat fileStat;
    if(stat(game->DynamicLibPath,&fileStat))
    {
        lastWriteTime = fileStat.st_mtimespec.tv_sec;
    }
    return lastWriteTime;
}

internal bool OSXDynamicGameNeedsReImport(OSXDynamicGame *game)
{
    time_t updateTime = OSXDynamicGameUpdateTime(game);
    if(updateTime != game->FileUpdateTime)
    {
        return true;
    }
    return false;
}

internal void OSXLoadGame(OSXDynamicGame *game)
{
    game->IsValid = false;
    char dylibPath[1024];
    OSXPathForFileRelativeToExecutable((char *)&dylibPath,1024,"emeaka_game.dylib");
    void *gameDylib = dlopen(game->DynamicLibPath, RTLD_LAZY | RTLD_GLOBAL);
    if(gameDylib)
    {
        game->DynamicLibrary = gameDylib;
        
        game->APIFunctions.UpdateAndRender = (GameUpdateAndRenderType *)dlsym(gameDylib,"GameUpdateAndRender");
        game->APIFunctions.GetSoundSamples = (GameGetSoundSamplesType *)dlsym(gameDylib,"GameGetSoundSamples");
        game->APIFunctions.ClearBitmap = (GameClearBitmapType *)dlsym(gameDylib, "ClearBitmap");
        game->APIFunctions.DrawPixel = (GameDrawPixelType *)dlsym(gameDylib, "DrawPixel");
        game->APIFunctions.DrawRect = (GameDrawRectType *)dlsym(gameDylib, "DrawRect");
        game->APIFunctions.DrawCircle = (GameDrawCircleType *)dlsym(gameDylib, "DrawCircle");
        game->APIFunctions.DrawLine = (GameDrawLineType *)dlsym(gameDylib, "DrawLine");
        game->APIFunctions.DrawChar = (GameDrawCharType *)dlsym(gameDylib, "DrawChar");
        game->APIFunctions.DrawText = (GameDrawTextType *)dlsym(gameDylib, "DrawText");

        if( game->APIFunctions.UpdateAndRender &&
            game->APIFunctions.GetSoundSamples &&
            game->APIFunctions.ClearBitmap &&
            game->APIFunctions.DrawPixel &&
            game->APIFunctions.DrawRect&&
            game->APIFunctions.DrawCircle&&
            game->APIFunctions.DrawLine&&
            game->APIFunctions.DrawChar&&
            game->APIFunctions.DrawText)
        {
            game->IsValid = true;
            game->FileUpdateTime = OSXDynamicGameUpdateTime(game);
        }
    }

    if(!game->IsValid)
    {
        printf("Failed to import dynamic game\n.");
        OSXSetupDynamicGameStruct(game);
    }
}

internal void OSXUnloadGame(OSXDynamicGame *game)
{
    if(game->DynamicLibrary)
    {
        dlclose(game->DynamicLibrary);
    }
    OSXSetupDynamicGameStruct(game);
}

internal void OSXResizeTexture(OSXOffscreenBuffer *offscreenBuffer, SDL_Renderer *renderer, int width, int height)
{
    if(offscreenBuffer->Memory)
    {
        munmap(offscreenBuffer->Memory, offscreenBuffer->Width * offscreenBuffer->Height * offscreenBuffer->BytesPerPixel);
    }

    if(offscreenBuffer->Texture)
    {
        SDL_DestroyTexture(offscreenBuffer->Texture);
    }

    offscreenBuffer->Texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    offscreenBuffer->Width = width;
    offscreenBuffer->Height = height;
    offscreenBuffer->BytesPerPixel = 4;
    offscreenBuffer->Pitch = width * offscreenBuffer->BytesPerPixel;
    offscreenBuffer->Memory = mmap(0, width * height * offscreenBuffer->BytesPerPixel,PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON,-1, 0);
}

internal void OSXUpdateWindow(SDL_Window *window, SDL_Renderer *renderer, OSXOffscreenBuffer *offscreenBuffer)
{
    SDL_UpdateTexture(offscreenBuffer->Texture, 0, offscreenBuffer->Memory, offscreenBuffer->Pitch);
    SDL_RenderCopy(renderer, offscreenBuffer->Texture, 0, 0);
    SDL_RenderPresent(renderer);
}

internal void 
RenderGradient(GameOffscreenBuffer *offscreenBuffer, int blueOffset, int greenOffset)
{
    uint8_t *row = (uint8_t *)offscreenBuffer->Memory;
    for(int y = 0; y < offscreenBuffer->Height; ++y)
    {
        uint32_t *pixel = (uint32_t *)row;
        for(int x = 0; x < offscreenBuffer->Width; ++x)
        {
            uint8_t blue = x + blueOffset;
            uint8_t green = y + greenOffset;
            *pixel++ = ((green<<8) | blue);
        }
        row += offscreenBuffer->Pitch;
    }
}

internal bool OSXGetExecutableDirectory(char *exePath, uint32_t exePathSize)
{
    if(_NSGetExecutablePath(exePath,&exePathSize) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

internal void OSXUpdateMouse(GameInputBuffer *inputBuffer)
{

}

internal void OSXUpdateControllers(GameInputBuffer *inputBuffer)
{

}

internal void OSXInitializeSound()
{

}
internal void OSXSwapInputBuffers(GameInputBuffer **current, GameInputBuffer **old)
{
    GameInputBuffer *storage = *old;
    *old = *current;
    *current = storage;
}
internal void OSXHandleEvent(SDL_Event *event, OSXOffscreenBuffer *offscreenBuffer, GameInputBuffer *inputBuffer)
{

    switch(event->type)
    {
        case SDL_QUIT:
        {
            IsRunning = false;
        }break;

        case SDL_WINDOWEVENT:
        {
            switch(event->window.event)
            {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                {
                    SDL_Window *window = SDL_GetWindowFromID(event->window.windowID);
                    SDL_Renderer *renderer = SDL_GetRenderer(window);
                    OSXResizeTexture(offscreenBuffer, renderer, event->window.data1, event->window.data2);
                }break;
                

                case SDL_WINDOWEVENT_EXPOSED:
                {
                    SDL_Window *window = SDL_GetWindowFromID(event->window.windowID);
                    SDL_Renderer *renderer = SDL_GetRenderer(window);
                    OSXUpdateWindow(window, renderer, offscreenBuffer);
                }break;
            }
        }break;        
    }

}

int main(int argc, char** argv)
{   
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("EMEAKA", 
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          960,
                                          540,
                                          SDL_WINDOW_RESIZABLE);

    if(window)
    {
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
        
        if(renderer)
        {   
            //game is going to run, let's get everything ready...
            GameInputBuffer *currentInputBuffer;
            GameInputBuffer *lastInputBuffer;

            GameInputBuffer inputBuffer[2] = {};
    
            currentInputBuffer = &inputBuffer[0];
            lastInputBuffer = &inputBuffer[1];

            OSXOffscreenBuffer offscreenBuffer = {};
            GameClocks gameClocks = {};
            GameMemory gameMemory = {};
            ThreadContext threadContext = {};
            gameClocks.UpdateDT = 1.f / 30.f;
            
            gameMemory.PermanentStorageSize = MegaBytes(64);
            gameMemory.TransientStorageSize = GigaBytes(2);
            size_t totalMemorySize = gameMemory.PermanentStorageSize + gameMemory.TransientStorageSize;
            gameMemory.PermanentStorage = mmap((void *)TeraBytes(2),totalMemorySize,PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON,-1, 0);
            gameMemory.TransientStorage = (void *)((size_t)gameMemory.PermanentStorage + gameMemory.PermanentStorageSize);
            
            OSXResizeTexture(&offscreenBuffer, renderer, 960, 540);

            OSXDynamicGame dynamicGame = {};
            OSXSetupDynamicGameStruct(&dynamicGame);
            OSXLoadGame(&dynamicGame);


            IsRunning = true;
            while(IsRunning)
            {
                //if we need to reload the game, let's do it...
                if(OSXDynamicGameNeedsReImport(&dynamicGame))
                {
                    OSXUnloadGame(&dynamicGame);
                    OSXLoadGame(&dynamicGame);
                }

                SDL_Event ev;
                while(SDL_PollEvent(&ev))
                {
                    OSXHandleEvent(&ev, &offscreenBuffer, currentInputBuffer); 
                }
                OSXUpdateMouse(currentInputBuffer);
                OSXUpdateControllers(currentInputBuffer);

                dynamicGame.APIFunctions.UpdateAndRender(&threadContext, &gameMemory, (GameOffscreenBuffer *)&offscreenBuffer, currentInputBuffer, &gameClocks);
                OSXUpdateWindow(window,renderer,&offscreenBuffer);

                OSXSwapInputBuffers(&currentInputBuffer, &lastInputBuffer);
            }
        }
    }
    SDL_Quit();
    return 0;
}