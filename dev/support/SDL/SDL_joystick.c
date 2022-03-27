#include <stdlib.h>

#include "SDL_joystick.h"

// Returns a joystick identifier or NULL if an error occurred.
// (Joystick is currently not supported.)
SDL_Joystick * SDL_JoystickOpen(int device_index) {
  (void) device_index;
  return NULL;
}

