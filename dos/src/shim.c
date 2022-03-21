#include <stdio.h>  // vsnprintf
#include <string.h> // strcpy
#include <stdarg.h> // va_start
#include <stddef.h> // size_t

// The caller must ensure that the formatted string does not exceed buf.
int snprintf(char* str, size_t size, const char* format, ...)
{
    char buf[400];
    int r;

    va_list args;
    va_start(args, format);
    r = vsprintf(buf, format, args);
    va_end(args);
    buf[size -1] = '\0';
    strcpy(str, buf);
    return r;
}
