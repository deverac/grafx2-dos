#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "SDL_image.h"

#define ARRAYSIZE(a) (sizeof(a) / sizeof((a)[0]))

extern SDL_Surface* malloc_surface(Uint32 flags, int width, int height, int bpp, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);

/* Table of image detection and loading functions */
static struct {
	char *type;
	int (*is)(SDL_RWops *src);
	SDL_Surface *(*load)(SDL_RWops *src);
} supported[] = {
	/* keep magicless formats first */
	{ "TGA", NULL,      IMG_LoadTGA_RW },
	{ "CUR", IMG_isCUR, IMG_LoadCUR_RW },
	{ "ICO", IMG_isICO, IMG_LoadICO_RW },
	{ "BMP", IMG_isBMP, IMG_LoadBMP_RW },
	{ "GIF", IMG_isGIF, IMG_LoadGIF_RW },
	{ "JPG", IMG_isJPG, IMG_LoadJPG_RW },
	{ "LBM", IMG_isLBM, IMG_LoadLBM_RW },
	{ "PCX", IMG_isPCX, IMG_LoadPCX_RW },
	{ "PNG", IMG_isPNG, IMG_LoadPNG_RW },
	{ "PNM", IMG_isPNM, IMG_LoadPNM_RW }, /* P[BGP]M share code */
	{ "TIF", IMG_isTIF, IMG_LoadTIF_RW },
	{ "XCF", IMG_isXCF, IMG_LoadXCF_RW },
	{ "XPM", IMG_isXPM, IMG_LoadXPM_RW },
	{ "XV",  IMG_isXV,  IMG_LoadXV_RW  },
	{ "WEBP", IMG_isWEBP, IMG_LoadWEBP_RW },
};


extern int IMG_InitJPG();
extern void IMG_QuitJPG();
extern int IMG_InitPNG();
extern void IMG_QuitPNG();
extern int IMG_InitTIF();
extern void IMG_QuitTIF();

extern int IMG_InitWEBP();
extern void IMG_QuitWEBP();

static int initialized = 0;

int IMG_Init(int flags)
{
	int result = 0;

	if (flags & IMG_INIT_JPG) {
		if ((initialized & IMG_INIT_JPG) || IMG_InitJPG() == 0) {
			result |= IMG_INIT_JPG;
		}
	}
	if (flags & IMG_INIT_PNG) {
		if ((initialized & IMG_INIT_PNG) || IMG_InitPNG() == 0) {
			result |= IMG_INIT_PNG;
		}
	}
	if (flags & IMG_INIT_TIF) {
		if ((initialized & IMG_INIT_TIF) || IMG_InitTIF() == 0) {
			result |= IMG_INIT_TIF;
		}
	}
	if (flags & IMG_INIT_WEBP) {
		if ((initialized & IMG_INIT_WEBP) || IMG_InitWEBP() == 0) {
			result |= IMG_INIT_WEBP;
		}
	}
	initialized |= result;

	return (initialized);
}

void IMG_Quit()
{
	if (initialized & IMG_INIT_JPG) {
		IMG_QuitJPG();
	}
	if (initialized & IMG_INIT_PNG) {
		IMG_QuitPNG();
	}
	if (initialized & IMG_INIT_TIF) {
		IMG_QuitTIF();
	}
	if (initialized & IMG_INIT_WEBP) {
		IMG_QuitWEBP();
	}
	initialized = 0;
}

/* Load an image from a file */
SDL_Surface *IMG_Load(const char *file)
{
    SDL_RWops *src = SDL_RWFromFile(file, "rb");
    char *ext = strrchr(file, '.');
    if(ext) {
        ext++;
    }
    if(!src) {
        /* The error message has been set in SDL_RWFromFile */
        return NULL;
    }
    return IMG_LoadTyped_RW(src, 1, ext);
}

/* Portable case-insensitive string compare function */
static int IMG_string_equals(const char *str1, const char *str2)
{
	while ( *str1 && *str2 ) {
		if ( toupper((unsigned char)*str1) !=
		     toupper((unsigned char)*str2) )
			break;
		++str1;
		++str2;
	}
	return (!*str1 && !*str2);
}

/* Load an image from an SDL datasource, optionally specifying the type */
SDL_Surface *IMG_LoadTyped_RW(SDL_RWops *src, int freesrc, char *type)
{
	unsigned int i;
	SDL_Surface *image;

	/* Make sure there is something to do.. */
	if ( src == NULL ) {
		IMG_SetError("Passed a NULL data source");
		return(NULL);
	}

	/* See whether or not this data source can handle seeking */
	if ( SDL_RWseek(src, 0, RW_SEEK_CUR) < 0 ) {
		IMG_SetError("Can't seek in this data source");
		if(freesrc)
			SDL_RWclose(src);
		return(NULL);
	}

	/* Detect the type of image being loaded */
	image = NULL;
	for ( i=0; i < ARRAYSIZE(supported); ++i ) {
		if(supported[i].is) {
			if(!supported[i].is(src))
				continue;
		} else {
			/* magicless format */
			if(!type
			   || !IMG_string_equals(type, supported[i].type))
				continue;
		}
#ifdef DEBUG_IMGLIB
		fprintf(stderr, "IMGLIB: Loading image as %s\n",
			supported[i].type);
#endif
		image = supported[i].load(src);
		if(freesrc)
			SDL_RWclose(src);
		return image;
	}

	if ( freesrc ) {
		SDL_RWclose(src);
	}
	IMG_SetError("Unsupported image format");
	return NULL;
}

