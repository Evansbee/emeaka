
#include "emeaka.h"
#include "emeaka_intrinsics.h"
#include "emeaka_platform.h"
#include "emeaka_vector.h"
#include "emeaka_font.cpp"

void ClearBitmap(GameOffscreenBuffer *offscreenBuffer, float r, float g, float b)
{
   uint8_t _r = (uint8_t)Round(255.f * r);
   uint8_t _g = (uint8_t)Round(255.f * g);
   uint8_t _b = (uint8_t)Round(255.f * b);
   uint32_t color = RGB_TO_UINT32(_r, _g, _b);
   for (int y = 0; y < offscreenBuffer->TextureHeight; ++y)
   {
      for (int x = 0; x < offscreenBuffer->TextureWidth; ++x)
      {
         ((uint32_t *)(offscreenBuffer->Memory))[(y * offscreenBuffer->TextureWidth + x)] = color;
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
   
   if (p.x >= 0 && p.x < offscreenBuffer->TextureWidth && p.y >= 0 && p.y < offscreenBuffer->TextureHeight)
   {
      if(a == 1.0f)
      {
         ((uint32_t *)(offscreenBuffer->Memory))[(p.y * offscreenBuffer->TextureWidth) + p.x] = color.rgba;
      }
      else if (a > 0.f && a < 1.f)
      {
         RGBA currentColor;
         currentColor.rgba = ((uint32_t *)(offscreenBuffer->Memory))[(p.y * offscreenBuffer->TextureWidth) + p.x];

         color.r = Round(255.f * (r * a + ((float)currentColor.r/255.f) * (1.0f - a)));
         color.g = Round(255.f * (g * a + ((float)currentColor.g/255.f) * (1.0f - a)));
         color.b = Round(255.f * (b * a + ((float)currentColor.b/255.f) * (1.0f - a)));
         ((uint32_t *)(offscreenBuffer->Memory))[(p.y * offscreenBuffer->TextureWidth) + p.x] = color.rgba;
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

   if(x1 >= osb->TextureWidth)
   {
      x1 = osb->TextureWidth-1;
   }

   for(auto x = x0; x <= x1; ++x)
   {
      DrawPixel(osb,vec2i(x,p0.y),r,g,b,a);
   }
}

extern "C" void DrawLine(GameOffscreenBuffer *offscreenBuffer, vec2i p0, vec2i p1, float r, float g, float b, float a = 1.0f)
{
   bool steep = abs(p1.y - p0.y) > abs(p1.x - p0.x);
   
   if(p0.y == p1.y)
   {
      DrawHorizontalLine(offscreenBuffer,p0,p1,r,g,b,a);
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
         DrawPixel(offscreenBuffer,inv,r,g,b,a);
      }
      else
      {
         DrawPixel(offscreenBuffer, drawPt, r,g,b,a);
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
void StrokeCircle(GameOffscreenBuffer *offscreenBuffer, vec2i p, int radius, float r, float g, float b, float a = 1.0f)
{
   int64_t x = -radius, y = 0, err = 2 - 2 * radius;
   do
   {
      DrawPixel(offscreenBuffer,vec2i(p.x-x,p.y+y),r,g,b,a);
      DrawPixel(offscreenBuffer,vec2i(p.x-y,p.y-x),r,g,b,a);
      DrawPixel(offscreenBuffer,vec2i(p.x+x,p.y-y),r,g,b,a);
      DrawPixel(offscreenBuffer,vec2i(p.x+y,p.y+x),r,g,b,a);
      radius = err;
      if(radius <= y) err += ++y*2+1;
      if(radius > x || err > 7) err += ++x*2+1;
      /* code */
   } while (x<0);
   
}

void DrawCircle(GameOffscreenBuffer *offscreenBuffer, vec2i p, int radius, float r, float g, float b, float a = 1.0f)
{
   size_t draw = 0;
   int64_t x = -radius, y = 0, err = 2 - 2 * radius;
   bool drawy = true;
   do
   {
      if(drawy && y != 0)
      {
      DrawHorizontalLine(offscreenBuffer, vec2i(p.x-x,p.y+y), vec2i(p.x+x,p.y+y),r,g,b,a);      
      //DrawHorizontalLine(offscreenBuffer, vec2i(p.x-y,p.y-x), vec2i(p.x,p.y-x),r,g,b,a);
      DrawHorizontalLine(offscreenBuffer, vec2i(p.x+x,p.y-y), vec2i(p.x-x,p.y-y),r,g,b,a);
      //DrawHorizontalLine(offscreenBuffer, vec2i(p.x+y,p.y+x), vec2i(p.x,p.y+x),r,g,b,a);
      }
      radius = err;
      drawy = false;
      if(radius <= y) 
      {
         err += ++y*2+1;
         drawy = true;
      }
      if(radius > x || err > 7) err += ++x*2+1;
   } while (x<0);
   DrawHorizontalLine(offscreenBuffer, vec2i(p.x-radius/2,p.y), vec2i(p.x+radius/2,p.y),r,g,b,a); 
}

extern "C" void DrawChar(GameOffscreenBuffer *offscreenBuffer, vec2i p, char c, float r, float g, float b, float a, bool shadow)
{
   //wipe it
   if (p.x < -(int64_t)FixedFontWidth || p.x > offscreenBuffer->TextureWidth || p.y < -(int64_t)FixedFontHeight || p.y > offscreenBuffer->TextureHeight)
   {
      return;
   }

   for (int plot_y = 0; plot_y < FixedFontHeight; ++plot_y)
   {
      for (int plot_x = 0; plot_x < FixedFontWidth; ++plot_x)
      {
         if ((FixedFont[c][plot_y] & (1 << (8 - plot_x))) > 0)
         {
            DrawPixel(offscreenBuffer, vec2i(p.x + plot_x, p.y + plot_y), r, g, b,a);
            if (shadow)
            {
               DrawPixel(offscreenBuffer, vec2i(p.x + plot_x + 1, p.y + plot_y + 1), 0.f, 0.f, 0.f,a);
            }
         }
      }
   }
}

extern "C" void DrawText(GameOffscreenBuffer *offscreenBuffer, vec2i p, const char *text, float r, float g, float b, float a, bool shadow)
{
   vec2i current = p;
   for (const char *c = text; *c != '\0'; ++c)
   {
      if (*c == '\n')
      {
         current.y += FixedFontYAdvance;
         current.x = p.x;
      }
      else
      {
         DrawChar(offscreenBuffer, current, *c, r, g, b, a, shadow);
         current.x += FixedFontXAdvance;
      }
   }
}

void DrawNewChar(GameOffscreenBuffer *osb, vec2i p, const FontInformation* font, char c, float r, float g, float b, float a)
{
   int width = font->GlyphData[c-font->FirstGlyph].Width;
   int height = font->GlyphData[c-font->FirstGlyph].Height;
   GlyphInformation *ThisGlyph = &(font->GlyphData[c-font->FirstGlyph]);
   for(int ploty = 0; ploty < height; ++ploty)
   {
      for(int plotx = 0; plotx < width; ++plotx)
      {
         float avalue = (float)(font->FontBitmapData[ThisGlyph->DataBufferStartPoint + (ploty * width) + plotx])/255.f;
         if(avalue > 0.f)
         {
            DrawPixel(osb, vec2i(p.x + plotx + ThisGlyph->XOffset, p.y + ploty + ThisGlyph->YOffset), r, g, b, a*avalue);
         }
      }
   }
}
void DrawNewText(GameOffscreenBuffer *osb, vec2i p, const FontInformation* font, const char *text, float r, float g, float b, float a)
{
   if(text == nullptr)
   {
      printf("TEXT WAS NULL\n");
      return;
   }
   vec2i current = p;
   for (const char *c = text; *c != '\0'; ++c)
   {
      if(*c >= font->FirstGlyph && *c <= font->LastGlyph)
      {
         if (*c == '\n')
         {
            current.y += font->LineStep;
            current.x = p.x;
         }
         else
         {
            DrawNewChar(osb, current, font, *c, r, g, b, a);
            current.x += font->GlyphData[*c - font->FirstGlyph].XAdvance;
         }
      }
      else
      {
         {
            printf("BAD GLYPH: %c(%d)\n",*c,*c);
         }
      }
      
   }
}


extern "C" void DrawTriangle(GameOffscreenBuffer *offscreenBuffer, vec2i p0, vec2i p1, vec2i p2, float r, float g, float b, float a = 1.0f)
{
   //not sure why i chose a lambda here...
   auto VecSwap = [](vec2i &e1, vec2i &e2){
      vec2i temp(e1);
      e1 = e2;
      e2 = temp;
   };

   int64_t _a, _b, y, last;


   //sort in ascending y
   if(p0.y > p1.y) VecSwap(p0,p1);
   if(p1.y > p2.y) VecSwap(p1,p2);
   if(p0.y > p1.y) VecSwap(p0,p1);

   if(p0.y == p2.y)
   {
      _a = _b = p0.x;
      if(p1.x < 1) _a = p1.x;
      else if(p1.x > _b) b = p1.x;
      if(p2.x < _a) _a = p2.x;
      else if(p2.x > _b) _b = p2.x;
      DrawHorizontalLine(offscreenBuffer,vec2i(_a,p0.y),vec2i(_b,p0.y),r,g,b,a);
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
      _a = p0.x + sa/dy01;
      _b = p0.x + sb/dy02;
      sa += dx01;
      sb += dx02;

      if(_a>_b)
      {
          int64_t tmp = _a;
          _a = _b; _b = tmp;
      }
      DrawHorizontalLine(offscreenBuffer,vec2i(_a,y),vec2i(_b,y),r,g,b,a);
     
   }

   sa = dx12 * (y - p1.y);
   sb = dx02 * (y - p0.y);

   for(;y <= p2.y; ++y)
   {
      _a = p1.x + sa/dy12;
      _b = p0.x + sb/dy02;
      sa+=dx12;
      sb+=dx02;
      if(a>_b)
      {
          int64_t tmp = _a;
          _a = _b; _b = tmp;
      }
      DrawHorizontalLine(offscreenBuffer,vec2i(_a,y),vec2i(_b,y),r,g,b,a);
   }
}
extern "C" void StrokeTriangle(GameOffscreenBuffer *offscreenBuffer, vec2i p0, vec2i p1, vec2i p2, float r, float g, float b, float a = 1.0f)
{
   DrawLine(offscreenBuffer,p0,p1,r,g,b, a);
   DrawLine(offscreenBuffer,p1,p2,r,g,b, a);
   DrawLine(offscreenBuffer,p2,p0,r,g,b, a);
}
extern "C" void DrawRect(GameOffscreenBuffer *offscreenBuffer, vec2i p0, vec2i p1, float r, float g, float b, float a = 1.0f)
{
   vec2i p_ur(p1.x, p0.y);
   vec2i p_ll(p0.x, p1.y);

   DrawTriangle(offscreenBuffer,p0,p_ur,p1,r,g,b,a);
   DrawTriangle(offscreenBuffer,p1,p_ll,p0,r,g,b,a);

   return;
}
extern "C" void StrokeRect(GameOffscreenBuffer *offscreenBuffer, vec2i p0, vec2i p1, float r, float g, float b,float a= 1.0f)
{
   vec2i p_ur(p1.x, p0.y);
   vec2i p_ll(p0.x, p1.y);
   DrawLine(offscreenBuffer,p0,p_ur,r,g,b,a);
   DrawLine(offscreenBuffer,p_ur,p1,r,g,b,a);
   DrawLine(offscreenBuffer,p1,p_ll,r,g,b,a);
   DrawLine(offscreenBuffer,p_ll,p0,r,g,b,a);
}

