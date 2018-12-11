#pragma once

#include <cstdint>
#include <Windows.h>
#include <dsound.h>

#include "emeaka.h"

struct Win32OffscreenBuffer
{
   void *Memory;
   int Height;
   int Width;
   int BytesPerPixel;
   int Pitch;
   BITMAPINFO Info;
};

struct Win32DrawingFunctions
{
   GameClearBitmapType *GameClearBitmap;
   GameDrawPixelType *GameDrawPixel;
   GameDrawRectType *GameDrawRect;
   GameDrawCircleType *DrawCircle;
   GameDrawLineType *GameDrawLine;
   GameDrawCharType *GameDrawChar;
   GameDrawTextType *GameDrawText;
};

struct Win32SoundOuput
{
   int SamplesPerSecond;
   uint32_t RunningSampleIndex;
   int BytesPerSample;
   int Channels;
   int BufferSize;
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

enum GameRecordingState : int
{
   InputNormal = 0,
   InputRecording,
   InputPlaying,
};

enum KeyboardSequenceState : int
{
   SequenceInactive,
   SequenceAwatingRecordingNumber,
   SequenceAwatingPlaybackNumber
};

struct Win32RecordingInformation
{
   HANDLE FileHandle;
   HANDLE MemoryMap;
   void *StoredGameMemory;
   char ReplayFileName[MAX_PATH];
};

#define MAX_REPLAY_BUFFERS (1)
struct Win32State
{
   Win32RecordingInformation ReplayBuffers[MAX_REPLAY_BUFFERS];
   GameRecordingState RecordingState[MAX_REPLAY_BUFFERS];
   void *GameMemory;
   size_t GameMemorySize;
   KeyboardSequenceState KeySequenceState;
   //HANDLE RecordingFileHandle;
   //HANDLE ReplayFileHandle;
};

