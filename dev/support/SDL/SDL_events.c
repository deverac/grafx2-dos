#include <dos.h>         // int86
#include <stdio.h>       // printf

#include "SDL_events.h"
#include "SDL_keysym.h"
#include "SDL_mouse.h"

#include "intvals.h"

static unsigned int PREV_MOUSE_BTN_LEFT   = 0;
static unsigned int PREV_MOUSE_BTN_MIDDLE = 0;
static unsigned int PREV_MOUSE_BTN_RIGHT  = 0;
static Sint32 PREV_MOUSE_X = 160;
static Sint32 PREV_MOUSE_Y = 100;
static SDLKey PREV_KEY = 0;


// Modify the saved mouse position so the next call to SDL_PollEvent() will
// trigger a SDL_MOUSEMOTION event causing the mouse cursor to be redrawn..
void mod_prev_mouse_position() {
  if (PREV_MOUSE_X) {
      PREV_MOUSE_X--;
  } else {
      PREV_MOUSE_X++;
  }
}



// When calling DOS_INT/0x7:
//   if non-zero is return, a key was pressed
//   if zero is returned, call DOS_INT/0x7 again to get the extended key.

// Keys with extended scancodes is emitted twice by FreeDOS.
// This function 'swallows' the second emitted key.
// The Enter on keypad is emitted twice but cannot easily be swallowed because
// it is exactly the same code as the Enter on the keyboard.
void swallow_duplicate_scancode(SDLKey sym) {
    union REGS regs;
    switch (sym) {
        case SDLK_HOME:
        case SDLK_UP:
        case SDLK_PAGEUP:
        case SDLK_LEFT:
        case SDLK_RIGHT:
        case SDLK_END:
        case SDLK_DOWN:
        case SDLK_PAGEDOWN:
        case SDLK_INSERT:
        case SDLK_DELETE:
        // case SDLK_RETURN:
        // case SDLK_KP_ENTER:

            // Read and ignore
            regs.h.ah = 0x07;
            int86(DOS_INT, &regs, &regs);

            // Read and ignore
            regs.h.ah = 0x07;
            int86(DOS_INT, &regs, &regs);
            break;
        default:
            // Ignore
            break;
    }
}


// This does not actually return a Unicode value (other than 0x01-0x7f).
// (DOS does not natively support Unicode.)
// Grafx2 uses the '.unicode' value when entering text in (e.g.) Text window.
int get_unicode_val(unsigned char ch) {
    if (ch > 0 && ch < 0x7f) {
        return ch;
    }
    return 0;
}




