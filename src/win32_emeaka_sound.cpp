#include <windows.h>
#include "emeaka.h"
#include "win32_emeaka.h"

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
                  //LOG("Failed to allocate primary buffer for DirectSound");
               }
            }
         }
         else
         {
            //LOG("Failed to set coooperative level for directsound");
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
            //LOG("Failed to create secondary buffer");
         }
      }
   }
   else
   {
      //LOG("Failed to load DirectSound");
   }
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