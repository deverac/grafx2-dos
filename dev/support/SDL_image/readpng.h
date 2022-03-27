#ifndef _READPNG_H_
#define _READPNG_H_

#define READPNG_MAX_PALETTE_NUM 256

typedef struct {
    int width;
    int height;
    int bit_depth;
    int bytes_per_pixel;
    int color_type;
    unsigned char * pixels;
    int  num_palette;
    unsigned char * palette;
} pngdat;

void free_pngdat(pngdat * pdat);

int read_png_file(const char* file_name, pngdat * dat);

void read_png_version_info();

#endif
