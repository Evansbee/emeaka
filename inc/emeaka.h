#pragma once
#include <cinttypes>
#include <cstdlib>

#include "emeaka_platform.h"
#include "emeaka_vector.h"
//helper defines

#define internal static
#define local_persist static
#define global_variable static

//Default values

#define PI32 3.14159265359f

//Macros
#define RGB_TO_UINT32(r, g, b) (((uint8_t)(r) << 16) | ((uint8_t)(g) << 8) | (uint8_t)(b))
#define RGBA_TO_UINT32(a, r, g, b) (((uint8_t)(a) <<24 ) | ((uint8_t)(r) << 16) | ((uint8_t)(g) << 8) | (uint8_t)(b))

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

struct MemoryBank;

struct MemoryAllocationHeader
{
  size_t Size;
  bool InUse;
  MemoryAllocationHeader *Previous; //points to the header
  MemoryAllocationHeader *Next; //points to the header
  MemoryBank *Bank; 
};

struct MemoryBank
{
  void *Start; //start of the bank
  size_t Size;
  size_t Entries;
  size_t Used; //will inlude the header space
  MemoryAllocationHeader *NextFree; //Points to the first byte of header of Freespace at end
};

struct MemoryArena
{
  void *Start;
  size_t Size;
  size_t Used;
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
};