/** @file SDL_joystick.h
 *  Include file for SDL joystick event handling
 */

#ifndef _SDL_joystick_h
#define _SDL_joystick_h


/** @file SDL_joystick.h
 *  @note In order to use these functions, SDL_Init() must have been called
 *        with the SDL_INIT_JOYSTICK flag.  This causes SDL to scan the system
 *        for joysticks, and load appropriate drivers.
 */

/** The joystick structure used to identify an SDL joystick */
struct _SDL_Joystick;
typedef struct _SDL_Joystick SDL_Joystick;


/**
 * Open a joystick for use.
 *
 * @param[in] device_index
 * The index passed as an argument refers to
 * the N'th joystick on the system.  This index is the value which will
 * identify this joystick in future joystick events.
 *
 * @return This function returns a joystick identifier, or NULL if an error occurred.
 */
SDL_Joystick * SDL_JoystickOpen(int device_index);

#endif
