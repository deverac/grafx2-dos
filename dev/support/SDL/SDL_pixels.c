#include "SDL_video.h"

/*
 * Match an RGB value to a particular palette index
 */
Uint8 SDL_FindColor(SDL_Palette *pal, Uint8 r, Uint8 g, Uint8 b)
{
	/* Do colorspace distance matching */
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
			if ( distance == 0 ) { /* Perfect match! */
				break;
			}
			smallest = distance;
		}
	}
	return(pixel);
}

/* Find the opaque pixel value corresponding to an RGB triple */
Uint32 SDL_MapRGB
(const SDL_PixelFormat * const format,
 const Uint8 r, const Uint8 g, const Uint8 b)
{
	if ( format->palette == NULL ) {
		return (r >> format->Rloss) << format->Rshift
		       | (g >> format->Gloss) << format->Gshift
		       | (b >> format->Bloss) << format->Bshift
		       | format->Amask;
	} else {
		return SDL_FindColor(format->palette, r, g, b);
	}
}
