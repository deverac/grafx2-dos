#ifndef _SDL_keyboard_h
#define _SDL_keyboard_h

#include "SDL_keysym.h"
#include "SDL_stdinc.h"


// Allows using alternate keys for modifiers.
//  LS  Left Shift
//  RS  Right Shift
//  CL  Caps Lock
//  NL  Num Lock
// Athough it is technically possible to set both (e.g.) LS_ALT and LS_CTRL,
// exactly one or zero should be set.
#define FAKE_MOD_LS_ALT   0x0001
#define FAKE_MOD_LS_CTRL  0x0002
#define FAKE_MOD_RS_ALT   0x0004
#define FAKE_MOD_RS_CTRL  0x0008
#define FAKE_MOD_CL_ALT   0x0010
#define FAKE_MOD_CL_CTRL  0x0020
#define FAKE_MOD_NL_ALT   0x0040
#define FAKE_MOD_NL_CTRL  0x0080
// #define FAKE_MOD_TOG_NL   0x0100
// #define FAKE_MOD_TOG_CL   0x0200



typedef enum {
  KMOD_NONE  = 0x0000,
  KMOD_LSHIFT= 0x0001,
  KMOD_RSHIFT= 0x0002,
  KMOD_LCTRL = 0x0040,
  KMOD_RCTRL = 0x0080,
  KMOD_LALT  = 0x0100,
  KMOD_RALT  = 0x0200,
  KMOD_LMETA = 0x0400,
  KMOD_RMETA = 0x0800,
  KMOD_NUM   = 0x1000,
  KMOD_CAPS  = 0x2000,
  KMOD_MODE  = 0x4000,
} SDLMod;

typedef struct SDL_keysym {
    Uint8 scancode;    /**< hardware specific scancode */
    SDLKey sym;        /**< SDL virtual keysym */
    SDLMod mod;        /**< current key modifiers */
    Uint16 unicode;    /**< translated character */
} SDL_keysym;

void           SDL_SetModState(SDLMod mod);
SDLMod         SDL_GetModState(void);
void           SDL_EnableUNICODE(int flag);
void           SDL_EnableKeyRepeat(int a, int b);

void set_fake_modifiers(Uint32 fake_mods);

#endif
