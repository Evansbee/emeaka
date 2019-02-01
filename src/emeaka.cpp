#include "emeaka.h"
#include "emeaka_intrinsics.h"

#include <cstdio>

#include "emeaka_font.cpp"

extern "C" void ClearBitmap(GameOffscreenBuffer *offscreenBuffer, float r, float g, float b)
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

extern "C" void DrawPixel(GameOffscreenBuffer *offscreenBuffer, float x, float y, float r, float g, float b)
{
   int32_t _x = (int32_t)Round(x);
   int32_t _y = (int32_t)Round(y);
   uint8_t _r = (uint8_t)Round(255.f * r);
   uint8_t _g = (uint8_t)Round(255.f * g);
   uint8_t _b = (uint8_t)Round(255.f * b);
   uint32_t color = RGB_TO_UINT32(_r, _g, _b);
   if (_x >= 0 && _x < offscreenBuffer->Width && _y >= 0 && _y < offscreenBuffer->Height)
   {
      ((uint32_t *)(offscreenBuffer->Memory))[(_y * offscreenBuffer->Width) + _x] = color;
   }
}



extern "C" void DrawRect(GameOffscreenBuffer *offscreenBuffer, float x0, float y0, float x1, float y1, float r, float g, float b)
{
   int _x0 = (int)Round(x0);
   int _y0 = (int)Round(y0);
   int _x1 = (int)Round(x1);
   int _y1 = (int)Round(y1);

   if(_x1 < _x0)
   {
      int swap = _x0;
      _x0 = _x1;
      _x1 = swap;
   }   

   if(_y1 < _y0)
   {
      int swap = _y0;
      _y0 = _y1;
      _y1 = swap;
   }

   for (int y = _y0; y < _y1; ++y)
   {
      for (int x = _x0; x < _x1; ++x)
      {
         DrawPixel(offscreenBuffer, (float)x, float(y), r, g, b);
      }
   }
}

extern "C" void StrokeRect(GameOffscreenBuffer *offscreenBuffer, float x0, float y0, float x1, float y1, float fillr, float fillg, float fillb, float stroker, float strokeg, float strokeb)
{
   
}

extern "C" void DrawCircle(GameOffscreenBuffer *offscreenBuffer, float cx, float cy, float radius, float r, float g, float b)
{
   float x = radius - 1.f;
   float y = 0;
   float dx = 1;
   float dy = 1;

   float err = dx - radius * 2.f;

   while (x >= y)
   {
      DrawPixel(offscreenBuffer, cx + x, cy + y, r, g, b);
      DrawPixel(offscreenBuffer, cx + y, cy + x, r, g, b);
      DrawPixel(offscreenBuffer, cx - y, cy + x, r, g, b);
      DrawPixel(offscreenBuffer, cx - x, cy + y, r, g, b);
      DrawPixel(offscreenBuffer, cx - x, cy - y, r, g, b);
      DrawPixel(offscreenBuffer, cx - y, cy - x, r, g, b);
      DrawPixel(offscreenBuffer, cx + y, cy - x, r, g, b);
      DrawPixel(offscreenBuffer, cx + x, cy - y, r, g, b);

      if (err <= 0.f)
      {
         y += 1.f;
         err += dy;
         dy += 2.f;
      }
      if (err > 0.f)
      {
         x -= 1.f;
         dx += 2.f;
         err += dx - radius * 2;
      }
   }
}

internal void _DrawLineHigh(GameOffscreenBuffer *offscreenBuffer, float x0, float y0, float x1, float y1, float r, float g, float b)
{
   float dx = x1 - x0;
   float dy = y1 - y0;
   float xi = 1.f;
   if (dx < 0.f)
   {
      xi = -1.f;
      dx = -dx;
   }

   float D = 2.f * dx - dy;
   float x = x0;

   for (float y = y0; y <= y1; y += 1.0f)
   {
      DrawPixel(offscreenBuffer, x, y, r, g, b);

      if (D > 0)
      {
         x = x + xi;
         D = D - 2 * dy;
      }

      D = D + 2 * dx;
   }
}

