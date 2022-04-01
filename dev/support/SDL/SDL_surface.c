#include <stdlib.h> // free
#include <string.h>      // memset


#include "SDL_video.h"

SDL_Surface* malloc_surface(Uint32 flags, int width, int height, int bpp, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask) {
    int i;
    int bytes_per_pixel;
    SDL_Color       * colors;
    SDL_Palette     * palette;
    SDL_PixelFormat * format;
    char            * pixels;
    SDL_Surface     * surface;
    bytes_per_pixel = (bpp / 8) + ((bpp % 8) ? 1 : 0);
    colors = (SDL_Color *) malloc(sizeof(SDL_Color) * 256);
    if (colors == NULL) {
        return NULL;
    }
    palette = (SDL_Palette *) malloc(sizeof(SDL_Palette)); 
    if (palette == NULL) {
        free(colors);
        return NULL;
    }
    format = (SDL_PixelFormat *) malloc(sizeof(SDL_PixelFormat));
    if (format == NULL) {
        free(colors);
        free(palette);
        return NULL;
    }
    pixels = (char *) malloc(width * height * bytes_per_pixel);
    if (pixels == NULL) {
        free(colors);
        free(palette);
        free(format);
        return NULL;
    }
    memset(pixels, 0, width * height * bytes_per_pixel);
    surface = (SDL_Surface *) malloc(sizeof(SDL_Surface));
    if (surface == NULL) {
        free(colors);
        free(palette);
        free(format);
        free(pixels);
        return NULL;
    }
    palette->colors = colors;
    format->palette = palette;
    surface->format = format;
    surface->pixels = pixels;
    palette->version = 0;
    palette->refcount = 1;
    palette->ncolors = 256;
    for (i=0; i<palette->ncolors; i++) {
        colors[i].r = 0;
        colors[i].g = 0;
        colors[i].b = 0;
    }
    format->BitsPerPixel = bpp;
    format->BytesPerPixel = bytes_per_pixel;
    format->Rloss = 0;
    format->Gloss = 0;
    format->Bloss = 0;
    format->Aloss = 0;
    format->Rshift = 0;
    format->Gshift = 0;
    format->Bshift = 0;
    format->Ashift = 0;
    format->Rmask = Rmask;
    format->Gmask = Gmask;
    format->Bmask = Bmask;
    format->Amask = Amask;
    surface->flags = flags;
    surface->pitch = width * bytes_per_pixel;
    surface->w = width;
    surface->h = height;
    surface->refcount = 1;
    surface->locked = 0;
    return surface;
}


/* Public routines */
/*
 * Create an empty RGB surface of the appropriate depth
 */
SDL_Surface * SDL_CreateRGBSurface (Uint32 flags,
			int width, int height, int depth,
			Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
  return malloc_surface(flags, width, height, depth, Rmask, Gmask, Bmask, Amask);
}
/*
 * Set the color key in a blittable surface
 */
int SDL_SetColorKey (SDL_Surface *surface, Uint32 flag, Uint32 key)
{
    if (flag & SDL_SRCCOLORKEY) {
        surface->flags |= SDL_SRCCOLORKEY; // Enable
        surface->format->colorkey = key;
    } else if (flag == 0) {
        surface->flags &= ~SDL_SRCCOLORKEY; // Disable
        surface->format->colorkey = 0; // Set color index to 0.
    }
    return 0;
}


/* 
 * This function performs a fast fill of the given rectangle with 'color'
 */
int SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color)
{
    int xx;
    int yy;

    char cval = (char)(color & 0xFF); // Just to be safe.
    char *px = (char*)(dst->pixels);

    for(yy=dstrect->y; yy<dstrect->y+dstrect->h; yy++) {
        for(xx=dstrect->x; xx<dstrect->x+dstrect->w; xx++) {
            *(px + (yy*(dst->w)+xx)) = cval;
        }
    }

   return(0);
}

/*
 * Lock a surface to directly access the pixels
 */
int SDL_LockSurface (SDL_Surface *surface)
{
	surface->locked++;
	return(0);
}
/*
 * Unlock a previously locked surface
 */
void SDL_UnlockSurface (SDL_Surface *surface)
{
	surface->locked--;
}





/*
 * Free a surface created by the above function.
 */
void SDL_FreeSurface (SDL_Surface *surface)
{
    if (surface) {
        if (surface->format) {
            if (surface->format->palette) {
                if (surface->format->palette->colors) {
                    free(surface->format->palette->colors);
                    surface->format->palette->colors = NULL;
                }
                free(surface->format->palette);
                surface->format->palette = NULL;
            }
            free(surface->format);
            surface->format = NULL;
        }
        if (surface->pixels) {
            free(surface->pixels);
            surface->pixels = NULL;
        }
        free(surface);
        surface = NULL;
    }
}
