#include "SDL_config.h"

/* This file contains portable string manipulation functions for SDL */

#include "SDL_stdinc.h"

int SDL_vsnprintf(char *text, size_t maxlen, const char *fmt, va_list ap);


size_t SDL_strlcpy(char *dst, const char *src, size_t maxlen)
{
    size_t srclen = SDL_strlen(src);
    if ( maxlen > 0 ) {
        size_t len = SDL_min(srclen, maxlen-1);
        SDL_memcpy(dst, src, len);
        dst[len] = '\0';
    }
    return srclen;
}


char *SDL_strrev(char *string)
{
    size_t len = SDL_strlen(string);
    char *a = &string[0];
    char *b = &string[len-1];
    len /= 2;
    while ( len-- ) {
        char c = *a;
        *a++ = *b;
        *b-- = c;
    }
    return string;
}

char *SDL_strlwr(char *string)
{
    char *bufp = string;
    while ( *bufp ) {
        *bufp = SDL_tolower((unsigned char) *bufp);
	++bufp;
    }
    return string;
}


static const char ntoa_table[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z'
};

char *SDL_ltoa(long value, char *string, int radix)
{
    char *bufp = string;

    if ( value < 0 ) {
        *bufp++ = '-';
        value = -value;
    }
    if ( value ) {
        while ( value > 0 ) {
            *bufp++ = ntoa_table[value % radix];
            value /= radix;
        }
    } else {
        *bufp++ = '0';
    }
    *bufp = '\0';

    /* The numbers went into the string backwards. :) */
    if ( *string == '-' ) {
        SDL_strrev(string+1);
    } else {
        SDL_strrev(string);
    }

    return string;
}

char *SDL_ultoa(unsigned long value, char *string, int radix)
{
    char *bufp = string;

    if ( value ) {
        while ( value > 0 ) {
            *bufp++ = ntoa_table[value % radix];
            value /= radix;
        }
    } else {
        *bufp++ = '0';
    }
    *bufp = '\0';

    /* The numbers went into the string backwards. :) */
    SDL_strrev(string);

    return string;
}


#if defined(__WATCOMC__) || defined(_WIN32) || !defined(HAVE_SNPRINTF)
int SDL_snprintf(char *text, size_t maxlen, const char *fmt, ...)
{
    va_list ap;
    int retval;

    va_start(ap, fmt);
    retval = SDL_vsnprintf(text, maxlen, fmt, ap);
    va_end(ap);

    return retval;
}
#endif







static size_t SDL_PrintLong(char *text, long value, int radix, size_t maxlen)
{
    char num[130];
    size_t size;

    SDL_ltoa(value, num, radix);
    size = SDL_strlen(num);
    if ( size >= maxlen ) {
        size = maxlen-1;
    }
    SDL_strlcpy(text, num, size+1);

    return size;
}
static size_t SDL_PrintUnsignedLong(char *text, unsigned long value, int radix, size_t maxlen)
{
    char num[130];
    size_t size;

    SDL_ultoa(value, num, radix);
    size = SDL_strlen(num);
    if ( size >= maxlen ) {
        size = maxlen-1;
    }
    SDL_strlcpy(text, num, size+1);

    return size;
}



