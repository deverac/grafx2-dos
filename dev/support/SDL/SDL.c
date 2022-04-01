#include <dos.h>         // int86
#include <sys/nearptr.h> // _djgpp_nearptr_disable

#include "SDL.h"
#include "intvals.h"

extern Uint32 TICKS_EPOCH;

// Returns 0 on success or -1 on error.
int SDL_Init(Uint32 flags)
{
  union REGS regs;
  int is_mouse_avail = 0;

  (void)flags; // ignored

  TICKS_EPOCH = get_millis();

  __djgpp_nearptr_enable(); // Disable 640K memory protection.
                            // This allows writing to video memory.



  // resets mouse to default driver values:
  //    mouse is positioned to screen center
  //    mouse cursor is reset and hidden
  //    no interrupts are enabled (mask = 0)
  //    double speed threshold set to 64 mickeys per second
  //    horizontal mickey to pixel ratio (8 to 8)
  //    vertical mickey to pixel ratio (16 to 8)
  //    max width and height are set to maximum for video mode
  regs.x.ax = 0x0;
  int86(MOUSE_INT, &regs, &regs);
  is_mouse_avail = regs.x.ax;
  
  if (is_mouse_avail == 0xFFFF) {
    // Mouse is available.

    // Set mouse sensitivity.
    regs.x.ax = 0x1a;
    regs.x.bx = 20; // horz-rez (0-100) default: 50
    regs.x.cx = 20; // vert-rez (0-100) default: 50
    regs.x.dx = 0; // double-speed threashold
    int86(MOUSE_INT, &regs, &regs);
  }

  return 0;
}


void SDL_Quit(void)
{
    union REGS regs;

    // Set video (Text mode)
    regs.h.ah = 0x00;
    regs.h.al = 0x03; // 80x25 16 color
    // regs.h.al = 0x07; // 80x25 black/white
    int86(VIDEO_INT, &regs, &regs);

    // Reset typematic rate
    regs.h.ah = 0x03; // set rate function
    regs.h.al = 0x00; // reset to default delay
    int86(KEYBOARD_INT, &regs, &regs);

    // Disable direct accett to lower 640K.
    __djgpp_nearptr_disable();
}