// Except where noted, these (should) match the scancodes listed in keyboard.c.
int get_sdlkey(unsigned char scancode) {
    switch(scancode) {

        case 0x00: return SDLK_UNKNOWN; // Pressing Ctrl-2 twice will generate this on second pressing.
        case 0x01: return SDLK_ESCAPE;
        case 0x02: return SDLK_1;
        case 0x03: return SDLK_2;
        case 0x04: return SDLK_3;
        case 0x05: return SDLK_4;
        case 0x06: return SDLK_5;
        case 0x07: return SDLK_6;
        case 0x08: return SDLK_7;
        case 0x09: return SDLK_8;
        case 0x0a: return SDLK_9;
        case 0x0b: return SDLK_0;
        case 0x0c: return SDLK_MINUS;
        case 0x0d: return SDLK_EQUALS;
        case 0x0e: return SDLK_BACKSPACE;
        case 0x0f: return SDLK_TAB;
        case 0x10: return SDLK_q;
        case 0x11: return SDLK_w;
        case 0x12: return SDLK_e;
        case 0x13: return SDLK_r;
        case 0x14: return SDLK_t;
        case 0x15: return SDLK_y;
        case 0x16: return SDLK_u;
        case 0x17: return SDLK_i;
        case 0x18: return SDLK_o;
        case 0x19: return SDLK_p;
        case 0x1a: return SDLK_LEFTBRACKET;
        case 0x1b: return SDLK_RIGHTBRACKET;
        case 0x1c: return SDLK_RETURN; 
        case 0x1d: return SDLK_LCTRL; // keyboard.c has SDLK_UNKNOWN
        case 0x1e: return SDLK_a;
        case 0x1f: return SDLK_s;
        case 0x20: return SDLK_d;
        case 0x21: return SDLK_f;
        case 0x22: return SDLK_g;
        case 0x23: return SDLK_h;
        case 0x24: return SDLK_j;
        case 0x25: return SDLK_k;
        case 0x26: return SDLK_l;
        case 0x27: return SDLK_SEMICOLON;
        case 0x28: return SDLK_QUOTE;
        case 0x29: return SDLK_BACKQUOTE;
        case 0x2a: return SDLK_LSHIFT;  // keyboard.c has SDLK_UNKNOWN
        case 0x2b: return SDLK_BACKSLASH;
        case 0x2c: return SDLK_z;
        case 0x2d: return SDLK_x;
        case 0x2e: return SDLK_c;
        case 0x2f: return SDLK_v;
        case 0x30: return SDLK_b;
        case 0x31: return SDLK_n;
        case 0x32: return SDLK_m;
        case 0x33: return SDLK_COMMA;
        case 0x34: return SDLK_PERIOD;
        case 0x35: return SDLK_SLASH;
        case 0x36: return SDLK_RSHIFT; // keyboard.c has SDLK_UNKNOWN
        case 0x37: return SDLK_KP_MULTIPLY;
        case 0x38: return SDLK_LALT; // keyboard.c has SDLK_UNKNOWN
        case 0x39: return SDLK_SPACE;
        case 0x3a: return SDLK_CAPSLOCK;  // keyboard.c has SDLK_UNKNOWN
        case 0x3b: return SDLK_F1;
        case 0x3c: return SDLK_F2;
        case 0x3d: return SDLK_F3;
        case 0x3e: return SDLK_F4;
        case 0x3f: return SDLK_F5;
        case 0x40: return SDLK_F6;
        case 0x41: return SDLK_F7;
        case 0x42: return SDLK_F8;
        case 0x43: return SDLK_F9;
        case 0x44: return SDLK_F10;
        case 0x45: return SDLK_NUMLOCK; // keyboard.c has SDLK_UNKNOWN
        case 0x46: return SDLK_SCROLLOCK;  // keyboard.c has SDLK_UNKNOWN
        case 0x47: return SDLK_HOME;
        case 0x48: return SDLK_UP;
        case 0x49: return SDLK_PAGEUP;
        case 0x4a: return SDLK_KP_MINUS;
        case 0x4b: return SDLK_LEFT;
        case 0x4c: return SDLK_KP5;
        case 0x4d: return SDLK_RIGHT;
        case 0x4e: return SDLK_KP_PLUS;
        case 0x4f: return SDLK_END;
        case 0x50: return SDLK_DOWN;
        case 0x51: return SDLK_PAGEDOWN;
        case 0x52: return SDLK_INSERT;
        case 0x53: return SDLK_DELETE;
        case 0x54: return SDLK_F1;
        case 0x55: return SDLK_F2;
        case 0x56: return SDLK_F3;
        case 0x57: return SDLK_F4;
        case 0x58: return SDLK_F5;
        case 0x59: return SDLK_F6;
        case 0x5a: return SDLK_F7;
        case 0x5b: return SDLK_F8;
        case 0x5c: return SDLK_F9;
        case 0x5d: return SDLK_F10;
        case 0x5e: return SDLK_F1;
        case 0x5f: return SDLK_F2;
        case 0x60: return SDLK_F3;
        case 0x61: return SDLK_F4;
        case 0x62: return SDLK_F5;
        case 0x63: return SDLK_F6;
        case 0x64: return SDLK_F7;
        case 0x65: return SDLK_F8;
        case 0x66: return SDLK_F9;
        case 0x67: return SDLK_F10;
        case 0x68: return SDLK_F1;
        case 0x69: return SDLK_F2;
        case 0x6a: return SDLK_F3;
        case 0x6b: return SDLK_F4;
        case 0x6c: return SDLK_F5;
        case 0x6d: return SDLK_F6;
        case 0x6e: return SDLK_F7;
        case 0x6f: return SDLK_F8;
        case 0x70: return SDLK_F9;
        case 0x71: return SDLK_F10;
        case 0x72: return SDLK_SYSREQ; // keyboard.c has SDLK_UNKNOWN
        case 0x73: return SDLK_LEFT;
        case 0x74: return SDLK_RIGHT;
        case 0x75: return SDLK_END;
        case 0x76: return SDLK_PAGEDOWN;
        case 0x77: return SDLK_HOME;
        case 0x78: return SDLK_1;
        case 0x79: return SDLK_2;
        case 0x7A: return SDLK_3;
        case 0x7B: return SDLK_4;
        case 0x7C: return SDLK_5;
        case 0x7D: return SDLK_6;
        case 0x7E: return SDLK_7;
        case 0x7F: return SDLK_8;
        case 0x80: return SDLK_9;
        case 0x81: return SDLK_0;
        case 0x82: return SDLK_MINUS;
        case 0x83: return SDLK_EQUALS;
        case 0x84: return SDLK_PAGEUP;
        case 0x85: return SDLK_F11;
        case 0x86: return SDLK_F12;
        case 0x87: return SDLK_F11;
        case 0x88: return SDLK_F12;
        case 0x89: return SDLK_F11;
        case 0x8a: return SDLK_F12;
        case 0x8b: return SDLK_F11;
        case 0x8c: return SDLK_F12;
        case 0x8d: return SDLK_UP;
        case 0x8e: return SDLK_KP_MINUS;
        case 0x8f: return SDLK_KP5;
        case 0x90: return SDLK_KP_PLUS;
        case 0x91: return SDLK_DOWN;
        case 0x92: return SDLK_INSERT;
        case 0x93: return SDLK_DELETE;
        case 0x94: return SDLK_TAB;
        case 0x95: return SDLK_KP_DIVIDE;
        case 0x96: return SDLK_KP_MULTIPLY;
        case 0x97: return SDLK_HOME;
        case 0x98: return SDLK_UP;
        case 0x99: return SDLK_PAGEUP;
        // 0x9a
        case 0x9b: return SDLK_LEFT;
        // 0x9c
        case 0x9d: return SDLK_RIGHT;
        // 0x9e
        case 0x9f: return SDLK_END;
        case 0xa0: return SDLK_DOWN;
        case 0xa1: return SDLK_PAGEDOWN;
        case 0xa2: return SDLK_INSERT;
        case 0xa3: return SDLK_DELETE;
        case 0xa4: return SDLK_KP_DIVIDE;
        case 0xa5: return SDLK_TAB;
        case 0xa6: return SDLK_KP_ENTER;
        default:
            printf("Err: unhandled scancode %x\n", scancode);
            return SDLK_UNKNOWN;
    }
}



