#include "emeaka.h"
#include "emeaka_intrinsics.h"
#include <cstdio>


#include "emeaka_font.cpp"

//stb stuff
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_ONLY_BMP
#include "stb_image.h"


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
   RGBA color;
   color.r = (uint8_t)Round(255.f * r);
   color.g = (uint8_t)Round(255.f * g);
   color.b = (uint8_t)Round(255.f * b);
   color.a = (uint8_t)Round(255.f * a);
   
   if (p.x >= 0 && p.x < offscreenBuffer->Width && p.y >= 0 && p.y < offscreenBuffer->Height)
   {
      if(a == 1.0f)
      {
         ((uint32_t *)(offscreenBuffer->Memory))[(p.y * offscreenBuffer->Width) + p.x] = color.rgba;
      }
      else if (a > 0.f && a < 1.f)
      {
         RGBA currentColor;
         currentColor.rgba = ((uint32_t *)(offscreenBuffer->Memory))[(p.y * offscreenBuffer->Width) + p.x];

         color.r = Round(255.f * (r * a + ((float)currentColor.r/255.f) * (1.0f - a)));
         color.g = Round(255.f * (g * a + ((float)currentColor.g/255.f) * (1.0f - a)));
         color.b = Round(255.f * (b * a + ((float)currentColor.b/255.f) * (1.0f - a)));
         ((uint32_t *)(offscreenBuffer->Memory))[(p.y * offscreenBuffer->Width) + p.x] = color.rgba;
      }
   }
}

internal void DrawHorizontalLine(GameOffscreenBuffer *osb, vec2i p0, vec2i p1, float r, float g, float b, float a = 1.0f)
{
   int64_t x0 = p0.x, x1 = p1.x;
   if(x1 < x0)
   {
      x0 = p1.x;
      x1 = p0.x;
   }
   
   if(x0 < 0)
   {
      x0 = 0;
   }

   if(x1 >= osb->Width)
   {
      x1 = osb->Width-1;
   }

   for(auto x = x0; x <= x1; ++x)
   {
      DrawPixel(osb,vec2i(x,p0.y),r,g,b,a);
   }
}

extern "C" void DrawLine(GameOffscreenBuffer *offscreenBuffer, vec2i p0, vec2i p1, float r, float g, float b)
{
   bool steep = abs(p1.y - p0.y) > abs(p1.x - p0.x);
   
   if(p0.y == p1.y)
   {
      DrawHorizontalLine(offscreenBuffer,p0,p1,r,g,b);
      return;
   }
   if(steep)
   {
      int64_t tmp;
      tmp = p0.x;
      p0.x = p0.y;
      p0.y = tmp;

      tmp = p1.x;
      p1.x = p1.y;
      p1.y = tmp;
   }

   if(p0.x > p1.x)
   {
      vec2i tmp = p0;
      p0 = p1;
      p1 = tmp;
   }

   int64_t dx, dy;
   dx = p1.x - p0.x;
   dy = abs(p1.y - p0.y);

   int64_t err = dx / 2;
   int64_t ystep;
   if(p0.y < p1.y)
   {
      ystep = 1;
   }
   else
   {
      ystep = -1;
   }

   vec2i drawPt = p0;
   for(;drawPt.x <= p1.x; drawPt.x++)
   {
      if(steep)
      {
         vec2i inv;
         inv.x = drawPt.y;
         inv.y = drawPt.x;
         DrawPixel(offscreenBuffer,inv,r,g,b);
      }
      else
      {
         DrawPixel(offscreenBuffer, drawPt, r,g,b);
      }
      err -= dy;
      if(err < 0)
      {
         drawPt.y += ystep;
         err += dx;
      }
   }
   return;
   
}
void StrokeCircle(GameOffscreenBuffer *offscreenBuffer, vec2i p, int radius, float r, float g, float b)
{
   int64_t x = -radius, y = 0, err = 2 - 2 * radius;
   do
   {
      DrawPixel(offscreenBuffer,vec2i(p.x-x,p.y+y),r,g,b);
      DrawPixel(offscreenBuffer,vec2i(p.x-y,p.y-x),r,g,b);
      DrawPixel(offscreenBuffer,vec2i(p.x+x,p.y-y),r,g,b);
      DrawPixel(offscreenBuffer,vec2i(p.x+y,p.y+x),r,g,b);
      radius = err;
      if(radius <= y) err += ++y*2+1;
      if(radius > x || err > 7) err += ++x*2+1;
      /* code */
   } while (x<0);
   
}

