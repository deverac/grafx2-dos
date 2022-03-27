#include "SDL_video.h"

// Copied from SDL-1.2/src/video/SDL_pixels.c
// Match an RGB value to a particular palette index
Uint8 SDL_FindColor(SDL_Palette *pal, Uint8 r, Uint8 g, Uint8 b)
{
    // Do colorspace distance matching
    unsigned int smallest;
    unsigned int distance;
    int rd, gd, bd;
    int i;
    Uint8 pixel=0;

    smallest = ~0;
    for ( i=0; i<pal->ncolors; ++i ) {
        rd = pal->colors[i].r - r;
        gd = pal->colors[i].g - g;
        bd = pal->colors[i].b - b;
        distance = (rd*rd)+(gd*gd)+(bd*bd);
        if ( distance < smallest ) {
            pixel = i;
            if ( distance == 0 ) { // Perfect match!
                break;
            }
            smallest = distance;
        }
    }
    return(pixel);
}

// Return the index of the color that most closely matches r, g, b.
Uint32 SDL_MapRGB(const SDL_PixelFormat * format, Uint8 r, Uint8 g, Uint8 b){
    return SDL_FindColor(format->palette, r, g, b);
}