// Returns 1 if there is a pending event or 0 if there are none available.
int SDL_PollEvent(SDL_Event * event){
   union REGS regs;
   unsigned char dosch = '\0';
   unsigned char scancode = '\0';
   SDLKey symch = 0;
   unsigned int chready = 0;
   Sint32 mouse_x = 0;
   Sint32 mouse_y = 0;
   unsigned int mouse_left   = 0;
   unsigned int mouse_middle = 0;
   unsigned int mouse_right  = 0;

    if (!event) {
        return 0;
    }

    if (PREV_KEY) {
        // Simulete a key-up event.
        event->type = SDL_KEYUP;
        event->key.type = event->type;
        event->key.keysym.sym = PREV_KEY;
        event->key.keysym.scancode = 0;
        event->key.keysym.unicode = 0;
        event->key.keysym.mod = 0;
        PREV_KEY = 0; // Clear
        return 1;
    }

    // Check if a key has been pressed
    regs.h.ah = 0x0b;
    int86(DOS_INT, &regs, &regs);
    chready = regs.h.al;

    if (chready) {

        // Peek at scancode. Does not remove data from keyboard buffer.
        regs.h.ah = 0x01;
        int86(KEYBOARD_INT, &regs, &regs);
        scancode = regs.h.ah;

        // Read key from keyboard buffer
        regs.h.ah = 0x7;
        int86(DOS_INT, &regs, &regs);
        dosch = regs.h.al;

        if (dosch == 0) {

            // Read (extended) key from keyboard buffer
            regs.h.ah = 0x07;
            int86(DOS_INT, &regs, &regs);
            scancode = regs.h.al;

            symch = get_sdlkey(scancode);

            swallow_duplicate_scancode(symch);

        } else if (dosch > 0 && dosch < 0x7f) {

            symch = get_sdlkey(scancode);

        } else {
            // Ignore (0x0 or 0x7f).
            return 0;
        }


        event->type = SDL_KEYDOWN;
        event->key.type = event->type;
        event->key.keysym.sym = symch; // SDL Keysym
        event->key.keysym.scancode = scancode;
        event->key.keysym.unicode = get_unicode_val(dosch);
        event->key.keysym.mod = 0; // Not used. Grafx2 uses SDL_GetModState().

        PREV_KEY = symch;
        return 1;
    }

  
    // Get mouse status and button states.
    // Middle mouse button is not supported by FreeDOS driver.
    // The interrupt often returns invalid values for mouse_x and mouse_y.
    // This results is a 'moving' mouse.
    regs.x.ax = 0x3;
    int86(MOUSE_INT, &regs, &regs);
    // Shouldn't need to '&0xFFF' but if not, sometimes values are way out of
    // line.
    mouse_x = regs.x.cx & 0xFFF; // 0..639
    mouse_y = regs.x.dx & 0xFFF; // 0..199
    mouse_left  = regs.x.bx & 0x1;
    mouse_right = regs.x.bx & 0x2;
    mouse_middle = regs.x.bx & 0x4;

    if (mouse_left != PREV_MOUSE_BTN_LEFT) { // left button changed
        event->type = (mouse_left ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP);
        event->button.type = event->type;
        event->button.button = SDL_BUTTON_LEFT;
        PREV_MOUSE_BTN_LEFT = mouse_left;
        return 1;
    }

    if (mouse_right != PREV_MOUSE_BTN_RIGHT) { // right button changed
        event->type = (mouse_right ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP);
        event->button.type = event->type;
        event->button.button = SDL_BUTTON_RIGHT;
        PREV_MOUSE_BTN_RIGHT = mouse_right;
        return 1;
    }

    if (mouse_middle != PREV_MOUSE_BTN_MIDDLE) { // middle button changed
        event->type = (mouse_right ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP);
        event->button.type = event->type;
        event->button.button = SDL_BUTTON_MIDDLE;
        PREV_MOUSE_BTN_MIDDLE = mouse_right;
        return 1;
    }

    if (mouse_x != PREV_MOUSE_X || mouse_y != PREV_MOUSE_Y) { // mouse moved
        event->type = SDL_MOUSEMOTION;
        event->motion.type = event->type;
        event->motion.x = mouse_x;
        event->motion.y = mouse_y;
        PREV_MOUSE_X = mouse_x;
        PREV_MOUSE_Y = mouse_y;
        return 1;
    }    

    return 0;
}
