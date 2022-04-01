#ifndef _SDL_mouse_h
#define _SDL_mouse_h

#include "SDL_stdinc.h"



/**
 * Set the position of the mouse cursor (generates a mouse motion event)
 */
void SDL_WarpMouse(Uint16 x, Uint16 y);

/**
 * Toggle whether or not the cursor is shown on the screen.
 * The cursor start off displayed, but can be turned off.
 * SDL_ShowCursor() returns 1 if the cursor was being displayed
 * before the call, or 0 if it was not.  You can query the current
 * state by passing a 'toggle' value of -1.
 */
int SDL_ShowCursor(int toggle);


/** Used as a mask when testing buttons in buttonstate
 *  Button 1:	Left mouse button
 *  Button 2:	Middle mouse button
 *  Button 3:	Right mouse button
 *  Button 4:	Mouse wheel up	 (may also be a real button)
 *  Button 5:	Mouse wheel down (may also be a real button)
 */
#define SDL_BUTTON_LEFT		1
#define SDL_BUTTON_MIDDLE	2
#define SDL_BUTTON_RIGHT	3
#define SDL_BUTTON_WHEELUP	4
#define SDL_BUTTON_WHEELDOWN	5

#endif
