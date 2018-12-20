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

struct OSXDynamicGame
{
    GameAPI APIFunctions;
    time_t FileUpdateTime;
    void *DynamicLibrary;
    char DynamicLibPath[1024];
    bool IsValid;
};
