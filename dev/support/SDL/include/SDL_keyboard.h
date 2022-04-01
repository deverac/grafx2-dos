#ifndef _SDL_keyboard_h
#define _SDL_keyboard_h

#include "SDL_stdinc.h"
#include "SDL_keysym.h"


/** Keysym structure
 *
 *  - The scancode is hardware dependent, and should not be used by general
 *    applications.  If no hardware scancode is available, it will be 0.
 *
 *  - The 'unicode' translated character is only available when character
 *    translation is enabled by the SDL_EnableUNICODE() API.  If non-zero,
 *    this is a UNICODE character corresponding to the keypress.  If the
 *    high 9 bits of the character are 0, then this maps to the equivalent
 *    ASCII character:
 *      @code
 *	char ch;
 *	if ( (keysym.unicode & 0xFF80) == 0 ) {
 *		ch = keysym.unicode & 0x7F;
 *	} else {
 *		An international character..
 *	}
 *      @endcode
 */
typedef struct SDL_keysym {
	Uint8 scancode;			/**< hardware specific scancode */
	SDLKey sym;			/**< SDL virtual keysym */
	SDLMod mod;			/**< current key modifiers */
	Uint16 unicode;			/**< translated character */
} SDL_keysym;

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


/* Function prototypes */
/**
 * Enable/Disable UNICODE translation of keyboard input.
 *
 * This translation has some overhead, so translation defaults off.
 *
 * @param[in] enable
 * If 'enable' is 1, translation is enabled.
 * If 'enable' is 0, translation is disabled.
 * If 'enable' is -1, the translation state is not changed.
 *
 * @return It returns the previous state of keyboard translation.
 */
void SDL_EnableUNICODE(int enable);

/**
 * Enable/Disable keyboard repeat.  Keyboard repeat defaults to off.
 *
 *  @param[in] delay
 *  'delay' is the initial delay in ms between the time when a key is
 *  pressed, and keyboard repeat begins.
 *
 *  @param[in] interval
 *  'interval' is the time in ms between keyboard repeat events.
 *
 *  If 'delay' is set to 0, keyboard repeat is disabled.
 */
void SDL_EnableKeyRepeat(int delay, int interval);


/**
 * Get the current key modifier state
 */
SDLMod SDL_GetModState(void);

/**
 * Set the current key modifier state.
 * This does not change the keyboard state, only the key modifier flags.
 */
void SDL_SetModState(SDLMod modstate);


void set_fake_modifiers(Uint32 fake_mods);

#endif
