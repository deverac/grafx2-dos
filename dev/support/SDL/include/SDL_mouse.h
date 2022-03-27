#ifndef _SDL_MOUSE_H
#define _SDL_MOUSE_H


#include "SDL_stdinc.h"


#define SDL_BUTTON_LEFT       1
#define SDL_BUTTON_MIDDLE     2
#define SDL_BUTTON_RIGHT      3
#define SDL_BUTTON_WHEELUP    4
#define SDL_BUTTON_WHEELDOWN  5



void SDL_WarpMouse(Uint16 x, Uint16 y);
int            SDL_ShowCursor(int toggle);


#endif
