#pragma once
#include <cinttypes>
#include <cstdlib>

#include "emeaka_platform.h"
//helper defines

#define internal static
#define local_persist static
#define global_variable static

//Default values

#define PI32 3.14159265359f

//Macros
#define RGB_TO_UINT32(r, g, b) (((uint8_t)(r) << 16) | ((uint8_t)(g) << 8) | (uint8_t)(b))
#define Y_TO_UINT32(y) (((uint8_t)(y) << 16) | ((uint8_t)(y) << 8) | (uint8_t)(y))
#define ArrayCount(array) (sizeof(array)/sizeof((array)[0]))

#define KiloBytes(x) ((x)*(size_t)1024)
#define MegaBytes(x) (KiloBytes(x) * (size_t)1024)
#define GigaBytes(x) (MegaBytes(x) * (size_t)1024)
#define TeraBytes(x) (GigaBytes(x) * (size_t)1024)
#define FLOAT_EPSILON 1.19209290E-07F

#if EMEAKA_SLOW
#define Assert(expression, msg) if(!(expression)) {*(int *)0 = 0;}
#else
#define Assert(expression, msg)
#endif


struct Position
{
   //let's just store these as raw tiles?
   int64_t TileX;
   int64_t TileY;
   int64_t TileZ;
   float TileOffsetX; //relative to the tile
   float TileOffsetY; //relative to the tile
};

struct TileMap
{
   uint32_t *Map;
   float UpperLeftX;
   float UpperLeftY;
};

//   ^ +y
//   |
//   |
//   | (0,0)
//---+----------> +x

struct Chunk
{
  static const size_t ChunkSize = 128;
  static const size_t ChunkMask = 0x7F;
  static const size_t ChunkShift= 7;
  int64_t ChunkBaseX;
  int64_t ChunkBaseY; //lower left tile number...
  uint32_t *Tiles;
};

struct World
{
   float TileSideInMeters;
   float TileSideInPixels;
   float PixelsPerMeter;
   size_t NumTileMaps;
   TileMap *TileMaps;
   Chunk *Chunks;
   uint32_t TileMapWidth;
   uint32_t TileMapHeight;
};


struct GameState
{
  Position PlayerPos;
  float PlayerX;
  float PlayerY;
};


struct GameControl 
{
  bool Running;
};



