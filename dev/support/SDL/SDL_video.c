
/* The high-level video driver subsystem */

#include <stdio.h>
#include <string.h>      // memcpy
#include <stdlib.h>      // malloc
#include <dos.h>         // outp
#include <sys/nearptr.h> // __djgpp_conventional_base

#include "SDL_video.h"
#include "intvals.h"

#define VGA_START_ADDR  0xA0000

static SDL_Rect  screen_rect;
static SDL_Rect* screen_modes[2];


// VGA video DAC PEL mask
// #define VGA_PALETTE_MASK_PORT  0x03c6

// VGA video DAC PEL address port
#define VGA_PALETTE_INDEX_PORT  0x03c8

// VGA video DAC port
#define VGA_PALETTE_COLOR_PORT  0x03c9


extern SDL_Surface* malloc_surface(Uint32 flags, int width, int height, int bpp, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);













/*
 * Return a pointer to an array of available screen dimensions for the
 * given format, sorted largest to smallest.  Returns NULL if there are
 * no dimensions available for a particular format, or (SDL_Rect **)-1
 * if any dimension is okay for the given format.  If 'format' is NULL,
 * the mode list will be for the format given by SDL_GetVideoInfo()->vfmt
 */
SDL_Rect ** SDL_ListModes (SDL_PixelFormat *format, Uint32 flags)
{
    (void)format;
    (void) flags;

    screen_rect.x = 0;
    screen_rect.y = 0;
    screen_rect.w = FDOS_SCREEN_WIDTH;
    screen_rect.h = FDOS_SCREEN_HEIGHT;

    screen_modes[0] = &screen_rect;
    screen_modes[1] = NULL; // Terminate list.

    return screen_modes;
}

int SDL_VideoModeOK (int width, int height, int bpp, Uint32 flags)
{
   if (width != FDOS_SCREEN_WIDTH) return 0;
   if (height != FDOS_SCREEN_HEIGHT) return 0;
   if (bpp != 8) return 0;  // 2^8 ==256
   if (flags != SDL_FULLSCREEN) return 0;
   return 1;
}




/*
 * Set the requested video mode, allocating a shadow buffer if necessary.
 */
SDL_Surface * SDL_SetVideoMode (int width, int height, int bpp, Uint32 flags)
{
   union REGS regs;

    if (width != FDOS_SCREEN_WIDTH) {
        fprintf(stderr, "Ignoring width %d; only %d is supported.\n", width, FDOS_SCREEN_WIDTH);
    }
    if (height != FDOS_SCREEN_HEIGHT) {
        fprintf(stderr, "Ignoring height %d; only %d is supported.\n", height, FDOS_SCREEN_HEIGHT);
    }
    if (bpp != 8) {
        fprintf(stderr, "Ignoring bpp %d; only 8 is supported.\n", bpp);
    }
    if (flags & SDL_FULLSCREEN) {
        fprintf(stderr, "SDL_FULLSCREEN is not supported; removing flag.\n");
        flags &= ~SDL_FULLSCREEN;
    }
    // The SDL_RESIZABLE flag is also not supported, but we'll ignore it.

    // set video (Graphics mode)
    regs.h.ah = 0x00;
    regs.h.al = 0x13; // set 256 color mode
    int86(VIDEO_INT, &regs, &regs);

    return malloc_surface(flags, FDOS_SCREEN_WIDTH, FDOS_SCREEN_HEIGHT, 8, 0, 0, 0, 0);
}

/* 
 * Convert a surface into the video pixel format.
 */
SDL_Surface * SDL_DisplayFormat (SDL_Surface *surface)
{
    int i;
    SDL_Surface * surf = NULL;
    surf = malloc_surface(surface->flags, surface->w, surface->h, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);
    if (surf) {
        // copy pixels
        memcpy(surf->pixels, surface->pixels, surface->h * surface->w * surface->format->BytesPerPixel);

        // copy palette
        for (i=0; i<surface->format->palette->ncolors; i++) {
            surf->format->palette->colors[i].r = surface->format->palette->colors[i].r;
            surf->format->palette->colors[i].g = surface->format->palette->colors[i].g;
            surf->format->palette->colors[i].b = surface->format->palette->colors[i].b;
        }
    }
    return surf;
}

/*
 * Update a specific portion of the physical screen
 */
void SDL_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h)
{
    Sint32 xx;
    Sint32 yy;
    char *VGA = (char *)VGA_START_ADDR;
    char* px = (char*)(screen->pixels);

    VGA += __djgpp_conventional_base;

    if (x == 0 && y == 0 && w == 0 && h == 0) {
        h = screen->h;
        w = screen->w;
    }

    for(yy=y; yy<y+h; yy++) {
      for(xx=x; xx<x+w; xx++) {
        VGA[yy*(screen->w)+xx] = px[yy*(screen->w)+xx];
      }
    }
}

/*
 * Set the physical and/or logical colormap of a surface:
 * Only the screen has a physical colormap. It determines what is actually
 * sent to the display.
 * The logical colormap is used to map blits to/from the surface.
 * 'which' is one or both of SDL_LOGPAL, SDL_PHYSPAL
 *
 * Return nonzero if all colours were set as requested, or 0 otherwise.
 */
