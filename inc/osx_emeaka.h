#pragma once

#include <SDL.h>
#include "emeaka.h"

struct OSXOffscreenBuffer
{
  void *Memory;
  int Height;
  int Width;
  int BytesPerPixel;
  int Pitch;
  SDL_Texture *Texture;
};
struct OSXAudioBuffer
{
  SDL_AudioSpec AudioSpec;
  SDL_AudioDeviceID AudioDevice;
  void *RingBuffer;
  size_t WriteCursor;
  size_t PlayCursor;
  size_t RingBufferLength;
  size_t ApplicationNextWritePtr;
};

struct OSXDynamicGame
{
    GameAPI APIFunctions;
    time_t FileUpdateTime;
    void *DynamicLibrary;
    char DynamicLibPath[1024];
    bool IsValid;
};
