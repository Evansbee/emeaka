#include "emeaka.h"
#include "emeaka_intrinsics.h"

#include <cstdio>

#include "emeaka_font.cpp"

//let's agree on



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

   for (int y = _y0; y < _y1; ++y)
   {
      for (int x = _x0; x < _x1; ++x)
      {
         DrawPixel(offscreenBuffer, (float)x, float(y), r, g, b);
      }
   }
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


internal void MakeCannonical(Position *position)
{
   while(position->TileOffsetX > 1.0f)
   {
      position->TileOffsetX -= 1.0f;
      position->TileX++;
   }

   while(position->TileOffsetX < 0.0f)
   {
      position->TileOffsetX += 1.0f;
      position->TileX--;
   }

   while(position->TileOffsetY > 1.0f)
   {
      position->TileOffsetY -= 1.0f;
      position->TileY++;
   }

   while(position->TileOffsetY < 0.0f)
   {
      position->TileOffsetY += 1.0f;
      position->TileY--;
   }
}

uint32_t GetTileIndex(uint32_t TileInfo)
{
   return TileInfo & 0x0000000F;
}


internal TileMap*
GetCurrentTileMapForPlayerLocation(World *world, float x, float y)
{
   for(size_t i = 0; i < world->NumTileMaps ; ++i)
   {  
      TileMap *testTileMap = &(world->TileMaps[i]);
      if(x >= testTileMap->UpperLeftX && x < testTileMap->UpperLeftX + world->TileSideInPixels*world->TileMapWidth &&
         y >= testTileMap->UpperLeftY && y < testTileMap->UpperLeftY + world->TileSideInPixels*world->TileMapHeight)
         {
            return testTileMap;
         }
   }
   return nullptr;
}

bool IsTilemapPointEmpty(TileMap *tileMap, World *world, float testX, float testY)
{
   bool result = false;
   testX -= tileMap->UpperLeftX;
   testY -= tileMap->UpperLeftY;
   int32_t tilex = (int32_t)Floor(testX / world->TileSideInPixels);
   int32_t tiley = (int32_t)Floor(testY / world->TileSideInPixels);
   if (tilex >= 0 && tilex < (int32_t)world->TileMapWidth &&
       tiley >= 0 && tiley < (int32_t)world->TileMapHeight)
   {
      return (tileMap->Map[tiley * world->TileMapWidth + tilex] == 0);
   }
   return false;
}

internal bool IsWorldPointEmpty(World *world, float x, float y)
{
   TileMap *testTileMap = GetCurrentTileMapForPlayerLocation(world, x, y);
   if(testTileMap != nullptr)
   {
      return IsTilemapPointEmpty(testTileMap, world, x, y);
   }
   return false;
}

internal void GetDrawCoordinates(World *world, float px, float py, float *dx, float *dy)
{
   TileMap *testTileMap = GetCurrentTileMapForPlayerLocation(world, px, py);
   if(testTileMap != nullptr)
   {
      *dx = px - testTileMap->UpperLeftX;
      *dy = py - testTileMap->UpperLeftY;
   }
}

inline internal uint32_t
GetTileValueUnchecked(TileMap *tileMap, World *world, int32_t tileX, int32_t tileY)
{
   return tileMap->Map[tileY * world->TileMapWidth + tileX];
}

