#include <stdio.h>
#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include "stb_truetype.h"

unsigned char ttf_buffer[1<<25];

int main(int argc, char **argv)
{
   stbtt_fontinfo font;
   unsigned char *bitmap;
   int w,h,xoff,yoff,i,j,c, s = (argc > 1 ? atoi(argv[1]) : 20);
   fread(ttf_buffer, 1, 1<<25, fopen(argc > 3 ? argv[3] : "DejaVuSans.ttf", "rb"));

    for(c = 0x20; c <= 0x7E; ++c)
    {
            stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0));
    float SF = stbtt_ScaleForPixelHeight(&font, s);
    bitmap = stbtt_GetCodepointBitmap(&font, 0, SF, c, &w, &h, &xoff,&yoff);
   
   int ascent, descent, linegap;
   stbtt_GetFontVMetrics(&font,&ascent, &descent, &linegap);

int advance, leftside;
   stbtt_GetCodepointHMetrics(&font,c,&advance,&leftside);
    advance = advance * SF;

    

printf("Code point: %c\n",c);
printf("w: %d h: %d xoff: %d yoff: %d advance: %d\n",w,h,xoff,yoff,advance);
   for (j=0; j < h; ++j) {
      for (i=0; i < w; ++i)
         putchar(" .:ioVM@"[bitmap[j*w+i]>>5]);
      putchar('\n');
   }
   }
   return 0;
}