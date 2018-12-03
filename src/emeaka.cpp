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

internal void TempComposeSineWave(GameSoundBuffer *soundBuffer)
{
  local_persist float tSine;
  int16_t toneVolume = 1000;
  int hz = 263;
  int WavePeriod = soundBuffer->SamplesPerSecond / hz;
  int16_t *sample_out = (int16_t *)soundBuffer->SampleBuffer;
  //DWORD soundBuffer1SampleCount = soundBuffer1Size / soundOutput->BytesPerSample;
  for (int sampleIndex = 0; sampleIndex < soundBuffer->SampleCount; ++sampleIndex)
  {
    float sinValue = sinf(tSine);
    int16_t sampleValue = (int16_t)(sinValue * (float)toneVolume);
    *sample_out++ = sampleValue;
    *sample_out++ = sampleValue;

    tSine += 2.0f * PI32 / (float)WavePeriod;
  }
}

internal void GameUpdateAndRender(GameOffscreenBuffer *offscreenBuffer, GameSoundBuffer *soundBuffer, int xOffset, int yOffset)
{
  TempRenderWeirdGradient(offscreenBuffer, xOffset, yOffset);
  TempComposeSineWave(soundBuffer);

  //Todo: make this more complicated
  //GameOutputSound(int sampleCount, soundBuffer)
}