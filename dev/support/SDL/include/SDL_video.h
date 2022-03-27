#ifndef _SDL_VIDEO_H
#define _SDL_VIDEO_H

#include "SDL_stdinc.h"

#define SDL_FULLSCREEN	0x80000000	/**< Surface is a full screen display */
#define SDL_RESIZABLE	0x00000010	/**< This video mode may be resized */



#define SDL_SWSURFACE   0x00000000    /**< Surface is in system memory */


#define SDL_LOGPAL  0x01
#define SDL_PHYSPAL 0x02

#define SDL_SRCCOLORKEY 0x00001000    /**< Blit uses a source color key */


#define FDOS_SCREEN_WIDTH   320
#define FDOS_SCREEN_HEIGHT  200


typedef struct SDL_Rect
{
    int x, y;
    int w, h;
} SDL_Rect;


typedef struct SDL_Color
{
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
} SDL_Color;

typedef struct SDL_Palette
{
    int ncolors;
    SDL_Color *colors;
    Uint32 version;
    int refcount;
} SDL_Palette;

typedef struct SDL_PixelFormat
{
    SDL_Palette *palette;
    Uint8  BitsPerPixel;
    Uint8  BytesPerPixel;
    Uint8  Rloss;
    Uint8  Gloss;
    Uint8  Bloss;
    Uint8  Aloss;
    Uint8  Rshift;
    Uint8  Gshift;
    Uint8  Bshift;
    Uint8  Ashift;
    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask;
    Uint32 Amask;

    /** RGB color key information */
    Uint32 colorkey;
    /** Alpha value information (per-surface alpha) */
    Uint8  alpha;
} SDL_PixelFormat;


typedef struct SDL_Surface
{
    Uint32 flags;               /**< Read-only */
    SDL_PixelFormat *format;    /**< Read-only */
    int w, h;                   /**< Read-only */
    Uint16 pitch;                  /**< Read-only */
    void *pixels;               /**< Read-write */

    /** clipping information */
    SDL_Rect clip_rect;         /**< Read-only */

    /** information needed for surfaces requiring locks */
    Uint32 locked;                 /**< Read-only */

    /** Reference count -- used when freeing surface */
    int refcount;               /**< Read-mostly */
} SDL_Surface;


SDL_Surface*   SDL_CreateRGBSurface(Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);


int            SDL_SetColorKey(SDL_Surface * surface, int flag, Uint32 key);
Uint32         SDL_MapRGB(const SDL_PixelFormat * format, Uint8 r, Uint8 g, Uint8 b);
void           SDL_WM_SetCaption(const char * s1, const char * s2);

void SDL_FreeSurface(SDL_Surface * surface);
void SDL_WM_SetIcon(SDL_Surface *icon, Uint8 *mask);
int            SDL_SetPalette(SDL_Surface *surface, int flags, SDL_Color *colors, int firstcolor, int ncolors);
int            SDL_SetColors(SDL_Surface *surface, SDL_Color *colors, int firstcolor, int ncolors);
int            SDL_BlitSurface(SDL_Surface* src, const SDL_Rect* srcrect, SDL_Surface* dst, SDL_Rect* dstrect);

int            SDL_FillRect(SDL_Surface * dst, const SDL_Rect * rect, Uint32 color);
int            SDL_LockSurface(SDL_Surface * surface);
void           SDL_UnlockSurface(SDL_Surface * surface);
void           SDL_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h);
SDL_Rect **    SDL_ListModes(SDL_PixelFormat *format, Uint32 flags);
SDL_Surface *  SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags);
int            SDL_VideoModeOK(int width, int height, int bpp, Uint32 flags);
SDL_Surface *  SDL_DisplayFormat(SDL_Surface *surface);

#endif
