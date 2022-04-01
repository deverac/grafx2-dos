#ifndef _SDL_rwops_h
#define _SDL_rwops_h

#include "SDL_stdinc.h"
#include "SDL_error.h"

/** This is the read/write operation structure -- very basic */

typedef struct SDL_RWops {
	/** Seek to 'offset' relative to whence, one of stdio's whence values:
	 *	SEEK_SET, SEEK_CUR, SEEK_END
	 *  Returns the final offset in the data source.
	 */
	int (*seek)(struct SDL_RWops *context, int offset, int whence);

	/** Read up to 'maxnum' objects each of size 'size' from the data
	 *  source to the area pointed at by 'ptr'.
	 *  Returns the number of objects read, or -1 if the read failed.
	 */
	int (*read)(struct SDL_RWops *context, void *ptr, int size, int maxnum);

	/** Write exactly 'num' objects each of size 'objsize' from the area
	 *  pointed at by 'ptr' to data source.
	 *  Returns 'num', or -1 if the write failed.
	 */
	int (*write)(struct SDL_RWops *context, const void *ptr, int size, int num);

	/** Close and free an allocated SDL_FSops structure */
	int (*close)(struct SDL_RWops *context);

	Uint32 type;
	union {
#if defined(__WIN32__) && !defined(__SYMBIAN32__)
	    struct {
		int   append;
		void *h;
		struct {
		    void *data;
		    int size;
		    int left;
		} buffer;
	    } win32io;
#endif
#ifdef HAVE_STDIO_H 
	    struct {
		int autoclose;
	 	FILE *fp;
	    } stdio;
#endif
	    struct {
		Uint8 *base;
	 	Uint8 *here;
		Uint8 *stop;
	    } mem;
	    struct {
		void *data1;
	    } unknown;
	} hidden;

} SDL_RWops;


/** @name Functions to create SDL_RWops structures from various data sources */

SDL_RWops * SDL_RWFromFile(const char *file, const char *mode);

#ifdef HAVE_STDIO_H
SDL_RWops * SDL_RWFromFP(FILE *fp, int autoclose);
#endif


extern SDL_RWops * SDL_AllocRW(void);
extern void SDL_FreeRW(SDL_RWops *area);


/** @name Seek Reference Points */
#define RW_SEEK_SET	0	/**< Seek from the beginning of data */
#define RW_SEEK_CUR	1	/**< Seek relative to current read point */
#define RW_SEEK_END	2	/**< Seek relative to the end of data */

/** @name Macros to easily read and write from an SDL_RWops structure */
/*@{*/
#define SDL_RWseek(ctx, offset, whence)	(ctx)->seek(ctx, offset, whence)
#define SDL_RWtell(ctx)			(ctx)->seek(ctx, 0, RW_SEEK_CUR)
#define SDL_RWread(ctx, ptr, size, n)	(ctx)->read(ctx, ptr, size, n)
#define SDL_RWwrite(ctx, ptr, size, n)	(ctx)->write(ctx, ptr, size, n)
#define SDL_RWclose(ctx)		(ctx)->close(ctx)


/** @name Read an item of the specified endianness and return in native format */

Uint16 SDL_ReadLE16 (SDL_RWops *src);
Uint32 SDL_ReadLE32 (SDL_RWops *src);
Uint32 SDL_ReadBE32(SDL_RWops *src);

#endif
