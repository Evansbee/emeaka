#include <stdio.h>
#define STB_TRUETYPE_IMPLEMENTATION // force following include to generate implementation
#include "stb_truetype.h"
#include <vector>
#include <cinttypes>
#include <cstdlib>

unsigned char ttf_buffer[1 << 25];

struct FontInformation
{
    int Ascent;
    int Descent;
    int LineGap;
    int FirstGlyph;
    int LastGlyph;
};

struct GlyphInformation
{
    int CodePoint;
    int Width;
    int Height;
    int XOffset;
    int YOffset;
    int XAdvance;
    size_t DataBufferStartPoint;
};

FontInformation FontData;
std::vector<GlyphInformation *> GlyphData;
std::vector<uint8_t> RenderBuffer;

int main(int argc, char **argv)
{
    stbtt_fontinfo font;
    unsigned char *bitmap;
    int w, h, xoff, yoff, i, j, s = (argc > 1 ? atoi(argv[1]) : 20);
    fread(ttf_buffer, 1, 1 << 25, fopen(argc > 2 ? argv[2] : "DejaVuSans.ttf", "rb"));

    FontData.FirstGlyph = 0x20;
    FontData.LastGlyph = 0x7E;

    stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer, 0));
    float SF = stbtt_ScaleForPixelHeight(&font, s);
    stbtt_GetFontVMetrics(&font, &FontData.Ascent, &FontData.Descent, &FontData.LineGap);
    
    for (int c = FontData.FirstGlyph; c <= FontData.LastGlyph; ++c)
    {
        GlyphInformation* ThisGlyph = (GlyphInformation *)malloc(sizeof(GlyphInformation));
        ThisGlyph->CodePoint = c;
        ThisGlyph->DataBufferStartPoint = RenderBuffer.size();
        bitmap = stbtt_GetCodepointBitmap(&font, 0, SF, c, &ThisGlyph->Width, &ThisGlyph->Height, &ThisGlyph->XOffset, &ThisGlyph->YOffset);
        int advance, leftside;
        stbtt_GetCodepointHMetrics(&font, c, &advance, &leftside);
        advance = advance * SF;
        ThisGlyph->XAdvance = advance;
        GlyphData.push_back(ThisGlyph);
        for (j = 0; j < ThisGlyph->Height; ++j)
        {
            for (i = 0; i < ThisGlyph->Width; ++i)
            {
                RenderBuffer.push_back(bitmap[j * w + i]);
            }
        }
    }

    printf("RenderBuffer Size: %zu\n",RenderBuffer.size());
    return 0;
}