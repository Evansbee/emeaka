#include "emeaka.h"

#include <math.h>

#include "emeaka_font.cpp"
//let's agree on

inline float roundf(float in)
{
  return floorf(in +0.5f);
}

extern "C" void ClearBitmap(GameOffscreenBuffer *offscreenBuffer, float r, float g, float b)
{
   uint8_t _r = uint8_t(255.f * r);
   uint8_t _g= uint8_t(255.f * g);
   uint8_t _b= uint8_t(255.f * b);
   uint32_t color = RGB_TO_UINT32(_r,_g,_b);
  for(int y = 0 ; y < offscreenBuffer->Height; ++y)
  {
    for(int x = 0; x < offscreenBuffer->Width; ++x)
    {
      ((uint32_t *)(offscreenBuffer->Memory))[(y * offscreenBuffer->Width + x)] = color;
    }
  }
}

extern "C" void DrawPixel(GameOffscreenBuffer *offscreenBuffer, float x, float y, float r, float g, float b)
{
   int32_t _x = (int32_t)roundf(x);
   int32_t _y = (int32_t)roundf(y);
   uint8_t _r = uint8_t(255.f * r);
   uint8_t _g= uint8_t(255.f * g);
   uint8_t _b= uint8_t(255.f * b);
   uint32_t color = RGB_TO_UINT32(_r,_g,_b);
  if(_x >= 0 && _x < offscreenBuffer->Width && _y >= 0 && _y < offscreenBuffer->Height)
  {
      ((uint32_t *)(offscreenBuffer->Memory))[(_y * offscreenBuffer->Width) + _x] = color;
  }
}

