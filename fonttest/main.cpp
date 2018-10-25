#include <malloc.h>
#include <stdio.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint8_t u8;

#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

//stbi_write_bmp(char const *filename, int w, int h, int comp, const void  *data);

// constexpr int globalw = 640, globalh = 480, globalcomp = 1;
// static u8 global_pixel_buffer[globalw*globalh*globalcomp];


// STBTT_DEF int stbtt_BakeFontBitmap(
//     const unsigned char *data, int offset,  // font location (use offset=0 for plain .ttf)
//     float pixel_height,                     // height of font in pixels
//     unsigned char *pixels, int pw, int ph,  // bitmap to be filled in
//     int first_char, int num_chars,          // characters to bake
//     stbtt_bakedchar *chardata);             // you allocate this, it's num_chars long

struct Offscreen_Buffer
{
    int w, h, comp = 1;
    void *data;

    Offscreen_Buffer(int _w, int _h) : w(_w), h(_h)
    {
        auto required_size = w*h*comp;
        data = malloc(required_size);
        Assert(data);
    }

    u8* at(int x, int y)
    {
        auto start = (u8*)data;
        return start + (w*comp*y) + x;
    }
};

const char lorem[] = "Lorem ipsum dolor sit amet";

int main(int argc, char **argv)
{

    static u8 ttf_buffer[Megabytes(1)];
    fread((void *)ttf_buffer, 1, Megabytes(1), fopen("UbuntuMono-R.ttf", "rb"));

    stbtt_fontinfo fontinfo;
    Assert(stbtt_InitFont(
            &fontinfo,
            ttf_buffer,
            stbtt_GetFontOffsetForIndex(ttf_buffer, 0)));
    printf("number of glyphs in ttf: %d\n", fontinfo.numGlyphs);

    Offscreen_Buffer bakedfont(400, 400);



    stbtt_bakedchar ttfchars[fontinfo.numGlyphs];

    float font_scale = 20.0f;

    Assert(stbtt_BakeFontBitmap(
        ttf_buffer, 0,
        font_scale,
        (unsigned char*)bakedfont.data,
        bakedfont.w,
        bakedfont.h,
        0, 255,
        ttfchars) > 0);

    stbi_write_bmp("font.bmp", bakedfont.w, bakedfont.h, bakedfont.comp, bakedfont.data);

    static Offscreen_Buffer target(640, 480);

    // x0,y0,x1,y1
    // xoff/yoff are the offset it pixel space from the glyph origin to the top-left of the bitmap
    // <current_point+SF*x0, baseline+SF*y0> to <current_point+SF*x1,baseline+SF*y1)

    int baseline = 20;
    int current_point = 20;


#if 0
    int cursor_x_pos = 10, cursor_y_pos = 10;

    for (int i = 0; lorem[i]; ++i) {

        auto glyph = ttfchars[lorem[i]];
        int w = glyph.x1 - glyph.x0;
        int h = glyph.y1 - glyph.y0;
        printf("%c : %03d  ***** xoff: %-2.f / yoff: %-2.f ***** w: %d / h: %d\n",
            lorem[i], lorem[i], glyph.xoff, glyph.yoff, w, h);
        // printf("width  of glyph: %d\n", w);
        // printf("height of glyph: %d\n", h);

        for (int row = 0; row < h; ++row) {

            u8 *src = bakedfont.at(glyph.x0, glyph.y0+row);
            u8 *dest = target.at(cursor_x_pos, cursor_y_pos+row);

            for (int col = 0; col < w; ++col)
                dest[col] = src[col];
        }

        // bounding box
        {
            u8 *dest = target.at(cursor_x_pos, cursor_y_pos);
            for (int col = 0; col < w; ++col) dest[col] = 255;
        }

        {
            u8 *dest = target.at(cursor_x_pos, cursor_y_pos+h);
            for (int col = 0; col < w; ++col) dest[col] = 255;
        }

        cursor_x_pos += glyph.xadvance;
    }
#endif


    stbi_write_bmp("output.bmp", target.w, target.h, target.comp, target.data);
}
