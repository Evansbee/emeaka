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
   uint64_t TileX;
   uint64_t TileY;
   uint64_t TileZ;
   float TileOffsetX; //relative to the tile
   float TileOffsetY; //relative to the tile, note that 0.0 is the MIDDLE of the tile, -0.5 - 0.5 are the distances.
   
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

struct MemoryArena
{
  void *Start;
  size_t Size;
  size_t Used;
};

struct GameState
{
  Position PlayerPos;
  MemoryArena WorldMemoryArena;
  GameWorld *World;
  float ToneHz;
  float tSin;
};


struct GameControl 
{
  bool Running;
};



