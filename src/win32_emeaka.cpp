//Big deal includes
#include <Windows.h>
#include <Xinput.h>
#include <dsound.h>
#include <math.h>
#include <cstdint>
#include <cstdio>

//your own includes
#include "win32_emeaka.h"
#include "emeaka.h"

//Unity Build Garbage
#include "emeaka.cpp"

#define LOG(x) OutputDebugStringA(x)

//GLOBALS
global_variable Win32OffscreenBuffer OffscreenBuffer;
global_variable int64_t PerformanceCounterFrequency;
global_variable bool Running;

//Stubbing out XInput to allow for dynamic loading
const WORD Win32GamepadButtonScanOrder[] = {
    XINPUT_GAMEPAD_DPAD_UP,
    XINPUT_GAMEPAD_DPAD_DOWN,
    XINPUT_GAMEPAD_DPAD_LEFT,
    XINPUT_GAMEPAD_DPAD_RIGHT,
    XINPUT_GAMEPAD_START,
    XINPUT_GAMEPAD_BACK,
    XINPUT_GAMEPAD_LEFT_THUMB,
    XINPUT_GAMEPAD_RIGHT_THUMB,
    XINPUT_GAMEPAD_LEFT_SHOULDER,
    XINPUT_GAMEPAD_RIGHT_SHOULDER,
    XINPUT_GAMEPAD_A,
    XINPUT_GAMEPAD_B,
    XINPUT_GAMEPAD_X,
    XINPUT_GAMEPAD_Y,
};

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_GET_STATE(XInputGetStateType);
typedef X_INPUT_SET_STATE(XInputSetStateType);

X_INPUT_GET_STATE(XInputGetStateStub)
{
   return ERROR_DEVICE_NOT_CONNECTED;
}

X_INPUT_SET_STATE(XInputSetStateStub)
{
   return ERROR_DEVICE_NOT_CONNECTED;
}

global_variable XInputGetStateType *XInputGetState_ = XInputGetStateStub;
global_variable XInputSetStateType *XInputSetState_ = XInputSetStateStub;

#define XInputGetState XInputGetState_
#define XInputSetState XInputSetState_

internal void Win32LoadXInput(void)
{
   HMODULE xInputLib = LoadLibraryA("xinput1_4.dll");
   if (!xInputLib)
   {
      xInputLib = LoadLibraryA("xinput9_1_0.dll");
      if (!xInputLib)
      {
         xInputLib = LoadLibraryA("xinput1_3.dll");
         if (!xInputLib)
         {
            LOG("Failed to Load XInput...");
            return;
         }
         else
         {
            LOG("Loaded XInput 1.3...");
         }
      }
      else
      {
         LOG("Loaded XInput 9.1...");
      }
   }
   else
   {
      LOG("Loaded XInput 1.4...");
   }

   if (xInputLib)
   {
      XInputGetState = (XInputGetStateType *)GetProcAddress(xInputLib, "XInputGetState");
      if (!XInputGetState)
      {
         LOG("Failed to locate XInputGetState");
         XInputGetState = XInputGetStateStub;
      }

      XInputSetState = (XInputSetStateType *)GetProcAddress(xInputLib, "XInputSetState");
      if (!XInputSetState)
      {
         LOG("Failed to locate XInputSetState");
         XInputSetState = XInputSetStateStub;
      }
   }
}

//End of Xinput Junk

//direct sound

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(DirectSoundCreateType);

internal void Win32InitDSound(HWND window, Win32SoundOuput *soundOutput)
{
   //Load
   HMODULE dSoundLib = LoadLibraryA("dsound.dll");
   if (dSoundLib)
   {
      DirectSoundCreateType *DirectSoundCreateLocal = (DirectSoundCreateType *)GetProcAddress(dSoundLib, "DirectSoundCreate");
      LPDIRECTSOUND directSound;
      if (DirectSoundCreateLocal && SUCCEEDED(DirectSoundCreateLocal(0, &directSound, 0)))
      {
         WAVEFORMATEX waveFormat = {};
         waveFormat.wFormatTag = WAVE_FORMAT_PCM;
         waveFormat.nChannels = (WORD)soundOutput->Channels;
         waveFormat.wBitsPerSample = 16;
         waveFormat.nSamplesPerSec = soundOutput->SamplesPerSecond;
         waveFormat.nBlockAlign = waveFormat.nChannels * waveFormat.wBitsPerSample / 8;
         waveFormat.nAvgBytesPerSec = waveFormat.nBlockAlign * waveFormat.nSamplesPerSec;

         if (SUCCEEDED(directSound->SetCooperativeLevel(window, DSSCL_PRIORITY)))
         {
            LPDIRECTSOUNDBUFFER primaryBuffer;
            DSBUFFERDESC bufferDescription = {};
            bufferDescription.dwSize = sizeof(DSBUFFERDESC);
            bufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

            if (SUCCEEDED(directSound->CreateSoundBuffer(&bufferDescription, &primaryBuffer, 0)))
            {
               HRESULT error = primaryBuffer->SetFormat(&waveFormat);
               if (SUCCEEDED(error))
               {
                  OutputDebugStringA("Primary Buffer Created\n");
               }

               else
               {
                  LOG("Failed to allocate primary buffer for DirectSound");
               }
            }
         }
         else
         {
            LOG("Failed to set coooperative level for directsound");
         }

         DSBUFFERDESC bufferDescription = {};
         bufferDescription.dwSize = sizeof(DSBUFFERDESC);
         bufferDescription.dwBufferBytes = soundOutput->BufferSize;
         bufferDescription.lpwfxFormat = &waveFormat;

         HRESULT error = directSound->CreateSoundBuffer(&bufferDescription, &soundOutput->SoundBuffer, 0);
         if (SUCCEEDED(error))
         {
            OutputDebugStringA("Secondary Buffer Created\n");
         }
         else
         {
            LOG("Failed to create secondary buffer");
         }
      }
   }
   else
   {
      LOG("Failed to load DirectSound");
   }
}
//end dsound
internal void PlatformAssertFail(char * msg)
{
   OutputDebugStringA(msg);
   __debugbreak();
}


