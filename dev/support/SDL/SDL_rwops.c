#include "SDL_config.h"

/* This file provides a general interface for SDL to read and write
   data sources.  It can easily be extended to files, memory, etc.
*/

#include "SDL_endian.h"
#include "SDL_rwops.h"




#ifdef HAVE_STDIO_H

/* Functions to read/write stdio file pointers */

static int stdio_seek(SDL_RWops *context, int offset, int whence)
{
	if ( fseek(context->hidden.stdio.fp, offset, whence) == 0 ) {
		return(ftell(context->hidden.stdio.fp));
	} else {
		SDL_Error(SDL_EFSEEK);
		return(-1);
	}
}
static int stdio_read(SDL_RWops *context, void *ptr, int size, int maxnum)
{
	size_t nread;

	nread = fread(ptr, size, maxnum, context->hidden.stdio.fp); 
	if ( nread == 0 && ferror(context->hidden.stdio.fp) ) {
		SDL_Error(SDL_EFREAD);
	}
	return(nread);
}
static int stdio_write(SDL_RWops *context, const void *ptr, int size, int num)
{
	size_t nwrote;

	nwrote = fwrite(ptr, size, num, context->hidden.stdio.fp);
	if ( nwrote == 0 && ferror(context->hidden.stdio.fp) ) {
		SDL_Error(SDL_EFWRITE);
	}
	return(nwrote);
}
static int stdio_close(SDL_RWops *context)
{
	if ( context ) {
		if ( context->hidden.stdio.autoclose ) {
			/* WARNING:  Check the return value here! */
			fclose(context->hidden.stdio.fp);
		}
		SDL_FreeRW(context);
	}
	return(0);
}
#endif /* !HAVE_STDIO_H */


SDL_RWops *SDL_RWFromFile(const char *file, const char *mode)
{
	SDL_RWops *rwops = NULL;
#ifdef HAVE_STDIO_H
	FILE *fp = NULL;
	(void) fp;
#endif
	if ( !file || !*file || !mode || !*mode ) {
		SDL_SetError("SDL_RWFromFile(): No file or no mode specified");
		return NULL;
	}

#if defined(__WIN32__) && !defined(__SYMBIAN32__)
	rwops = SDL_AllocRW();
	if (!rwops)
		return NULL; /* SDL_SetError already setup by SDL_AllocRW() */
	if (win32_file_open(rwops,file,mode) < 0) {
		SDL_FreeRW(rwops);
		return NULL;
	}	
	rwops->seek  = win32_file_seek;
	rwops->read  = win32_file_read;
	rwops->write = win32_file_write;
	rwops->close = win32_file_close;

#elif HAVE_STDIO_H

#ifdef __MACOS__
	{
		char *mpath = unix_to_mac(file);
		fp = fopen(mpath, mode);
		SDL_free(mpath);
	}
#else
	fp = fopen(file, mode);
#endif
	if ( fp == NULL ) {
		SDL_SetError("Couldn't open %s", file);
	} else {
		rwops = SDL_RWFromFP(fp, 1);
	}
#else
	SDL_SetError("SDL not compiled with stdio support");
#endif /* !HAVE_STDIO_H */

	return(rwops);
}

#ifdef HAVE_STDIO_H
SDL_RWops *SDL_RWFromFP(FILE *fp, int autoclose)
{
	SDL_RWops *rwops = NULL;

	rwops = SDL_AllocRW();
	if ( rwops != NULL ) {
		rwops->seek = stdio_seek;
		rwops->read = stdio_read;
		rwops->write = stdio_write;
		rwops->close = stdio_close;
		rwops->hidden.stdio.fp = fp;
		rwops->hidden.stdio.autoclose = autoclose;
	}
	return(rwops);
}
#endif /* HAVE_STDIO_H */


SDL_RWops *SDL_AllocRW(void)
{
	SDL_RWops *area;

	area = (SDL_RWops *)SDL_malloc(sizeof *area);
	if ( area == NULL ) {
		SDL_OutOfMemory();
	}
	return(area);
}

void SDL_FreeRW(SDL_RWops *area)
{
	SDL_free(area);
}

/* Functions for dynamically reading and writing endian-specific values */

Uint16 SDL_ReadLE16 (SDL_RWops *src)
{
	Uint16 value;

	SDL_RWread(src, &value, (sizeof value), 1);
	return(SDL_SwapLE16(value));
}
Uint32 SDL_ReadLE32 (SDL_RWops *src)
{
	Uint32 value;

	SDL_RWread(src, &value, (sizeof value), 1);
	return(SDL_SwapLE32(value));
}
Uint32 SDL_ReadBE32 (SDL_RWops *src)
{
	Uint32 value;

	SDL_RWread(src, &value, (sizeof value), 1);
	return(SDL_SwapBE32(value));
}