int SDL_SetPalette(SDL_Surface *screen, int which,
		   SDL_Color *colors, int firstcolor, int ncolors)
{
    int i;

    if (firstcolor < 0 || firstcolor >= 256) {
        printf("Err: firstcolor must be between 0 and 255.\n");
        return -1;
    }
    if ( (ncolors != 1) && (ncolors != 256) ) {
        printf("Warn: ncolors must be 1 or 256\n");
        return -1;
    }

    if (which & SDL_PHYSPAL) {
        // VGA only uses the lower six bits of each color byte. Shifting the value of
        // the actual color value right by two bits will give approximately the same
        // color when displayed by VGA hardware.
        // Writing to the VGA_PALETTE_COLOR_PORT will automatically advance the index.
        if (ncolors == 1) {
            outp(VGA_PALETTE_INDEX_PORT, firstcolor); // Index palette to change
            outp(VGA_PALETTE_COLOR_PORT, colors[0].r >> 2);
            outp(VGA_PALETTE_COLOR_PORT, colors[0].g >> 2);
            outp(VGA_PALETTE_COLOR_PORT, colors[0].b >> 2);
        } else { //  ncolors == 256
            outp(VGA_PALETTE_INDEX_PORT, 0); // Signal output 256 colors.
            for(i=0; i<256; i++)
            {
                outp(VGA_PALETTE_COLOR_PORT, colors[i].r >> 2);
                outp(VGA_PALETTE_COLOR_PORT, colors[i].g >> 2);
                outp(VGA_PALETTE_COLOR_PORT, colors[i].b >> 2);
            }
        }
    }

    if (screen) {
        if (ncolors == 1) {
            screen->format->palette->colors[firstcolor].r = colors[0].r;
            screen->format->palette->colors[firstcolor].g = colors[0].g;
            screen->format->palette->colors[firstcolor].b = colors[0].b;
        } else { // ncolors == 256
            for(i=0; i<256; i++)
            {
                screen->format->palette->colors[i].r = colors[i].r;
                screen->format->palette->colors[i].g = colors[i].g;
                screen->format->palette->colors[i].b = colors[i].b;
            }
        }
    }

    return 0;
}

int SDL_SetColors(SDL_Surface *screen, SDL_Color *colors, int firstcolor,
		  int ncolors)
{
	return SDL_SetPalette(screen, SDL_LOGPAL | SDL_PHYSPAL,
			      colors, firstcolor, ncolors);
}


/*
 * Sets/Gets the title and icon text of the display window, if any.
 */
void  SDL_WM_SetCaption(const char * s1, const char * s2) {
  (void)s1;
  (void)s2;
  // Ignored
}

/*
 * Sets the window manager icon for the display window.
 */
void SDL_WM_SetIcon (SDL_Surface *icon, Uint8 *mask)
{
  (void)icon;
  (void)mask;
  // Ignored
}




// Performs a blit from the source surface to the destination surface.
// Returns 0 if the blit is successful or a negative value on failure.
int SDL_BlitSurface(SDL_Surface* src, const SDL_Rect* src_rect, SDL_Surface* dst, SDL_Rect* dst_rect){
    int x, y;
    unsigned int src_offs;
    unsigned int dst_offs;
    unsigned char * src_ptr;
    unsigned char * dst_ptr;
    int do_copy = 0;
    unsigned char px = 0;

    if (src_rect->w != dst_rect->w) {
        printf("Err: width not equal\n");
        return -1;
    }
    if (src_rect->h !=  dst_rect->h) {
        printf("Err: height not equal\n");
        return -1;
    }
    // if (src->format->BytesPerPixel != 1) {
    //     printf("Err: src BytesPerPixel not 1 %d\n", src->format->BytesPerPixel);
    //     return -1;
    // }
    // if (dst->format->BytesPerPixel != 1) {
    //     printf("Err: dst BytesPerPixel not 1 %d\n", dst->format->BytesPerPixel);
    //     return -1;
    // }
    if (dst->format->BytesPerPixel != dst->format->BytesPerPixel) {
        printf("Err: src, dst BytesPerPixel not equal %d %d\n", src->format->BytesPerPixel, dst->format->BytesPerPixel); getchar();
        return -1;
    }

    src_offs = ((src_rect->y * src->w) + src_rect->x) * src->format->BytesPerPixel;
    dst_offs = ((dst_rect->y * dst->w) + dst_rect->x) * dst->format->BytesPerPixel;

    for (y=0; y<src_rect->h; y++) {
        for (x=0; x<src_rect->w; x++) {
            if (src_rect->x + x < src->w && src_rect->y + y < src->h &&
                dst_rect->x + x < dst->w && dst_rect->y + y < dst->h) {

                  src_ptr = (src->pixels + src_offs + (((y * src->w) + x) * src->format->BytesPerPixel));
                  dst_ptr = (dst->pixels + dst_offs + (((y * dst->w) + x) * dst->format->BytesPerPixel));

                  px = *(src_ptr);

                  // Per SDL documentation, if SDL_SRCCOLORKEY is set, only
                  // copy pixels that do not match colorkey.
                  do_copy = ((src->flags & SDL_SRCCOLORKEY) ? (px != src->format->colorkey) : 1);
                  if (do_copy) {
                      *(dst_ptr) = px;
                  }
           }
        }
    }
    return 0;
}