internal DebugFileResult PlatformReadEntireFile(char *filename)
{
   HANDLE fileHandle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
   DebugFileResult fileReadResult = {};

   if (fileHandle != INVALID_HANDLE_VALUE)
   {
      LARGE_INTEGER fileSize;
      if (GetFileSizeEx(fileHandle, &fileSize))
      {
         DWORD bytesRead;
         fileReadResult.Contents = VirtualAlloc(0, fileSize.QuadPart, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
         if (fileReadResult.Contents)
         {
            Assert(fileSize.QuadPart <= 0xFFFFFFFF, "Only 32Bit Files supported in windows API");
            uint32_t fileSize32 = (uint32_t)fileSize.QuadPart;
            if (ReadFile(fileHandle, fileReadResult.Contents, fileSize32, &bytesRead, 0) && bytesRead == fileSize32)
            {
               fileReadResult.FileSize = bytesRead;
            }
            else
            {
               PlatformFreeFileMemory(fileReadResult.Contents);
            }
         }
         else
         {
         }
      }
      else
      {
      }
      CloseHandle(fileHandle);
   }
   return fileReadResult;
}

internal void PlatformFreeFileMemory(void *memory)
{
   if (memory)
   {
      VirtualFree(memory, 0, MEM_RELEASE);
   }
}
internal bool PlatformWriteEntireFile(char *filename, size_t memorySize, void *memory)
{
   HANDLE fileHandle = CreateFileA(filename, GENERIC_WRITE, 0, 0, CREATE_NEW, 0, 0);
   if (fileHandle != INVALID_HANDLE_VALUE)
   {
      Assert(memorySize <= 0xFFFFFFFF, "Windows only supports 32 bit files");
      DWORD bytesWritten;
      if (WriteFile(fileHandle, memory, (DWORD)memorySize, &bytesWritten, 0) && bytesWritten == memorySize)
      {
         CloseHandle(fileHandle);
         return true;
      }
      else
      {
         CloseHandle(fileHandle);
         return false;
      }
   }
   else
   {
      return false;
   }
}

internal Win32WindowDimensions GetWin32WindowDimensions(HWND window)
{
   RECT clientRect;
   Win32WindowDimensions windowDimensions;
   GetClientRect(window, &clientRect);
   windowDimensions.Width = clientRect.right - clientRect.left;
   windowDimensions.Height = clientRect.bottom - clientRect.top;
   return windowDimensions;
}

internal void Win32ResizeDIBSection(Win32OffscreenBuffer *Buffer, int width, int height) //Device Independent Bitmap
{
   if (Buffer->Memory)
   {
      VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
   }

   Buffer->Width = width;
   Buffer->Height = height;
   Buffer->BytesPerPixel = 4;
   Buffer->Pitch = Buffer->Width * Buffer->BytesPerPixel;
   Buffer->Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   Buffer->Info.bmiHeader.biWidth = Buffer->Width;
   Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
   Buffer->Info.bmiHeader.biPlanes = 1;
   Buffer->Info.bmiHeader.biBitCount = 32;
   Buffer->Info.bmiHeader.biCompression = BI_RGB;

   int bitmapMemorySize = Buffer->BytesPerPixel * Buffer->Width * Buffer->Height;
   Buffer->Memory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
   ZeroMemory(Buffer->Memory, bitmapMemorySize);
}

internal void Win32DisplayBufferInWindow(HDC deviceContext, int windowWidth, int windowHeight, Win32OffscreenBuffer *Buffer, int x, int y, int width, int height)
{
   //bufferRatio = buffer->Width / Buffer->Height;

   StretchDIBits(
       deviceContext,
       //x, y, width, height,
       //x, y, width, height,
       0, 0, windowWidth, windowHeight,
       0, 0, Buffer->Width, Buffer->Height,

       Buffer->Memory,
       &Buffer->Info,
       DIB_RGB_COLORS,
       SRCCOPY);
}

internal void Win32ProcessWindowMessages(HWND window, GameInputBuffer *inputBuffer)
{
   MSG message;
   while (PeekMessage(&message, window, 0, 0, PM_REMOVE))
   {
      if (message.message == WM_QUIT)
      {
         Running = false;
      }
      else if (message.message == WM_SYSKEYDOWN ||
               message.message == WM_SYSKEYUP ||
               message.message == WM_KEYDOWN ||
               message.message == WM_KEYUP)
      {
         LPARAM lParam = message.lParam;
         WPARAM wParam = message.wParam;
         uint32_t VKCode = (uint32_t)wParam;
         bool wasDown = ((lParam & (1 << 30)) != 0);
         bool isDown = ((lParam & (1 << 31)) == 0);
         if (isDown != wasDown)
         {
            if (VKCode == VK_UP)
            {
               inputBuffer->KeyboardInput.UpArrow.IsDown = isDown;
               inputBuffer->KeyboardInput.UpArrow.HalfTransitions = (isDown == wasDown) ? 0 : 1;
            }
            else if (VKCode == VK_DOWN)
            {
               inputBuffer->KeyboardInput.DownArrow.IsDown = isDown;
               inputBuffer->KeyboardInput.DownArrow.HalfTransitions = 1;
            }
            else if (VKCode == VK_LEFT)
            {
               inputBuffer->KeyboardInput.LeftArrow.IsDown = isDown;
               inputBuffer->KeyboardInput.LeftArrow.HalfTransitions = 1;
            }
            else if (VKCode == VK_RIGHT)
            {
               inputBuffer->KeyboardInput.RightArrow.IsDown = isDown;
               inputBuffer->KeyboardInput.RightArrow.HalfTransitions = 1;
            }
            else if (VKCode == VK_ESCAPE)
            {
               inputBuffer->KeyboardInput.EscapeButton.IsDown = isDown;
               inputBuffer->KeyboardInput.EscapeButton.HalfTransitions = 1;
            }
            else if (VKCode == VK_SPACE)
            {
               inputBuffer->KeyboardInput.Key[' '].IsDown = isDown;
               inputBuffer->KeyboardInput.Key[' '].HalfTransitions = 1;
            }
            else if (VKCode >= 'A' && VKCode <= 'Z' ||
                     VKCode >= '0' && VKCode <= '9')
            {
               inputBuffer->KeyboardInput.Key[VKCode].IsDown = isDown;
               inputBuffer->KeyboardInput.Key[VKCode].HalfTransitions = 1;
            }
            else if (VKCode == VK_F4 && ((lParam & (1 << 29)) != 0))
            {
               Running = false;
            }
         }
      }
      else
      {
         TranslateMessage(&message);
         DispatchMessage(&message);
      }
   }
}
LRESULT CALLBACK Win32MainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
   case WM_SYSKEYDOWN:
   case WM_SYSKEYUP:
   case WM_KEYDOWN:
   case WM_KEYUP:
      Assert(false, "FAILURE") break;

   case WM_PAINT:
   {
      PAINTSTRUCT Paint;
      HDC deviceContext = BeginPaint(window, &Paint);

      int x = Paint.rcPaint.left;
      int y = Paint.rcPaint.top;
      int width = Paint.rcPaint.right - Paint.rcPaint.left;
      int height = Paint.rcPaint.bottom - Paint.rcPaint.top;

      Win32WindowDimensions windowDimensions = GetWin32WindowDimensions(window);
      Win32DisplayBufferInWindow(deviceContext, windowDimensions.Width, windowDimensions.Height, &OffscreenBuffer, x, y, width, height);
      EndPaint(window, &Paint);
   }
   break;

   case WM_SIZE:
   case WM_ACTIVATEAPP:
      break;

   case WM_CLOSE:   //message
   case WM_DESTROY: //error recreate
      Running = false;
      break;

   default:
      return DefWindowProcA(window, message, wParam, lParam);
   }

   return 0;
}

