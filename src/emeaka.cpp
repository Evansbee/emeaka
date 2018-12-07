#include "emeaka.h"

#include <math.h>



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

internal void TempComposeSineWave(GameSoundBuffer *soundBuffer, float toneHz)
{
  local_persist float tSine = 0.0f;
  int16_t toneVolume = 1500;
  float wavePeriod = ((float)soundBuffer->SamplesPerSecond / (float)toneHz);
  int16_t *sample_out = soundBuffer->SampleBuffer;
  //DWORD soundBuffer1SampleCount = soundBuffer1Size / soundOutput->BytesPerSample;
  for (int sampleIndex = 0; sampleIndex < soundBuffer->SampleCount; ++sampleIndex)
  {
    float sinValue = sinf(tSine);
    int16_t sampleValue = (int16_t)(sinValue * (float)toneVolume);
    *sample_out++ = sampleValue;
    *sample_out++ = sampleValue; //sampleValue;

    tSine += 2.0f * PI32 / wavePeriod;
    tSine = fmodf(tSine, 2.0f * PI32);
  }
}

internal void GameUpdateAndRender(GameMemory *gameMemory, GameOffscreenBuffer *offscreenBuffer, GameInputBuffer *inputBuffer, GameClocks *gameClocks)
{
  Assert(sizeof(GameState) <= gameMemory->PermanentStorageSize, "Permanent Storage Inadequate");

  GameState *gameState = (GameState *)gameMemory->PermanentStorage;
  if (!gameMemory->IsInitialized)
  {
    char *filename = "test.png";
    DebugFileResult fileReadResult = PlatformReadEntireFile(filename);
    if(fileReadResult.Contents)
    {
      PlatformWriteEntireFile("test2.png", fileReadResult.FileSize, fileReadResult.Contents);
      PlatformFreeFileMemory(fileReadResult.Contents);
    }
    //why is this a bad idea?
    gameMemory->IsInitialized = true;
  }

  gameState->BlueOffset += int(50.f * inputBuffer->ControllerInput[0].LeftStick.AverageX);
  gameState->GreenOffset -= int(50.f * inputBuffer->ControllerInput[0].LeftStick.AverageY);
  gameState->ToneHz = 1024.f + inputBuffer->ControllerInput[0].RightStick.AverageY * 768.f;

  if(inputBuffer->KeyboardInput.Key['A'].IsDown)
  {
    inputBuffer->ControllerInput[0].LeftVibration = 65535;
    gameState->ToneHz = 100;
  }

  TempRenderWeirdGradient(offscreenBuffer, gameState->BlueOffset, gameState->GreenOffset);
  
  //Todo: make this more complicated
  
//Todo: make this more complicated
//GameOutputSound(int sampleCount, soundBuffer)
}


internal void
GameGetSoundSamples(GameMemory *gameMemory,  GameSoundBuffer *soundBuffer)
{
  GameState *gameState = (GameState *)gameMemory->PermanentStorage;
  TempComposeSineWave(soundBuffer, gameState->ToneHz);
}