extern "C" void DrawRect(GameOffscreenBuffer *offscreenBuffer, float x0, float y0, float x1, float y1, float r, float g, float b)
{
   int _x0 = (int)roundf(x0);
   int _y0 = (int)roundf(y0);
   int _x1 = (int)roundf(x1);
   int _y1 = (int)roundf(y1);

   for(int y = _y0; y < _y1; ++y)
   {
      for(int x = _x0; x < _x1; ++x)
      {
         DrawPixel(offscreenBuffer, (float)x, float(y) , r,g,b);
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

   while(x>=y)
   {
      DrawPixel(offscreenBuffer, cx+x, cy+y, r,g,b);
      DrawPixel(offscreenBuffer, cx+y, cy+x, r,g,b);
      DrawPixel(offscreenBuffer, cx-y, cy+x, r,g,b);
      DrawPixel(offscreenBuffer, cx-x, cy+y, r,g,b);
      DrawPixel(offscreenBuffer, cx-x, cy-y, r,g,b);
      DrawPixel(offscreenBuffer, cx-y, cy-x, r,g,b);
      DrawPixel(offscreenBuffer, cx+y, cy-x, r,g,b);
      DrawPixel(offscreenBuffer, cx+x, cy-y, r,g,b);
      
      if(err <= 0.f)
      {
         y+=1.f;
         err += dy;
         dy += 2.f;
      }
      if(err>0.f)
      {
         x-=1.f;
         dx+=2.f;
         err += dx - radius * 2;
      }
   }
}

internal void _DrawLineHigh(GameOffscreenBuffer *offscreenBuffer, float x0, float y0, float x1, float y1,float r, float g, float b)
{
   float dx = x1 - x0;
  float dy = y1 - y0;
  float xi = 1.f;
  if (dx < 0.f)
    {
       xi = -1.f;
    dx = -dx;
    }
  
  float D = 2.f*dx - dy;
  float x = x0;

  for(float y = y0; y <= y1; y += 1.0f)
    {
       DrawPixel(offscreenBuffer, x, y, r,g,b);

    if (D > 0)
       {
          x = x + xi;
       D = D - 2*dy;
       }
    
    D = D + 2*dx;
    }
}


internal void _DrawLineLow(GameOffscreenBuffer *offscreenBuffer, float x0, float y0, float x1, float y1,float r, float g, float b)
{
   float dx = x1 - x0;
  float dy = y1 - y0;
  float yi = 1.f;
  if (dy < 0.f)
    {yi = -1.f;
    dy = -dy;}

  float D = 2.f*dy - dx;
  float y = y0;

  for(float x = x0; x <= x1; x+= 1.f)
  {
    DrawPixel(offscreenBuffer, x, y, r,g,b);
    if (D > 0.f)
      { y = y + yi;
       D = D - 2.f*dx;}
    
    D = D + 2.f*dy;}
}




extern "C" void DrawLine(GameOffscreenBuffer *offscreenBuffer, float x0, float y0, float x1, float y1,float r, float g, float b)
{
   if(fabsf(y1 - y0) < fabsf(x1 - x0))
   {
      if (x0 > x1)
      {
         _DrawLineLow(offscreenBuffer,x1, y1, x0, y0,r,g,b);
      }
      else
      {  
         _DrawLineLow(offscreenBuffer,x0, y0, x1, y1,r,g,b);
      }
   }
   else
   {
      if(y0 > y1)
      { 
         _DrawLineHigh(offscreenBuffer,x1, y1, x0, y0,r,g,b); 
      }
      else
      { 
         _DrawLineHigh(offscreenBuffer,x0, y0, x1, y1,r,g,b) ; 
      }
   }
}

extern "C" void DrawChar(GameOffscreenBuffer *offscreenBuffer, float x, float y, char c, float r, float g, float b, bool shadow)
{
   //wipe it
   if(x < -(float)FixedFontWidth || x > (float)offscreenBuffer->Width || y < -(float)FixedFontHeight || y > (float)offscreenBuffer->Height)
   {
      return;
   }

   for(int plot_y = 0; plot_y < FixedFontHeight; ++plot_y)
   {
      for(int plot_x = 0; plot_x < FixedFontWidth; ++plot_x)
      {
         if((FixedFont[c][plot_y] & (1 << (8-plot_x))) > 0)
         {
            DrawPixel(offscreenBuffer, x + float(plot_x), y + float(plot_y) , r,g,b);
            if(shadow)
            {
               DrawPixel(offscreenBuffer, x + float(plot_x) + 1.f, y + float(plot_y) + 1.f , 0.f, 0.f, 0.f);
            }
         }
      }
   }
}

extern "C" void DrawText(GameOffscreenBuffer *offscreenBuffer, float x, float y, char* text, float r, float g, float b, bool shadow)
{
   float current_x = x;
   float current_y = y;
   for(char* c = text; *c != '\0'; ++c)
   {
      if(*c =='\n')
      {
         current_y += float(FixedFontYAdvance);
         current_x = x;
      }
      else
      {
		  DrawChar(offscreenBuffer,current_x, current_y, *c, r,g,b, shadow);
		  current_x += FixedFontXAdvance;
      }
   }
}


extern "C" void GameUpdateAndRender(ThreadContext *threadContext, GameMemory *gameMemory, GameOffscreenBuffer *offscreenBuffer, GameInputBuffer *inputBuffer, GameClocks *gameClocks)
{
  Assert(sizeof(GameState) <= gameMemory->PermanentStorageSize, "Permanent Storage Inadequate");

  GameState *gameState = (GameState *)gameMemory->PermanentStorage;
  if (!gameMemory->IsInitialized)
  {
   gameState->PlayerX = 200.0f;
   gameState->PlayerY = 200.0f;
    gameMemory->IsInitialized = true;
  }
  uint8_t intensity = 0;
  if(inputBuffer->MouseInput.MouseLocationY >= 0.f && inputBuffer->MouseInput.MouseLocationY <= 1.0f)
  {
     intensity = (uint8_t)(50.f * (1.0f - inputBuffer->MouseInput.MouseLocationY));
  }
  ClearBitmap(offscreenBuffer, 0.2f, 0.f, 0.2f);
  
  uint32_t tileMap[9][16] = {
      {1, 1, 1, 1,   1, 1, 1, 0,  1, 1, 1,10,  1, 1, 1, 1},
      {1, 0, 0, 0,   0, 0, 1, 0,  0, 0, 0, 0,  0, 1, 0, 1},
      {1, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 1, 1},
      {1, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
      {0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0},
      {1, 0, 0, 0,   0, 0, 1, 0,  0, 0, 0, 0,  0, 0, 0, 1},
      {1, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
      {1, 0, 1, 0,   0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1},
      {1, 1, 1, 1,   1, 1, 1, 1,  1, 0, 1, 1,  1, 1, 1, 1},
   };

   float tileWidth = 100;
   float tileHeight = 100;
   for(size_t row = 0; row < 9; ++row)
   {
      for(size_t col = 0; col < 16; ++col)
      {
         float startx = (float)col * tileWidth;
            float starty = (float)row * tileHeight;
            float endx = startx + tileWidth;
            float endy = starty + tileHeight;
         if(tileMap[row][col] == 0)
         {
            
            DrawRect(offscreenBuffer,startx, starty, endx, endy, 1.0f, 1.0f, 1.0f);
         }
         else
         {
            DrawRect(offscreenBuffer,startx, starty, endx, endy, 0.1f, 0.1f, 0.1f);
         }
      }
   }

   float movementSpeed = 750.f;
   float playerHeight = 0.75f * tileHeight;
   float playerWidth = 0.5f * tileWidth;
   float playerHalfWidth = playerWidth /2.f;

   float px = gameState->PlayerX;
   float py = gameState->PlayerY;
   if(inputBuffer->ControllerInput[0].Connected)
   {
      gameState->PlayerX += movementSpeed * gameClocks->UpdateDT * inputBuffer->ControllerInput[0].LeftStick.AverageX;
      gameState->PlayerY -= movementSpeed * gameClocks->UpdateDT * inputBuffer->ControllerInput[0].LeftStick.AverageY;
      //truncation doedsn't work in the first negative spot (-.9999 = 0)
      int32_t idx_x = int32_t(gameState->PlayerX/tileWidth);
      int32_t idx_y = int32_t(gameState->PlayerY/tileHeight);
      if(tileMap[idx_y][idx_x] == 1 || idx_x >= 16 || idx_x < 0 || idx_y < 0 || idx_y >= 9)
      {
         gameState->PlayerX = px;
         gameState->PlayerY = py;
      }

      if(inputBuffer->ControllerInput[0].LeftThumbButton.IsDown)
      {
         gameState->PlayerX = 200.f;
         gameState->PlayerY = 200.f;
      }
   }
    px = gameState->PlayerX;
    py = gameState->PlayerY;
   float pr = 1.0f;
   float pg = .5f;
   float pb = 0;
   DrawRect(offscreenBuffer,px - playerHalfWidth , py - playerHeight, px + playerHalfWidth, py , pr, pg, pb);



}

extern "C" void GameGetSoundSamples(ThreadContext *threadContext, GameMemory *gameMemory, GameSoundBuffer *soundBuffer)
{
  GameState *gameState = (GameState *)gameMemory->PermanentStorage;
}