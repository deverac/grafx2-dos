/* vim:expandtab:ts=2 sw=2:
*/
#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#if defined(FDOS)
#include <limits.h> // PATH_MAX  (512 bytes)
#endif

#if defined(__AROS__) || defined(__BEOS__) || defined(__MORPHOS__) || defined(__GP2X__) || defined(__amigaos__)
// These platforms don't have realpath().
// We use the following implementation, found in:
// http://amiga.sourceforge.net/amigadevhelp/FUNCTIONS/GeekGadgets/realpath/ex02_realpath.c
//
// When tested on Debian, this piece of code doesn't resolve
// symbolic link in the filename itself, only on the directories in
// the path. So this implementation is limited, it's really better to
// use realpath() if your platform has it.
  
    #if defined(__GP2X__)
        // This is a random default value ...
        #define PATH_MAX 32768
    #endif
  
    static char *sep(char *path)
    {
        char *tmp, c;
        
        tmp = strrchr(path, '/');
        if(tmp) {
            c = tmp[1];
            tmp[1] = 0;
            if (chdir(path)) {
                return NULL;
            }
            tmp[1] = c;
            
            return tmp + 1;
        }
        return path;
    }

    char *Realpath(const char *_path, char *resolved_path)
    {
        int fd = open(".", O_RDONLY), l;
        char current_dir_path[PATH_MAX];
        char path[PATH_MAX], lnk[PATH_MAX], *tmp = (char *)"";
        
        if (fd < 0) {
            return NULL;
        }
        getcwd(current_dir_path,PATH_MAX);
        strncpy(path, _path, PATH_MAX);
        
        if (chdir(path)) {
            if (errno == ENOTDIR) {
                #if defined(__WIN32__) || defined(__MORPHOS__) || defined(__amigaos__)
                    // No symbolic links and no readlink()
                    l = -1;
                #else
                    l = readlink(path, lnk, PATH_MAX);
                    #endif
                    if (!(tmp = sep(path))) {
                        resolved_path = NULL;
                        goto abort;
                    }
                    if (l < 0) {
                        if (errno != EINVAL) {
                            resolved_path = NULL;
                            goto abort;
                        }
                    } else {
                        lnk[l] = 0;
                        if (!(tmp = sep(lnk))) {
                            resolved_path = NULL;
                            goto abort;
                        }
                    }
            } else {
                resolved_path = NULL;
                goto abort;
            }
        }
        
        if(resolved_path==NULL) // if we called realpath with null as a 2nd arg
            resolved_path = (char*) malloc( PATH_MAX );
                
        if (!getcwd(resolved_path, PATH_MAX)) {
            resolved_path = NULL;
            goto abort;
        }
        
        if(strcmp(resolved_path, "/") && *tmp) {
            strcat(resolved_path, "/");
        }
        
        strcat(resolved_path, tmp);
      abort:
        chdir(current_dir_path);
        close(fd);
        return resolved_path;
    }
            
#elif defined (__WIN32__)
// Mingw has a working equivalent. It only has reversed arguments.
    char *Realpath(const char *_path, char *resolved_path)
    {
        return _fullpath(resolved_path,_path,260);
    }
#elif defined(FDOS)
    // Appends tail to body only if total length is less than PATH_MAX. 
    // Returns 1 on success, 0 on failure.
    int catpaths(char* body, const char* tail) {
        if (strlen(body) + strlen(tail) < PATH_MAX) {
            strcat(body, tail);
            return 1;
        }
        return 0;
    }

    // Returns the absolute path of the '_path' parameter or NULL on error.
    // If NULL is supplied as the 'resolved_path' parameter, the returned
    // string will have been malloc()ed and should be free()ed when it is
    // no longer needed.
    //
    // Both forward and back slashes can be used as the directory separator,
    // however, if _path is from the command line the path must use '\' as the
    // dircetory separator otherwise Grafx2 will interpret it as a parameter.
    // The returned string will have all '\' converted to '/'.
    // Paths can be relative or absolute. e.g.:
    //     c:\dir\file.png
    //     file.png
    //     dir\file.png
    //     ..\dir\file.png
    // Invalid paths such as 'c:file.png' or 'c:dir/file.png' will cause Grafx2
    // to abort with file not found error.
    char *Realpath(const char *_path, char *resolved_path)
    {
        char* p;
        if (! resolved_path) {
            resolved_path = malloc(PATH_MAX);
        }

        if (resolved_path) {
            resolved_path[0] = '\0';

            if (index(_path, ':')) { // We have absolute path.
                if (!catpaths(resolved_path, _path)) {
                    return NULL;
                }
            } else {
                char current_dir_path[PATH_MAX];
                // getcwd() returns 'c:/' at root and 'c:/dir' in dir.
                if (getcwd(current_dir_path, PATH_MAX-2)) {
                    int n = strlen(current_dir_path);
                    if (current_dir_path[n-1] != '/') {
                        current_dir_path[n] = '/';
                        current_dir_path[n+1] = '\0';
                    }
                    if (!catpaths(resolved_path, current_dir_path)) {
                        return NULL;
                    }
                    if (!catpaths(resolved_path, _path)) {
                        return NULL;
                    }
                }
            }

            // Change all '\' to '/'.
            p = resolved_path;
            while (*p) {
                if (*p == '\\') { *p = '/'; }
                p++;
            }

        }
        return resolved_path;
    }
#else
// Use the stdlib function.
    char *Realpath(const char *_path, char *resolved_path)
    {
        return realpath(_path, resolved_path);
    }
#endif


