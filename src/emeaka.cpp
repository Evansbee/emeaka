#include "emeaka.h"
#include "emeaka_intrinsics.h"
#include <cstdio>

#include "emeaka_memory.cpp"
#include "emeaka_drawing.cpp"

//stb stuff
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_ONLY_BMP
#include "stb_image.h"




void TestMemoryAllocation(void * start, size_t maxsize)
{
   MemoryBank testBank;
   InitialzeMemoryBank(&testBank, start, 1000);
   DumpMemoryBank(&testBank);
   void *test1 = AllocateMemory(&testBank,100);
   void *test2 = AllocateMemory(&testBank,100);
   void *test3 = AllocateMemory(&testBank,100);
   void *test4 = AllocateMemory(&testBank,100);
   void *test5 = AllocateMemory(&testBank,100);
   void *test6 = AllocateMemory(&testBank,100);
   void *test7 = AllocateMemory(&testBank,100);

   FreeMemory(test1);

   FreeMemory(test3);

   FreeMemory(test2);
 
   void *test10 = AllocateMemory(&testBank, 55);
   DumpMemoryBank(&testBank);
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
   gameState->World->TileMap->Chunks[chunkidx].Tiles = (uint64_t *)AllocateMemory(&gameState->WorldMemoryBank,tiles_per_chunk * sizeof(uint64_t));

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
   gameState->World = (GameWorld *)AllocateMemory(&gameState->WorldMemoryBank,sizeof(GameWorld));
   gameState->World->TileMap = (WorldTileMap *)AllocateMemory(&gameState->WorldMemoryBank, sizeof(WorldTileMap));

   gameState->World->TileMap->ChunksX = 64;
   gameState->World->TileMap->ChunksY = 64;
   gameState->World->TileMap->ChunkSize = 32;
   gameState->World->TileMap->ChunkMask = 0x1f;
   gameState->World->TileMap->ChunkShift = 5;


   gameState->World->TileSideInMeters = 2.f;
   gameState->World->TileSideInPixels = 32.f;
   gameState->World->PixelsPerMeter = gameState->World->TileSideInPixels / gameState->World->TileSideInMeters;
   
   gameState->World->TileMap->Chunks = (Chunk *)AllocateMemory(&gameState->WorldMemoryBank, 
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
void DrawBitmap(GameOffscreenBuffer *offscreenBuffer, vec2i p, BitmapData* bitmap)
{
   for(int64_t y = 0; y < bitmap->height; ++y)
   {
      for(int64_t x = 0; x < bitmap->width; ++x)
      {
         uint32_t rgba = ((uint32_t *)bitmap->data)[(y * bitmap->width) + x];
         uint8_t 
         a = ((rgba >> 24) & 0xFF),
         b = ((rgba >> 16) & 0xFF),
         g = ((rgba >> 8) & 0xFF),
         r = ((rgba >> 0) & 0xFF);


         float 
         fa = (float)a/255.f,
         fr = (float)r/255.f,
         fg = (float)g/255.f,
         fb = (float)b/255.f;

         DrawPixel(offscreenBuffer, vec2i(x + p.x, y + p.y),fr,fg,fb,fa);        
      }
   }
}

extern "C" void GameUpdateAndRender(ThreadContext *threadContext, GameMemory *gameMemory, GameOffscreenBuffer *offscreenBuffer, GameInputBuffer *inputBuffer, GameClocks *gameClocks)
{
   Assert(sizeof(GameState) <= gameMemory->PermanentStorageSize, "Permanent Storage Inadequate");

   GameState *gameState = (GameState *)gameMemory->PermanentStorage;
   if (!gameMemory->IsInitialized)
   {
      gameState->PlayerPos.Tile.x = 5;
      gameState->PlayerPos.Tile.y = 30;
      gameState->PlayerPos.Tile.z = 0;
      gameState->PlayerPos.TileOffset.x = 0.0f;
      gameState->PlayerPos.TileOffset.y = 0.0f;
      gameState->ToneHz = 500.f;
      gameState->tSin = 0.f;
      gameState->leftTime = 0.f;
      gameState->rightTime = 0.f;
      gameState->centerTime = 0.f;

      InitialzeMemoryBank(&gameState->WorldMemoryBank, (void *)((size_t)gameMemory->PermanentStorage + sizeof(GameState)), gameMemory->PermanentStorageSize - sizeof(GameState));
      InitializeWorld(gameState);
      gameMemory->IsInitialized = true;
      DebugFileResult bitmapFile = gameMemory->PlatformFunctions.PlatformReadEntireFile(threadContext, "bmptest.bmp");

      if(bitmapFile.FileSize > 0)
      {
         int comp = 4;
         gameState->testBitmap.data = stbi_load_from_memory((const unsigned char *)bitmapFile.Contents, bitmapFile.FileSize,&gameState->testBitmap.width,&gameState->testBitmap.height,&comp,4);
      }
   }
   if(inputBuffer->KeyboardInput.Key[KeyCode::R].IsDown)
   {
      gameMemory->IsInitialized = false;
   }
   gameState->ToneHz = 500.f + (-250.f * inputBuffer->ControllerInput[0].RightStick.AverageY);

   int midy = offscreenBuffer->Height / 2;
   int midx = offscreenBuffer->Width / 2;
   int offsetx = offscreenBuffer->Width / 8;
   int height = offscreenBuffer->Height / 4;
   

   float leftHz=0.125f/5.f;
   float rightHz=0.25f/5.f;
   float centerHz=0.5f/5.f;

   float leftTimeUpdate = gameClocks->UpdateDT * leftHz * 2 * PI32;
   float rightTimeUpdate = gameClocks->UpdateDT * rightHz * 2 * PI32;
   float centerTimeUpdate = gameClocks->UpdateDT * centerHz * 2 * PI32;
   
   
   vec2i left(midx  - offsetx+ height*sinf(gameState->leftTime ),midy + height*cosf(gameState->leftTime ));
   gameState->leftTime  += leftTimeUpdate;
   gameState->leftTime  = fmodf(gameState->leftTime , 2 * PI32);

   vec2i right(midx + offsetx+ height*sinf(gameState->rightTime ),midy + height*cosf(gameState->rightTime ));

   gameState->rightTime  += rightTimeUpdate;
   gameState->rightTime  = fmodf(gameState->rightTime , 2 * PI32);

   
   vec2i center(midx + height*sinf(gameState->centerTime ),midy + height*cosf(gameState->centerTime ));

   gameState->centerTime  += centerTimeUpdate;
   gameState->centerTime  = fmodf(gameState->centerTime , 2 * PI32);


   ClearBitmap(offscreenBuffer, 0.1f, 0.1f, 0.1f);
   DrawBitmap(offscreenBuffer, vec2i(350,250),&gameState->testBitmap);
   
   //DrawCircle(offscreenBuffer,vec2i(350,250),150,1,0.2f,0.3f,.5);
   //StrokeCircle(offscreenBuffer,vec2i(350,250),150,1.0f,1.0f,1.0f);
   
   DrawCircle(offscreenBuffer,left,150,1.0f,0,0,.75f);
   StrokeCircle(offscreenBuffer,left,150,1.f,1.f,1.f);
   
   DrawCircle(offscreenBuffer,center,150,0.0f,1,0,.75f);
   StrokeCircle(offscreenBuffer,center,150,1.f,1.f,1.f);

   DrawCircle(offscreenBuffer,right,150,0.0f,0,1,.75f);
   StrokeCircle(offscreenBuffer,right,150,1.f,1.f,1.f);

   //DrawTriangle(offscreenBuffer,left,right,center,0,0,1.f);
   //StrokeTriangle(offscreenBuffer,left,right,center,1.f,1.f,1.f);
   return;


}


extern "C" void GameGetSoundSamples(ThreadContext *threadContext, GameMemory *gameMemory, GameSoundBuffer *soundBuffer)
{
  GameState *gameState = (GameState *)gameMemory->PermanentStorage;
}