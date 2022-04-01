#include <dos.h>         // int86

#include "intvals.h"
#include "SDL_video.h"
#include "SDL_mouse.h"


// The 'toggle' parameter should be SDL_ENABLE or SDL_DISABLE.
// The value passed in will be returned.
int SDL_ShowCursor (int toggle)
{
    union REGS regs;
    if (toggle) {
        regs.x.ax=0x1; // show;
    } else {
        regs.x.ax=0x2; // hide;
    }
    int86(MOUSE_INT, &regs, &regs);

   return toggle;
}

void SDL_WarpMouse (Uint16 x, Uint16 y)
{
    union REGS regs;
    if (x >= FDOS_SCREEN_WIDTH) x = FDOS_SCREEN_WIDTH - 1;
    if (y >= FDOS_SCREEN_HEIGHT) y = FDOS_SCREEN_HEIGHT - 1;
    regs.x.ax=0x4; // set mouse cursor position
    regs.x.cx=x;   // horz pos
    regs.x.dx=y;   // vert pos
    int86(MOUSE_INT, &regs, &regs);
}
