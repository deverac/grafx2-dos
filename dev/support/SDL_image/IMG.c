#include <stdlib.h> // NULL
#include <string.h> // strlen
#include "SDL_image.h"

#include "readpng.h"


extern SDL_Surface* malloc_surface(Uint32 flags, int width, int height, int bpp, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);


SDL_Surface * IMG_Load(const char* path) {
    SDL_Surface * surf = NULL;
    int i;
    pngdat pdat;
    Uint32 flags = 0;
    int pathlen = strlen(path);
    if (pathlen > 3) {
        if ((strcmp(&path[pathlen-4], ".png") == 0) || (strcmp(&path[pathlen-4], ".PNG") == 0)) {
            if (read_png_file(path, &pdat)) {
                flags = SDL_SWSURFACE;

                surf = malloc_surface(flags, pdat.width, pdat.height, pdat.bit_depth, 0, 0, 0, 0);
                if (surf) {
                    surf->format->palette->ncolors = pdat.num_palette;

                    // Copy pixels
                    memcpy(surf->pixels, pdat.pixels, pdat.height * pdat.width * pdat.bytes_per_pixel);

                    // Copy palette
                    for (i=0; i<pdat.num_palette; i++) {
                        surf->format->palette->colors[i].r = pdat.palette[i*3];
                        surf->format->palette->colors[i].g = pdat.palette[i*3+1];
                        surf->format->palette->colors[i].b = pdat.palette[i*3+2];
                        surf->format->palette->colors[i].a = 0;
                    }

                    free_pngdat(&pdat);

                }
            }
        }
    }
    return surf;
}
