#pragma once
#include <cstdint>
#include <cstdlib>

#include "emeaka_platform.h"
#include "emeaka_vector.h"
#include "emeaka_memory.h"
//helper defines

#ifndef EMEAKA_TEST //test framework uses this
#define internal static
#endif
#define local_persist static
#define global_variable static

//Default values

#define PI32 3.14159265359f


struct RGBA
{
  RGBA(): rgba(0){}
  union 
  {
    struct {
      uint8_t b;
      uint8_t g;
      uint8_t r;
      uint8_t a;
    };
    uint32_t rgba;
  };
};

#define Y_TO_UINT32(y) (((uint8_t)(y) << 16) | ((uint8_t)(y) << 8) | (uint8_t)(y))
#define ArrayCount(array) (sizeof(array)/sizeof((array)[0]))

#define KiloBytes(x) ((x)*(size_t)1024)
#define MegaBytes(x) (KiloBytes(x) * (size_t)1024)
#define GigaBytes(x) (MegaBytes(x) * (size_t)1024)
#define TeraBytes(x) (GigaBytes(x) * (size_t)1024)
#define FLOAT_EPSILON 1.19209290E-07F

#if EMEAKA_SLOW
#define Assert(expression, msg) if(!(expression)) {*(volatile int *)0 = 0;}
#else
#define Assert(expression, msg)
#endif


struct Position
{
   //let's just store these as raw tiles?
   vec3u Tile;
   vec2f TileOffset;
  
};

//   ^ +y
//   |
//   |
//   | (0,0)
//---+----------> +x



struct Chunk
{
  uint64_t *Tiles;
};

struct WorldTileMap
{
  size_t ChunkSize;
  size_t ChunkMask;
  size_t ChunkShift;
  size_t ChunksX;
  size_t ChunksY;
  size_t ChunksZ;
  Chunk *Chunks;
};

struct GameWorld
{ 
  WorldTileMap *TileMap;
   float TileSideInMeters;
   float TileSideInPixels;
   float PixelsPerMeter;
   
};



struct GameControl 
{
  bool Running;
};



struct BitmapData
{
  int32_t width;
  int32_t height;
  uint8_t* data;
};

struct GlyphInformation
{
  size_t DataBufferStartPoint;
  int Height;
  int Width;
  int YOffset;
  int XOffset;
  int XAdvance;
};

struct FontInformation
{
  int FirstGlyph;
  int LastGlyph;
  int LineStep;
  uint8_t* FontBitmapData;
  GlyphInformation* GlyphData;
};

struct LogEntry;

struct GameState
{
  Position PlayerPos;  
  MemoryBank WorldMemoryBank;
  GameWorld *World;
  float ToneHz;
  float tSin;
  float leftTime;
  float rightTime;
  float centerTime;
  LogEntry *Logger;
  BitmapData testBitmap;
  float GameTime;
};