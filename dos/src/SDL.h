// SDL - Simple DirectMedia Layer
// This is not a port of SDL; it emulates the functionality needed for Grafx2.

#ifndef _SDL_H_
#define _SDL_H_

typedef unsigned char      Uint8;
typedef unsigned short int Uint16;
typedef signed short       Sint16;
typedef unsigned int       Uint32;
typedef signed long        Sint32;


/* Make sure the types really have the right sizes */
#define SDL_COMPILE_TIME_ASSERT(name, x)               \
       typedef int SDL_dummy_ ## name[(x) * 2 - 1]

SDL_COMPILE_TIME_ASSERT(uint8, sizeof(Uint8) == 1);
SDL_COMPILE_TIME_ASSERT(uint16, sizeof(Uint16) == 2);
SDL_COMPILE_TIME_ASSERT(sint16, sizeof(Sint16) == 2);
SDL_COMPILE_TIME_ASSERT(uint32, sizeof(Uint32) == 4);
SDL_COMPILE_TIME_ASSERT(sint32, sizeof(Sint32) == 4);


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



// from SDL_sysjoystick.h
typedef struct SDL_Joystick
{
  // Dummy
} SDL_Joystick;

typedef struct SDL_Color
{
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
} SDL_Color;

typedef struct SDL_Palette
{
    int ncolors;
    SDL_Color *colors;
    Uint32 version;
    int refcount;
} SDL_Palette;

typedef struct SDL_PixelFormat
{
    SDL_Palette *palette;
    Uint8  BitsPerPixel;
    Uint8  BytesPerPixel;
    Uint8  Rloss;
    Uint8  Gloss;
    Uint8  Bloss;
    Uint8  Aloss;
    Uint8  Rshift;
    Uint8  Gshift;
    Uint8  Bshift;
    Uint8  Ashift;
    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask;
    Uint32 Amask;

    /** RGB color key information */
    Uint32 colorkey;
    /** Alpha value information (per-surface alpha) */
    Uint8  alpha;
} SDL_PixelFormat;



typedef struct SDL_Rect
{
    int x, y;
    int w, h;
} SDL_Rect;

typedef struct SDL_Surface
{
    Uint32 flags;               /**< Read-only */
    SDL_PixelFormat *format;    /**< Read-only */
    int w, h;                   /**< Read-only */
    Uint16 pitch;                  /**< Read-only */
    void *pixels;               /**< Read-write */

    /** clipping information */
    SDL_Rect clip_rect;         /**< Read-only */

    /** information needed for surfaces requiring locks */
    Uint32 locked;                 /**< Read-only */

    /** Reference count -- used when freeing surface */
    int refcount;               /**< Read-mostly */
} SDL_Surface;



typedef Sint32 SDL_Keycode;


