#pragma once

#include <cstdint>
#include <Windows.h>
#include <dsound.h>

struct Win32OffscreenBuffer
{
   void *Memory;
   int Height;
   int Width;
   int BytesPerPixel;
   int Pitch;
   BITMAPINFO Info;
};

struct Win32SoundOuput
{
   int SamplesPerSecond;
   uint32_t RunningSampleIndex;
   int BytesPerSample;
   int Channels;
   int BufferSize;
   int LatencySampleCount;
   int SafetyBytes;
   LPDIRECTSOUNDBUFFER SoundBuffer;
};

struct Win32WindowDimensions
{
   int Width;
   int Height;
};


struct Win32DebugTimeMarker
{
   DWORD OutputPlayCursor;
   DWORD OutputWriteCursor;
   DWORD OutputLocation;
   DWORD OutputByteCount;
   DWORD ExpectedFlipPlayCursor;
   DWORD FlipPlayCursor;
   DWORD FlipWriteCursor;
};