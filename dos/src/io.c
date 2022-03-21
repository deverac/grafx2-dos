/* vim:expandtab:ts=2 sw=2:
*/
/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2008 Yves Rizoud
    Copyright 2007 Adrien Destugues
    Copyright 1996-2001 Sunset Design (Guillaume Dorme & Karl Maritaud)

    Grafx2 is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; version 2
    of the License.

    Grafx2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Grafx2; if not, see <http://www.gnu.org/licenses/>
*/

// Fonctions de lecture/ecriture file, gèrent les systèmes big-endian et
// little-endian.

#define _XOPEN_SOURCE 500

#include <SDL_endian.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#if defined(__amigaos4__)
    #include <proto/dos.h>
    #include <dirent.h>
#elif defined(__WIN32__)
    #include <dirent.h>
    #include <windows.h>
#else
    #include <dirent.h>
#endif

#include "struct.h"
#include "io.h"
#include "realpath.h"

// Lit un octet
// Renvoie -1 si OK, 0 en cas d'erreur
int Read_byte(FILE *file, byte *dest)
{
  return fread(dest, 1, 1, file) == 1;
}
// Ecrit un octet
// Renvoie -1 si OK, 0 en cas d'erreur
int Write_byte(FILE *file, byte b)
{
  return fwrite(&b, 1, 1, file) == 1;
}
// Lit des octets
// Renvoie -1 si OK, 0 en cas d'erreur
int Read_bytes(FILE *file, void *dest, size_t size)
{
  return fread(dest, 1, size, file) == size;
}
// Ecrit des octets
// Renvoie -1 si OK, 0 en cas d'erreur
int Write_bytes(FILE *file, void *src, size_t size)
{
  return fwrite(src, 1, size, file) == size;
}

// Lit un word (little-endian)
// Renvoie -1 si OK, 0 en cas d'erreur
int Read_word_le(FILE *file, word *dest)
{
  if (fread(dest, 1, sizeof(word), file) != sizeof(word))
    return 0;
  #if SDL_BYTEORDER != SDL_LIL_ENDIAN
    *dest = SDL_Swap16(*dest);
  #endif
  return -1;
}
// Ecrit un word (little-endian)
// Renvoie -1 si OK, 0 en cas d'erreur
int Write_word_le(FILE *file, word w)
{
  #if SDL_BYTEORDER != SDL_LIL_ENDIAN
    w = SDL_Swap16(w);
  #endif
  return fwrite(&w, 1, sizeof(word), file) == sizeof(word);
}
// Lit un word (big-endian)
// Renvoie -1 si OK, 0 en cas d'erreur
int Read_word_be(FILE *file, word *dest)
{
  if (fread(dest, 1, sizeof(word), file) != sizeof(word))
    return 0;
  #if SDL_BYTEORDER != SDL_BIG_ENDIAN
    *dest = SDL_Swap16(*dest);
  #endif
  return -1;
}
// Ecrit un word (big-endian)
// Renvoie -1 si OK, 0 en cas d'erreur
int Write_word_be(FILE *file, word w)
{
  #if SDL_BYTEORDER != SDL_BIG_ENDIAN
    w = SDL_Swap16(w);
  #endif
  return fwrite(&w, 1, sizeof(word), file) == sizeof(word);
}
// Lit un dword (little-endian)
// Renvoie -1 si OK, 0 en cas d'erreur
int Read_dword_le(FILE *file, dword *dest)
{
  if (fread(dest, 1, sizeof(dword), file) != sizeof(dword))
    return 0;
  #if SDL_BYTEORDER != SDL_LIL_ENDIAN
    *dest = SDL_Swap32(*dest);
  #endif
  return -1;
}
// Ecrit un dword (little-endian)
// Renvoie -1 si OK, 0 en cas d'erreur
int Write_dword_le(FILE *file, dword dw)
{
  #if SDL_BYTEORDER != SDL_LIL_ENDIAN
    dw = SDL_Swap32(dw);
  #endif
  return fwrite(&dw, 1, sizeof(dword), file) == sizeof(dword);
}

// Lit un dword (big-endian)
// Renvoie -1 si OK, 0 en cas d'erreur
int Read_dword_be(FILE *file, dword *dest)
{
  if (fread(dest, 1, sizeof(dword), file) != sizeof(dword))
    return 0;
  #if SDL_BYTEORDER != SDL_BIG_ENDIAN
    *dest = SDL_Swap32(*dest);
  #endif
  return -1;
}
// Ecrit un dword (big-endian)
// Renvoie -1 si OK, 0 en cas d'erreur
int Write_dword_be(FILE *file, dword dw)
{
  #if SDL_BYTEORDER != SDL_BIG_ENDIAN
    dw = SDL_Swap32(dw);
  #endif
  return fwrite(&dw, 1, sizeof(dword), file) == sizeof(dword);
}

