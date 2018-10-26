#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

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

struct Offscreen_Buffer
{
    int w, h, comp = 1;
    void *data;

    Offscreen_Buffer(int _w, int _h) : w(_w), h(_h)
    {
        auto required_size = w*h*comp;
        data = malloc(required_size);
        Assert(data);
        memset(data, 0, required_size);
    }

    u8* at(int x, int y)
    {
        auto start = (u8*)data;
        return start + (w*comp*y) + x;
    }
};

void draw_line(u8 *buffer, int w, int h, int x0, int y0, int x1, int y1)
{
    // printf("\ndrawing line (%d/%d) -> (%d/%d)\n", x0, y0, x1, y1);

    //
    // Bresenham-Algorithmus
    //
    int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = dx+dy, e2; /* error value e_xy */

    while (!(x0==x1 && y0==y1)) {

        int comp = 1;  // @Todo
        buffer[(w*comp*y0) + x0] = 100;

        e2 = 2*err;
        if (e2 > dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
        if (e2 < dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
    }
}

const char lorem[] = "Lorem ipsum dolor";

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

    Offscreen_Buffer bakedfont(2000, 2000);


    stbtt_bakedchar ttfchars[fontinfo.numGlyphs];

    float font_scale = 40.0f;

    Assert(stbtt_BakeFontBitmap(
        ttf_buffer, 0,
        font_scale,
        (unsigned char*)bakedfont.data,
        bakedfont.w,
        bakedfont.h,
        0, 255,
        ttfchars) > 0);

    stbi_write_bmp("font.bmp", bakedfont.w, bakedfont.h, bakedfont.comp, bakedfont.data);

    Offscreen_Buffer target(640, 250);

    int current_point_x = 100;
    int current_point_y = 120;  // this is also the baseline

    draw_line(
        (u8*)target.data, target.w, target.h,
        0, 0, target.w, target.h);
#if 1
    for (int i = 0; lorem[i]; ++i) {
        //
        // glyph
        //
        char c = lorem[i];
        auto glyph = ttfchars[c];
        {
            int w = glyph.x1 - glyph.x0;
            int h = glyph.y1 - glyph.y0;
            printf("%c : %03d  ***** xoff: %-2.f / yoff: %-2.f ***** w: %d / h: %d\n",
                c, c, glyph.xoff, glyph.yoff, w, h);
            for (int row = 0; row < h; ++row) {

                u8 *src = bakedfont.at(glyph.x0, (glyph.y0)+row);
                u8 *dest = target.at(current_point_x+glyph.xoff, current_point_y+glyph.yoff+row);

                for (int col = 0; col < w; ++col) {
                    dest[col] = src[col];
                }
            }
        }
        //
        // current point
        //
        draw_line(
            (u8*)target.data, target.w, target.h,
            0, current_point_y, target.w, current_point_y);
        draw_line(
            (u8*)target.data, target.w, target.h,
            0, current_point_y+2, target.w, current_point_y+2);
        draw_line(
            (u8*)target.data, target.w, target.h,
            current_point_x, 0, current_point_x, target.h);

        current_point_x += glyph.xadvance;
    }
#endif

    stbi_write_bmp("output.bmp", target.w, target.h, target.comp, target.data);
}