internal void _DrawLineLow(GameOffscreenBuffer *offscreenBuffer, float x0, float y0, float x1, float y1, float r, float g, float b)
{
   float dx = x1 - x0;
   float dy = y1 - y0;
   float yi = 1.f;
   if (dy < 0.f)
   {
      yi = -1.f;
      dy = -dy;
   }

   float D = 2.f * dy - dx;
   float y = y0;

   for (float x = x0; x <= x1; x += 1.f)
   {
      DrawPixel(offscreenBuffer, x, y, r, g, b);
      if (D > 0.f)
      {
         y = y + yi;
         D = D - 2.f * dx;
      }

      D = D + 2.f * dy;
   }
}
extern "C" void DrawBitmap(GameOffscreenBuffer *offscreenBuffer, V2 position, size_t height, size_t width, void* memory)
{
   
}
extern "C" void DrawLine(GameOffscreenBuffer *offscreenBuffer, float x0, float y0, float x1, float y1, float r, float g, float b)
{
   if (Abs(y1 - y0) < Abs(x1 - x0))
   {
      if (x0 > x1)
      {
         _DrawLineLow(offscreenBuffer, x1, y1, x0, y0, r, g, b);
      }
      else
      {
         _DrawLineLow(offscreenBuffer, x0, y0, x1, y1, r, g, b);
      }
   }
   else
   {
      if (y0 > y1)
      {
         _DrawLineHigh(offscreenBuffer, x1, y1, x0, y0, r, g, b);
      }
      else
      {
         _DrawLineHigh(offscreenBuffer, x0, y0, x1, y1, r, g, b);
      }
   }
}

extern "C" void DrawChar(GameOffscreenBuffer *offscreenBuffer, float x, float y, char c, float r, float g, float b, bool shadow)
{
   //wipe it
   if (x < -(float)FixedFontWidth || x > (float)offscreenBuffer->Width || y < -(float)FixedFontHeight || y > (float)offscreenBuffer->Height)
   {
      return;
   }

   for (int plot_y = 0; plot_y < FixedFontHeight; ++plot_y)
   {
      for (int plot_x = 0; plot_x < FixedFontWidth; ++plot_x)
      {
         if ((FixedFont[c][plot_y] & (1 << (8 - plot_x))) > 0)
         {
            DrawPixel(offscreenBuffer, x + float(plot_x), y + float(plot_y), r, g, b);
            if (shadow)
            {
               DrawPixel(offscreenBuffer, x + float(plot_x) + 1.f, y + float(plot_y) + 1.f, 0.f, 0.f, 0.f);
            }
         }
      }
   }
}

extern "C" void DrawText(GameOffscreenBuffer *offscreenBuffer, float x, float y, char *text, float r, float g, float b, bool shadow)
{
   float current_x = x;
   float current_y = y;
   for (char *c = text; *c != '\0'; ++c)
   {
      if (*c == '\n')
      {
         current_y += float(FixedFontYAdvance);
         current_x = x;
      }
      else
      {
         DrawChar(offscreenBuffer, current_x, current_y, *c, r, g, b, shadow);
         current_x += FixedFontXAdvance;
      }
   }
}

void _FillTopFlatTriangle(GameOffscreenBuffer *offscreenBuffer, float x0, float y0, float x1, float y1, float x2, float y2, float r, float g, float b)
{
   float invslope1 = (x2 - x0) / (y2 - y0);
  float invslope2 = (x2 - x1) / (y2 - y1);

  float curx1 = x2;
  float curx2 = x2;

  for (int scanlineY = (int)Round(y2); scanlineY > (int)Round(y0); scanlineY--)
  {
   DrawLine(offscreenBuffer,curx1,(float)scanlineY,curx2,(float)scanlineY,r,g,b);

    curx1 -= invslope1;
    curx2 -= invslope2;
  }
}
void _FillBottomFlatTriangle(GameOffscreenBuffer *offscreenBuffer, float x0, float y0, float x1, float y1, float x2, float y2, float r, float g, float b)
{
  float invslope1 = (x1 - x0) / (y1 - y0);
  float invslope2 = (x2 - x0) / (y2 - y0);

  float curx1 = x0;
  float curx2 = x0;

  for (int scanlineY = (int)Round(y0); scanlineY <= (int)Round(y1); scanlineY++)
  {
   DrawLine(offscreenBuffer,curx1,(float)scanlineY,curx2,(float)scanlineY,r,g,b);
    curx1 += invslope1;
    curx2 += invslope2;
  } 

}
extern "C" void DrawTriangle(GameOffscreenBuffer *offscreenBuffer, float x0, float y0, float x1, float y1, float x2, float y2, float r, float g, float b)
{
   //sort by y...
   if(y0 > y1)
   {
      float xt = x0, yt = y0;
      x0 = x1;
      y0 = y1;
      x1=xt;
      y1=yt;
   }

   if(y1 > y2)
   {
      float xt = x1, yt = y1;
      x1 = x2;
      y1 = y2;
      x2 = xt;
      y2 = yt;
   }

   if(y0 > y2)
   {
      float xt = x0, yt = y0;
      x0 = x2;
      y0 = y2;
      x2=xt;
      y2=yt;
   }
if (y1==y2)
  {
    _FillBottomFlatTriangle(offscreenBuffer, x0, y0, x1,y1,x2,y2,r,g,b);
  }
  /* check for trivial case of top-flat triangle */
  else if (y0==y1)
  {
    _FillTopFlatTriangle(offscreenBuffer, x0, y0, x1,y1,x2,y2,r,g,b);
  } 
  else
  {
    /* general case - split the triangle in a topflat and bottom-flat one */ 
   float x3,y3;

   y3 = y2;
   x3 = x0 + (((y1-y0)/(y2-y0))) * (x2-x0);


    _FillBottomFlatTriangle(offscreenBuffer, x0, y0, x1,y1,x3,y3,r,g,b);
    _FillTopFlatTriangle(offscreenBuffer, x1, y1, x3,y3,x2,y2,r,g,b);
  }

}

