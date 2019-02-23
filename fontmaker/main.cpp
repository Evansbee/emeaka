#include <stdio.h>
#include <vector>
#include <cinttypes>
#include <cstdlib>

#include <ft2build.h>
#include FT_GLYPH_H
#include FT_TRUETYPE_DRIVER_H

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
    FT_Library library;
    FT_Face face;
    FT_Glyph glyph;
    FT_Bitmap *ftbitmap;
    FT_BitmapGlyphRec *g;
    int bitcount = 1;
    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s FontFile Size FontName [Bits{1,8}]\n", argv[0]);
        return -1;
    }
    unsigned char *bitmap;
    int w, h, xoff, yoff, i, j, s = atoi(argv[2]);
    char *fontName = argv[3];

    if(argc==5)
    {
        bitcount = atoi(argv[4]);
        if(bitcount != 1 || bitcount != 8)
        {
            bitcount = 1;
        }
    }

    int err;
    if ((err = FT_Init_FreeType(&library)))
    {
        fprintf(stderr, "FreeType init error: %d", err);
        return err;
    }

    if ((err = FT_New_Face(library, argv[1], 0, &face)))
    {
        fprintf(stderr, "Font load error: %d", err);
        FT_Done_FreeType(library);
        return err;
    }

    //FT_Set_Char_Size(face, s << 6, 0, 0, 0);
    FT_Set_Pixel_Sizes(
        face, /* handle to face object */
        0,    /* pixel_width           */
        s);
    char outputFileName[128];
    char debugOutputFileName[128];
    sprintf(outputFileName, "%s%dptFont.h", fontName, s);
    sprintf(debugOutputFileName, "%s%dptFontDebug.txt", fontName, s);
    FILE *outputFile = fopen(outputFileName, "w");
    FILE *debugOutputFile = fopen(debugOutputFileName, "w");
    FontData.FirstGlyph = 0x20;
    FontData.LastGlyph = 0x7E;

    for (int c = FontData.FirstGlyph; c <= FontData.LastGlyph; ++c)
    {
        FT_Render_Mode render_mode = (bitcount==1)?FT_RENDER_MODE_MONO:FT_RENDER_MODE_NORMAL;
        GlyphInformation *ThisGlyph = (GlyphInformation *)malloc(sizeof(GlyphInformation));
        if ((err = FT_Load_Char(face, c, FT_LOAD_TARGET_(render_mode))))
        {
            fprintf(stderr, "Error %d loading char '%c'\n", err, c);
            continue;
        }

        if ((err = FT_Render_Glyph(face->glyph, render_mode)))
        {
            fprintf(stderr, "Error %d rendering char '%c'\n",
                    err, c);
            continue;
        }

        if ((err = FT_Get_Glyph(face->glyph, &glyph)))
        {
            fprintf(stderr, "Error %d getting glyph '%c'\n",
                    err, i);
            continue;
        }

        ftbitmap = &face->glyph->bitmap;
        g = (FT_BitmapGlyphRec *)glyph;

        ThisGlyph->CodePoint = c;
        ThisGlyph->DataBufferStartPoint = RenderBuffer.size();
        ThisGlyph->Width = ftbitmap->width;
        ThisGlyph->Height = ftbitmap->rows;
        ThisGlyph->XAdvance = face->glyph->advance.x >> 6;
        ThisGlyph->XOffset = g->left;
        ThisGlyph->YOffset = 1 - g->top;

        GlyphData.push_back(ThisGlyph);
        fprintf(debugOutputFile,"Character: %c\n\n",c);
        for (j = 0; j < ThisGlyph->Height; ++j)
        {
            for (i = 0; i < ThisGlyph->Width; ++i)
            {
                if (render_mode == FT_RENDER_MODE_MONO)
                {
                    size_t byte = i / 8;
                    size_t bit = 0x80 >> (i & 7);
                    if ((ftbitmap->buffer[j * ftbitmap->pitch + byte] & bit) > 0)
                    {
                        RenderBuffer.push_back(0xFF);
                    }
                    else
                    {
                        RenderBuffer.push_back(0);
                    }
                }
                else
                {
                    RenderBuffer.push_back(ftbitmap->buffer[j*ftbitmap->pitch + i]);
                }
                size_t last = RenderBuffer.size() - 1;
                fprintf(debugOutputFile,"%c"," .:ioVM@"[RenderBuffer[last]>>5]);
            }
            fprintf(debugOutputFile,"\n");
        }
        fprintf(debugOutputFile,"\n\n\n");
        FT_Done_Glyph(glyph);
    }

    fprintf(outputFile, "#pragma once\n\n#include \"emeaka.h\"\n#include <cinttypes>\n\n\n\n");
    fprintf(outputFile, "static const uint8_t %s%dptBitmaps[] = {\n   ", fontName, s);
    int cnt = 0;
    for (auto d : RenderBuffer)
    {
        fprintf(outputFile, "0x%02x, ", d);
        if (cnt++ == 20)
        {
            fprintf(outputFile, "\n   ");
            cnt = 0;
        }
    }
    fprintf(outputFile, "};\n\n");

    fprintf(outputFile, "//start, height, width, yoffset, xoffset, xadvance\n");
    fprintf(outputFile, "static const GlyphInformation %s%dptGlyphs[] = {\n", fontName, s);
    for (auto &g : GlyphData)
    {
        fprintf(outputFile, "   { %zu, %d, %d, %d, %d, %d },  //0x%02x = '%c'\n", g->DataBufferStartPoint, g->Height, g->Width, g->YOffset, g->XOffset, g->XAdvance, g->CodePoint, g->CodePoint);
    }
    fprintf(outputFile, "};\n\n");

    fprintf(outputFile, "static const FontInformation %s%dptFont = {\n", fontName, s);
    fprintf(outputFile, "   0x%02x, 0x%02x,\n", FontData.FirstGlyph, FontData.LastGlyph);
    fprintf(outputFile, "   %ld,\n", face->size->metrics.height >> 6);
    fprintf(outputFile, "   (uint8_t *)%s%dptBitmaps,\n", fontName, s);
    fprintf(outputFile, "   (GlyphInformation *)%s%dptGlyphs\n};\n\n\n", fontName, s);
    fclose(outputFile);
    fclose(debugOutputFile);
    FT_Done_FreeType(library);
    return 0;
}