extern "C" void GameUpdateAndRender(ThreadContext *threadContext, GameMemory *gameMemory, GameOffscreenBuffer *offscreenBuffer, GameInputBuffer *inputBuffer, GameClocks *gameClocks)
{
   Assert(sizeof(GameState) <= gameMemory->PermanentStorageSize, "Permanent Storage Inadequate");

   GameState *gameState = (GameState *)gameMemory->PermanentStorage;
   if (!gameMemory->IsInitialized)
   {
      gameState->PlayerX = 200.0f;
      gameState->PlayerY = 200.0f;
      gameState->PlayerPos.TileX = 3;
      gameState->PlayerPos.TileY = 3;
      gameState->PlayerPos.TileZ = 0;
      gameState->PlayerPos.TileOffsetX = 0.0f;
      gameState->PlayerPos.TileOffsetY = 0.0f;
      
      gameMemory->IsInitialized = true;

   }
   uint8_t intensity = 0;
   if (inputBuffer->MouseInput.MouseLocationY >= 0.f && inputBuffer->MouseInput.MouseLocationY <= 1.0f)
   {
      intensity = (uint8_t)(50.f * (1.0f - inputBuffer->MouseInput.MouseLocationY));
   }
   ClearBitmap(offscreenBuffer, 0.2f, 0.f, 0.2f);
   TileMap tileMap[3];
   uint32_t myTileMap00[9][16] = {
       {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
       {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
       {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
       {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
       {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
       {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
       {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
       {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
       {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
   };

   uint32_t myTileMap01[9][16] = {
       {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
       {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
       {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
       {1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
       {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1},
       {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
       {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
       {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
       {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
   };

   uint32_t myTileMap02[9][16] = {
       {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
       {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
       {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1},
       {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
       {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
       {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1},
       {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1},
       {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
       {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
   };

   World world;
   world.TileSideInMeters = 1.4f;
   world.TileSideInPixels = 60.0f;
   world.TileMapWidth = 16;
   world.TileMapHeight = 9;
   world.NumTileMaps = 3;

   tileMap[0].Map = (uint32_t *)&myTileMap00;
   tileMap[0].UpperLeftX = 0.0f;
   tileMap[0].UpperLeftY = 0.0f;

   tileMap[1].Map = (uint32_t *)&myTileMap01;
   tileMap[1].UpperLeftX = 0.0f;
   tileMap[1].UpperLeftY = world.TileSideInPixels * (float)world.TileMapHeight;

   tileMap[2].Map = (uint32_t *)&myTileMap02;
   tileMap[2].UpperLeftX = world.TileSideInPixels * (float)world.TileMapWidth;
   tileMap[2].UpperLeftY = 0.f;

   world.TileMaps = (TileMap *)&tileMap;


   TileMap *currentTileMap = GetCurrentTileMapForPlayerLocation(&world, gameState->PlayerX, gameState->PlayerY);

   if(currentTileMap == nullptr)
   {
      return;
   }
   for (int32_t row = 0; row < (int32_t)world.TileMapHeight; ++row)
   {
      for (int32_t col = 0; col < (int32_t)world.TileMapWidth; ++col)
      {
         float startx = (float)col * world.TileSideInPixels;
         float starty = (float)row * world.TileSideInPixels;
         float endx = startx + world.TileSideInPixels;
         float endy = starty + world.TileSideInPixels;
         if (GetTileValueUnchecked(currentTileMap, &world, col, row) == 0)
         {
            DrawRect(offscreenBuffer, startx, starty, endx, endy, 1.0f, 1.0f, 1.0f);
         }
         else
         {
            DrawRect(offscreenBuffer, startx, starty, endx, endy, 0.1f, 0.1f, 0.1f);
         }
      }
   }

   float movementSpeed = 5.f * world.TileSideInPixels;
   float playerHeight = 0.75f * world.TileSideInPixels;
   float playerWidth = 0.5f * world.TileSideInPixels;
   float playerHalfWidth = playerWidth / 2.f;
   float dx = 0.f;
   float dy = 0.f;
   if (inputBuffer->ControllerInput[0].Connected)
   {

      dx = movementSpeed * gameClocks->UpdateDT * inputBuffer->ControllerInput[0].LeftStick.AverageX;
      dy = movementSpeed * gameClocks->UpdateDT * inputBuffer->ControllerInput[0].LeftStick.AverageY;

      if (inputBuffer->ControllerInput[0].LeftThumbButton.IsDown)
      {
         gameState->PlayerX = 200.f;
         gameState->PlayerY = 200.f;
      }
   }

   if (inputBuffer->KeyboardInput.Key['W'].IsDown)
   {
      dy = movementSpeed * gameClocks->UpdateDT;
   }
   if (inputBuffer->KeyboardInput.Key['S'].IsDown)
   {
      dy = -movementSpeed * gameClocks->UpdateDT;
   }
   if (inputBuffer->KeyboardInput.Key['A'].IsDown)
   {
      dx = -movementSpeed * gameClocks->UpdateDT;
   }
   if (inputBuffer->KeyboardInput.Key['D'].IsDown)
   {
      dx = movementSpeed * gameClocks->UpdateDT;
   }
   if (inputBuffer->KeyboardInput.Key['B'].IsDown)
   {
      gameState->PlayerX = 200.f;
      gameState->PlayerY = 200.f;
   }

   float newPlayerX = gameState->PlayerX + dx;

   if (IsWorldPointEmpty(&world, newPlayerX, gameState->PlayerY) && IsWorldPointEmpty(&world, newPlayerX + playerHalfWidth, gameState->PlayerY) && IsWorldPointEmpty(&world, newPlayerX - playerHalfWidth, gameState->PlayerY))
   {
      gameState->PlayerX = newPlayerX;
   }
   else
   { //moveing left -- this doesn't work...
      if (newPlayerX < gameState->PlayerX)
      {
         gameState->PlayerX = playerHalfWidth + Floor(gameState->PlayerX / world.TileSideInPixels) * world.TileSideInPixels;
      }
      else
      {
         gameState->PlayerX = Floor((gameState->PlayerX + world.TileSideInPixels) / world.TileSideInPixels) * world.TileSideInPixels - playerHalfWidth - 0.1f;
      }
   }

   float newPlayerY = gameState->PlayerY - dy;
   if (IsWorldPointEmpty(&world, gameState->PlayerX, newPlayerY) && IsWorldPointEmpty(&world, gameState->PlayerX + playerHalfWidth, newPlayerY) && IsWorldPointEmpty(&world, gameState->PlayerX - playerHalfWidth, newPlayerY))
   {
      gameState->PlayerY = newPlayerY;
   }
   else
   {
      if (newPlayerY < gameState->PlayerY)
      {
         gameState->PlayerY = Floor(gameState->PlayerY / world.TileSideInPixels) * world.TileSideInPixels;
      }
      else
      {
         gameState->PlayerY = Floor((gameState->PlayerY + world.TileSideInPixels) / world.TileSideInPixels) * world.TileSideInPixels - 0.1f;
      }
   }

   float pr = 1.0f;
   float pg = .5f;
   float pb = 0;
   float drawx = 0;
   float drawy = 0;

   GetDrawCoordinates(&world, gameState->PlayerX, gameState->PlayerY, &drawx, &drawy);
   DrawRect(offscreenBuffer, drawx - playerHalfWidth, drawy - playerHeight, drawx + playerHalfWidth, drawy, pr, pg, pb);
   char playerInfo[255];
   snprintf(playerInfo,255,"Player Location: (%0.2f, %0.2f)",gameState->PlayerX,gameState->PlayerY);
   DrawText(offscreenBuffer,16,16,playerInfo,0.f,1.f,0.f,true);
}

extern "C" void GameGetSoundSamples(ThreadContext *threadContext, GameMemory *gameMemory, GameSoundBuffer *soundBuffer)
{
   GameState *gameState = (GameState *)gameMemory->PermanentStorage;
}