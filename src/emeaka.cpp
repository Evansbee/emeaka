#include "emeaka.h"
#include "emeaka_intrinsics.h"
#include <cstdio>
#include "emeaka_font.cpp"

void ClearBitmap(GameOffscreenBuffer *offscreenBuffer, float r, float g, float b)
{
   uint8_t _r = (uint8_t)Round(255.f * r);
   uint8_t _g = (uint8_t)Round(255.f * g);
   uint8_t _b = (uint8_t)Round(255.f * b);
   uint32_t color = RGB_TO_UINT32(_r, _g, _b);
   for (int y = 0; y < offscreenBuffer->Height; ++y)
   {
      for (int x = 0; x < offscreenBuffer->Width; ++x)
      {
         ((uint32_t *)(offscreenBuffer->Memory))[(y * offscreenBuffer->Width + x)] = color;
      }
   }
}

void DrawPixel(GameOffscreenBuffer *offscreenBuffer, vec2i p, float r, float g, float b, float a = 1.0f)
{
   uint8_t _r = (uint8_t)Round(255.f * r);
   uint8_t _g = (uint8_t)Round(255.f * g);
   uint8_t _b = (uint8_t)Round(255.f * b);
   uint32_t color = RGB_TO_UINT32(_r, _g, _b);
   if (p.x >= 0 && p.x < offscreenBuffer->Width && p.y >= 0 && p.y < offscreenBuffer->Height)
   {
      ((uint32_t *)(offscreenBuffer->Memory))[(p.y * offscreenBuffer->Width) + p.x] = color;
   }
}

extern "C" void DrawLine(GameOffscreenBuffer *offscreenBuffer, vec2i p0, vec2i p1, float r, float g, float b)
{

}

extern "C" void DrawChar(GameOffscreenBuffer *offscreenBuffer, vec2i p, char c, float r, float g, float b, bool shadow)
{
   //wipe it
   if (p.x < -(int64_t)FixedFontWidth || p.x > offscreenBuffer->Width || p.y < -(int64_t)FixedFontHeight || p.y > offscreenBuffer->Height)
   {
      return;
   }

   for (int plot_y = 0; plot_y < FixedFontHeight; ++plot_y)
   {
      for (int plot_x = 0; plot_x < FixedFontWidth; ++plot_x)
      {
         if ((FixedFont[c][plot_y] & (1 << (8 - plot_x))) > 0)
         {
            DrawPixel(offscreenBuffer, vec2i(p.x + plot_x, p.y + plot_y), r, g, b);
            if (shadow)
            {
               DrawPixel(offscreenBuffer, vec2i(p.x + plot_x + 1, p.y + plot_y + 1), 0.f, 0.f, 0.f);
            }
         }
      }
   }
}

extern "C" void DrawText(GameOffscreenBuffer *offscreenBuffer, vec2i p, char *text, float r, float g, float b, bool shadow)
{
   vec2i current = p;
   for (char *c = text; *c != '\0'; ++c)
   {
      if (*c == '\n')
      {
         current.y += FixedFontYAdvance;
         current.x = p.x;
      }
      else
      {
         DrawChar(offscreenBuffer, current, *c, r, g, b, shadow);
         current.x += FixedFontXAdvance;
      }
   }
}

