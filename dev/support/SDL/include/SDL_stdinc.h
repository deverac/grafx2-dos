#ifndef _SDL_stdinc_h
#define _SDL_stdinc_h

#include <stdint.h>
#include <stddef.h>
#include <ctype.h> // tolower
#include <stdlib.h> // _ltoa
#include <string.h> // strncmp
#include <stdio.h>

typedef int8_t		Sint8;
typedef uint8_t		Uint8;
typedef int16_t		Sint16;
typedef uint16_t	Uint16;
typedef int32_t		Sint32;
typedef uint32_t	Uint32;

#define SDL_tolower(X)  tolower(X)

typedef enum {
	SDL_FALSE = 0,
	SDL_TRUE  = 1
} SDL_bool;

#define SDL_arraysize(array)	(sizeof(array)/sizeof(array[0]))
#define SDL_min(x, y)	(((x) < (y)) ? (x) : (y))

#define SDL_itoa        itoa
#define SDL_memcpy      memcpy
#define SDL_strlen      strlen
#define SDL_strcmp      strcmp
#define SDL_strncmp     strncmp
#define SDL_malloc	malloc
#define SDL_free	free

extern char * SDL_ltoa(long value, char *string, int radix);

extern char * SDL_ultoa(unsigned long value, char *string, int radix);

extern char * SDL_strlwr(char *string);
extern size_t SDL_strlcpy(char *dst, const char *src, size_t maxlen);
extern int SDL_snprintf(char *text, size_t maxlen, const char *fmt, ...);
#endif