void DrawCircle(GameOffscreenBuffer *offscreenBuffer, vec2i p, int radius, float r, float g, float b, float a)
{
   size_t draw = 0;
   int64_t x = -radius, y = 0, err = 2 - 2 * radius;
   do
   {
      DrawHorizontalLine(offscreenBuffer, vec2i(p.x-x,p.y+y), vec2i(p.x,p.y+y),r,g,b,a);      
      DrawHorizontalLine(offscreenBuffer, vec2i(p.x-y,p.y-x), vec2i(p.x,p.y-x),r,g,b,a);
      DrawHorizontalLine(offscreenBuffer, vec2i(p.x+x,p.y-y), vec2i(p.x,p.y-y),r,g,b,a);
      DrawHorizontalLine(offscreenBuffer, vec2i(p.x+y,p.y+x), vec2i(p.x,p.y+x),r,g,b,a);
      radius = err;
      if(radius <= y) err += ++y*2+1;
      if(radius > x || err > 7) err += ++x*2+1;
      /* code */
   } while (true && x<0);
   
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
   //not sure why i chose a lambda here...
   auto VecSwap = [](vec2i &e1, vec2i &e2){
      vec2i temp(e1);
      e1 = e2;
      e2 = temp;
   };

   int64_t a, _b, y, last;


   //sort in ascending y
   if(p0.y > p1.y) VecSwap(p0,p1);
   if(p1.y > p2.y) VecSwap(p1,p2);
   if(p0.y > p1.y) VecSwap(p0,p1);

   if(p0.y == p2.y)
   {
      a = _b = p0.x;
      if(p1.x < 1) a = p1.x;
      else if(p1.x > _b) b = p1.x;
      if(p2.x < a) a = p2.x;
      else if(p2.x > _b) _b = p2.x;
      DrawHorizontalLine(offscreenBuffer,vec2i(a,p0.y),vec2i(_b,p0.y),r,g,b);
      return;
   }

   int64_t 
   dx01 = p1.x - p0.x,
   dy01 = p1.y - p0.y,
   dx02 = p2.x - p0.x,
   dy02 = p2.y - p0.y,
   dx12 = p2.x - p1.x,
   dy12 = p2.y - p1.y,
   sa = 0,
   sb = 0;


   if(p1.y == p2.y) last = p1.y;
   else last = p1.y-1;


   for(y = p0.y; y <= last; ++y)
   {
      a = p0.x + sa/dy01;
      _b = p0.x + sb/dy02;
      sa += dx01;
      sb += dx02;

      if(a>_b)
      {
          int64_t tmp = a;
          a = _b; _b = tmp;
      }
      DrawHorizontalLine(offscreenBuffer,vec2i(a,y),vec2i(_b,y),r,g,b);
     
   }

   sa = dx12 * (y - p1.y);
   sb = dx02 * (y - p0.y);

   for(;y <= p2.y; ++y)
   {
      a = p1.x + sa/dy12;
      _b = p0.x + sb/dy02;
      sa+=dx12;
      sb+=dx02;
      if(a>_b)
      {
          int64_t tmp = a;
          a = _b; _b = tmp;
      }
      DrawHorizontalLine(offscreenBuffer,vec2i(a,y),vec2i(_b,y),r,g,b);
   }
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



internal void InitialzeMemoryBank(MemoryBank *bank, void *start, size_t size)
{
   bank->Start = start;
   bank->Size=size;
   bank->Entries = 1;
   bank->Used = 0;
   bank->NextFree = (MemoryAllocationHeader *)start;

   MemoryAllocationHeader *header = (MemoryAllocationHeader *)start;
   header->InUse = false;
   header->Next = nullptr;
   header->Previous = nullptr;
   header->Size = size - sizeof(MemoryAllocationHeader);
}

void *AllocateMemory(MemoryBank *bank, size_t size)
{
   MemoryAllocationHeader *current = (MemoryAllocationHeader *)bank->Start;

   while(current->InUse || current->Size < size)
   {
      if(current->Next != nullptr)
      {
         current = current->Next;
      }
      else
      {
         return nullptr;
      }
   }

   void *returnValue = (void *)((size_t)current + sizeof(MemoryAllocationHeader));
   
   //add new header afterwards...
   return returnValue;
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
      printf("Initializing In Game Memory...\n");
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
      InitializeMemoryArena(&gameState->WorldMemoryArena, (void *)((size_t)gameMemory->PermanentStorage + sizeof(GameState)), gameMemory->PermanentStorageSize - sizeof(GameState));
      InitializeWorld(gameState);
      gameMemory->IsInitialized = true;
      printf("World Memory Location: 0x%p\n",gameState->World);
      printf("World->TileMap Location: 0x%p\n",gameState->World->TileMap);
      char stringToRead[] = "bmptest.bmp";
      DebugFileResult bitmapFile = gameMemory->PlatformFunctions.PlatformReadEntireFile(threadContext, stringToRead);
      
      printf("Read Filesize: %zu\n",bitmapFile.FileSize);

      if(bitmapFile.FileSize > 0)
      {
         int comp = 4;
         gameState->testBitmap.data = stbi_load_from_memory((const unsigned char *)bitmapFile.Contents, bitmapFile.FileSize,&gameState->testBitmap.width,&gameState->testBitmap.height,&comp,4);
         printf("Bitmap Data: %d %d %d\n",gameState->testBitmap.height, gameState->testBitmap.width, comp);
      }
   }
   gameState->ToneHz = 500.f + (-250.f * inputBuffer->ControllerInput[0].RightStick.AverageY);

   int midy = offscreenBuffer->Height / 2;
   int midx = offscreenBuffer->Width / 2;
   int offsetx = offscreenBuffer->Width / 8;
   int height = offscreenBuffer->Height / 4;
   

   float leftHz=0.125f/2.f;
   float rightHz=0.25f/2.f;
   float centerHz=0.5f/2.f;

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
   DrawBitmap(offscreenBuffer, vec2i(10,10),&gameState->testBitmap);
   
   DrawCircle(offscreenBuffer,vec2i(350,250),150,1,0.2f,0.3f,.5);
   //StrokeCircle(offscreenBuffer,vec2i(250,250),9,1.0f,1.0f,1.0f);
   
   //DrawCircle(offscreenBuffer,left,150,1.0f,0,0,.5f);
   //StrokeCircle(offscreenBuffer,left,150,1.f,1.f,1.f);
   
   //DrawCircle(offscreenBuffer,center,150,0.0f,1,0,.5f);
   //StrokeCircle(offscreenBuffer,center,150,1.f,1.f,1.f);

   //DrawCircle(offscreenBuffer,right,150,0.0f,0,1,.5f);
   //StrokeCircle(offscreenBuffer,right,150,1.f,1.f,1.f);

   //DrawTriangle(offscreenBuffer,left,right,center,0,0,1.f);
   //StrokeTriangle(offscreenBuffer,left,right,center,1.f,1.f,1.f);
   return;


}


extern "C" void GameGetSoundSamples(ThreadContext *threadContext, GameMemory *gameMemory, GameSoundBuffer *soundBuffer)
{
  GameState *gameState = (GameState *)gameMemory->PermanentStorage;
}