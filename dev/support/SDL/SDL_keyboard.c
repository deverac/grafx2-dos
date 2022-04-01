
#include <dos.h> // int86

#include "SDL_keyboard.h"

#include "intvals.h"

#define MOD_UNSET(m, f) (m &= ~(f))
#define MOD_SET(m, f) (m |= (f))

#define FAKE_MOD_UNSET(f) (MOD_UNSET(FAKE_MODIFIERS, (f)))
#define FAKE_MOD_SET(f)   (MOD_SET(FAKE_MODIFIERS, (f)))

#define IS_MOD_SET(m, f) ((m) & (f))
#define IS_FAKE_MOD_SET(f) (IS_MOD_SET(FAKE_MODIFIERS, (f)))





static SDLMod JOYSTICK_MODIFIERS = 0; // Not used

static Uint32 FAKE_MODIFIERS = 0;



// Set the appropriate bits in FAKE_MODIFIERS.
// The 'fake_mods' parameter can be a bitfield values OR'ed together.
void set_fake_modifiers(Uint32 fake_mods) {
    if (fake_mods & FAKE_MOD_LS_ALT) {
        FAKE_MOD_SET(FAKE_MOD_LS_ALT);
        FAKE_MOD_UNSET(FAKE_MOD_LS_CTRL);
    }
    if (fake_mods & FAKE_MOD_LS_CTRL) {
        FAKE_MOD_SET(FAKE_MOD_LS_CTRL);
        FAKE_MOD_UNSET(FAKE_MOD_LS_ALT);
    }
    if (fake_mods & FAKE_MOD_RS_ALT) {
        FAKE_MOD_SET(FAKE_MOD_RS_ALT);
        FAKE_MOD_UNSET(FAKE_MOD_RS_CTRL);
    }
    if (fake_mods & FAKE_MOD_RS_CTRL) {
        FAKE_MOD_SET(FAKE_MOD_RS_CTRL);
        FAKE_MOD_UNSET(FAKE_MOD_RS_ALT);
    }
    if (fake_mods & FAKE_MOD_CL_ALT) {
        FAKE_MOD_SET(FAKE_MOD_CL_ALT);
        FAKE_MOD_UNSET(FAKE_MOD_CL_CTRL);
    }
    if (fake_mods & FAKE_MOD_CL_CTRL) {
        FAKE_MOD_SET(FAKE_MOD_CL_CTRL);
        FAKE_MOD_UNSET(FAKE_MOD_CL_ALT);
    }
    if (fake_mods & FAKE_MOD_NL_ALT) {
        FAKE_MOD_SET(FAKE_MOD_NL_ALT);
        FAKE_MOD_UNSET(FAKE_MOD_NL_CTRL);
    }
    if (fake_mods & FAKE_MOD_NL_CTRL) {
        FAKE_MOD_SET(FAKE_MOD_NL_CTRL);
        FAKE_MOD_UNSET(FAKE_MOD_NL_ALT);
    }
}



void SDL_EnableUNICODE(int enable)
{
  (void)enable; // Silence 'unused parameter' compiler warning.
  // Ignored
}


