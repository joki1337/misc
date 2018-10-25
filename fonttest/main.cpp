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


// STBTT_DEF int stbtt_BakeFontBitmap(
//     const unsigned char *data, int offset,  // font location (use offset=0 for plain .ttf)
//     float pixel_height,                     // height of font in pixels
//     unsigned char *pixels, int pw, int ph,  // bitmap to be filled in
//     int first_char, int num_chars,          // characters to bake
//     stbtt_bakedchar *chardata);             // you allocate this, it's num_chars long



int main(int argc, char **argv)
{

    auto ttffile = fopen("UbuntuMono-R.ttf", "rb");
    fseek(ttffile, 0, SEEK_END);
    auto filesize = ftell(ttffile);
    printf("font filesize: %d\n", filesize);
    fseek(ttffile, 0, SEEK_SET);
    auto ttf_buffer = (const unsigned char*)malloc(filesize);
    fread((void *)ttf_buffer, 1, filesize, ttffile);
    fclose(ttffile);

    stbtt_fontinfo fontinfo;
    Assert(stbtt_InitFont(
            &fontinfo,
            ttf_buffer,
            stbtt_GetFontOffsetForIndex(ttf_buffer, 0)));


    stbi_write_bmp("output.bmp", w, h, 1, global_pixel_buffer);
}