// Détermine la position du dernier '/' ou '\\' dans une chaine,
// typiquement pour séparer le nom de file d'un chemin.
// Attention, sous Windows, il faut s'attendre aux deux car 
// par exemple un programme lancé sous GDB aura comme argv[0]:
// d:\Data\C\GFX2\grafx2/grafx2.exe
char * Find_last_slash(const char * str)
{
  const char * position = NULL;
  for (; *str != '\0'; str++)
    if (*str == PATH_SEPARATOR[0]
#ifdef __WIN32__    
     || *str == '/'
#endif
     )
      position = str;
  return (char *)position;
}
// Récupère la partie "nom de file seul" d'un chemin
void Extract_filename(char *dest, const char *source)
{
  const char * position = Find_last_slash(source);

  if (position)
    strcpy(dest,position+1);
  else
    strcpy(dest,source);
}
// Récupère la partie "répertoire+/" d'un chemin.
void Extract_path(char *dest, const char *source)
{
  char * position=NULL;

  Realpath(source,dest);
  position = Find_last_slash(dest);
  if (position)
    *(position+1) = '\0';
  else
    strcat(dest, PATH_SEPARATOR);
}

int File_exists(char * fname)
//   Détermine si un file passé en paramètre existe ou non dans le
// répertoire courant.
{
    struct stat buf;
    int result;

    result=stat(fname,&buf);
    if (result!=0)
        return(errno!=ENOENT);
    else
        return 1;

}
int Directory_exists(char * directory)
//   Détermine si un répertoire passé en paramètre existe ou non dans le
// répertoire courant.
{
  DIR* entry;    // Structure de lecture des éléments

  if (strcmp(directory,PARENT_DIR)==0)
    return 1;
  else
  {
    //  On va chercher si le répertoire existe à l'aide d'un Opendir. S'il
    //  renvoie NULL c'est que le répertoire n'est pas accessible...

    entry=opendir(directory);
    if (entry==NULL)
        return 0;
    else
    {
        closedir(entry);
        return 1;
    }
  }
}

// Taille de fichier, en octets
int File_length(const char * fname)
{
    struct stat infos_fichier;
    if (stat(fname,&infos_fichier))
        return 0;
    return infos_fichier.st_size;
}
int File_length_file(FILE * file)
{
    struct stat infos_fichier;
    if (fstat(fileno(file),&infos_fichier))
        return 0;
    return infos_fichier.st_size;
}

void For_each_file(const char * directory_name, void Callback(const char *))
{
  // Pour scan de répertoire
  DIR*  current_directory; //Répertoire courant
  struct dirent* entry; // Structure de lecture des éléments
  char full_filename[MAX_PATH_CHARACTERS];
  int filename_position;
  strcpy(full_filename, directory_name);
  current_directory=opendir(directory_name);
  if(current_directory == NULL) return;        // Répertoire invalide ...
  filename_position = strlen(full_filename);
  if (filename_position==0 || strcmp(full_filename+filename_position-1,PATH_SEPARATOR))
  {
    strcat(full_filename, PATH_SEPARATOR);
    filename_position = strlen(full_filename);
  }
  while ((entry=readdir(current_directory)))
  {
    struct stat Infos_enreg;
    strcpy(&full_filename[filename_position], entry->d_name);
    stat(full_filename,&Infos_enreg);
    if (S_ISREG(Infos_enreg.st_mode))
    {
      Callback(full_filename);
    }
  }
  closedir(current_directory);
}

void Get_full_filename(char * output_name, char * file_name, char * directory_name)
{
  strcpy(output_name,directory_name);
  if (output_name[0] != '\0')
  {
    // Append a separator at the end of path, if there isn't one already.
    // This handles the case of directory variables which contain one,
    // as well as directories like "/" on Unix.
    if (output_name[strlen(output_name)-1]!=PATH_SEPARATOR[0])
        strcat(output_name,PATH_SEPARATOR);
  }
  strcat(output_name,file_name);
}

/// Lock file used to prevent several instances of grafx2 from harming each others' backups
#ifdef __WIN32__
HANDLE Lock_file_handle = INVALID_HANDLE_VALUE;
#else
int Lock_file_handle = -1;
#endif

byte Create_lock_file(const char *file_directory)
{
  char lock_filename[MAX_PATH_CHARACTERS];
  
  strcpy(lock_filename,file_directory);
  strcat(lock_filename,"gfx2.lck");
  
  #ifdef __WIN32__
  // Windowzy method for creating a lock file
  Lock_file_handle = CreateFile(
    lock_filename,
    GENERIC_WRITE,
    0, // No sharing
    NULL,
    OPEN_ALWAYS,
    FILE_ATTRIBUTE_NORMAL,
    NULL);
  if (Lock_file_handle == INVALID_HANDLE_VALUE)
  {
    return -1;
  }
  #else
  // Unixy method for lock file
  Lock_file_handle = open(lock_filename,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
  if (Lock_file_handle == -1)
  {
    // Usually write-protected media
    return -1;
  }
#if !defined(FDOS)
  // Lock file is useless on DOS.
  if (lockf(Lock_file_handle, F_TLOCK, 0)==-1)
  {
    close(Lock_file_handle);
    // Usually write-protected media
    return -1;
  }
#endif
  #endif
  return 0;
}

void Release_lock_file(const char *file_directory)
{
  char lock_filename[MAX_PATH_CHARACTERS];
    
  #ifdef __WIN32__
  if (Lock_file_handle != INVALID_HANDLE_VALUE)
  {
    CloseHandle(Lock_file_handle);
  }
  #else
  if (Lock_file_handle != -1)
  {
    close(Lock_file_handle);
    Lock_file_handle = -1;
  }  
  #endif
  
  // Actual deletion
  strcpy(lock_filename,file_directory);
  strcat(lock_filename,"gfx2.lck");
  remove(lock_filename);
}
