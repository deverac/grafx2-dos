// Adapted from:
// http://www.libpng.org/pub/png/libpng-1.2.5-manual.html#section-3
#include <stdlib.h>
#include <png.h>
#include <zlib.h>

#include "readpng.h"


void errmsg(const char * s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);
}


int read_png_file(char* file_name, pngdat * pdat)
{
    int x, y;
    int i, n;

    png_byte header[8];    // 8 is the maximum size that can be checked
    png_bytep * row_pointers = NULL;
    png_structp png_ptr;
    png_infop info_ptr;

    png_colorp palette;
    int num_palette;

    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
        errmsg("[read_png_file] File %s could not be opened for reading", file_name);
        return 0;
    }

    fread(&header[0], 1, 8, fp);
    if (png_sig_cmp(&header[0], 0, 8)) {
        fclose(fp);
        errmsg("[read_png_file] File %s is not recognized as a PNG file", file_name);
        return 0;
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        errmsg("[read_png_file] png_create_read_struct failed");
        return 0;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        fclose(fp);
        errmsg("[read_png_file] png_create_info_struct failed");
        return 0;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fclose(fp);
        errmsg("[read_png_file] Error during init_io");
        return 0;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);



    pdat->width = png_get_image_width(png_ptr, info_ptr);
    pdat->height = png_get_image_height(png_ptr, info_ptr);
    pdat->bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    pdat->color_type = png_get_color_type(png_ptr, info_ptr);

    pdat->bytes_per_pixel = (pdat->bit_depth / 8) + ((pdat->bit_depth % 8) ? 1 : 0);

    png_read_update_info(png_ptr, info_ptr);



    if (setjmp(png_jmpbuf(png_ptr))) {
        fclose(fp);
        errmsg("[read_png_file] Error during read_image");
        return 0;
    }

    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * (pdat->height));
    if (!row_pointers) {
        fclose(fp);
        errmsg("[read_png_file] Error malloc row_pointers");
        return 0;
    }

    for (y=0; y<pdat->height; y++) {
        row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr, info_ptr));
        if (!row_pointers[y]) {
            while(y>=0) {
                free(row_pointers[y--]);
            }
            return 0;
        }
    }

    // For each pass of an interlaced image, use png_read_rows() instead.
    png_read_image(png_ptr, row_pointers);

    fclose(fp);



    pdat->pixels = malloc(pdat->height * pdat->width * pdat->bytes_per_pixel);
    if (pdat->pixels) {
        for (y=0; y<pdat->height; y++) {
            png_byte* row = row_pointers[y];
            for (x=0; x<pdat->width; x++) {
                png_byte* ptr = &(row[x*pdat->bytes_per_pixel]);
                n = ((y * pdat->width) + x) * pdat->bytes_per_pixel;
                for (i=0; i<pdat->bytes_per_pixel; i++) {
                    *(pdat->pixels + n + i) = ptr[i];
                }
            }
        }

        if (pdat->color_type == PNG_COLOR_TYPE_PALETTE) {
            if (png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette)) {
                if (num_palette > READPNG_MAX_PALETTE_NUM) {
                    num_palette = READPNG_MAX_PALETTE_NUM;
                }
                pdat->num_palette = num_palette;
                pdat->palette = malloc(READPNG_MAX_PALETTE_NUM * sizeof(png_color));
                if (pdat->palette) {
                    for(i=0; i<num_palette; i++) {
                        *(pdat->palette + i*3 + 0) = palette[i].red;
                        *(pdat->palette + i*3 + 1) = palette[i].green;
                        *(pdat->palette + i*3 + 2) = palette[i].blue;
                    }
                    for(i=num_palette; i<READPNG_MAX_PALETTE_NUM; i++) {
                        *(pdat->palette + i*3 + 0) = 0;
                        *(pdat->palette + i*3 + 1) = 0;
                        *(pdat->palette + i*3 + 2) = 0;
                    }
                }
            }
        }
    }

    if (row_pointers[0]) {
        y = pdat->height-1;
        while (y>=0) {
            free(row_pointers[y--]);
        }
    }

    return 1;
}



void free_pngdat(pngdat * pdat) {
    if (pdat) {
        if (pdat->pixels) {
            free(pdat->pixels);
            pdat->pixels = NULL;
        }
        if (pdat->palette) {
            free(pdat->palette);
            pdat->palette = NULL;
        }
    }
}


void read_png_version_info() {
    fprintf(stderr, "   Compiled with libpng %s; using libpng %s.\n",
      PNG_LIBPNG_VER_STRING, png_libpng_ver);
    fprintf(stderr, "   Compiled with zlib %s; using zlib %s.\n",
      ZLIB_VERSION, zlib_version);
}



