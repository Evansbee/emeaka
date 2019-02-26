#include "emeaka.h"
#include "emeaka_intrinsics.h"
#include "emeaka_drawing.h"


#include <cstdio>

#include "emeaka_random.cpp"
#include "emeaka_memory.cpp"
#include "emeaka_string.cpp"

#include "Fonts/DejaVuSansMono24ptFont.h"
#include "Fonts/DejaVuSansMono12ptFont.h"

//stb stuff
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_ONLY_BMP
#define STBI_MALLOC GlobalMemoryBankMalloc
#define STBI_REALLOC GlobalMemoryBankRealloc
#define STBI_FREE GlobalMemoryBankFree
#include "stb_image.h"


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

struct LogEntry
{
   char *TextString;
   RGBA color;
   bool On;
   bool Fading;
   float OnTimeLeft;
   float FadeTimeLeft;
   float FadeTime;
   LogEntry *Next;
   LogEntry *Prev;
};

//copies the string, it'll be responsible for 
void AddToLog(LogEntry** Head, MemoryBank *bank, const char* string, RGBA color, float OnTime, float FadeTime)
{
   LogEntry *newEntry;
   if(*Head == nullptr)
   {
      *Head = (LogEntry *)AllocateMemory(bank,sizeof(LogEntry));
      (*Head)->Prev = nullptr;
      (*Head)->Next = nullptr;
      newEntry = *Head;
   }
   else
   {
      LogEntry *cursor = *Head;
      while(cursor->Next != nullptr)
      {
         cursor = cursor->Next;
      }
      newEntry = (LogEntry*)AllocateMemory(bank,sizeof(LogEntry));
      cursor->Next = newEntry;
      newEntry->Prev = cursor;
      newEntry->Next = nullptr;
   }

   auto loglen = StringLen(string);
   newEntry->TextString = (char *)AllocateMemory(bank, loglen+1 + rand() %1000);
   StringCopy(string, newEntry->TextString);

   newEntry->OnTimeLeft = OnTime;
   newEntry->FadeTimeLeft = FadeTime;
   newEntry->FadeTime = FadeTime;

   newEntry->On = true;
   newEntry->Fading = false;

   newEntry->color = color;
}

void DisplayLog(LogEntry** logHead, GameOffscreenBuffer *buf, vec2i p, float dt)
{
   //if we're done with an item we have to both free the string and free the log entry
   //printf("Displaying Log - Header = %p\n",*logHead);
   LogEntry *cursor = *logHead;

   const FontInformation* font = (buf->TextureHeight > buf->WindowHeight)?(&DejaVuSansMono24ptFont):(&DejaVuSansMono12ptFont);

   while(cursor != nullptr)
   {
      float r = (float)cursor->color.r/255.f;
      float g = (float)cursor->color.g/255.f;
      float b = (float)cursor->color.b/255.f;
      float a = (float)cursor->color.a/255.f;

      if(cursor->On)
      {
         cursor->OnTimeLeft -= dt;
         if(cursor->OnTimeLeft < 0)
         {
            cursor->On = false;
            cursor->Fading = true;
         }
         
         DrawNewText(buf,p,font,cursor->TextString,r,g,b,a);
         //DrawText(buf,p,cursor->TextString,r,g,b,a,true);
         p.y += font->LineStep;
         cursor = cursor->Next;
      } 
      else if(cursor->Fading)
      {
         cursor->FadeTimeLeft -= dt;
         if(cursor->FadeTimeLeft < 0)
         {
            cursor->On = false;
            cursor->Fading = false;
            cursor->FadeTimeLeft = 0;
         }
         float alphaScale = cursor->FadeTimeLeft / cursor->FadeTime;
         DrawNewText(buf,p,font,cursor->TextString,r,g,b,a*alphaScale);
         p.y += font->LineStep;
         cursor = cursor->Next;
      }
      else
      {
         if(cursor->Prev != nullptr)
         {
            cursor->Prev->Next = cursor->Next;
         }
         if(cursor->Next != nullptr)
         {
            cursor->Next->Prev = cursor->Prev;
         }
         LogEntry *toDelete = cursor;

         if(cursor == *logHead)
         {
            *logHead = cursor->Next;
         }
         cursor = cursor->Next;
         FreeMemory(toDelete->TextString);
         FreeMemory(toDelete);
         toDelete = nullptr;   
      }    
   }
}