extern "C" void StrokeTriangle(GameOffscreenBuffer *offscreenBuffer, float x0, float y0, float x1, float y1, float x2, float y2, float r, float g, float b)
{
   DrawLine(offscreenBuffer,x0,y0,x1,y1,r,g,b);
   DrawLine(offscreenBuffer,x1,y1,x2,y2,r,g,b);
   DrawLine(offscreenBuffer,x2,y2,x0,y0,r,g,b);
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
bool OffsetAndNormalizePosition(GameWorld *world, Position *pos, V2 deltaP)
{
   pos->TileOffset.X += deltaP.X;
   pos->TileOffset.Y += deltaP.Y;

   int32_t tileChangeX = (int32_t)Round(pos->TileOffset.X);
   int32_t tileChangeY = (int32_t)Round(pos->TileOffset.Y);

   pos->TileOffset.X -= (float)tileChangeX;
   pos->Tile.X += tileChangeX;

   pos->TileOffset.Y -= (float)tileChangeY;
   pos->Tile.Y += tileChangeY;

   pos->Tile.X = pos->Tile.X % (world->TileMap->ChunksX * world->TileMap->ChunkSize);
   pos->Tile.Y = pos->Tile.Y % (world->TileMap->ChunksY * world->TileMap->ChunkSize);

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
Chunk *GetChunkForPosition(GameState *gameState, GameWorld *world, uV2 p)
{
   uint64_t chunkx = (p.X >> world->TileMap->ChunkShift);
   uint64_t chunky = (p.Y >> world->TileMap->ChunkShift);

   chunkx = (chunkx % world->TileMap->ChunksX);
   chunky = (chunky % world->TileMap->ChunksY);

   if(world->TileMap->Chunks[chunky * world->TileMap->ChunksX + chunkx].Tiles == NULL)
   {
      GenerateChunk(gameState, chunkx, chunky);
   }
   return &world->TileMap->Chunks[chunky * world->TileMap->ChunksX + chunkx];
}

uint64_t GetTileValueForPosition(GameState *gameState, GameWorld *world, uV2 p)
{
   Chunk *chunk = GetChunkForPosition(gameState, world, p);
   if(chunk != NULL and chunk->Tiles != NULL)
   {
      uint64_t relx = p.X & world->TileMap->ChunkMask;
      uint64_t rely = p.Y & world->TileMap->ChunkMask;
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
   DrawRect(offscreenBuffer,x-width/2.f, y-height, x+width/2.f, y,r,g,b);

}

extern "C" void GameUpdateAndRender(ThreadContext *threadContext, GameMemory *gameMemory, GameOffscreenBuffer *offscreenBuffer, GameInputBuffer *inputBuffer, GameClocks *gameClocks)
{
   Assert(sizeof(GameState) <= gameMemory->PermanentStorageSize, "Permanent Storage Inadequate");

   GameState *gameState = (GameState *)gameMemory->PermanentStorage;
   if (!gameMemory->IsInitialized)
   {
      printf("Initializing In Game Memory...\n");
      gameState->PlayerPos.Tile.X = 5;
      gameState->PlayerPos.Tile.Y = 30;
      gameState->PlayerPos.Tile.Z = 0;
      gameState->PlayerPos.TileOffset.X = 0.0f;
      gameState->PlayerPos.TileOffset.Y = 0.0f;
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
   
   float dx = 0;
   float dy = 0;
   if(inputBuffer->KeyboardInput.Key[KeyCode::A].IsDown || inputBuffer->KeyboardInput.Key[KeyCode::S].IsDown || inputBuffer->KeyboardInput.Key[KeyCode::D].IsDown || inputBuffer->KeyboardInput.Key[KeyCode::W].IsDown)
   {
      dx = 5.f * gameClocks->UpdateDT * ((float)inputBuffer->KeyboardInput.Key[KeyCode::D].IsDown - (float)inputBuffer->KeyboardInput.Key[KeyCode::A].IsDown);
      dy = 5.f * gameClocks->UpdateDT * ((float)inputBuffer->KeyboardInput.Key[KeyCode::W].IsDown - (float)inputBuffer->KeyboardInput.Key[KeyCode::S].IsDown);
   }
   else
   {
      dx = 5.f * gameClocks->UpdateDT * inputBuffer->ControllerInput[0].LeftStick.AverageX;
      dy = -5.f * gameClocks->UpdateDT * inputBuffer->ControllerInput[0].LeftStick.AverageY;
   }

   Position storedPosition = gameState->PlayerPos;
   
   if(OffsetAndNormalizePosition(gameState->World, &gameState->PlayerPos,V2(dx,dy)) && GetTileValueForPosition(gameState,gameState->World,uV2(gameState->PlayerPos.Tile.X,gameState->PlayerPos.Tile.Y)) == 0)
   {
      gameState->PlayerPos = storedPosition;
   }
   
   char movementString[1024];
   snprintf(movementString, 1024, "TileX: %llu\nTileY: %llu\nOffsetX: %0.2f\nOffsetY: %0.2f",gameState->PlayerPos.Tile.X, gameState->PlayerPos.Tile.Y, gameState->PlayerPos.TileOffset.X, gameState->PlayerPos.TileOffset.Y);
   char memoryString[1024];
   float UsedMB = (float)gameState->WorldMemoryArena.Used / (1024.f * 1024.f);
   float SizeMB = (float)gameState->WorldMemoryArena.Size / (1024.f * 1024.f);
   float Percent = (UsedMB/SizeMB) * 100.f;
   snprintf(memoryString, 1024, "World Memory Arena Size: %0.1fMB/%0.1fMB (%0.1f%%)", UsedMB, SizeMB,Percent);
   int32_t cameraViewWidth = 17;
   int32_t cameraViewHeight = 9;
   Position cameraPosition = {};
   cameraPosition.Tile = gameState->PlayerPos.Tile;
   //cameraPosition.Tile.X -= cameraPosition.Tile.X % cameraViewWidth;
   //cameraPosition.Tile.X += (cameraViewWidth  >> 1);
   //cameraPosition.Tile.Y -= cameraPosition.Tile.Y % cameraViewHeight;
   //cameraPosition.Tile.Y += (cameraViewHeight >> 1);
   cameraPosition.TileOffset = V2(0,0);//gameState->PlayerPos.TileOffset;

   if(inputBuffer->ControllerInput[0].AButton.IsDown)
   {
      cameraPosition.TileOffset = gameState->PlayerPos.TileOffset;
   }

   

   float tileSize = gameState->World->TileSideInPixels;

   Position upperLeftTile = cameraPosition, lowerRightTile = cameraPosition;

   
   float middleX = (float)offscreenBuffer->Width/2.f;
   float middleY = (float)offscreenBuffer->Height/2.f;
   float drawStartX = middleX - ((float)gameState->World->TileSideInPixels * (float)cameraViewWidth/2.f);
   float drawStartY = middleY + ((float)gameState->World->TileSideInPixels * (float)cameraViewHeight/2.f);
   float drawStopX = middleX + ((float)gameState->World->TileSideInPixels * (float)cameraViewWidth/2.f);
   float drawStopY = middleY - ((float)gameState->World->TileSideInPixels * (float)cameraViewHeight/2.f);

   
   //< > and out because of the UINT wrapping we rely on

   size_t maxTilesY = gameState->World->TileMap->ChunksY * gameState->World->TileMap->ChunkSize;
   size_t maxTilesX = gameState->World->TileMap->ChunksX * gameState->World->TileMap->ChunkSize;

   float cameraStartX = middleX + tileSize * (0.5f - cameraPosition.TileOffset.X );
   float cameraStartY = middleY - tileSize * (0.5f - cameraPosition.TileOffset.Y );

   for(int yOffset = (-cameraViewHeight/2) - 2; yOffset < cameraViewHeight/2 + 1; ++yOffset)
   {
      for(int xOffset = (-cameraViewWidth/2) - 2; xOffset < cameraViewWidth/2 + 1; ++xOffset)
      {
         float startx = cameraStartX + (float)xOffset * (float)gameState->World->TileSideInPixels;
         float endx = startx + (float)gameState->World->TileSideInPixels;
         float starty = cameraStartY - (float)yOffset * (float)gameState->World->TileSideInPixels;
         float endy = starty - (float)gameState->World->TileSideInPixels;

         if(endx < drawStartX){continue;}
         if(startx > drawStopX){continue;}
         if(starty < drawStopY){continue;}
         if(endy > drawStartY){continue;}

         if(startx < drawStartX){startx = drawStartX;}
         
         if(endx > drawStopX){endx = drawStopX;}
         if(startx < drawStartX){startx = drawStartX;}
         if(endy < drawStopY){endy = drawStopY;}
         if(starty > drawStartY){starty = drawStartY;}
 
         //not currently 100% sure where that +1 is coming from...
         size_t xtile = (cameraPosition.Tile.X + xOffset +1)%maxTilesX;
         size_t ytile = (cameraPosition.Tile.Y + yOffset +1)%maxTilesY;
         
         
         uint64_t tileValue = GetTileValueForPosition(gameState, gameState->World, uV2(xtile,ytile));
         
         if(xtile == gameState->PlayerPos.Tile.X && ytile == gameState->PlayerPos.Tile.Y)
         {
            DrawRect(offscreenBuffer,startx, starty, endx, endy,1,0,0);
         }
         else if(tileValue == 0)
         {
            DrawRect(offscreenBuffer,startx, starty, endx, endy,.1f,.1f,.1f);
         }
         else if(tileValue == 1)
         {
            DrawRect(offscreenBuffer,startx, starty, endx, endy,1,1,1);
         }
         else 
         {
            DrawRect(offscreenBuffer,startx, starty, endx, endy,1,0,1);
         }
      }
   }

   float whateverX = (float)(gameState->PlayerPos.Tile.X - cameraPosition.Tile.X) * tileSize;
   float whateverY = (float)(gameState->PlayerPos.Tile.Y - cameraPosition.Tile.Y) * tileSize;
   float drawX = (float)offscreenBuffer->Width/2.f + (gameState->PlayerPos.TileOffset.X - cameraPosition.TileOffset.X ) * tileSize + whateverX;
   float drawY = (float)offscreenBuffer->Height/2.f - (gameState->PlayerPos.TileOffset.Y - cameraPosition.TileOffset.Y ) * tileSize - whateverY;
   DrawCharacter(offscreenBuffer, drawX, drawY, tileSize * 0.75f, tileSize * 0.9f, 1.f, 0.6f, 0.f,1.f);
   
   DrawText(offscreenBuffer,16,16,movementString,0.f,1.f,0.f,true);
   DrawText(offscreenBuffer,16,130,memoryString,0.f,1.f,0.f,true);

   DrawTriangle(offscreenBuffer,30,30,100,100,100,30,1.f,0,0);
   StrokeTriangle(offscreenBuffer,30,30,100,100,100,30,1.f,1.f,0);
   return;

}

void PlaySinWave(GameState *gameState, GameSoundBuffer *soundBuffer)
{
   
    int16_t ToneVolume = 500;
    int WavePeriod = soundBuffer->SamplesPerSecond/gameState->ToneHz;

    int16_t *sampleOut = soundBuffer->SampleBuffer;
    for(int SampleIndex = 0;
        SampleIndex < soundBuffer->SampleCount;
        ++SampleIndex)
    {
        // TODO(casey): Draw this out for people
        float SineValue = sinf(gameState->tSin);
        int16_t SampleValue = (int16_t)(SineValue * ToneVolume);
        *sampleOut++ = SampleValue;
        *sampleOut++ = SampleValue;

        gameState->tSin += 2.0f*PI32*1.0f/(float)WavePeriod;
        gameState->tSin = fmodf(gameState->tSin, 2.0f * PI32);
    }
}

extern "C" void GameGetSoundSamples(ThreadContext *threadContext, GameMemory *gameMemory, GameSoundBuffer *soundBuffer)
{
  GameState *gameState = (GameState *)gameMemory->PermanentStorage;
  //PlaySinWave(gameState, soundBuffer);
}