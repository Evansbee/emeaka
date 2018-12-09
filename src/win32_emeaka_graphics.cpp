#include <windows.h>
#include <cstdint>

#include "emeaka.h"
#include "win32_emeaka.h"


internal Win32WindowDimensions GetWin32WindowDimensions(HWND window)
{
   RECT clientRect;
   Win32WindowDimensions windowDimensions;
   GetClientRect(window, &clientRect);
   windowDimensions.Width = clientRect.right - clientRect.left;
   windowDimensions.Height = clientRect.bottom - clientRect.top;
   return windowDimensions;
}

internal void Win32ResizeDIBSection(Win32OffscreenBuffer *Buffer, int width, int height) //Device Independent Bitmap
{
   if (Buffer->Memory)
   {
      VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
   }

   Buffer->Width = width;
   Buffer->Height = height;
   Buffer->BytesPerPixel = 4;
   Buffer->Pitch = Buffer->Width * Buffer->BytesPerPixel;
   Buffer->Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   Buffer->Info.bmiHeader.biWidth = Buffer->Width;
   Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
   Buffer->Info.bmiHeader.biPlanes = 1;
   Buffer->Info.bmiHeader.biBitCount = 32;
   Buffer->Info.bmiHeader.biCompression = BI_RGB;

   int bitmapMemorySize = Buffer->BytesPerPixel * Buffer->Width * Buffer->Height;
   Buffer->Memory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
   ZeroMemory(Buffer->Memory, bitmapMemorySize);
}

internal void Win32DisplayBufferInWindow(HDC deviceContext, int windowWidth, int windowHeight, Win32OffscreenBuffer *Buffer, int x, int y, int width, int height)
{
   //bufferRatio = buffer->Width / Buffer->Height;

   StretchDIBits(
       deviceContext,
       //x, y, width, height,
       //x, y, width, height,
       0, 0, windowWidth, windowHeight,
       0, 0, Buffer->Width, Buffer->Height,

       Buffer->Memory,
       &Buffer->Info,
       DIB_RGB_COLORS,
       SRCCOPY);
}


internal void
Win32DebugDrawLine(Win32OffscreenBuffer *offscreenBuffer, int32_t startx, int32_t starty, int32_t endx, int32_t endy, uint8_t r, uint8_t g, uint8_t b)
{
   //do a thing
}

internal inline void Win32Plot(Win32OffscreenBuffer *offscreenBuffer, int32_t x, int32_t y, uint32_t color)
{
   if(x >= 0 && x < offscreenBuffer->Width && y >= 0 && y < offscreenBuffer->Height)
   {
      ((uint32_t *)(offscreenBuffer->Memory))[(y * offscreenBuffer->Width) + x] = color;
   }
}

internal void
Win32DebugDrawVertical(Win32OffscreenBuffer *offscreenBuffer, int32_t x, int32_t y, int32_t height, uint8_t r, uint8_t g, uint8_t b)
{
   for(int32_t cur_y = y; cur_y < y + height; cur_y++)
   {
      Win32Plot(offscreenBuffer, x, cur_y, RGB_TO_UINT32(r,g,b));
   }
}

internal void
Win32DebugDrawCharacter(Win32OffscreenBuffer *offscreenBuffer, int32_t x, int32_t y, char c,uint32_t color ,bool shadow)
{
   for(int plot_y = 0; plot_y < Win32FixedFontHeight; ++plot_y)
   {
      for(int plot_x = 0; plot_x < Win32FixedFontWidth; ++plot_x)
      {
         if((Win32FixedFont[c][plot_y] & (1 << (8-plot_x))) > 0)
         {
            Win32Plot(offscreenBuffer, x + plot_x, y + plot_y , color);
            if(shadow)
            {
               Win32Plot(offscreenBuffer, x + plot_x + 1, y + plot_y + 1 , 0);
            }
         }
      }
   }
}


internal void
Win32DebugDrawText(Win32OffscreenBuffer *offscreenBuffer, int32_t x, int32_t y, char* string,uint8_t r, uint8_t g, uint8_t b,bool shadow)
{
   uint32_t current_x = x;
   uint32_t current_y = y;
   uint32_t color = RGB_TO_UINT32(r,g,b);
   for(char* c = string; *c != '\0'; ++c)
   {
      if(*c =='\n')
      {
         current_y += Win32FixedFontYAdvance;
         current_x = x;
      }
      else
      {
		  Win32DebugDrawCharacter(offscreenBuffer,current_x, current_y, *c, color, shadow);
		  current_x += Win32FixedFontXAdvance;
      }
   }
} 