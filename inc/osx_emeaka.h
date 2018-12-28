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
  bool IsPlaying;
  void *RingBuffer;
  size_t WriteCursor; //in bytes
  size_t PlayCursor; //in bytes
  size_t RingBufferLength; //in bytes
  size_t ApplicationNextWritePtr; //in bytes
};

struct OSXDynamicGame
{
    GameAPI APIFunctions;
    time_t FileUpdateTime;
    void *DynamicLibrary;
    char DynamicLibPath[1024];
    bool IsValid;
};