static size_t SDL_PrintFloat(char *text, double arg, size_t maxlen)
{
    char *textstart = text;
    if ( arg ) {
        /* This isn't especially accurate, but hey, it's easy. :) */
        const double precision = 0.00000001;
        size_t len;
        unsigned long value;

        if ( arg < 0 ) {
            *text++ = '-';
            --maxlen;
            arg = -arg;
        }
        value = (unsigned long)arg;
        len = SDL_PrintUnsignedLong(text, value, 10, maxlen);
        text += len;
        maxlen -= len;
        arg -= value;
        if ( arg > precision && maxlen ) {
            int mult = 10;
            *text++ = '.';
            while ( (arg > precision) && maxlen ) {
                value = (unsigned long)(arg * mult);
                len = SDL_PrintUnsignedLong(text, value, 10, maxlen);
                text += len;
                maxlen -= len;
                arg -= (double)value / mult;
                mult *= 10;
            }
        }
    } else {
        *text++ = '0';
    }
    return (text - textstart);
}
static size_t SDL_PrintString(char *text, const char *string, size_t maxlen)
{
    char *textstart = text;
    while ( *string && maxlen-- ) {
        *text++ = *string++;
    }
    return (text - textstart);
}
int SDL_vsnprintf(char *text, size_t maxlen, const char *fmt, va_list ap)
{
    char *textstart = text;
    if ( maxlen <= 0 ) {
        return 0;
    }
    --maxlen; /* For the trailing '\0' */
    while ( *fmt && maxlen ) {
        if ( *fmt == '%' ) {
            SDL_bool done = SDL_FALSE;
            size_t len = 0;
            SDL_bool do_lowercase = SDL_FALSE;
            int radix = 10;
            enum {
                DO_INT,
                DO_LONG,
                DO_LONGLONG
            } inttype = DO_INT;

            ++fmt;
            /* FIXME: implement more of the format specifiers */
            while ( *fmt == '.' || (*fmt >= '0' && *fmt <= '9') ) {
                ++fmt;
            }
            while (!done) {
                switch(*fmt) {
                    case '%':
                        *text = '%';
                        len = 1;
                        done = SDL_TRUE;
                        break;
                    case 'c':
                        /* char is promoted to int when passed through (...) */
                        *text = (char)va_arg(ap, int);
                        len = 1;
                        done = SDL_TRUE;
                        break;
                    case 'h':
                        /* short is promoted to int when passed through (...) */
                        break;
                    case 'l':
                        if ( inttype < DO_LONGLONG ) {
                            ++inttype;
                        }
                        break;
                    case 'I':
                        if ( SDL_strncmp(fmt, "I64", 3) == 0 ) {
                            fmt += 2;
                            inttype = DO_LONGLONG;
                        }
                        break;
                    case 'i':
                    case 'd':
                        switch (inttype) {
                            case DO_INT:
                                len = SDL_PrintLong(text, (long)va_arg(ap, int), radix, maxlen);
                                break;
                            case DO_LONG:
                                len = SDL_PrintLong(text, va_arg(ap, long), radix, maxlen);
                                break;
                            case DO_LONGLONG:
#ifdef SDL_HAS_64BIT_TYPE
                                len = SDL_PrintLongLong(text, va_arg(ap, Sint64), radix, maxlen);
#else
                                len = SDL_PrintLong(text, va_arg(ap, long), radix, maxlen);
#endif
                                break;
                        }
                        done = SDL_TRUE;
                        break;
                    case 'p':
                    case 'x':
                        do_lowercase = SDL_TRUE;
                        /* Fall through to 'X' handling */
                    case 'X':
                        if ( radix == 10 ) {
                            radix = 16;
                        }
                        if ( *fmt == 'p' ) {
                            inttype = DO_LONG;
                        }
                        /* Fall through to unsigned handling */
                    case 'o':
                        if ( radix == 10 ) {
                            radix = 8;
                        }
                        /* Fall through to unsigned handling */
                    case 'u':
                        switch (inttype) {
                            case DO_INT:
                                len = SDL_PrintUnsignedLong(text, (unsigned long)va_arg(ap, unsigned int), radix, maxlen);
                                break;
                            case DO_LONG:
                                len = SDL_PrintUnsignedLong(text, va_arg(ap, unsigned long), radix, maxlen);
                                break;
                            case DO_LONGLONG:
#ifdef SDL_HAS_64BIT_TYPE
                                len = SDL_PrintUnsignedLongLong(text, va_arg(ap, Uint64), radix, maxlen);
#else
                                len = SDL_PrintUnsignedLong(text, va_arg(ap, unsigned long), radix, maxlen);
#endif
                                break;
                        }
                        if ( do_lowercase ) {
                            SDL_strlwr(text);
                        }
                        done = SDL_TRUE;
                        break;
                    case 'f':
                        len = SDL_PrintFloat(text, va_arg(ap, double), maxlen);
                        done = SDL_TRUE;
                        break;
                    case 's':
                        len = SDL_PrintString(text, va_arg(ap, char*), maxlen);
                        done = SDL_TRUE;
                        break;
                    default:
                        done = SDL_TRUE;
                        break;
                }
                ++fmt;
            }
            text += len;
            maxlen -= len;
        } else {
            *text++ = *fmt++;
            --maxlen;
        }
    }
    *text = '\0';

    return (text - textstart);
}
