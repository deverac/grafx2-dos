#ifndef _SDL_H
#define _SDL_H

#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_rwops.h"
#include "SDL_stdinc.h"
#include "SDL_timer.h"
#include "SDL_video.h"

#define SDL_INIT_TIMER    0x00000001
#define SDL_INIT_VIDEO    0x00000020
#define SDL_INIT_JOYSTICK 0x00000200

int            SDL_Init(Uint32 flags);

void           SDL_Quit(void);

#endif