internal void Win32NormalizeStick(int16_t rawX, int16_t rawY, float *normalizedX, float *normalizedY, int16_t deadZone)
{
   if (rawX >= deadZone)
   {
      *normalizedX = (float)(rawX - deadZone) / (float)(INT16_MAX - deadZone);
   }
   else if (rawX <= -deadZone)
   {
      *normalizedX = -1.f * (float)(rawX + deadZone) / (float)(INT16_MIN + deadZone);
   }
   else
   {
      *normalizedX = 0.f;
   }

   if (rawY >= deadZone)
   {
      *normalizedY = (float)(rawY - deadZone) / (float)(INT16_MAX - deadZone);
   }
   else if (rawY <= -deadZone)
   {
      *normalizedY = -1.f * (float)(rawY + deadZone) / (float)(INT16_MIN + deadZone);
   }
   else
   {
      *normalizedY = 0.f;
   }
}

internal void Win32NormalizeTrigger(int16_t raw, float *normalized, int16_t deadZone)
{
   if (raw >= deadZone)
   {
      *normalized = (float)(raw - deadZone) / (float)(255 - deadZone);
   }
   else
   {
      *normalized = 0.f;
   }
}
internal void Win32ProcessGamepadInput(XINPUT_GAMEPAD *gamepad, size_t controllerIndex, GameInputBuffer *currentInputBuffer, GameInputBuffer *lastInputBuffer)
{

   currentInputBuffer->ControllerInput[controllerIndex].Connected = true;
   //this scan order works becasue platform and game are aligned in windows, don't make the same assumption in other
   //platforms
   for (int i = 0; i < ArrayCount(Win32GamepadButtonScanOrder); ++i)
   {

      currentInputBuffer->ControllerInput[controllerIndex].Buttons[i].IsDown = (gamepad->wButtons & Win32GamepadButtonScanOrder[i]) > 0;
      if (lastInputBuffer->ControllerInput[controllerIndex].Buttons[i].IsDown != currentInputBuffer->ControllerInput[controllerIndex].Buttons[i].IsDown)
      {
         currentInputBuffer->ControllerInput[controllerIndex].Buttons[i].HalfTransitions = 1;
      }
      else
      {
         currentInputBuffer->ControllerInput[controllerIndex].Buttons[i].HalfTransitions = 0;
      }
   }

   float LeftStickXNormalized = 0.f;
   float LeftStickYNormalized = 0.f;

   Win32NormalizeStick(gamepad->sThumbLX, gamepad->sThumbLY, &LeftStickXNormalized, &LeftStickYNormalized, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
   currentInputBuffer->ControllerInput[controllerIndex].LeftStick.AverageX = LeftStickXNormalized;
   currentInputBuffer->ControllerInput[controllerIndex].LeftStick.AverageY = LeftStickYNormalized;

   float RightStickXNormalized = 0.f;
   float RightStickYNormalized = 0.f;
   Win32NormalizeStick(gamepad->sThumbRX, gamepad->sThumbRY, &RightStickXNormalized, &RightStickYNormalized, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

   currentInputBuffer->ControllerInput[controllerIndex].RightStick.AverageX = RightStickXNormalized;
   currentInputBuffer->ControllerInput[controllerIndex].RightStick.AverageY = RightStickYNormalized;

   float LeftTriggerNormalized = 0.f;
   float RightTriggerNormalized = 0.f;
   Win32NormalizeTrigger(gamepad->bLeftTrigger, &LeftTriggerNormalized, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
   Win32NormalizeTrigger(gamepad->bRightTrigger, &RightTriggerNormalized, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

   currentInputBuffer->ControllerInput[controllerIndex].RightTrigger = RightTriggerNormalized;
   currentInputBuffer->ControllerInput[controllerIndex].LeftTrigger = LeftTriggerNormalized;
}

internal void
Win32PrepareInputBuffers(GameInputBuffer *currentInputBuffer, GameInputBuffer *lastInputBuffer)
{
   ZeroMemory(currentInputBuffer, sizeof(GameInputBuffer));
   currentInputBuffer->KeyboardInput.ShiftButton.IsDown = lastInputBuffer->KeyboardInput.ShiftButton.IsDown;
   currentInputBuffer->KeyboardInput.AltButton.IsDown = lastInputBuffer->KeyboardInput.AltButton.IsDown;
   currentInputBuffer->KeyboardInput.EscapeButton.IsDown = lastInputBuffer->KeyboardInput.EscapeButton.IsDown;
   currentInputBuffer->KeyboardInput.UpArrow.IsDown = lastInputBuffer->KeyboardInput.UpArrow.IsDown;
   currentInputBuffer->KeyboardInput.DownArrow.IsDown = lastInputBuffer->KeyboardInput.DownArrow.IsDown;
   currentInputBuffer->KeyboardInput.RightArrow.IsDown = lastInputBuffer->KeyboardInput.RightArrow.IsDown;
   currentInputBuffer->KeyboardInput.LeftArrow.IsDown = lastInputBuffer->KeyboardInput.LeftArrow.IsDown;
   currentInputBuffer->KeyboardInput.CtrlButton.IsDown = lastInputBuffer->KeyboardInput.CtrlButton.IsDown;
   for (int i = 0; i < 255; ++i)
   {
      currentInputBuffer->KeyboardInput.Key[i].IsDown = lastInputBuffer->KeyboardInput.Key[i].IsDown;
   }
   for (int i = 0; i < 12; ++i)
   {
      currentInputBuffer->KeyboardInput.FKey[i].IsDown = lastInputBuffer->KeyboardInput.FKey[i].IsDown;
   }

   currentInputBuffer->MouseInput.MouseInWindow = lastInputBuffer->MouseInput.MouseInWindow;
   currentInputBuffer->MouseInput.MouseLocationX = lastInputBuffer->MouseInput.MouseLocationX;
   currentInputBuffer->MouseInput.MouseLocationY = lastInputBuffer->MouseInput.MouseLocationY;
}
internal void Win32ClearSoundBuffer(Win32SoundOuput *soundOutput)
{
   int32_t *soundBuffer1;
   int32_t *soundBuffer2;
   DWORD soundBuffer1Size;
   DWORD soundBuffer2Size;

   if (SUCCEEDED(soundOutput->SoundBuffer->Lock(0, soundOutput->SamplesPerSecond, &(void *)soundBuffer1, &soundBuffer1Size, &(void *)soundBuffer2, &soundBuffer2Size, 0)))
   {
      for (DWORD i = 0; i < soundBuffer1Size; ++i)
      {
         soundBuffer1[i] = 0;
         soundOutput->RunningSampleIndex++;
      }
      for (DWORD i = 0; i < soundBuffer2Size; ++i)
      {
         soundBuffer2[i] = 0;
         soundOutput->RunningSampleIndex++;
      }
      soundOutput->SoundBuffer->Unlock(soundBuffer1, soundBuffer1Size, soundBuffer2, soundBuffer2Size);
   }
}

internal void Win32FillSoundBuffer(Win32SoundOuput *soundOutput, DWORD byteToLock, DWORD bytesToWrite, GameSoundBuffer *sourceBuffer)
{
   int32_t *soundBuffer1;
   int32_t *soundBuffer2;
   DWORD soundBuffer1Size;
   DWORD soundBuffer2Size;
   if (SUCCEEDED(soundOutput->SoundBuffer->Lock(byteToLock, bytesToWrite, &(void *)soundBuffer1, &soundBuffer1Size, &(void *)soundBuffer2, &soundBuffer2Size, 0)))
   {
      int32_t *srcSample = (int32_t *)sourceBuffer->SampleBuffer;
      DWORD bank1SampleCount = soundBuffer1Size / 4; //) soundOutput->BytesPerSample;
      DWORD bank2SampleCount = soundBuffer2Size / 4; // soundOutput->BytesPerSample;
      for (DWORD srcSampleIndex = 0; srcSampleIndex < sourceBuffer->SampleCount && srcSampleIndex < bank1SampleCount + bank2SampleCount; srcSampleIndex++)
      {

         if (srcSampleIndex < bank1SampleCount)
         {
            soundBuffer1[srcSampleIndex] = srcSample[srcSampleIndex];
         }
         else
         {
            int destSample = srcSampleIndex - bank1SampleCount;
            soundBuffer2[destSample] = srcSample[srcSampleIndex];
         }
         soundOutput->RunningSampleIndex++;
      }
      soundOutput->SoundBuffer->Unlock(soundBuffer1, soundBuffer1Size, soundBuffer2, soundBuffer2Size);
   }
}

internal void
Win32DebugDrawLine(Win32OffscreenBuffer *offscreenBuffer, int32_t startx, int32_t starty, int32_t endx, int32_t endy, uint8_t r, uint8_t g, uint8_t b)
{
   //do a thing
}


internal void
Win32DebugDrawVertical(Win32OffscreenBuffer *offscreenBuffer, uint32_t x, uint32_t y, uint32_t height, uint8_t r, uint8_t g, uint8_t b)
{
   for(uint32_t cur_y = y; cur_y < y + height; cur_y++)
   {
      size_t pixel = (cur_y * offscreenBuffer->Width) + x;
      ((uint32_t *)(offscreenBuffer->Memory))[pixel] = RGB_TO_UINT32(r, g, b);
   }
}


internal void Win32DebugSyncDisplay(Win32OffscreenBuffer *offscreenBuffer, size_t markerCount, Win32DebugTimeMarker *markers, size_t currentMarker, Win32SoundOuput *soundOutputBuffer, float secondsPerFrame)
{
   int padx = 16;
   float C = (float)(offscreenBuffer->Width-2*padx) / (float)soundOutputBuffer->BufferSize;
   
   for(int i = 0; i < markerCount; ++i)
   {
      uint32_t xpos = padx + (uint32_t)( C * ((float)markers[i].OutputPlayCursor));
      uint32_t ypos = 16;
      uint32_t height = 64;
      Win32DebugDrawVertical(offscreenBuffer, xpos, ypos, height, 255, 255 , 255);

      xpos = padx + (uint32_t)(C * (float)markers[i].OutputWriteCursor);
      Win32DebugDrawVertical(offscreenBuffer, xpos, ypos, height, 0, 0 , 255);

      ypos += height + 16;
      xpos = padx + (uint32_t)(C * ((float)markers[i].OutputLocation));
      Win32DebugDrawVertical(offscreenBuffer, xpos, ypos, height, 255, 255 , 255);
      xpos = padx + (uint32_t)(C * ((float)markers[i].OutputByteCount+(float)markers[i].OutputLocation));
      Win32DebugDrawVertical(offscreenBuffer, xpos, ypos, height, 0, 0 , 255);

      ypos += height + 16;
      xpos = padx + (uint32_t)(C * ((float)markers[i].ExpectedFlipPlayCursor));
      Win32DebugDrawVertical(offscreenBuffer, xpos, ypos, height, 255, 0 , 0);
//
      //xpos = padx + (uint32_t)((float)(offscreenBuffer->Width-2*padx) * ((float)markers[i].FlipPlayCursor / (float)soundOutputBuffer->BufferSize));
      //ypos = 64;
      //height = 100;
      //Win32DebugDrawVertical(offscreenBuffer, xpos, ypos, height, 255, 255 , 255);
//
      //xpos = padx + (uint32_t)((float)(offscreenBuffer->Width-2*padx) * ((float)markers[i].FlipWriteCursor / (float)soundOutputBuffer->BufferSize));
      //ypos = 64;
      //height = 100;
      //Win32DebugDrawVertical(offscreenBuffer, xpos, ypos, height, 255, 255 , 255);

      
   }
}












internal inline float Win32GetSecondsElapsed(LARGE_INTEGER start, LARGE_INTEGER end)
{
   float result = (((float)(end.QuadPart - start.QuadPart)) / ((float)PerformanceCounterFrequency));
   return result;
}

internal inline LARGE_INTEGER Win32GetPerformanceCounter()
{
   LARGE_INTEGER counter;
   QueryPerformanceCounter(&counter);
   return counter;
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode)
{

   //**************************************************************************
   //Performance Counters & Timing
   //**************************************************************************
   LARGE_INTEGER frequency;
   QueryPerformanceFrequency(&frequency);
   PerformanceCounterFrequency = frequency.QuadPart;

   LARGE_INTEGER startOfFrameTime;
   LARGE_INTEGER inputCompleteTime;
   LARGE_INTEGER gameUpdateTime;
   LARGE_INTEGER soundUpdateStartTime;
   LARGE_INTEGER preFlipTime;
   LARGE_INTEGER endFrameTime;

   UINT desiredSchedulerMS = 1;
   bool sleepIsGranular = (TIMERR_NOERROR == timeBeginPeriod(desiredSchedulerMS));

   GameClocks gameClocks = {};


   //get this a better way
   const int monitorRefreshHz = 60;
   const int gameUpdateHz = monitorRefreshHz / 2;
   const float targetFrameTime = 1.f / (float)gameUpdateHz;


   //**************************************************************************
   //XInput Work
   //**************************************************************************
   Win32LoadXInput();

   GameInputBuffer inputBuffer1 = {};
   GameInputBuffer inputBuffer2 = {};
   GameInputBuffer *currentInputBuffer = &inputBuffer1;
   GameInputBuffer *lastInputBuffer = &inputBuffer2;


   //**************************************************************************
   //XInput Work
   //**************************************************************************
   Win32ResizeDIBSection(&OffscreenBuffer, 1280, 720);

   WNDCLASSEXA windowClass = {};
   windowClass.cbSize = sizeof(WNDCLASSEX);
   windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
   windowClass.lpfnWndProc = Win32MainWindowCallback;
   windowClass.hInstance = instance;
   windowClass.hIcon = LoadIcon(instance, IDI_APPLICATION);
   windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
   windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
   windowClass.lpszClassName = "emeakaWindowClass";
   windowClass.hIconSm = LoadIcon(windowClass.hInstance, IDI_APPLICATION);

   

   if (RegisterClassExA(&windowClass))
   {
      HWND window = CreateWindowExA(
          0,
          windowClass.lpszClassName,
          "Eme Aka",
          WS_OVERLAPPEDWINDOW | WS_VISIBLE,
          CW_USEDEFAULT, CW_USEDEFAULT,
          CW_USEDEFAULT, CW_USEDEFAULT,
          NULL,
          NULL,
          instance,
          NULL);
      if (window)
      {
         ShowWindow(window, showCode);
         UpdateWindow(window);
         HDC deviceContext = GetDC(window);
         Running = true;



         LPVOID baseAddress = 0;
#if EMEAKA_INTERNAL
         baseAddress = (LPVOID)TeraBytes(2);
#endif
         GameMemory gameMemory;
         gameMemory.PermanentStorageSize = MegaBytes(64);
         gameMemory.TransientStorageSize = GigaBytes(4);

         size_t totalMemorySize = gameMemory.PermanentStorageSize + gameMemory.TransientStorageSize;
         gameMemory.PermanentStorage = VirtualAlloc(baseAddress, totalMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
         gameMemory.TransientStorage = (uint8_t *)gameMemory.PermanentStorage + gameMemory.PermanentStorageSize;

         gameMemory.IsInitialized = false;

         //sound test
         Win32SoundOuput win32SoundOutput = {};
         win32SoundOutput.SamplesPerSecond = 48000;
         win32SoundOutput.RunningSampleIndex = 0;
         win32SoundOutput.Channels = 2;
         win32SoundOutput.BytesPerSample = 4;
         win32SoundOutput.BufferSize = win32SoundOutput.SamplesPerSecond * win32SoundOutput.BytesPerSample;
         win32SoundOutput.LatencySampleCount = 3 * (win32SoundOutput.SamplesPerSecond / gameUpdateHz);
         win32SoundOutput.SafetyBytes = (win32SoundOutput.SamplesPerSecond * win32SoundOutput.BytesPerSample / gameUpdateHz)/3;

         Win32InitDSound(window, &win32SoundOutput);
         Win32ClearSoundBuffer(&win32SoundOutput);

         GameSoundBuffer soundOutputBuffer = {};
         soundOutputBuffer.SampleBuffer = (int16_t *)VirtualAlloc(0, win32SoundOutput.BufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
         soundOutputBuffer.SamplesPerSecond = win32SoundOutput.SamplesPerSecond;
         win32SoundOutput.SoundBuffer->Play(0, 0, DSBPLAY_LOOPING);

         if (!soundOutputBuffer.SampleBuffer || !gameMemory.PermanentStorage || !gameMemory.TransientStorage)
         {
            return -1;
         }

         //random debug stuff
         size_t debugTimeMarkersIndex = 0;
         Win32DebugTimeMarker debugTimeMarkers[gameUpdateHz/2] = {};

         DWORD audioLatencyBytes = 0;
         float audioLatencySeconds = 0;
         bool soundIsValid = false;


         startOfFrameTime = Win32GetPerformanceCounter();
         uint64_t lastCycleCount = __rdtsc();
         while (Running)
         {
            //win32 only needs current since we're going to get teh transition information from windows...
            Win32PrepareInputBuffers(currentInputBuffer, lastInputBuffer);
            Win32ProcessWindowMessages(window, currentInputBuffer);
            //game input, do we need more frequent polls
            for (int controllerIndex = 0; controllerIndex < XUSER_MAX_COUNT; ++controllerIndex)
            {
               XINPUT_STATE state;
               ZeroMemory(&state, sizeof(XINPUT_STATE));
               if (XInputGetState(controllerIndex, &state) == ERROR_SUCCESS)
               {
                  currentInputBuffer->ControllerInput[controllerIndex].Connected = true;
                  Win32ProcessGamepadInput(&state.Gamepad, controllerIndex, currentInputBuffer, lastInputBuffer);
               }
            }

            inputCompleteTime = Win32GetPerformanceCounter();

            //**************************************************************************
            // UPDATE AND RENDER
            //**************************************************************************
            GameUpdateAndRender(&gameMemory, (GameOffscreenBuffer *)(&OffscreenBuffer), currentInputBuffer, &gameClocks);
            //**************************************************************************
            // UPDATE AND RENDER
            //**************************************************************************


            soundUpdateStartTime = Win32GetPerformanceCounter();
            float fromBeginToAudioSeconds = Win32GetSecondsElapsed(startOfFrameTime,soundUpdateStartTime);
            //Note:Direct sound output test
            DWORD playCursor = 0;
            DWORD writeCursor = 0;
            if (win32SoundOutput.SoundBuffer->GetCurrentPosition(&playCursor, &writeCursor)==DS_OK)
            {
               if(!soundIsValid)
               {
                  win32SoundOutput.RunningSampleIndex = writeCursor/win32SoundOutput.BytesPerSample;
                  soundIsValid = true;
               }

               DWORD byteToLock = (win32SoundOutput.RunningSampleIndex * win32SoundOutput.BytesPerSample) % win32SoundOutput.BufferSize;

               DWORD soundBytesPerFrame = (win32SoundOutput.SamplesPerSecond * win32SoundOutput.BytesPerSample)/gameUpdateHz;
               
               float secondsUntilFlip = targetFrameTime - fromBeginToAudioSeconds;
               
               DWORD expectedBytesUntilFlip = (DWORD)((secondsUntilFlip/targetFrameTime)*(float)soundBytesPerFrame);

               DWORD expectedFrameEnd = playCursor + expectedBytesUntilFlip;
               DWORD safeWriteCursor = writeCursor;
               
               if(safeWriteCursor < playCursor)
               {
                  safeWriteCursor += win32SoundOutput.BufferSize;
               }

               bool audioIsLowLatency = (safeWriteCursor < expectedFrameEnd);

               DWORD targetCursor = 0;

               if(audioIsLowLatency)
               {
                  targetCursor = expectedFrameEnd + soundBytesPerFrame;
               }
               else
               {
                  targetCursor = writeCursor + soundBytesPerFrame + win32SoundOutput.SafetyBytes;
               }

               targetCursor = targetCursor % win32SoundOutput.BufferSize;
               
               DWORD bytesToWrite = 0;

               if(byteToLock > targetCursor)
               {
                  bytesToWrite = win32SoundOutput.BufferSize - byteToLock;
                  bytesToWrite += targetCursor;
               }
               else
               {
                  bytesToWrite = targetCursor - byteToLock;
               }               
               soundOutputBuffer.SamplesPerSecond = win32SoundOutput.SamplesPerSecond;

               soundOutputBuffer.SampleCount = bytesToWrite / win32SoundOutput.BytesPerSample;

               //**************************************************************************
               // SOUND UPDATE
               //**************************************************************************
               GameGetSoundSamples(&gameMemory, &soundOutputBuffer);

               
#ifdef EMEAKA_INTERNAL
               Win32DebugTimeMarker *marker = &debugTimeMarkers[debugTimeMarkersIndex];
               marker->OutputPlayCursor = playCursor;
               marker->OutputWriteCursor = writeCursor;
               marker->OutputLocation = byteToLock;
               marker->OutputByteCount = bytesToWrite;
               marker->ExpectedFlipPlayCursor = expectedFrameEnd;

               DWORD unwrappedWriteCursor = writeCursor;
               if(unwrappedWriteCursor < playCursor)
               {
                  unwrappedWriteCursor += win32SoundOutput.BufferSize;
               }
               audioLatencyBytes = unwrappedWriteCursor - playCursor;
               audioLatencySeconds =
                  (((float)audioLatencyBytes / (float)win32SoundOutput.BytesPerSample) /
                  (float)win32SoundOutput.SamplesPerSecond);
                        
               char TextBuffer[256];
               _snprintf_s(TextBuffer, sizeof(TextBuffer),
                           "BTL:%u TC:%u BTW:%u - PC:%u WC:%u DELTA:%u (%fs)\n",
                           byteToLock, targetCursor, bytesToWrite,
                           playCursor, writeCursor, audioLatencyBytes, audioLatencySeconds);
               OutputDebugStringA(TextBuffer);

#endif
               Win32FillSoundBuffer(&win32SoundOutput, byteToLock, bytesToWrite, &soundOutputBuffer);
            }
            else
            {
               soundIsValid = false;
            }

   

            

            gameUpdateTime = Win32GetPerformanceCounter();

            for (int controllerIndex = 0; controllerIndex < XUSER_MAX_COUNT; ++controllerIndex)
            {
               if (currentInputBuffer->ControllerInput[controllerIndex].Connected)
               {
                  XINPUT_VIBRATION vibration = {};
                  vibration.wLeftMotorSpeed = (WORD)currentInputBuffer->ControllerInput[controllerIndex].LeftVibration;
                  vibration.wRightMotorSpeed = (WORD)currentInputBuffer->ControllerInput[controllerIndex].RightVibration;
                  XInputSetState(controllerIndex, &vibration);
               }
            }
            //swap buffers
            GameInputBuffer *tempInputBuffer = currentInputBuffer;
            currentInputBuffer = lastInputBuffer;
            lastInputBuffer = tempInputBuffer;

            


            

            Win32WindowDimensions windowDimensions = GetWin32WindowDimensions(window);

            preFlipTime = Win32GetPerformanceCounter();

            float timeElapsedThisFrame = Win32GetSecondsElapsed(startOfFrameTime, preFlipTime);

			if (timeElapsedThisFrame < targetFrameTime)
            {
               if (sleepIsGranular)
               {
                  DWORD timeToSleep = (DWORD)(1000.f * (targetFrameTime - timeElapsedThisFrame));
                  if (timeToSleep > 0)
                  {
                     Sleep(timeToSleep);
                  }
               }
               timeElapsedThisFrame = Win32GetSecondsElapsed(startOfFrameTime, Win32GetPerformanceCounter());
               while (timeElapsedThisFrame < targetFrameTime)
               {
                  timeElapsedThisFrame = Win32GetSecondsElapsed(startOfFrameTime, Win32GetPerformanceCounter());
               }
            }
            else
            {
               OutputDebugStringA("Missed Frame\n");
            }

            endFrameTime = Win32GetPerformanceCounter();
            uint64_t endCycleCount = __rdtsc();


#if EMEAKA_INTERNAL
            Win32DebugSyncDisplay(&OffscreenBuffer, ArrayCount(debugTimeMarkers), debugTimeMarkers, debugTimeMarkersIndex, &win32SoundOutput,targetFrameTime);
#endif
            Win32DisplayBufferInWindow(deviceContext, windowDimensions.Width, windowDimensions.Height, &OffscreenBuffer, 0, 0, windowDimensions.Width, windowDimensions.Height);
            //ReleaseDC(window, deviceContext);

            
            //strange platform debug code goes here...
            #if EMEAKA_INTERNAL
            {
               DWORD flipplayCursor;
               DWORD flipwriteCursor;
               if(win32SoundOutput.SoundBuffer->GetCurrentPosition(&flipplayCursor, &flipwriteCursor)==DS_OK)
               {
                  Win32DebugTimeMarker *marker = &debugTimeMarkers[debugTimeMarkersIndex];
                  marker->FlipPlayCursor = flipplayCursor;
                  marker->FlipWriteCursor = flipwriteCursor;
               }
            }
            #endif
         
            uint64_t cyclesElapsed = endCycleCount - lastCycleCount;

            local_persist int printy = 0;
            if ((printy++ % 60) == 0)
            {
               float counterElapsed = Win32GetSecondsElapsed(startOfFrameTime, endFrameTime);
               char counterReport[512];
               float msElapsed = 1000.f * counterElapsed;
               float fps = 1.f / counterElapsed;
               float mcpf = (float)cyclesElapsed / 1000.f / 1000.f;
               snprintf(counterReport, sizeof(counterReport), "Frame Information: %0.2fmc/f   %0.2fms/f   %0.1ffps\n", mcpf, msElapsed, fps);
               OutputDebugStringA(counterReport);
            }
            debugTimeMarkersIndex++;
            debugTimeMarkersIndex = debugTimeMarkersIndex % ArrayCount(debugTimeMarkers);
            lastCycleCount = endCycleCount;
            startOfFrameTime = endFrameTime;
         }
      }
   }

   return 0; //useless
}
