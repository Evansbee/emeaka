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
  float leftTime;
  float rightTime;
  float centerTime;
};


struct GameControl 
{
  bool Running;
};

struct BitmapHeader
{
  uint32_t HeaderSize;
  int32_t ImageWidth;
  int32_t ImageHeight;
  uint16_t Planes;
  uint16_t BitsPerPixel;
  uint32_t Compression;
  uint32_t XPixelsPerMeter;
  uint32_t YPixelsPerMeter;
  uint32_t ColorsInColorTable;
  uint32_t ImportantColorCount;
  uint32_t RedChannelBitmask;
  uint32_t GreenChannelBitmask;
  uint32_t BlueChannelBitmask;
  uint32_t AlphaChannelBitmask;
  
}__attribute__((packed));
struct Bitmap
{
  char Signature[2];
  uint32_t FileSize;
  uint16_t Reserved1;
  uint16_t Reserved2;
  uint32_t FileOffsetToPixelArray;
  BitmapHeader Header;
  void *Data;
}__attribute__((packed));