// Get the state of modifiers.
SDLMod SDL_GetModState (void)
{
    unsigned int mod_flags = 0;
    unsigned int mod = 0;
    union REGS regs;

    // Returns the current status of the shift flags in ax.
    // The shift flags are defined as follows:
    //   bit 15: SysReq key pressed             0x8000
    //   bit 14: Capslock key currently down    0x4000
    //   bit 13: Numlock key currently down     0x2000
    //   bit 12: Scroll lock key currently down 0x1000
    //   bit 11: Right alt key is down          0x0800
    //   bit 10: Right ctrl key is down         0x0400
    //   bit 9: Left alt key is down            0x0200
    //   bit 8: Left ctrl key is down           0x0100
    //   bit 7: Insert toggle                   0x80
    //   bit 6: Capslock toggle                 0x40
    //   bit 5: Numlock toggle                  0x20
    //   bit 4: Scroll lock toggle              0x10
    //   bit 3: Either alt key is down (some machines, left only) 0x8
    //   bit 2: Either ctrl key is down         0x4
    //   bit 1: Left shift key is down          0x2
    //   bit 0: Right shift key is down         0x1

    regs.h.ah = 0x12;
    int86(KEYBOARD_INT, &regs, &regs);
    mod_flags = regs.x.ax;

    if (mod_flags & 0x4000) {
        MOD_SET(mod, KMOD_CAPS);
    }
    if (mod_flags & 0x2000) {
        MOD_SET(mod, KMOD_NUM);
    }
    if (mod_flags & 0x8) {
        MOD_SET(mod, KMOD_ALT);
    }
    if (mod_flags & 0x4) {
        MOD_SET(mod, KMOD_CTRL);
    }
    if (mod_flags & 0x2) {
        MOD_SET(mod, KMOD_LSHIFT);
    }
    if (mod_flags & 0x1) {
        MOD_SET(mod, KMOD_RSHIFT);
    }


    if (IS_MOD_SET(mod, KMOD_CAPS)) {
        if (IS_FAKE_MOD_SET(FAKE_MOD_CL_CTRL)) {
            MOD_UNSET(mod, KMOD_CAPS);
            MOD_SET(mod, KMOD_CTRL);
        } else if (IS_FAKE_MOD_SET(FAKE_MOD_CL_ALT)) {
            MOD_UNSET(mod, KMOD_CAPS);
            MOD_SET(mod, KMOD_ALT);
        }
    }

    if (IS_MOD_SET(mod, KMOD_NUM)) {
        if (IS_FAKE_MOD_SET(FAKE_MOD_NL_CTRL)) {
            MOD_UNSET(mod, KMOD_NUM);
            MOD_SET(mod, KMOD_CTRL);
        } else if (IS_FAKE_MOD_SET(FAKE_MOD_NL_ALT)) {
            MOD_UNSET(mod, KMOD_NUM);
            MOD_SET(mod, KMOD_ALT);
        }
    }

    if (IS_MOD_SET(mod, KMOD_RSHIFT)) {
        if (IS_FAKE_MOD_SET(FAKE_MOD_RS_CTRL)) {
            MOD_UNSET(mod, KMOD_RSHIFT);
            MOD_SET(mod, KMOD_CTRL);
        } else if (IS_FAKE_MOD_SET(FAKE_MOD_RS_ALT)) {
            MOD_UNSET(mod, KMOD_RSHIFT);
            MOD_SET(mod, KMOD_ALT);
        }
    }

    if (IS_MOD_SET(mod, KMOD_LSHIFT)) {
        if (IS_FAKE_MOD_SET(FAKE_MOD_LS_CTRL)) {
            MOD_UNSET(mod, KMOD_LSHIFT);
            MOD_SET(mod, KMOD_CTRL);
        } else if (IS_FAKE_MOD_SET(FAKE_MOD_LS_ALT)) {
            MOD_UNSET(mod, KMOD_LSHIFT);
            MOD_SET(mod, KMOD_ALT);
        }
    }

    // return mod | JOYSTICK_MODIFIERS;
    return mod;
}


// In Grafx2 SDL_SetModState() is only used to set modifier keys when using
// a joystick. Since a Joysticks is not supported, this function is basically
// useless and only exists to allow compiling to succeed.
void SDL_SetModState(SDLMod mod) {
  JOYSTICK_MODIFIERS = mod;
}

// e.g. SDL_EnableKeyRepeat(250, 32);
void SDL_EnableKeyRepeat(int delay_ms, int interval_ms)
{
  union REGS regs;
  int delay_val = 0;
  int interval_val = 30;

  if (delay_ms == 250) {
    delay_val = 0;
  }
  if (interval_ms == 32) {
    interval_val = 30;
  }

  regs.h.ah = 0x03;         // typematic function
  regs.h.al = 0x05;         // set rate/delay
  regs.h.bh = delay_val;    // 0=250, 1=500, 2=750, 3=1000
  regs.h.bl = interval_val; // 0=30, 0x1f=2
  int86(VIDEO_INT, &regs, &regs);
}
