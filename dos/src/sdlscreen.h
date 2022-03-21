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

//////////////////////////////////////////////////////////////////////////////
///@file sdlscreen.h
/// Screen update (refresh) system, and some SDL-specific graphic functions.
//////////////////////////////////////////////////////////////////////////////

#ifndef SDLSCREEN_H_INCLUDED
#define SDLSCREEN_H_INCLUDED

#include <SDL.h>
#include "struct.h"

///
/// This is the number of bytes in a video line for the current mode.
/// On many platforms it will be the video mode's width (in pixels), rounded up
/// to be a multiple of 4.
#define VIDEO_LINE_WIDTH (Screen_SDL->pitch)

void Set_mode_SDL(int *,int *,int);

SDL_Rect ** List_SDL_video_modes;
byte* Screen_pixels;

void Update_rect(short x, short y, unsigned short width, unsigned short height);
void Flush_update(void);
///
/// Converts a SDL_Surface (indexed colors or RGB) into an array of bytes
/// (indexed colors).
/// If dest is NULL, it's allocated by malloc(). Otherwise, be sure to
/// pass a buffer of the right dimensions.
byte * Surface_to_bytefield(SDL_Surface *source, byte * dest);
/// Gets the RGB 24-bit color currently associated with a palette index.
SDL_Color Color_to_SDL_color(byte);
/// Reads a pixel in a 8-bit SDL surface.
byte Get_SDL_pixel_8(SDL_Surface *bmp, int x, int y);
/// Reads a pixel in a multi-byte SDL surface.
dword Get_SDL_pixel_hicolor(SDL_Surface *bmp, int x, int y);
/// Convert a SDL Palette to a grafx2 palette
void Get_SDL_Palette(const SDL_Palette * sdl_palette, T_Palette palette);

///
/// Clears the parts of screen that are outside of the editing area.
/// There is such area only if the screen mode is not a multiple of the pixel
/// size, eg: 3x3 pixels in 1024x768 leaves 1 column on the right, 0 rows on bottom.
void Clear_border(byte color);
  
#endif // SDLSCREEN_H_INCLUDED