typedef enum {
        /** @name ASCII mapped keysyms
         *  The keyboard syms have been cleverly chosen to map to ASCII
         */
        /*@{*/
    SDLK_UNKNOWN      = 0,
    SDLK_FIRST        = 0,
    SDLK_BACKSPACE    = 8,
    SDLK_TAB          = 9,
    SDLK_CLEAR        = 12,
    SDLK_RETURN       = 13,
    SDLK_PAUSE        = 19,
    SDLK_ESCAPE       = 27,
    SDLK_SPACE        = 32,
    SDLK_EXCLAIM      = 33,
    SDLK_QUOTEDBL     = 34,
    SDLK_HASH         = 35,
    SDLK_DOLLAR       = 36,
    SDLK_AMPERSAND    = 38,
    SDLK_QUOTE        = 39,
    SDLK_LEFTPAREN    = 40,
    SDLK_RIGHTPAREN   = 41,
    SDLK_ASTERISK     = 42,
    SDLK_PLUS         = 43,
    SDLK_COMMA        = 44,
    SDLK_MINUS        = 45,
    SDLK_PERIOD       = 46,
    SDLK_SLASH        = 47,
    SDLK_0            = 48,
    SDLK_1            = 49,
    SDLK_2            = 50,
    SDLK_3            = 51,
    SDLK_4            = 52,
    SDLK_5            = 53,
    SDLK_6            = 54,
    SDLK_7            = 55,
    SDLK_8            = 56,
    SDLK_9            = 57,
    SDLK_COLON        = 58,
    SDLK_SEMICOLON    = 59,
    SDLK_LESS         = 60,
    SDLK_EQUALS       = 61,
    SDLK_GREATER      = 62,
    SDLK_QUESTION     = 63,
    SDLK_AT           = 64,
    /* 
       Skip uppercase letters
     */
    SDLK_LEFTBRACKET  = 91,
    SDLK_BACKSLASH    = 92,
    SDLK_RIGHTBRACKET = 93,
    SDLK_CARET        = 94,
    SDLK_UNDERSCORE   = 95,
    SDLK_BACKQUOTE    = 96,
    SDLK_a            = 97,
    SDLK_b            = 98,
    SDLK_c            = 99,
    SDLK_d            = 100,
    SDLK_e            = 101,
    SDLK_f            = 102,
    SDLK_g            = 103,
    SDLK_h            = 104,
    SDLK_i            = 105,
    SDLK_j            = 106,
    SDLK_k            = 107,
    SDLK_l            = 108,
    SDLK_m            = 109,
    SDLK_n            = 110,
    SDLK_o            = 111,
    SDLK_p            = 112,
    SDLK_q            = 113,
    SDLK_r            = 114,
    SDLK_s            = 115,
    SDLK_t            = 116,
    SDLK_u            = 117,
    SDLK_v            = 118,
    SDLK_w            = 119,
    SDLK_x            = 120,
    SDLK_y            = 121,
    SDLK_z            = 122,
    SDLK_DELETE       = 127,
    /* End of ASCII mapped keysyms */
        /*@}*/

    /** @name International keyboard syms */
        /*@{*/
    SDLK_WORLD_0      = 160,    /* 0xA0 */
    SDLK_WORLD_1      = 161,
    SDLK_WORLD_2      = 162,
    SDLK_WORLD_3      = 163,
    SDLK_WORLD_4      = 164,
    SDLK_WORLD_5      = 165,
    SDLK_WORLD_6      = 166,
    SDLK_WORLD_7      = 167,
    SDLK_WORLD_8      = 168,
    SDLK_WORLD_9      = 169,
    SDLK_WORLD_10     = 170,
    SDLK_WORLD_11     = 171,
    SDLK_WORLD_12     = 172,
    SDLK_WORLD_13     = 173,
    SDLK_WORLD_14     = 174,
    SDLK_WORLD_15     = 175,
    SDLK_WORLD_16     = 176,
    SDLK_WORLD_17     = 177,
    SDLK_WORLD_18     = 178,
    SDLK_WORLD_19     = 179,
    SDLK_WORLD_20     = 180,
    SDLK_WORLD_21     = 181,
    SDLK_WORLD_22     = 182,
    SDLK_WORLD_23     = 183,
    SDLK_WORLD_24     = 184,
    SDLK_WORLD_25     = 185,
    SDLK_WORLD_26     = 186,
    SDLK_WORLD_27     = 187,
    SDLK_WORLD_28     = 188,
    SDLK_WORLD_29     = 189,
    SDLK_WORLD_30     = 190,
    SDLK_WORLD_31     = 191,
    SDLK_WORLD_32     = 192,
    SDLK_WORLD_33     = 193,
    SDLK_WORLD_34     = 194,
    SDLK_WORLD_35     = 195,
    SDLK_WORLD_36     = 196,
    SDLK_WORLD_37     = 197,
    SDLK_WORLD_38     = 198,
    SDLK_WORLD_39     = 199,
    SDLK_WORLD_40     = 200,
    SDLK_WORLD_41     = 201,
    SDLK_WORLD_42     = 202,
    SDLK_WORLD_43     = 203,
    SDLK_WORLD_44     = 204,
    SDLK_WORLD_45     = 205,
    SDLK_WORLD_46     = 206,
    SDLK_WORLD_47     = 207,
    SDLK_WORLD_48     = 208,
    SDLK_WORLD_49     = 209,
    SDLK_WORLD_50     = 210,
    SDLK_WORLD_51     = 211,
    SDLK_WORLD_52     = 212,
    SDLK_WORLD_53     = 213,
    SDLK_WORLD_54     = 214,
    SDLK_WORLD_55     = 215,
    SDLK_WORLD_56     = 216,
    SDLK_WORLD_57     = 217,
    SDLK_WORLD_58     = 218,
    SDLK_WORLD_59     = 219,
    SDLK_WORLD_60     = 220,
    SDLK_WORLD_61     = 221,
    SDLK_WORLD_62     = 222,
    SDLK_WORLD_63     = 223,
    SDLK_WORLD_64     = 224,
    SDLK_WORLD_65     = 225,
    SDLK_WORLD_66     = 226,
    SDLK_WORLD_67     = 227,
    SDLK_WORLD_68     = 228,
    SDLK_WORLD_69     = 229,
    SDLK_WORLD_70     = 230,
    SDLK_WORLD_71     = 231,
    SDLK_WORLD_72     = 232,
    SDLK_WORLD_73     = 233,
    SDLK_WORLD_74     = 234,
    SDLK_WORLD_75     = 235,
    SDLK_WORLD_76     = 236,
    SDLK_WORLD_77     = 237,
    SDLK_WORLD_78     = 238,
    SDLK_WORLD_79     = 239,
    SDLK_WORLD_80     = 240,
    SDLK_WORLD_81     = 241,
    SDLK_WORLD_82     = 242,
    SDLK_WORLD_83     = 243,
    SDLK_WORLD_84     = 244,
    SDLK_WORLD_85     = 245,
    SDLK_WORLD_86     = 246,
    SDLK_WORLD_87     = 247,
    SDLK_WORLD_88     = 248,
    SDLK_WORLD_89     = 249,
    SDLK_WORLD_90     = 250,
    SDLK_WORLD_91     = 251,
    SDLK_WORLD_92     = 252,
    SDLK_WORLD_93     = 253,
    SDLK_WORLD_94     = 254,
    SDLK_WORLD_95     = 255,    /* 0xFF */
        /*@}*/

    /** @name Numeric keypad */
        /*@{*/
    SDLK_KP0          = 256,
    SDLK_KP1          = 257,
    SDLK_KP2          = 258,
    SDLK_KP3          = 259,
    SDLK_KP4          = 260,
    SDLK_KP5          = 261,
    SDLK_KP6          = 262,
    SDLK_KP7          = 263,
    SDLK_KP8          = 264,
    SDLK_KP9          = 265,
    SDLK_KP_PERIOD    = 266,
    SDLK_KP_DIVIDE    = 267,
    SDLK_KP_MULTIPLY  = 268,
    SDLK_KP_MINUS     = 269,
    SDLK_KP_PLUS      = 270,
    SDLK_KP_ENTER     = 271,
    SDLK_KP_EQUALS    = 272,
        /*@}*/

    /** @name Arrows + Home/End pad */
        /*@{*/
    SDLK_UP           = 273,
    SDLK_DOWN         = 274,
    SDLK_RIGHT        = 275,
    SDLK_LEFT         = 276,
    SDLK_INSERT       = 277,
    SDLK_HOME         = 278,
    SDLK_END          = 279,
    SDLK_PAGEUP       = 280,
    SDLK_PAGEDOWN     = 281,
        /*@}*/

    /** @name Function keys */
        /*@{*/
    SDLK_F1           = 282,
    SDLK_F2           = 283,
    SDLK_F3           = 284,
    SDLK_F4           = 285,
    SDLK_F5           = 286,
    SDLK_F6           = 287,
    SDLK_F7           = 288,
    SDLK_F8           = 289,
    SDLK_F9           = 290,
    SDLK_F10          = 291,
    SDLK_F11          = 292,
    SDLK_F12          = 293,
    SDLK_F13          = 294,
    SDLK_F14          = 295,
    SDLK_F15          = 296,
        /*@}*/

    /** @name Key state modifier keys */
        /*@{*/
    SDLK_NUMLOCK      = 300,
    SDLK_CAPSLOCK     = 301,
    SDLK_SCROLLOCK    = 302,
    SDLK_RSHIFT       = 303,
    SDLK_LSHIFT       = 304,
    SDLK_RCTRL        = 305,
    SDLK_LCTRL        = 306,
    SDLK_RALT         = 307,
    SDLK_LALT         = 308,
    SDLK_RMETA        = 309,
    SDLK_LMETA        = 310,
    SDLK_LSUPER       = 311,    /**< Left "Windows" key */
    SDLK_RSUPER       = 312,   /**< Right "Windows" key */
    SDLK_MODE         = 313,   /**< "Alt Gr" key */
    SDLK_COMPOSE      = 314,    /**< Multi-key compose key */
        /*@}*/

    /** @name Miscellaneous function keys */
        /*@{*/
    SDLK_HELP         = 315,
    SDLK_PRINT        = 316,
    SDLK_SYSREQ       = 317,
    SDLK_BREAK        = 318,
    SDLK_MENU         = 319,
    SDLK_POWER        = 320,    /**< Power Macintosh power key */
    SDLK_EURO         = 321,   /**< Some european keyboards */
    SDLK_UNDO         = 322,   /**< Atari keyboard has Undo */
        /*@}*/

    /* Add any other keys here */

    SDLK_LAST
} SDLKey;

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

