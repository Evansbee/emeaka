#include "emeaka.h"

#include <math.h>

#define RGB_TO_UINT32(r, g, b) (((uint8_t)(r) << 16) | ((uint8_t)(g) << 8) | (uint8_t)(b))

internal void TempRenderWeirdGradient(GameOffscreenBuffer *offscreenBuffer, int xOffset, int yOffset)
{
  uint8_t *row = (uint8_t *)offscreenBuffer->Memory;
  for (int y = 0; y < offscreenBuffer->Height; ++y)
  {
    uint32_t *pixel = (uint32_t *)row;
    for (int x = 0; x < offscreenBuffer->Width; ++x)
    {
      *pixel++ = RGB_TO_UINT32(0, y + yOffset, x + xOffset); //cute
    }
    row += offscreenBuffer->Pitch;
  }
}

//let's agree on

internal void TempComposeSineWave(GameSoundBuffer *soundBuffer, int toneHz)
{
  local_persist float tSine = 0.0f;
  int16_t toneVolume = 1500;
  int WavePeriod = soundBuffer->SamplesPerSecond / toneHz;
  int16_t *sample_out = soundBuffer->SampleBuffer;
  //DWORD soundBuffer1SampleCount = soundBuffer1Size / soundOutput->BytesPerSample;
  for (int sampleIndex = 0; sampleIndex < soundBuffer->SampleCount; ++sampleIndex)
  {
    float sinValue = sinf(tSine);
    int16_t sampleValue = (int16_t)(sinValue * (float)toneVolume);
    *sample_out++ = sampleValue;
    *sample_out++ = sampleValue; //sampleValue;

    tSine += 2.0f * PI32 / (float)WavePeriod;
  }
}

internal void GameUpdateAndRender(GameOffscreenBuffer *offscreenBuffer, GameSoundBuffer *soundBuffer, GameInputBuffer *inputBuffer)
{
  local_persist int xOffset = 0;
  local_persist int yOffset = 0;
  local_persist int toneHz = 261;
  if (inputBuffer->ControllerInput[0].Connected)
  {
    if (inputBuffer->ControllerInput[0].AButton.IsDown)
    {
      toneHz = 466;
    }
  else if (inputBuffer->ControllerInput[0].BButton.IsDown)
  {
toneHz = 493;
  }
  else if (inputBuffer->ControllerInput[0].XButton.IsDown)
  {
    toneHz = 523;
  }
  else if (inputBuffer->ControllerInput[0].YButton.IsDown)
  {toneHz = 554;
    
  }
  else
  {
    toneHz = 60;
  }
    yOffset += 5.f * inputBuffer->ControllerInput[0].LeftStick.EndY;
    //toneHz = int(512.f + inputBuffer->ControllerInput[0].LeftStick.EndY * 256.f);
  }


  TempRenderWeirdGradient(offscreenBuffer, xOffset, yOffset);
  TempComposeSineWave(soundBuffer, toneHz);

//Todo: make this more complicated
//GameOutputSound(int sampleCount, soundBuffer)
}
