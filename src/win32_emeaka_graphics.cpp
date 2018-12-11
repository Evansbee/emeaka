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