extern "C" void DrawTriangle(GameOffscreenBuffer *offscreenBuffer, vec2i p0, vec2i p1, vec2i p2, float r, float g, float b)
{


}
extern "C" void StrokeTriangle(GameOffscreenBuffer *offscreenBuffer, vec2i p0, vec2i p1, vec2i p2, float r, float g, float b)
{
   DrawLine(offscreenBuffer,p0,p1,r,g,b);
   DrawLine(offscreenBuffer,p1,p2,r,g,b);
   DrawLine(offscreenBuffer,p2,p0,r,g,b);
}
extern "C" void DrawRect(GameOffscreenBuffer *offscreenBuffer, vec2i p0, vec2i p1, float r, float g, float b)
{
   vec2i p_ur(p1.x, p0.y);
   vec2i p_ll(p0.x, p1.y);

   DrawTriangle(offscreenBuffer,p0,p_ur,p1,r,g,b);
   DrawTriangle(offscreenBuffer,p1,p_ll,p0,r,g,b);

   return;
}
extern "C" void StrokeRect(GameOffscreenBuffer *offscreenBuffer, vec2i p0, vec2i p1, float r, float g, float b)
{
   vec2i p_ur(p1.x, p0.y);
   vec2i p_ll(p0.x, p1.y);
   DrawLine(offscreenBuffer,p0,p_ur,r,g,b);
   DrawLine(offscreenBuffer,p_ur,p1,r,g,b);
   DrawLine(offscreenBuffer,p1,p_ll,r,g,b);
   DrawLine(offscreenBuffer,p_ll,p0,r,g,b);
}



//MEMORY
internal void InitializeMemoryArena(MemoryArena *arena, void *start, size_t size)
{
   arena->Start = start;
   arena->Size = size;
   arena->Used = 0;
}

void *PushStruct(MemoryArena *arena, size_t size)
{
   Assert(size < (arena->Size - arena->Used), "Memory Overload!");
   void *result = (void *)((size_t)arena->Start + arena->Used); 
   arena->Used += size;
   return result;
}


//bool true if position changed.
bool OffsetAndNormalizePosition(GameWorld *world, Position *pos, vec2f deltaP)
{
   pos->TileOffset.x += deltaP.x;
   pos->TileOffset.y += deltaP.y;

   int32_t tileChangeX = (int32_t)Round(pos->TileOffset.x);
   int32_t tileChangeY = (int32_t)Round(pos->TileOffset.y);

   pos->TileOffset.x -= (float)tileChangeX;
   pos->Tile.x += tileChangeX;

   pos->TileOffset.y -= (float)tileChangeY;
   pos->Tile.y += tileChangeY;

   pos->Tile.x = pos->Tile.x % (world->TileMap->ChunksX * world->TileMap->ChunkSize);
   pos->Tile.y = pos->Tile.y % (world->TileMap->ChunksY * world->TileMap->ChunkSize);

   return (tileChangeX != 0 || tileChangeY != 0);
}
internal void GenerateChunk(GameState *gameState, uint64_t chunkX, uint64_t chunkY)
{
   size_t chunkidx = chunkY * gameState->World->TileMap->ChunksX + chunkX;
   size_t tiles_per_chunk = gameState->World->TileMap->ChunkSize * gameState->World->TileMap->ChunkSize;
   gameState->World->TileMap->Chunks[chunkidx].Tiles = (uint64_t *)PushStruct(&gameState->WorldMemoryArena,tiles_per_chunk * sizeof(uint64_t));

   for(uint64_t tiley = 0; tiley < gameState->World->TileMap->ChunkSize; ++tiley)
   {
      for(uint64_t tilex = 0; tilex < gameState->World->TileMap->ChunkSize; ++tilex)
      {
         gameState->World->TileMap->Chunks[chunkidx].Tiles[tiley * gameState->World->TileMap->ChunkSize + tilex] = ((rand()%100)<75);
      }
   }
}
Chunk *GetChunkForPosition(GameState *gameState, GameWorld *world, vec2u p)
{
   uint64_t chunkx = (p.x >> world->TileMap->ChunkShift);
   uint64_t chunky = (p.y >> world->TileMap->ChunkShift);

   chunkx = (chunkx % world->TileMap->ChunksX);
   chunky = (chunky % world->TileMap->ChunksY);

   if(world->TileMap->Chunks[chunky * world->TileMap->ChunksX + chunkx].Tiles == NULL)
   {
      GenerateChunk(gameState, chunkx, chunky);
   }
   return &world->TileMap->Chunks[chunky * world->TileMap->ChunksX + chunkx];
}

