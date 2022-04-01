#ifndef _SDL_IMAGE_H
#define _SDL_IMAGE_H

#include "SDL.h"


typedef enum
{
    IMG_INIT_JPG = 0x00000001,
    IMG_INIT_PNG = 0x00000002,
    IMG_INIT_TIF = 0x00000004,
    IMG_INIT_WEBP = 0x00000008
} IMG_InitFlags;

/* Loads dynamic libraries and prepares them for use.  Flags should be
   one or more flags from IMG_InitFlags OR'd together.
   It returns the flags successfully initialized, or 0 on failure.
 */
int IMG_Init(int flags);

/* Unloads libraries loaded with IMG_Init */
void IMG_Quit(void);

/* Load an image from an SDL data source.
   The 'type' may be one of: "BMP", "GIF", "PNG", etc.

   If the image format supports a transparent pixel, SDL will set the
   colorkey for the surface.  You can enable RLE acceleration on the
   surface afterwards by calling:
	SDL_SetColorKey(image, SDL_RLEACCEL, image->format->colorkey);
 */
SDL_Surface *IMG_LoadTyped_RW(SDL_RWops *src, int freesrc, char *type);
/* Convenience functions */
SDL_Surface * IMG_Load(const char *file);

/* Functions to detect a file type, given a seekable source */
extern int IMG_isICO(SDL_RWops *src);
extern int IMG_isCUR(SDL_RWops *src);
extern int IMG_isBMP(SDL_RWops *src);
extern int IMG_isGIF(SDL_RWops *src);
extern int IMG_isJPG(SDL_RWops *src);
extern int IMG_isLBM(SDL_RWops *src);
extern int IMG_isPCX(SDL_RWops *src);
extern int IMG_isPNG(SDL_RWops *src);
extern int IMG_isPNM(SDL_RWops *src);
extern int IMG_isTIF(SDL_RWops *src);
extern int IMG_isXCF(SDL_RWops *src);
extern int IMG_isXPM(SDL_RWops *src);
extern int IMG_isXV(SDL_RWops *src);
extern int IMG_isWEBP(SDL_RWops *src);

/* Individual loading functions */
extern SDL_Surface * IMG_LoadICO_RW(SDL_RWops *src);
extern SDL_Surface * IMG_LoadCUR_RW(SDL_RWops *src);
extern SDL_Surface * IMG_LoadBMP_RW(SDL_RWops *src);
extern SDL_Surface * IMG_LoadGIF_RW(SDL_RWops *src);
extern SDL_Surface * IMG_LoadJPG_RW(SDL_RWops *src);
extern SDL_Surface * IMG_LoadLBM_RW(SDL_RWops *src);
extern SDL_Surface * IMG_LoadPCX_RW(SDL_RWops *src);
extern SDL_Surface * IMG_LoadPNG_RW(SDL_RWops *src);
extern SDL_Surface * IMG_LoadPNM_RW(SDL_RWops *src);
extern SDL_Surface * IMG_LoadTGA_RW(SDL_RWops *src);
extern SDL_Surface * IMG_LoadTIF_RW(SDL_RWops *src);
extern SDL_Surface * IMG_LoadXCF_RW(SDL_RWops *src);
extern SDL_Surface * IMG_LoadXPM_RW(SDL_RWops *src);
extern SDL_Surface * IMG_LoadXV_RW(SDL_RWops *src);
extern SDL_Surface * IMG_LoadWEBP_RW(SDL_RWops *src);

/* We'll use SDL for reporting errors */
#define IMG_SetError	SDL_SetError


#endif
