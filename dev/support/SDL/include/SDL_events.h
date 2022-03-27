#ifndef _SDL_events_h
#define _SDL_events_h

#include "SDL_stdinc.h"
#include "SDL_mouse.h"
#include "SDL_joystick.h"

#define SDL_ENABLE  1
#define SDL_DISABLE 0

typedef enum {
    SDL_NOEVENT = 0,
    SDL_KEYDOWN,
    SDL_KEYUP,
    SDL_MOUSEMOTION,
    SDL_MOUSEBUTTONDOWN,
    SDL_MOUSEBUTTONUP,
    SDL_JOYAXISMOTION,
    SDL_JOYBUTTONDOWN,
    SDL_JOYBUTTONUP,
    SDL_VIDEORESIZE,
    SDL_QUIT
} SDL_EventType;



/** Joystick button event structure */
typedef struct SDL_JoyButtonEvent {
    Uint8 type;   /**< SDL_JOYBUTTONDOWN or SDL_JOYBUTTONUP */
    Uint8 which;  /**< The joystick device index */
    Uint8 button; /**< The joystick button index */
    Uint8 state;  /**< SDL_PRESSED or SDL_RELEASED */
} SDL_JoyButtonEvent;

/** Joystick axis motion event structure */
typedef struct SDL_JoyAxisEvent {
    Uint8 type;   /**< SDL_JOYAXISMOTION */
    Uint8 which;  /**< The joystick device index */
    Uint8 axis;   /**< The joystick axis index */
    Sint16 value; /**< The axis value (range: -32768 to 32767) */
} SDL_JoyAxisEvent;

#include "SDL_keyboard.h"

typedef struct{
  Uint8 type;
} SDL_QuitEvent;

typedef struct{
    Uint32 type;
    int w;
    int h;
} SDL_ResizeEvent;

typedef struct SDL_KeyboardEvent
{
    Uint32 type;        /**< ::SDL_KEYDOWN or ::SDL_KEYUP */
    Uint32 timestamp;
    Uint32 windowID;    /**< The window with keyboard focus, if any */
    Uint8 state;        /**< ::SDL_PRESSED or ::SDL_RELEASED */
    Uint8 repeat;       /**< Non-zero if this is a key repeat */
    Uint8 padding2;
    Uint8 padding3;
    SDL_keysym keysym;  /**< The key that was pressed or released */
} SDL_KeyboardEvent;


typedef struct SDL_MouseMotionEvent
{
    Uint32 type;        /**< ::SDL_MOUSEMOTION */
    Uint8 state;        /**< The current button state */
    Sint32 x;           /**< X coordinate, relative to window */
    Sint32 y;           /**< Y coordinate, relative to window */
    Sint32 xrel;        /**< The relative motion in the X direction */
    Sint32 yrel;        /**< The relative motion in the Y direction */
} SDL_MouseMotionEvent;

typedef struct SDL_MouseButtonEvent
{
    Uint32 type;        /**< ::SDL_MOUSEBUTTONDOWN or ::SDL_MOUSEBUTTONUP */
    Uint8 button;       /**< The mouse button index */
    Uint8 state;        /**< ::SDL_PRESSED or ::SDL_RELEASED */
    Uint8 padding1;
    Uint8 padding2;
    Sint32 x;           /**< X coordinate, relative to window */
    Sint32 y;           /**< Y coordinate, relative to window */
} SDL_MouseButtonEvent;


typedef union SDL_Event
{
    Uint32 type;                    /**< Event type, shared with all events */
     SDL_KeyboardEvent key;          /**< Keyboard event data */
     SDL_MouseMotionEvent motion;    /**< Mouse motion event data */
     SDL_MouseButtonEvent button;    /**< Mouse button event data */
     SDL_QuitEvent quit;             /**< Quit request event data */
     SDL_JoyAxisEvent jaxis;
     SDL_JoyButtonEvent jbutton;
  // 
  //   / * This is necessary for ABI compatibility between Visual C++ and GCC
  //      Visual C++ will respect the push pack pragma and use 52 bytes for
  //      this structure, and GCC will use the alignment of the largest datatype
  //      within the union, which is 8 bytes.
  //      So... we'll add padding to force the size to be 56 bytes for both.
  //   * /
     Uint8 padding[56];
} SDL_Event;


void mod_prev_mouse_position(void);
int            SDL_PollEvent(SDL_Event * event);

#endif
