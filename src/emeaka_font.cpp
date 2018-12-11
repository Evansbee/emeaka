//mapped in font is 6x13 -- a single unsigned char holds a ROW of font data.  
const size_t FixedFontWidth = 6;
const size_t FixedFontHeight = 13;
const size_t FixedFontAscent = 11;
const size_t FixedFontXAdvance = FixedFontWidth + 1;
const size_t FixedFontYAdvance = FixedFontHeight;

const unsigned char FixedFont[][13] = {
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x78,0x84,0xcc,0x84,0xb4,0x84,0xcc,0xb4,0x84,0x78,0x00},
{0x00,0x00,0x78,0xfc,0xb4,0xfc,0xcc,0xfc,0xb4,0xcc,0x78,0x00,0x00},
{0x00,0x00,0x00,0x00,0x50,0xf8,0xf8,0xf8,0x70,0x20,0x20,0x00,0x00},
{0x00,0x00,0x00,0x00,0x20,0x70,0x70,0xf8,0x70,0x70,0x20,0x00,0x00},
{0x00,0x00,0x20,0x70,0x20,0xa8,0xf8,0xf8,0xa8,0x20,0x70,0x00,0x00},
{0x00,0x00,0x00,0x20,0x20,0x70,0x70,0xf8,0xf8,0x20,0x70,0x00,0x00},
{0x00,0x00,0x00,0x00,0x70,0xf8,0xf8,0xf8,0x70,0x00,0x00,0x00,0x00},
{0xfc,0xfc,0xfc,0xfc,0xfc,0xcc,0x84,0x84,0xcc,0xfc,0xfc,0xfc,0xfc},
{0x00,0x00,0x00,0x00,0x70,0x88,0x88,0x88,0x70,0x00,0x00,0x00,0x00},
{0xfc,0xfc,0xfc,0xfc,0xcc,0xb4,0x78,0x78,0xb4,0xcc,0xfc,0xfc,0xfc},
{0x00,0x00,0x00,0x00,0x1c,0x0c,0x74,0x88,0x88,0x88,0x70,0x00,0x00},
{0x00,0x00,0x00,0x70,0x88,0x88,0x88,0x70,0x20,0x70,0x20,0x00,0x00},
{0x00,0x00,0x10,0x18,0x14,0x10,0x10,0x10,0x70,0xf0,0x60,0x00,0x00},
{0x00,0x00,0x40,0x60,0x50,0x48,0x48,0xc8,0xc8,0x18,0x18,0x00,0x00},
{0x00,0x00,0x20,0x20,0x88,0x20,0x50,0x20,0x88,0x20,0x20,0x00,0x00},
{0x00,0x00,0x80,0xc0,0xe0,0xf0,0xf8,0xf0,0xe0,0xc0,0x80,0x00,0x00},
{0x00,0x00,0x08,0x18,0x38,0x78,0xf8,0x78,0x38,0x18,0x08,0x00,0x00},
{0x00,0x00,0x20,0x70,0xa8,0x20,0x20,0x20,0xa8,0x70,0x20,0x00,0x00},
{0x00,0x00,0x50,0x50,0x50,0x50,0x50,0x50,0x50,0x00,0x50,0x00,0x00},
{0x00,0x00,0x78,0xe8,0xe8,0xe8,0xe8,0x68,0x28,0x28,0x28,0x00,0x00},
{0x00,0x30,0x48,0x40,0x30,0x48,0x48,0x30,0x08,0x48,0x30,0x00,0x00},
{0x00,0x00,0x00,0x00,0xfc,0xfc,0xfc,0xfc,0xfc,0x00,0x00,0x00,0x00},
{0x00,0x00,0x20,0x70,0xa8,0x20,0x20,0xa8,0x70,0x20,0xf8,0x00,0x00},
{0x00,0x00,0x20,0x70,0xa8,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00},
{0x00,0x00,0x20,0x20,0x20,0x20,0x20,0x20,0xa8,0x70,0x20,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x20,0x10,0xf8,0x10,0x20,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x20,0x40,0xf8,0x40,0x20,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0xf8,0x80,0x80,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x48,0xfc,0x48,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x20,0x20,0x20,0x70,0x70,0x70,0xf8,0xf8,0xf8,0x00,0x00},
{0x00,0x00,0xf8,0xf8,0xf8,0x70,0x70,0x70,0x20,0x20,0x20,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x20,0x00,0x00},
{0x00,0x00,0x50,0x50,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x50,0x50,0xf8,0x50,0xf8,0x50,0x50,0x00,0x00,0x00},
{0x00,0x00,0x20,0x78,0xa0,0xa0,0x70,0x28,0x28,0xf0,0x20,0x00,0x00},
{0x00,0x00,0x48,0xa8,0x50,0x10,0x20,0x40,0x50,0xa8,0x90,0x00,0x00},
{0x00,0x00,0x00,0x40,0xa0,0xa0,0x40,0xa0,0x98,0x90,0x68,0x00,0x00},
{0x00,0x00,0x30,0x20,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x10,0x20,0x20,0x40,0x40,0x40,0x40,0x40,0x20,0x20,0x10,0x00},
{0x00,0x40,0x20,0x20,0x10,0x10,0x10,0x10,0x10,0x20,0x20,0x40,0x00},
{0x00,0x00,0x20,0xa8,0x70,0xa8,0x20,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x20,0x20,0xf8,0x20,0x20,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x20,0x40,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0xf8,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x70,0x20,0x00},
{0x00,0x00,0x08,0x08,0x10,0x10,0x20,0x40,0x40,0x80,0x80,0x00,0x00},
{0x00,0x00,0x20,0x50,0x88,0x88,0x88,0x88,0x88,0x50,0x20,0x00,0x00},
{0x00,0x00,0x20,0x60,0xa0,0x20,0x20,0x20,0x20,0x20,0xf8,0x00,0x00},
{0x00,0x00,0x70,0x88,0x88,0x08,0x10,0x20,0x40,0x80,0xf8,0x00,0x00},
{0x00,0x00,0xf8,0x08,0x10,0x20,0x70,0x08,0x08,0x88,0x70,0x00,0x00},
{0x00,0x00,0x10,0x10,0x30,0x50,0x50,0x90,0xf8,0x10,0x10,0x00,0x00},
{0x00,0x00,0xf8,0x80,0x80,0xb0,0xc8,0x08,0x08,0x88,0x70,0x00,0x00},
{0x00,0x00,0x70,0x88,0x80,0x80,0xf0,0x88,0x88,0x88,0x70,0x00,0x00},
{0x00,0x00,0xf8,0x08,0x10,0x10,0x20,0x20,0x40,0x40,0x40,0x00,0x00},
{0x00,0x00,0x70,0x88,0x88,0x88,0x70,0x88,0x88,0x88,0x70,0x00,0x00},
{0x00,0x00,0x70,0x88,0x88,0x88,0x78,0x08,0x08,0x88,0x70,0x00,0x00},
{0x00,0x00,0x00,0x00,0x20,0x70,0x20,0x00,0x00,0x20,0x70,0x20,0x00},
{0x00,0x00,0x00,0x00,0x20,0x70,0x20,0x00,0x00,0x30,0x20,0x40,0x00},
{0x00,0x00,0x08,0x10,0x20,0x40,0x80,0x40,0x20,0x10,0x08,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0xf8,0x00,0x00,0xf8,0x00,0x00,0x00,0x00},
{0x00,0x00,0x80,0x40,0x20,0x10,0x08,0x10,0x20,0x40,0x80,0x00,0x00},
{0x00,0x00,0x70,0x88,0x88,0x08,0x10,0x20,0x20,0x00,0x20,0x00,0x00},
{0x00,0x00,0x70,0x88,0x88,0x98,0xa8,0xa8,0xb0,0x80,0x78,0x00,0x00},
{0x00,0x00,0x20,0x50,0x88,0x88,0x88,0xf8,0x88,0x88,0x88,0x00,0x00},
{0x00,0x00,0xf0,0x48,0x48,0x48,0x70,0x48,0x48,0x48,0xf0,0x00,0x00},
{0x00,0x00,0x70,0x88,0x80,0x80,0x80,0x80,0x80,0x88,0x70,0x00,0x00},
{0x00,0x00,0xf0,0x48,0x48,0x48,0x48,0x48,0x48,0x48,0xf0,0x00,0x00},
{0x00,0x00,0xf8,0x80,0x80,0x80,0xf0,0x80,0x80,0x80,0xf8,0x00,0x00},
{0x00,0x00,0xf8,0x80,0x80,0x80,0xf0,0x80,0x80,0x80,0x80,0x00,0x00},
{0x00,0x00,0x70,0x88,0x80,0x80,0x80,0x98,0x88,0x88,0x70,0x00,0x00},
{0x00,0x00,0x88,0x88,0x88,0x88,0xf8,0x88,0x88,0x88,0x88,0x00,0x00},
{0x00,0x00,0x70,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x70,0x00,0x00},
{0x00,0x00,0x38,0x10,0x10,0x10,0x10,0x10,0x10,0x90,0x60,0x00,0x00},
{0x00,0x00,0x88,0x88,0x90,0xa0,0xc0,0xa0,0x90,0x88,0x88,0x00,0x00},
{0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xf8,0x00,0x00},
{0x00,0x00,0x88,0x88,0xd8,0xa8,0xa8,0x88,0x88,0x88,0x88,0x00,0x00},
{0x00,0x00,0x88,0xc8,0xc8,0xa8,0xa8,0x98,0x98,0x88,0x88,0x00,0x00},
{0x00,0x00,0x70,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x70,0x00,0x00},
{0x00,0x00,0xf0,0x88,0x88,0x88,0xf0,0x80,0x80,0x80,0x80,0x00,0x00},
{0x00,0x00,0x70,0x88,0x88,0x88,0x88,0x88,0x88,0xa8,0x70,0x08,0x00},
{0x00,0x00,0xf0,0x88,0x88,0x88,0xf0,0xa0,0x90,0x88,0x88,0x00,0x00},
{0x00,0x00,0x70,0x88,0x80,0x80,0x70,0x08,0x08,0x88,0x70,0x00,0x00},
{0x00,0x00,0xf8,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00},
{0x00,0x00,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x70,0x00,0x00},
{0x00,0x00,0x88,0x88,0x88,0x88,0x50,0x50,0x50,0x20,0x20,0x00,0x00},
{0x00,0x00,0x88,0x88,0x88,0x88,0xa8,0xa8,0xa8,0xa8,0x50,0x00,0x00},
{0x00,0x00,0x88,0x88,0x50,0x50,0x20,0x50,0x50,0x88,0x88,0x00,0x00},
{0x00,0x00,0x88,0x88,0x50,0x50,0x20,0x20,0x20,0x20,0x20,0x00,0x00},
{0x00,0x00,0xf8,0x08,0x10,0x10,0x20,0x40,0x40,0x80,0xf8,0x00,0x00},
{0x00,0x70,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x70,0x00},
{0x00,0x00,0x80,0x80,0x40,0x40,0x20,0x10,0x10,0x08,0x08,0x00,0x00},
{0x00,0x70,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x70,0x00},
{0x00,0x00,0x20,0x50,0x88,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf8,0x00},
{0x00,0x00,0x30,0x10,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x70,0x08,0x78,0x88,0x98,0x68,0x00,0x00},
{0x00,0x00,0x80,0x80,0x80,0xf0,0x88,0x88,0x88,0x88,0xf0,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x70,0x88,0x80,0x80,0x88,0x70,0x00,0x00},
{0x00,0x00,0x08,0x08,0x08,0x78,0x88,0x88,0x88,0x88,0x78,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x70,0x88,0xf8,0x80,0x88,0x70,0x00,0x00},
{0x00,0x00,0x30,0x48,0x40,0x40,0xf0,0x40,0x40,0x40,0x40,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x70,0x88,0x88,0x88,0x78,0x08,0x88,0x70},
{0x00,0x00,0x80,0x80,0x80,0xb0,0xc8,0x88,0x88,0x88,0x88,0x00,0x00},
{0x00,0x00,0x00,0x20,0x00,0x60,0x20,0x20,0x20,0x20,0x70,0x00,0x00},
{0x00,0x00,0x00,0x10,0x00,0x30,0x10,0x10,0x10,0x10,0x90,0x90,0x60},
{0x00,0x00,0x80,0x80,0x80,0x90,0xa0,0xc0,0xa0,0x90,0x88,0x00,0x00},
{0x00,0x00,0x60,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x70,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0xd0,0xa8,0xa8,0xa8,0xa8,0x88,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0xb0,0xc8,0x88,0x88,0x88,0x88,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x70,0x88,0x88,0x88,0x88,0x70,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0xf0,0x88,0x88,0x88,0xf0,0x80,0x80,0x80},
{0x00,0x00,0x00,0x00,0x00,0x78,0x88,0x88,0x88,0x78,0x08,0x08,0x08},
{0x00,0x00,0x00,0x00,0x00,0xb0,0xc8,0x80,0x80,0x80,0x80,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x70,0x88,0x60,0x10,0x88,0x70,0x00,0x00},
{0x00,0x00,0x00,0x40,0x40,0xf0,0x40,0x40,0x40,0x48,0x30,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x88,0x88,0x88,0x88,0x98,0x68,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x88,0x88,0x88,0x50,0x50,0x20,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x88,0x88,0xa8,0xa8,0xa8,0x50,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x88,0x50,0x20,0x20,0x50,0x88,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x88,0x88,0x88,0x98,0x68,0x08,0x88,0x70},
{0x00,0x00,0x00,0x00,0x00,0xf8,0x10,0x20,0x40,0x80,0xf8,0x00,0x00},
{0x00,0x18,0x20,0x20,0x20,0x20,0xc0,0x20,0x20,0x20,0x20,0x18,0x00},
{0x00,0x00,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00},
{0x00,0xc0,0x20,0x20,0x20,0x20,0x18,0x20,0x20,0x20,0x20,0xc0,0x00},
{0x00,0x00,0x48,0xa8,0x90,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x70,0x88,0x80,0x80,0x80,0x80,0x80,0x88,0x70,0x20,0x40},
{0x00,0x00,0x50,0x50,0x00,0x88,0x88,0x88,0x88,0x98,0x68,0x00,0x00},
{0x00,0x00,0x10,0x20,0x00,0x70,0x88,0xf8,0x80,0x88,0x70,0x00,0x00},
{0x00,0x00,0x30,0x48,0x00,0x70,0x08,0x78,0x88,0x98,0x68,0x00,0x00},
{0x00,0x00,0x50,0x50,0x00,0x70,0x08,0x78,0x88,0x98,0x68,0x00,0x00},
{0x00,0x00,0x40,0x20,0x00,0x70,0x08,0x78,0x88,0x98,0x68,0x00,0x00},
{0x00,0x30,0x48,0x30,0x00,0x70,0x08,0x78,0x88,0x98,0x68,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x70,0x88,0x80,0x80,0x88,0x70,0x20,0x40},
{0x00,0x00,0x30,0x48,0x00,0x70,0x88,0xf8,0x80,0x88,0x70,0x00,0x00},
{0x00,0x00,0x50,0x50,0x00,0x70,0x88,0xf8,0x80,0x88,0x70,0x00,0x00},
{0x00,0x00,0x40,0x20,0x00,0x70,0x88,0xf8,0x80,0x88,0x70,0x00,0x00},
{0x00,0x00,0x50,0x50,0x00,0x60,0x20,0x20,0x20,0x20,0x70,0x00,0x00},
{0x00,0x00,0x30,0x48,0x00,0x60,0x20,0x20,0x20,0x20,0x70,0x00,0x00},
{0x00,0x00,0x40,0x20,0x00,0x60,0x20,0x20,0x20,0x20,0x70,0x00,0x00},
{0x00,0x50,0x50,0x00,0x20,0x50,0x88,0x88,0xf8,0x88,0x88,0x00,0x00},
{0x00,0x20,0x50,0x20,0x20,0x50,0x88,0x88,0xf8,0x88,0x88,0x00,0x00},
{0x00,0x10,0x20,0x00,0xf8,0x80,0x80,0xf0,0x80,0x80,0xf8,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x70,0x28,0x70,0xa0,0xa8,0x50,0x00,0x00},
{0x00,0x00,0x58,0xa0,0xa0,0xa0,0xb0,0xe0,0xa0,0xa0,0xb8,0x00,0x00},
{0x00,0x00,0x30,0x48,0x00,0x70,0x88,0x88,0x88,0x88,0x70,0x00,0x00},
{0x00,0x00,0x50,0x50,0x00,0x70,0x88,0x88,0x88,0x88,0x70,0x00,0x00},
{0x00,0x00,0x40,0x20,0x00,0x70,0x88,0x88,0x88,0x88,0x70,0x00,0x00},
{0x00,0x00,0x30,0x48,0x00,0x88,0x88,0x88,0x88,0x98,0x68,0x00,0x00},
{0x00,0x00,0x40,0x20,0x00,0x88,0x88,0x88,0x88,0x98,0x68,0x00,0x00},
{0x00,0x00,0x50,0x50,0x00,0x88,0x88,0x88,0x98,0x68,0x08,0x88,0x70},
{0x00,0x50,0x50,0x00,0x70,0x88,0x88,0x88,0x88,0x88,0x70,0x00,0x00},
{0x00,0x50,0x50,0x00,0x88,0x88,0x88,0x88,0x88,0x88,0x70,0x00,0x00},
{0x00,0x00,0x20,0x70,0xa8,0xa0,0xa0,0xa8,0x70,0x20,0x00,0x00,0x00},
{0x00,0x00,0x30,0x48,0x40,0x40,0xe0,0x40,0x40,0x48,0xb0,0x00,0x00},
{0x00,0x00,0x88,0x88,0x50,0x50,0xf8,0x20,0xf8,0x20,0x20,0x00,0x00},
{0x00,0x00,0x70,0x48,0xfc,0x48,0x70,0x40,0x40,0x40,0x40,0x00,0x00},
{0x00,0x00,0x10,0x28,0x20,0x20,0x70,0x20,0x20,0x20,0x20,0xa0,0x40},
{0x00,0x00,0x10,0x20,0x00,0x70,0x08,0x78,0x88,0x98,0x68,0x00,0x00},
{0x00,0x00,0x10,0x20,0x00,0x60,0x20,0x20,0x20,0x20,0x70,0x00,0x00},
{0x00,0x00,0x10,0x20,0x00,0x70,0x88,0x88,0x88,0x88,0x70,0x00,0x00},
{0x00,0x00,0x10,0x20,0x00,0x88,0x88,0x88,0x88,0x98,0x68,0x00,0x00},
{0x00,0x00,0x28,0x50,0x00,0xb0,0xc8,0x88,0x88,0x88,0x88,0x00,0x00},
{0x00,0x28,0x50,0x00,0x88,0x88,0xc8,0xa8,0x98,0x88,0x88,0x00,0x00},
{0x00,0x00,0x70,0x08,0x78,0x88,0x78,0x00,0xf8,0x00,0x00,0x00,0x00},
{0x00,0x00,0x70,0x88,0x88,0x88,0x70,0x00,0xf8,0x00,0x00,0x00,0x00},
{0x00,0x00,0x20,0x00,0x20,0x20,0x40,0x80,0x88,0x88,0x70,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0xf8,0x80,0x80,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0xf8,0x08,0x08,0x00,0x00,0x00,0x00},
{0x00,0x40,0xc0,0x40,0x40,0xe0,0x10,0x28,0x08,0x10,0x38,0x00,0x00},
{0x00,0x40,0xc0,0x40,0x40,0xe0,0x08,0x18,0x28,0x38,0x08,0x00,0x00},
{0x00,0x00,0x20,0x00,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00},
{0x00,0x00,0x00,0x00,0x28,0x50,0xa0,0xa0,0x50,0x28,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0xa0,0x50,0x28,0x28,0x50,0xa0,0x00,0x00,0x00},
{0xa8,0x00,0x54,0x00,0xa8,0x00,0x54,0x00,0xa8,0x00,0x54,0x00,0xa8},
{0xa8,0x54,0xa8,0x54,0xa8,0x54,0xa8,0x54,0xa8,0x54,0xa8,0x54,0xa8},
{0x54,0xfc,0xa8,0xfc,0x54,0xfc,0xa8,0xfc,0x54,0xfc,0xa8,0xfc,0x54},
{0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
{0x20,0x20,0x20,0x20,0x20,0x20,0xe0,0x20,0x20,0x20,0x20,0x20,0x20},
{0x20,0x20,0x20,0x20,0x20,0xe0,0x20,0xe0,0x20,0x20,0x20,0x20,0x20},
{0x50,0x50,0x50,0x50,0x50,0x50,0xd0,0x50,0x50,0x50,0x50,0x50,0x50},
{0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x50,0x50,0x50,0x50,0x50,0x50},
{0x00,0x00,0x00,0x00,0x00,0xe0,0x20,0xe0,0x20,0x20,0x20,0x20,0x20},
{0x50,0x50,0x50,0x50,0x50,0xd0,0x10,0xd0,0x50,0x50,0x50,0x50,0x50},
{0x50,0x50,0x50,0x50,0x50,0x50,0x50,0x50,0x50,0x50,0x50,0x50,0x50},
{0x00,0x00,0x00,0x00,0x00,0xf0,0x10,0xd0,0x50,0x50,0x50,0x50,0x50},
{0x50,0x50,0x50,0x50,0x50,0xd0,0x10,0xf0,0x00,0x00,0x00,0x00,0x00},
{0x50,0x50,0x50,0x50,0x50,0x50,0xf0,0x00,0x00,0x00,0x00,0x00,0x00},
{0x20,0x20,0x20,0x20,0x20,0xe0,0x20,0xe0,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0xe0,0x20,0x20,0x20,0x20,0x20,0x20},
{0x20,0x20,0x20,0x20,0x20,0x20,0x3c,0x00,0x00,0x00,0x00,0x00,0x00},
{0x20,0x20,0x20,0x20,0x20,0x20,0xfc,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0xfc,0x20,0x20,0x20,0x20,0x20,0x20},
{0x20,0x20,0x20,0x20,0x20,0x20,0x3c,0x20,0x20,0x20,0x20,0x20,0x20},
{0x00,0x00,0x00,0x00,0x00,0x00,0xfc,0x00,0x00,0x00,0x00,0x00,0x00},
{0x20,0x20,0x20,0x20,0x20,0x20,0xfc,0x20,0x20,0x20,0x20,0x20,0x20},
{0x20,0x20,0x20,0x20,0x20,0x3c,0x20,0x3c,0x20,0x20,0x20,0x20,0x20},
{0x50,0x50,0x50,0x50,0x50,0x50,0x5c,0x50,0x50,0x50,0x50,0x50,0x50},
{0x50,0x50,0x50,0x50,0x50,0x5c,0x40,0x7c,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x7c,0x40,0x5c,0x50,0x50,0x50,0x50,0x50},
{0x50,0x50,0x50,0x50,0x50,0xdc,0x00,0xfc,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0xfc,0x00,0xdc,0x50,0x50,0x50,0x50,0x50},
{0x50,0x50,0x50,0x50,0x50,0x5c,0x40,0x5c,0x50,0x50,0x50,0x50,0x50},
{0x00,0x00,0x00,0x00,0x00,0xfc,0x00,0xfc,0x00,0x00,0x00,0x00,0x00},
{0x50,0x50,0x50,0x50,0x50,0xdc,0x00,0xdc,0x50,0x50,0x50,0x50,0x50},
{0x20,0x20,0x20,0x20,0x20,0xfc,0x00,0xfc,0x00,0x00,0x00,0x00,0x00},
{0x50,0x50,0x50,0x50,0x50,0x50,0xfc,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0xfc,0x00,0xfc,0x20,0x20,0x20,0x20,0x20},
{0x00,0x00,0x00,0x00,0x00,0x00,0xfc,0x50,0x50,0x50,0x50,0x50,0x50},
{0x50,0x50,0x50,0x50,0x50,0x50,0x7c,0x00,0x00,0x00,0x00,0x00,0x00},
{0x20,0x20,0x20,0x20,0x20,0x3c,0x20,0x3c,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x3c,0x20,0x3c,0x20,0x20,0x20,0x20,0x20},
{0x00,0x00,0x00,0x00,0x00,0x00,0x7c,0x50,0x50,0x50,0x50,0x50,0x50},
{0x50,0x50,0x50,0x50,0x50,0x50,0xfc,0x50,0x50,0x50,0x50,0x50,0x50},
{0x20,0x20,0x20,0x20,0x20,0xfc,0x20,0xfc,0x20,0x20,0x20,0x20,0x20},
{0x20,0x20,0x20,0x20,0x20,0x20,0xe0,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x20,0x20,0x20,0x20,0x20,0x20},
{0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc},
{0x00,0x00,0x00,0x00,0x00,0x00,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc},
{0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0,0xe0},
{0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c},
{0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x70,0x88,0x88,0x88,0x98,0x68,0x00,0x00},
{0x00,0x00,0x60,0x90,0x90,0xa0,0xa0,0x90,0x88,0x88,0xb0,0x00,0x00},
{0x00,0x00,0xf8,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0xf8,0x50,0x50,0x50,0x50,0x50,0x00,0x00},
{0x00,0x00,0xf8,0x80,0x40,0x20,0x10,0x20,0x40,0x80,0xf8,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x78,0x90,0x88,0x88,0x88,0x70,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x88,0x88,0x88,0x88,0x98,0xe8,0x80,0x80},
{0x00,0x00,0x00,0x00,0x00,0xf8,0x20,0x20,0x20,0x20,0x10,0x00,0x00},
{0x00,0x00,0x20,0x70,0xa8,0xa8,0xa8,0xa8,0x70,0x20,0x20,0x00,0x00},
{0x00,0x00,0x70,0x88,0x88,0x88,0xf8,0x88,0x88,0x88,0x70,0x00,0x00},
{0x00,0x00,0x70,0x88,0x88,0x88,0x88,0x88,0x50,0x50,0xd8,0x00,0x00},
{0x00,0x00,0x70,0x88,0x40,0x70,0x88,0x88,0x88,0x88,0x70,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x50,0xa8,0xa8,0x50,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0xb0,0xa8,0xa8,0xa8,0xa8,0x70,0x20,0x20},
{0x00,0x00,0x00,0x00,0x00,0x70,0x88,0x60,0x80,0x88,0x70,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x70,0x88,0x88,0x88,0x88,0x88,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0xf8,0x00,0xf8,0x00,0xf8,0x00,0x00,0x00},
{0x00,0x00,0x00,0x20,0x20,0xf8,0x20,0x20,0x00,0xf8,0x00,0x00,0x00},
{0x00,0x00,0x00,0xc0,0x30,0x08,0x30,0xc0,0x00,0xf8,0x00,0x00,0x00},
{0x00,0x00,0x00,0x18,0x60,0x80,0x60,0x18,0x00,0xf8,0x00,0x00,0x00},
{0x10,0x28,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
{0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0xa0,0x40},
{0x00,0x00,0x00,0x20,0x20,0x00,0xf8,0x00,0x20,0x20,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x48,0xa8,0x90,0x48,0xa8,0x90,0x00,0x00,0x00},
{0x00,0x00,0x30,0x48,0x48,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x20,0x70,0x20,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x04,0x04,0x04,0x08,0x08,0x08,0x10,0x90,0x90,0x60,0x60,0x20},
{0x00,0xc0,0xa0,0xa0,0xa0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x40,0xa0,0x20,0x40,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0xf8,0xf8,0xf8,0xf8,0xf8,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
};