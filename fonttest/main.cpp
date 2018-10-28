#include <stdlib.h>
#include <stdio.h>

#include "default.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

struct Offscreen_Buffer
{
    int w, h, stride, comp = 1;
    void *data;

    Offscreen_Buffer(int _w, int _h) : w(_w), h(_h)
    {
        auto required_size = w*h*comp;
        stride = w*comp;
        data = malloc(required_size);
        Assert(data);
        memset(data, 80, required_size);
    }

    u8* at(int x, int y)
    {
        auto start = (u8*)data;
        return start + (stride*y) + x;
    }
};

void _draw_line(u8 *buffer, int w, int h, int x0, int y0, int x1, int y1)
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

void draw_line(Offscreen_Buffer *ob, int x0, int y0, int x1, int y1)
{
    _draw_line((u8*)ob->data, ob->w, ob->h, x0, y0, x1, y1);
}


struct Loaded_Font
{
    stbtt_fontinfo fontinfo;
    stbtt_bakedchar ttfchars[1000];
    u8 *ttf_data;
    Offscreen_Buffer *bakedfont;

    Loaded_Font(const char *font_filepath)
    {
        ttf_data = (u8*)malloc(Megabytes(1)); Assert(ttf_data);
        fread((void *)ttf_data, 1, Megabytes(1), fopen(font_filepath, "rb"));

        Assert(stbtt_InitFont(
                &fontinfo,
                ttf_data,
                stbtt_GetFontOffsetForIndex(ttf_data, 0)));
        printf("number of glyphs in ttf: %d\n", fontinfo.numGlyphs);

        bakedfont = new Offscreen_Buffer(1024, 1024);

        float font_hight_in_pixels = 40.0f;
        auto font_scaling_factor = stbtt_ScaleForPixelHeight(&fontinfo, font_hight_in_pixels);
        printf("font scale %f (hight in pixels: %f)\n", font_scaling_factor, font_hight_in_pixels);

        int ascent, descent, lineGap;
        stbtt_GetFontVMetrics(&fontinfo, &ascent, &descent, &lineGap);
        printf("vertical advance should be: %f", (ascent - descent + lineGap)*font_scaling_factor);


        Assert(stbtt_BakeFontBitmap(
            ttf_data, 0,
            font_hight_in_pixels,
            (unsigned char*)bakedfont->data,
            bakedfont->w,
            bakedfont->h,
            0, 255,
            ttfchars) > 0);

        stbi_write_bmp("font_test.bmp", bakedfont->w, bakedfont->h, bakedfont->comp, bakedfont->data);

    }
};


void draw_text(Offscreen_Buffer *target, const char *text, int cursor_x, int cursor_y, const Loaded_Font *font)
{
    for (int i = 0; text[i]; ++i) {

        char c = text[i];
        auto glyph = font->ttfchars[c];
        {
            int w = glyph.x1 - glyph.x0;
            int h = glyph.y1 - glyph.y0;
            printf("%c : %03d  ***** xoff: %-2.f / yoff: %-2.f ***** w: %d / h: %d\n",
                c, c, glyph.xoff, glyph.yoff, w, h);

            for (int row = 0; row < h; ++row) {

                u8 *src = font->bakedfont->at(glyph.x0, (glyph.y0)+row);
                u8 *at = target->at(cursor_x+glyph.xoff, cursor_y+glyph.yoff+row);

                for (int col = 0; col < w; ++col) {

                    float alpha = src[col] / 255.0f;
                    at[col] = (u8)lerp(at[col], src[col], alpha);

                }
            }
        }

        // visualize baseline
        draw_line(target, 0, cursor_y,   target->w, cursor_y);
        draw_line(target, 0, cursor_y+2, target->w, cursor_y+2);

        cursor_x += glyph.xadvance;
    }
}

int main(int argc, char **argv)
{
    Loaded_Font font("UbuntuMono-R.ttf");


    Offscreen_Buffer target(640, 250);

    draw_text(&target, "Moin1337!!!!", 50, 50, &font);
    draw_text(&target, "Lorem ipsum dolor", 100, 120, &font);

    stbi_write_bmp("output.bmp", target.w, target.h, target.comp, target.data);
}
