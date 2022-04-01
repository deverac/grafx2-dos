#ifndef _SDL_timer_h
#define _SDL_timer_h

/** @file SDL_timer.h
 *  Header for the SDL time management routines
 */

/**
 * Get the number of milliseconds since the SDL library initialization.
 * Note that this value wraps if the program runs for more than ~49 days.
 */ 
Uint32 SDL_GetTicks(void);

/** Wait a specified number of milliseconds before returning */
void SDL_Delay(Uint32 ms);

Uint32 get_millis(void);

#endif