#define KMOD_CTRL   (KMOD_LCTRL  | KMOD_RCTRL )
#define KMOD_SHIFT  (KMOD_LSHIFT | KMOD_RSHIFT)
#define KMOD_ALT    (KMOD_LALT   | KMOD_RALT  )
#define KMOD_META   (KMOD_LMETA  | KMOD_RMETA )

typedef struct SDL_keysym {
    Uint8 scancode;    /**< hardware specific scancode */
    SDLKey sym;        /**< SDL virtual keysym */
    SDLMod mod;        /**< current key modifiers */
    Uint16 unicode;    /**< translated character */
} SDL_keysym;


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


// From SDL_video.h
#define SDL_SWSURFACE   0x00000000    /**< Surface is in system memory */

#define SDL_FULLSCREEN  0x80000000    /**< Surface is a full screen display */
#define SDL_RESIZABLE   0x00000010    /**< This video mode may be resized */

#define SDL_LOGPAL  0x01
#define SDL_PHYSPAL 0x02

#define SDL_SRCCOLORKEY 0x00001000    /**< Blit uses a source color key */









#define SDL_INIT_TIMER    0x00000001
#define SDL_INIT_VIDEO    0x00000020
#define SDL_INIT_JOYSTICK 0x00000200




// SDL Events
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


