#include <malloc.h>
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

//stbi_write_bmp(char const *filename, int w, int h, int comp, const void  *data);

constexpr int globalw = 640, globalh = 480, globalcomp = 1;
static u8 global_pixel_buffer[globalw*globalh*globalcomp];

int main(int argc, char **argv)
{

    auto fontfile = fopen("UbuntuMono-R.ttf", "r");
    fseek(fontfile, 0, SEEK_END);
    auto filesize = ftell(fontfile);
    fseek(fontfile, 0, SEEK_SET);
    auto font_buffer = (const unsigned char*)malloc(filesize);
    fclose(fontfile);

    stbtt_fontinfo fontinfo;
    auto rc =
        stbtt_InitFont(
            &fontinfo,
            font_buffer,
            0 // stbtt_GetFontOffsetForIndex(font_buffer, 0)
        );
    Assert(rc);

    int w = globalw, h = globalh, comp = globalcomp;

    // Gray Background
    u8 *at = global_pixel_buffer;
    for (int row = 0; row < globalh; ++row) {
        
        for (int col = 0; col < globalw; ++col)
            *at++ = 100;
    }


    stbi_write_bmp("output.bmp", w, h, 1, global_pixel_buffer);
}
