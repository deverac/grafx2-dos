
#include <dos.h> // int86
#include "SDL_stdinc.h" // Uint32
#include "intvals.h" // DOS_INT

Uint32 TICKS_EPOCH = 0;


void SDL_Delay(int ms) {
    union REGS regs;
    // CX:DX number of microseconds to wait
    // granularity is 976 microseconds
    long micro_secs = ms * 1000;
    int cx = micro_secs / 0xFFFF;
    int dx = micro_secs - (cx * 0xFFFF);
    regs.h.ah = 0x86;
    regs.x.cx = cx;
    regs.x.dx = dx;
    int86(BIOS15_INT, &regs, &regs);
}


// Return elapsed time in milliseconds since starting. This should give
// run-time for a month before rollover. FreeDOS has 5/100 sec resolution.
// Uint32 is 2^32     = 4,294,967,295
// 31*24*60*60*100*10 = 2,678,400,000
Uint32 get_millis() {
  static int init_day = 0;
  static int max_day = 0;
  union REGS regs;
  int multiplier;
  int days;
  int hours;
  int minutes;
  int seconds;
  int hundreths;


  // Get date
  regs.h.ah = 0x2a; // function Get system date
  int86(DOS_INT, &regs, &regs);
  // year = regs.x.cx;  // (1980-2099)
  // month = regs.h.dh; // (1-12)
  days = regs.h.dl;     // (1-31)

  if (!init_day) {
    init_day = days;
    init_day--;
  }

  if (days > max_day) {
    max_day = days;
  }

  multiplier = max_day - init_day;
  if (days < init_day) {
       multiplier += days;
  }

  // Get time
  regs.h.ah = 0x2c; // function Get system time
  int86(DOS_INT, &regs, &regs);
  hours = regs.h.ch;     // (0-23)
  minutes = regs.h.cl;   // (0-59)
  seconds = regs.h.dh;   // (0-59)
  hundreths = regs.h.dl; // (0-99)

  //return multiplier * (((((((hours*60) + minutes)*60) + seconds)*100) + hundreths)*10);

  // FIXME? This seems to result in fewer eratic mouse movements.
  return (((((hours*60) + minutes)*60) + seconds)*100) + hundreths;
}


// Return the number of milliseconds since the SDL library initialization.
// Due to DOS, resolution is 50 ms (5/100 second).
Uint32 SDL_GetTicks(void) {
  return get_millis() - TICKS_EPOCH;
}
