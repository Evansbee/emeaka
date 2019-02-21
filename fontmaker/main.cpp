#include <stdio.h>
#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include "stb_truetype.h"

unsigned char ttf_buffer[1<<25];

int main(int argc, char **argv)
{
   stbtt_fontinfo font;
   unsigned char *bitmap;
   int w,h,xoff,yoff,i,j,c = (argc > 1 ? (argv[1][0]) : 'a'), s = (argc > 2 ? atoi(argv[2]) : 20);
   fread(ttf_buffer, 1, 1<<25, fopen(argc > 3 ? argv[3] : "DejaVuSans.ttf", "rb"));

    for(int c = 0x20; c <= 0xFE; ++c)
    {

   stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0));
   bitmap = stbtt_GetCodepointBitmap(&font, 0,stbtt_ScaleForPixelHeight(&font, s), c, &w, &h, &xoff,&yoff);
   
   int ascent, descent, linegap;
   stbtt_GetFontVMetrics(&font,&ascent, &descent, &linegap);

    int advance, leftside;
   stbtt_GetCodepointHMetrics(&font,c,&advance,&leftside);

   for (j=0; j < h; ++j) {
      for (i=0; i < w; ++i)
         putchar(" .:ioVM@"[bitmap[j*w+i]>>5]);
      putchar('\n');
   }
   }
   return 0;
}