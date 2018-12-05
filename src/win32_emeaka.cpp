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
global_variable bool Running;

//Stubbing out XInput to allow for dynamic loading
const WORD Win32GamepadButtonScanOrder [] = {
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


internal DebugFileResult PlatformReadEntireFile(char *filename)
{
   HANDLE fileHandle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
   DebugFileResult fileReadResult = {};

   if(fileHandle != INVALID_HANDLE_VALUE)
   {
      LARGE_INTEGER fileSize;
      if(GetFileSizeEx(fileHandle, &fileSize))
      {
         DWORD bytesRead;
         fileReadResult.Contents = VirtualAlloc(0,fileSize.QuadPart,MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
         if(fileReadResult.Contents)
         {
            Assert(fileSize.QuadPart <= 0xFFFFFFFF, "Only 32Bit Files supported in windows API");
            uint32_t fileSize32 = (uint32_t)fileSize.QuadPart;
            if(ReadFile(fileHandle, fileReadResult.Contents, fileSize32, &bytesRead, 0) && bytesRead == fileSize32)
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
   if(memory)
   {
      VirtualFree(memory,0,MEM_RELEASE);
   }

}
internal bool PlatformWriteEntireFile(char *filename, size_t memorySize, void *memory)
{
   HANDLE fileHandle = CreateFileA(filename, GENERIC_WRITE, 0, 0, CREATE_NEW, 0, 0);
   if(fileHandle != INVALID_HANDLE_VALUE)
   {
      Assert(memorySize <= 0xFFFFFFFF, "Windows only supports 32 bit files");
      DWORD bytesWritten;
      if(WriteFile(fileHandle, memory, (DWORD)memorySize, &bytesWritten, 0) && bytesWritten == memorySize)
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

LRESULT CALLBACK Win32MainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
   case WM_SYSKEYDOWN:
   case WM_SYSKEYUP:
   case WM_KEYDOWN:
   case WM_KEYUP:
      Assert(false, "FAILURE")
   break;

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

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode)
{
   LARGE_INTEGER frequency;
   QueryPerformanceFrequency(&frequency);
   int64_t performanceCounterFrequency = frequency.QuadPart;
   performanceCounterFrequency = performanceCounterFrequency;

   Win32LoadXInput();
   
   
   GameClocks gameClocks = {};


   GameInputBuffer inputBuffer1 = {};
   GameInputBuffer inputBuffer2 = {};
   GameInputBuffer *currentInputBuffer = &inputBuffer1;
   GameInputBuffer *lastInputBuffer = &inputBuffer2;

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

         MSG message;
         Running = true;

         LPVOID baseAddress = 0;
#if EMEAKA_INTERNAL
         baseAddress = (LPVOID)TeraBytes(2);
#endif
         GameMemory gameMemory;
         gameMemory.PermanentStorageSize =   MegaBytes(64);
         gameMemory.TransientStorageSize =   GigaBytes(4);

         size_t totalMemorySize = gameMemory.PermanentStorageSize + gameMemory.TransientStorageSize;
         gameMemory.PermanentStorage =       VirtualAlloc(baseAddress,totalMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
         gameMemory.TransientStorage = (uint8_t *)gameMemory.PermanentStorage + gameMemory.PermanentStorageSize;
         
         gameMemory.IsInitialized = false;

         //sound test
         Win32SoundOuput win32SoundOutput = {};
         win32SoundOutput.SamplesPerSecond = 48000;
         win32SoundOutput.RunningSampleIndex = 0;
         win32SoundOutput.Channels = 2;
         win32SoundOutput.BytesPerSample = 4;
         win32SoundOutput.BufferSize = win32SoundOutput.SamplesPerSecond * win32SoundOutput.BytesPerSample;
         win32SoundOutput.LatencySampleCount = win32SoundOutput.SamplesPerSecond / 15;

         Win32InitDSound(window, &win32SoundOutput);
         Win32ClearSoundBuffer(&win32SoundOutput);

         GameSoundBuffer soundOutputBuffer = {};
         soundOutputBuffer.SampleBuffer = (int16_t *)VirtualAlloc(0, win32SoundOutput.BufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
         soundOutputBuffer.SamplesPerSecond = win32SoundOutput.SamplesPerSecond;
         win32SoundOutput.SoundBuffer->Play(0, 0, DSBPLAY_LOOPING);

         if(!soundOutputBuffer.SampleBuffer || !gameMemory.PermanentStorage || !gameMemory.TransientStorage)
         {
            return -1;
         }


         LARGE_INTEGER lastCounter;
         QueryPerformanceCounter(&lastCounter);
         uint64_t lastCycleCount = __rdtsc();
         while (Running)
         {

            while (PeekMessage(&message, window, 0, 0, PM_REMOVE))
            {
               if (message.message == WM_QUIT)
               {
                  Running = false;
               }
               else if(message.message == WM_SYSKEYDOWN ||
                        message.message == WM_SYSKEYUP  ||
                        message.message == WM_KEYDOWN   ||
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
                        currentInputBuffer->ControllerInput[0].UpButton.IsDown = isDown;
                        currentInputBuffer->ControllerInput[0].UpButton.HalfTransitions = (isDown == wasDown)?0:1;
                     }
                     
                     else if (VKCode == VK_DOWN)
                     {
                        currentInputBuffer->ControllerInput[0].DownButton.IsDown = isDown;
                        currentInputBuffer->ControllerInput[0].DownButton.HalfTransitions = (isDown == wasDown)?0:1;
                     }
                     else if (VKCode == VK_LEFT)
                     {
                        currentInputBuffer->ControllerInput[0].LeftButton.IsDown = isDown;
                        currentInputBuffer->ControllerInput[0].LeftButton.HalfTransitions = (isDown == wasDown)?0:1;
                     }
                     else if (VKCode == VK_RIGHT)
                     {
                        currentInputBuffer->ControllerInput[0].RightButton.IsDown = isDown;
                        currentInputBuffer->ControllerInput[0].RightButton.HalfTransitions = (isDown == wasDown)?0:1;
                     }
                     else if (VKCode == VK_ESCAPE)
                     {
                     }
                     else if (VKCode == VK_SPACE)
                     {
                     }
                     else if (VKCode == 'W')
                     {
                        currentInputBuffer->ControllerInput[0].UpButton.IsDown = isDown;
                        currentInputBuffer->ControllerInput[0].UpButton.HalfTransitions = (isDown == wasDown)?0:1;
                     }
                     else if (VKCode == 'A')
                     {
                        currentInputBuffer->ControllerInput[0].LeftButton.IsDown = isDown;
                        currentInputBuffer->ControllerInput[0].LeftButton.HalfTransitions = (isDown == wasDown)?0:1;

                     }
                     else if (VKCode == 'S')
                     {
                        currentInputBuffer->ControllerInput[0].DownButton.IsDown = isDown;
                        currentInputBuffer->ControllerInput[0].DownButton.HalfTransitions = (isDown == wasDown)?0:1;

                     }
                     else if (VKCode == 'D')
                     {
                        currentInputBuffer->ControllerInput[0].RightButton.IsDown = isDown;
                        currentInputBuffer->ControllerInput[0].RightButton.HalfTransitions = (isDown == wasDown)?0:1;

                     }
                     else if (VKCode == 'Q')
                     {
                        Running = false;
                     }
                     else if (VKCode == 'E')
                     {
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
            
            //game input, do we need more frequent polls
            for (int controllerIndex = 0; controllerIndex < XUSER_MAX_COUNT; ++controllerIndex)
            {
               XINPUT_STATE state;
               ZeroMemory(&state, sizeof(XINPUT_STATE));
               currentInputBuffer->ControllerInput[controllerIndex].Connected = false;
               if (XInputGetState(controllerIndex, &state) == ERROR_SUCCESS)
               {
                  XINPUT_GAMEPAD *pad = &state.Gamepad;
                  currentInputBuffer->ControllerInput[controllerIndex].Connected = true;
                  //this scan order works becasue platform and game are aligned in windows, don't make the same assumption in other
                  //platforms
                  for(int i = 0; i < ArrayCount(Win32GamepadButtonScanOrder); ++i)
                  {
                     
                     currentInputBuffer->ControllerInput[controllerIndex].Buttons[i].IsDown = (pad->wButtons & Win32GamepadButtonScanOrder[i]) > 0;
                     if(lastInputBuffer->ControllerInput[controllerIndex].Buttons[i].IsDown != currentInputBuffer->ControllerInput[controllerIndex].Buttons[i].IsDown)
                     {
                        currentInputBuffer->ControllerInput[controllerIndex].Buttons[i].HalfTransitions = 1;
                     }
                     else
                     {
                        currentInputBuffer->ControllerInput[controllerIndex].Buttons[i].HalfTransitions = 0;
                     }
                  }

                  int16_t LeftStickX = pad->sThumbLX;
                  int16_t LeftStickY = pad->sThumbLY;
                  int16_t RightStickX = pad->sThumbRX;
                  int16_t RightStickY = pad->sThumbRY;
                  uint8_t RightTrigger = pad->bRightTrigger;
                  uint8_t LeftTrigger = pad->bLeftTrigger;

                  float LeftStickXNormalized = 0.0;
                  float LeftStickYNormalized = 0.0;
                  if (LeftStickX >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
                  {
                     LeftStickXNormalized = (float)LeftStickX / (float)INT16_MAX;
                  }
                  if (LeftStickX <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
                  {
                     LeftStickXNormalized = -1.f * (float)LeftStickX / (float)INT16_MIN;
                  }
                  currentInputBuffer->ControllerInput[0].LeftStick.EndX = 
                  currentInputBuffer->ControllerInput[0].LeftStick.StartX = 
                  currentInputBuffer->ControllerInput[0].LeftStick.MinX = 
                  currentInputBuffer->ControllerInput[0].LeftStick.MaxX = 
                  LeftStickXNormalized;

                  if (LeftStickY >= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
                  {
                     LeftStickYNormalized = (float)LeftStickY / (float)INT16_MAX;
                  }
                  if (LeftStickY <= -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
                  {
                     LeftStickYNormalized = -1.f * (float)LeftStickY / (float)INT16_MIN;
                  }
                  currentInputBuffer->ControllerInput[controllerIndex].LeftStick.EndY = 
                  currentInputBuffer->ControllerInput[controllerIndex].LeftStick.StartY = 
                  currentInputBuffer->ControllerInput[controllerIndex].LeftStick.MinY = 
                  currentInputBuffer->ControllerInput[controllerIndex].LeftStick.MaxY = LeftStickYNormalized;

                  float RightStickXNormalized = 0.0;
                  float RightStickYNormalized = 0.0;
                  if (RightStickX >= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
                  {
                     RightStickXNormalized = (float)RightStickX / (float)INT16_MAX;
                  }
                  if (RightStickX <= -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
                  {
                     RightStickXNormalized = -1.f * (float)RightStickX / (float)INT16_MIN;
                  }
                  currentInputBuffer->ControllerInput[controllerIndex].RightStick.EndX = 
                  currentInputBuffer->ControllerInput[controllerIndex].RightStick.StartX = 
                  currentInputBuffer->ControllerInput[controllerIndex].RightStick.MinX = 
                  currentInputBuffer->ControllerInput[controllerIndex].RightStick.MaxX = RightStickXNormalized;

                  if (RightStickY >= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
                  {
                     RightStickYNormalized = (float)RightStickY / (float)INT16_MAX;
                  }
                  if (RightStickY <= -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
                  {
                     RightStickYNormalized = -1.f * (float)RightStickY / (float)INT16_MIN;
                  }
                  currentInputBuffer->ControllerInput[0].RightStick.EndY = 
                  currentInputBuffer->ControllerInput[0].RightStick.StartY = 
                  currentInputBuffer->ControllerInput[0].RightStick.MinY = 
                  currentInputBuffer->ControllerInput[0].RightStick.MaxY = RightStickYNormalized;

                  float LeftTriggerNormalized = 0;
                  float RightTriggerNormalized = 0;

                  if (LeftTrigger >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
                  {
                     LeftTriggerNormalized = (float)LeftTrigger / 255.f;
                  }

                  if (RightTrigger >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
                  {
                     RightTriggerNormalized = (float)RightTrigger / 255.f;
                  }

                  currentInputBuffer->ControllerInput[controllerIndex].RightTrigger = RightTriggerNormalized;
                  currentInputBuffer->ControllerInput[controllerIndex].LeftTrigger = LeftTriggerNormalized;
               }
            }

            bool soundWasValid = false;
            //Note:Direct sound output test
            DWORD playCursor = 0;
            DWORD writeCursor=0; 
            DWORD bytesToWrite=0; 
            DWORD byteToLock=0;
            if (SUCCEEDED(win32SoundOutput.SoundBuffer->GetCurrentPosition(&playCursor, &writeCursor)))
            {
               bytesToWrite = 0;
               byteToLock = (win32SoundOutput.RunningSampleIndex * win32SoundOutput.BytesPerSample) % win32SoundOutput.BufferSize;
               DWORD targetCursor = (playCursor + (2 * win32SoundOutput.LatencySampleCount * win32SoundOutput.BytesPerSample)) % win32SoundOutput.BufferSize;

               if (byteToLock > targetCursor)
               {
                  bytesToWrite = (win32SoundOutput.BufferSize - byteToLock) + targetCursor;
               }
               else
               {
                  bytesToWrite = targetCursor - byteToLock;
               }
               soundWasValid = true;
            }

            soundOutputBuffer.SampleCount = bytesToWrite / win32SoundOutput.BytesPerSample;

            GameUpdateAndRender(&gameMemory, (GameOffscreenBuffer *)(&OffscreenBuffer), &soundOutputBuffer, currentInputBuffer, &gameClocks);

            //swap buffers
            GameInputBuffer *tempInputBuffer = currentInputBuffer;
            currentInputBuffer = lastInputBuffer;
            lastInputBuffer = tempInputBuffer;

            if (soundWasValid)
            {
               Win32FillSoundBuffer(&win32SoundOutput, byteToLock, bytesToWrite, &soundOutputBuffer);
            }

            HDC deviceContext = GetDC(window);

            Win32WindowDimensions windowDimensions = GetWin32WindowDimensions(window);
            Win32DisplayBufferInWindow(deviceContext, windowDimensions.Width, windowDimensions.Height, &OffscreenBuffer, 0, 0, windowDimensions.Width, windowDimensions.Height);
            ReleaseDC(window, deviceContext);

            LARGE_INTEGER endCounter;
            QueryPerformanceCounter(&endCounter);
            uint64_t endCycleCount = __rdtsc();
            local_persist int printy = 0;
            #if 0
            if (0 && (printy++ % 60) == 0)
            {
               int64_t counterElapsed = endCounter.QuadPart - lastCounter.QuadPart;
               uint64_t cyclesElapsed = endCycleCount - lastCycleCount;
               char counterReport[512];
               float msElapsed = (1000.f * (float)counterElapsed) / (float)performanceCounterFrequency;
               int64_t fps = performanceCounterFrequency / counterElapsed;
               float mcpf = (float)cyclesElapsed / 1000.f / 1000.f;
               snprintf(counterReport, sizeof(counterReport), "Frame Information: %0.2fmc/f   %0.2fms/f   %lldfps\n", mcpf, msElapsed, fps);
               OutputDebugStringA(counterReport);
            }
            #endif
            lastCycleCount = endCycleCount;
            lastCounter.QuadPart = endCounter.QuadPart;
         }
      }
   }

   return 0; //useless
}