// SDL_mouse.h
#define SDL_BUTTON_LEFT       1
#define SDL_BUTTON_MIDDLE     2
#define SDL_BUTTON_RIGHT      3
#define SDL_BUTTON_WHEELUP    4
#define SDL_BUTTON_WHEELDOWN  5


// For SDL_ShowCursor
#define SDL_ENABLE  1
#define SDL_DISABLE 0




SDL_Surface * IMG_Load(char * path);

int            SDL_BlitSurface(SDL_Surface* src, const SDL_Rect* srcrect, SDL_Surface* dst, SDL_Rect* dstrect);
SDL_Surface*   SDL_CreateRGBSurface(Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
void           SDL_Delay(int ms);
SDL_Surface *  SDL_DisplayFormat(SDL_Surface *surface);
void           SDL_EnableKeyRepeat(int a, int b);
void           SDL_EnableUNICODE(int flag);
int            SDL_FillRect(SDL_Surface * dst, const SDL_Rect * rect, Uint32 color);
void           SDL_FreeSurface(SDL_Surface * icon);
SDLMod         SDL_GetModState(void);
void           SDL_SetModState(SDLMod mod);
Uint32         SDL_GetTicks(void);
int            SDL_Init(int flags);
SDL_Joystick * SDL_JoystickOpen(int num);
SDL_Rect **    SDL_ListModes(SDL_PixelFormat *format, Uint32 flags);
int            SDL_LockSurface(SDL_Surface * surface);
Uint32         SDL_MapRGB(const SDL_PixelFormat * format, Uint8 r, Uint8 g, Uint8 b);
int            SDL_PollEvent(SDL_Event * event);
void           SDL_Quit(void);
int            SDL_SetColorKey(SDL_Surface * surface, int flag, Uint32 key);
int            SDL_SetColors(SDL_Surface *surface, SDL_Color *colors, int firstcolor, int ncolors);
int            SDL_SetPalette(SDL_Surface *surface, int flags, SDL_Color *colors, int firstcolor, int ncolors);
SDL_Surface *  SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags);
int            SDL_ShowCursor(int toggle);
void           SDL_UnlockSurface(SDL_Surface * surface);
void           SDL_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h);
int            SDL_VideoModeOK(int width, int height, int bpp, Uint32 flags);
void           SDL_WarpMouse(Uint16 x, Uint16 y);
void           SDL_WM_SetCaption(const char * s1, const char * s2);
void           SDL_WM_SetIcon(SDL_Surface *icon, Uint8 *mask);

void reset_video(void);
void set_fake_modifiers(Uint32 fake_mods);
void mod_prev_mouse_position(void);
#endif
