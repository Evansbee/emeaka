#pragma once
#include <cinttypes>
//helper defines

#define internal static
#define local_persist static
#define global_variable static

//Default values

#define PI32 3.14159265359f

//types
struct GameOffscreenBuffer
{
  void *Memory;
  int Height;
  int Width;
  int BytesPerPixel;
  int Pitch;
};

struct GameSoundBuffer
{
  int16_t *SampleBuffer;
  size_t SampleCount;
  int32_t SamplesPerSecond;
};

struct GameInputBuffer
{

};
//Services platform layer provides to game
//who knows..

//Game provides to platform layer
//input, bitmap to output and sound output, timing
internal void
GameUpdateAndRender(GameOffscreenBuffer *offscreenBuffer, GameSoundBuffer *soundBuffer, GameInputBuffer &inputBuffer);
