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
#define Assert(expression, msg) if(!(expression)) {*(volatile int *)0 = 0;}
#else
#define Assert(expression, msg)
#endif


template <typename T>
struct tV2
{
  tV2():tV2(T(),T()){}
  tV2(T _x, T _y) : X(_x), Y(_y){}
  T X;
  T Y;
};

typedef tV2<float> V2;
typedef tV2<int64_t> iV2;
typedef tV2<uint64_t> uV2;

template <typename T>
struct tV3{
  tV3(): tV3(T(), T(),T()){}
  tV3(T _x, T _y, T _z) : X(_x), Y(_y), Z(_z){}
  T X;
  T Y;
  T Z;
};

typedef tV3<float> V3;
typedef tV3<int64_t> iV3;
typedef tV3<uint64_t> uV3;


struct Position
{
   //let's just store these as raw tiles?
   uV3 Tile;
   V2 TileOffset;
  
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

#pragma packed
struct BitmapHeader
{
  

};
struct Bitmap
{
  char Signature[2];
  uint16_t Unused1;
  uint32_t FileSize;
  uint16_t Reserved1;
  uint16_t Reserved2;
  uint32_t FileOffsetToPixelArray;
  BitmapHeader Header;
  void *Data;
};
