#ifndef _SDL_endian_h
#define _SDL_endian_h

#include "SDL_stdinc.h"

#define SDL_LIL_ENDIAN	1234
#define SDL_BIG_ENDIAN	4321

#define SDL_BYTEORDER	SDL_LIL_ENDIAN

static __inline__ Uint16 SDL_Swap16(Uint16 x)
{
	__asm__("xchgb %b0,%h0" : "=q" (x) :  "0" (x));
	return x;
}


static __inline__ Uint32 SDL_Swap32(Uint32 x)
{
	__asm__("bswapl %0" : "=r" (x) : "0" (x));
	return x;
}

#define SDL_SwapLE16(X)	(X)
#define SDL_SwapLE32(X)	(X)
#define SDL_SwapBE16(X)	SDL_Swap16(X)
#define SDL_SwapBE32(X)	SDL_Swap32(X)

#endif