extern "C" void GameUpdateAndRender(ThreadContext *threadContext, GameMemory *gameMemory, GameOffscreenBuffer *offscreenBuffer, GameInputBuffer *inputBuffer, GameClocks *gameClocks)
{
   Assert(sizeof(GameState) <= gameMemory->PermanentStorageSize, "Permanent Storage Inadequate");
  
   GameState *gameState = (GameState *)gameMemory->PermanentStorage;
   
   //this lets us throw random shit into our global memory bank.  useful for stb
   SetGlobalMemoryBank(&gameState->WorldMemoryBank);


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
      gameState->Logger = nullptr;
      InitializeMemoryBank(&gameState->WorldMemoryBank, (void *)((size_t)gameMemory->PermanentStorage + sizeof(GameState)), gameMemory->PermanentStorageSize - sizeof(GameState));
      InitializeWorld(gameState);
      gameMemory->IsInitialized = true;
      DebugFileResult bitmapFile = gameMemory->PlatformFunctions.PlatformReadEntireFile(threadContext, "bmptest.bmp");

      SetGlobalMemoryBank(&gameState->WorldMemoryBank);
      if(bitmapFile.FileSize > 0)
      {
         int comp = 4;
         gameState->testBitmap.data = stbi_load_from_memory((const unsigned char *)bitmapFile.Contents, bitmapFile.FileSize,&gameState->testBitmap.width,&gameState->testBitmap.height,&comp,4);
      }
      RGBA color;
      color.g = 255;
      color.a = 255;
      AddToLog(&gameState->Logger,&gameState->WorldMemoryBank,"[NOTICE] Game Memory Initialized", color, 5.0f, 1.0f);
   }




   if(inputBuffer->KeyboardInput.Key[KeyCode::R].IsDown)
   {
      gameMemory->IsInitialized = false;
   }
   if(inputBuffer->KeyboardInput.Key[KeyCode::M].IsDown)// && inputBuffer->KeyboardInput.Key[KeyCode::M].HalfTransitions > 0)
   {
      RGBA color;
      color.g = rand() %255;
      color.b = rand() %255;
      color.r = rand()  %255;
      color.a = 255;
      float run = (rand() % 5);
      float fade =rand() % 5;
      AddToLog(&gameState->Logger,&gameState->WorldMemoryBank,"[NOTICE] M Key Pressed", color, run, fade);
   }
   if(inputBuffer->KeyboardInput.Key[KeyCode::D].IsDown && inputBuffer->KeyboardInput.Key[KeyCode::D].HalfTransitions > 0)
   {
      DumpMemoryBank(&gameState->WorldMemoryBank);
   }

   gameState->ToneHz = 500.f + (-250.f * inputBuffer->ControllerInput[0].RightStick.AverageY);

   int midy = offscreenBuffer->TextureHeight / 2;
   int midx = offscreenBuffer->TextureWidth / 2;
   int offsetx = offscreenBuffer->TextureWidth / 8;
   int height = offscreenBuffer->TextureHeight / 4;
   

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
   
   float leftalpha = Simplex2D((float)left.x/100.f, (float)left.y/100.f);
   float centeralpha = Simplex2D((float)center.x/100.f, (float)center.y/100.f);
   float rightalpha = Simplex2D((float)right.x/100.f, (float)right.y/100.f);
   DrawCircle(offscreenBuffer,left,150,1.0f,0,0,leftalpha);
   StrokeCircle(offscreenBuffer,left,150,1.f,1.f,1.f);
   
   DrawCircle(offscreenBuffer,center,150,0.0f,1,0,centeralpha);
   StrokeCircle(offscreenBuffer,center,150,1.f,1.f,1.f);

   DrawCircle(offscreenBuffer,right,150,0.0f,0,1,rightalpha);
   StrokeCircle(offscreenBuffer,right,150,1.f,1.f,1.f);
   char memorydiag[256];
   snprintf(memorydiag,256,"Used: %zuKB/%zuKB, Entries: %zu",gameState->WorldMemoryBank.Used/KiloBytes(1),gameState->WorldMemoryBank.Size/KiloBytes(1), gameState->WorldMemoryBank.Entries);
   const FontInformation* font = (offscreenBuffer->TextureHeight > offscreenBuffer->WindowHeight)?(&DejaVuSansMono24ptFont):(&DejaVuSansMono12ptFont);
   DrawNewText(offscreenBuffer,vec2i(50,offscreenBuffer->TextureHeight-50), font,memorydiag,1,1,1,1);
   //DrawTriangle(offscreenBuffer,left,right,center,0,0,1.f);
   //StrokeTriangle(offscreenBuffer,left,right,center,1.f,1.f,1.f);
   DisplayLog(&gameState->Logger,offscreenBuffer,vec2i(10,10),gameClocks->UpdateDT);
   return;


}


extern "C" void GameGetSoundSamples(ThreadContext *threadContext, GameMemory *gameMemory, GameSoundBuffer *soundBuffer)
{
  GameState *gameState = (GameState *)gameMemory->PermanentStorage;
}