uint64_t GetTileValueForPosition(GameState *gameState, GameWorld *world, vec2u p)
{
   Chunk *chunk = GetChunkForPosition(gameState, world, p);
   if(chunk != NULL and chunk->Tiles != NULL)
   {
      uint64_t relx = p.x & world->TileMap->ChunkMask;
      uint64_t rely = p.y & world->TileMap->ChunkMask;
      return chunk->Tiles[rely * world->TileMap->ChunkSize + relx];
   }
   return uint64_t(-1);
}





void InitializeWorld(GameState *gameState)
{
   gameState->World = (GameWorld *)PushStruct(&gameState->WorldMemoryArena,sizeof(GameWorld));
   gameState->World->TileMap = (WorldTileMap *)PushStruct(&gameState->WorldMemoryArena, sizeof(WorldTileMap));

   gameState->World->TileMap->ChunksX = 64;
   gameState->World->TileMap->ChunksY = 64;
   gameState->World->TileMap->ChunkSize = 32;
   gameState->World->TileMap->ChunkMask = 0x1f;
   gameState->World->TileMap->ChunkShift = 5;


   gameState->World->TileSideInMeters = 2.f;
   gameState->World->TileSideInPixels = 32.f;
   gameState->World->PixelsPerMeter = gameState->World->TileSideInPixels / gameState->World->TileSideInMeters;
   
   gameState->World->TileMap->Chunks = (Chunk *)PushStruct(&gameState->WorldMemoryArena, 
                                                  gameState->World->TileMap->ChunksX * gameState->World->TileMap->ChunksY * sizeof(Chunk));
   for(uint64_t y = 0; y < gameState->World->TileMap->ChunksY; ++y)
   {
      for(uint64_t x = 0; x < gameState->World->TileMap->ChunksX; ++x)
      {
         gameState->World->TileMap->Chunks[y*gameState->World->TileMap->ChunksX + x].Tiles = NULL;
      }
   }
   
}

void DrawCharacter(GameOffscreenBuffer *offscreenBuffer, float x, float y, float width, float height, float r, float g, float b, float a)
{
   DrawRect(offscreenBuffer,vec2i(x-width/2.f, y-height), vec2i(x+width/2.f, y),r,g,b);

}

extern "C" void GameUpdateAndRender(ThreadContext *threadContext, GameMemory *gameMemory, GameOffscreenBuffer *offscreenBuffer, GameInputBuffer *inputBuffer, GameClocks *gameClocks)
{
   Assert(sizeof(GameState) <= gameMemory->PermanentStorageSize, "Permanent Storage Inadequate");

   GameState *gameState = (GameState *)gameMemory->PermanentStorage;
   if (!gameMemory->IsInitialized)
   {
      printf("Initializing In Game Memory...\n");
      gameState->PlayerPos.Tile.x = 5;
      gameState->PlayerPos.Tile.y = 30;
      gameState->PlayerPos.Tile.z = 0;
      gameState->PlayerPos.TileOffset.x = 0.0f;
      gameState->PlayerPos.TileOffset.y = 0.0f;
      gameState->ToneHz = 500.f;
      gameState->tSin = 0.f;
      InitializeMemoryArena(&gameState->WorldMemoryArena, (void *)((size_t)gameMemory->PermanentStorage + sizeof(GameState)), gameMemory->PermanentStorageSize - sizeof(GameState));
      InitializeWorld(gameState);
      gameMemory->IsInitialized = true;
      printf("World Memory Location: 0x%p\n",gameState->World);
      printf("World->TileMap Location: 0x%p\n",gameState->World->TileMap);
   }
   gameState->ToneHz = 500.f + (-250.f * inputBuffer->ControllerInput[0].RightStick.AverageY);

   ClearBitmap(offscreenBuffer, 0.1f, 0.2f, 0.1f);
   DrawPixel(offscreenBuffer, vec2i(10,10),1.f,1.f,1.f);
   return;


}


extern "C" void GameGetSoundSamples(ThreadContext *threadContext, GameMemory *gameMemory, GameSoundBuffer *soundBuffer)
{
  GameState *gameState = (GameState *)gameMemory->PermanentStorage;
}