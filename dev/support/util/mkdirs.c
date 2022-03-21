#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

// This program can create parent directories, similar to the 'mkdir -p' command.

// Modified from:
// https://rosettacode.org/wiki/Make_directory_path#C
// This only *creates* directories. It does not set permissions.
// On linux, the directory is created, but the created directories cannot be ls, cd
// until permissions are set.
int main (int argc, char **argv) {
    char *str, *s;
    struct stat statBuf;
 
    if (argc != 2) {
        fprintf (stderr, "usage: %s <path>\n", basename (argv[0]));
        exit (1);
    }
    s = argv[1];
    while ((str = strtok (s, "/")) != NULL) {
        if (str != s) {
            str[-1] = '/';
        }
        if (stat (argv[1], &statBuf) == -1) {
            mkdir (argv[1], 0);
        } else {
            if (! S_ISDIR (statBuf.st_mode)) {
                fprintf (stderr, "couldn't create directory %s\n", argv[1]);
                exit (1);
            }
        }
        s = NULL;
    }
    return 0;
}
