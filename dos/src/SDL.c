#if defined(FDOS)
// This implements SDL functions necessary for Grafx2 to run under FreeDOS.
// Alhough this implements SDL functions, do not assume the functions behave
// as described in official SDL documentation. Attempting to use these SDL
// functions with any program other than Grafx2 may or may not succeed.

#include <stdio.h>       // printf
#include <stdlib.h>      // free
#include <string.h>      // memset
#include <dos.h>         // int86
#include <sys/nearptr.h> // _djgpp_nearptr_disable

#include "SDL.h"
#include "readpng.h"


#define VIDEO_INT    0x10
#define BIOS15_INT   0x15
#define KEYBOARD_INT 0x16
#define DOS_INT      0x21
#define MOUSE_INT    0x33


// VGA video DAC PEL mask
// #define VGA_PALETTE_MASK_PORT  0x03c6

// VGA video DAC PEL address port
#define VGA_PALETTE_INDEX_PORT  0x03c8

// VGA video DAC port
#define VGA_PALETTE_COLOR_PORT  0x03c9


#define VGA_START_ADDR  0xA0000


#define FDOS_SCREEN_WIDTH   320
#define FDOS_SCREEN_HEIGHT  200


#define MOD_UNSET(m, f) (m &= ~(f))
#define MOD_SET(m, f) (m |= (f))

#define FAKE_MOD_UNSET(f) (MOD_UNSET(FAKE_MODIFIERS, (f)))
#define FAKE_MOD_SET(f)   (MOD_SET(FAKE_MODIFIERS, (f)))

#define IS_MOD_SET(m, f) ((m) & (f))

#define IS_FAKE_MOD_SET(f) (IS_MOD_SET(FAKE_MODIFIERS, (f)))


static SDL_Rect  screen_rect;
static SDL_Rect* screen_modes[2];


static unsigned int PREV_MOUSE_BTN_LEFT   = 0;
static unsigned int PREV_MOUSE_BTN_MIDDLE = 0;
static unsigned int PREV_MOUSE_BTN_RIGHT  = 0;
static Sint32 PREV_MOUSE_X = 160;
static Sint32 PREV_MOUSE_Y = 100;
static SDLKey PREV_KEY = 0;


static Uint32 TICKS_EPOCH = 0;


static SDLMod JOYSTICK_MODIFIERS = 0;

static Uint32 FAKE_MODIFIERS = 0;

// Modify the saved mouse position so the next call to SDL_PollEvent() will
// trigger a SDL_MOUSEMOTION event causing the mouse cursor to be redrawn..
void mod_prev_mouse_position() {
  if (PREV_MOUSE_X) {
      PREV_MOUSE_X--;
  } else {
      PREV_MOUSE_X++;
  }
}

SDL_Surface* malloc_surface(Uint32 flags, int width, int height, int bpp, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask) {
    int i;
    int bytes_per_pixel;
    SDL_Color       * colors;
    SDL_Palette     * palette;
    SDL_PixelFormat * format;
    char            * pixels;
    SDL_Surface     * surface;

    bytes_per_pixel = (bpp / 8) + ((bpp % 8) ? 1 : 0);

    colors = (SDL_Color *) malloc(sizeof(SDL_Color) * 256);
    if (colors == NULL) {
        return NULL;
    }

    palette = (SDL_Palette *) malloc(sizeof(SDL_Palette)); 
    if (palette == NULL) {
        free(colors);
        return NULL;
    }

    format = (SDL_PixelFormat *) malloc(sizeof(SDL_PixelFormat));
    if (format == NULL) {
        free(colors);
        free(palette);
        return NULL;
    }

    pixels = (char *) malloc(width * height * bytes_per_pixel);
    if (pixels == NULL) {
        free(colors);
        free(palette);
        free(format);
        return NULL;
    }
    memset(pixels, 0, width * height * bytes_per_pixel);

    surface = (SDL_Surface *) malloc(sizeof(SDL_Surface));
    if (surface == NULL) {
        free(colors);
        free(palette);
        free(format);
        free(pixels);
        return NULL;
    }

    palette->colors = colors;
    format->palette = palette;
    surface->format = format;
    surface->pixels = pixels;

    palette->version = 0;
    palette->refcount = 1;
    palette->ncolors = 256;
    for (i=0; i<palette->ncolors; i++) {
        colors[i].r = 0;
        colors[i].g = 0;
        colors[i].b = 0;
        colors[i].a = 0;
    }

    format->BitsPerPixel = bpp;
    format->BytesPerPixel = bytes_per_pixel;
    format->Rmask = Rmask;
    format->Gmask = Gmask;
    format->Bmask = Bmask;
    format->Amask = Amask;

    surface->flags = flags;
    surface->pitch = width * bytes_per_pixel;
    surface->w = width;
    surface->h = height;
    surface->refcount = 1;
    surface->locked = 0;

    return surface;
}


SDL_Surface * IMG_Load(char * path) {
    SDL_Surface * surf = NULL;
    int i;
    pngdat pdat;
    Uint32 flags = 0;
    int pathlen = strlen(path);
    if (pathlen > 3) {
        if ((strcmp(&path[pathlen-4], ".png") == 0) || (strcmp(&path[pathlen-4], ".PNG") == 0)) {
            if (read_png_file(path, &pdat)) {
                flags = SDL_SWSURFACE;

                surf = malloc_surface(flags, pdat.width, pdat.height, pdat.bit_depth, 0, 0, 0, 0);
                if (surf) {
                    surf->format->palette->ncolors = pdat.num_palette;

                    // Copy pixels
                    memcpy(surf->pixels, pdat.pixels, pdat.height * pdat.width * pdat.bytes_per_pixel);

                    // Copy palette
                    for (i=0; i<pdat.num_palette; i++) {
                        surf->format->palette->colors[i].r = pdat.palette[i*3];
                        surf->format->palette->colors[i].g = pdat.palette[i*3+1];
                        surf->format->palette->colors[i].b = pdat.palette[i*3+2];
                        surf->format->palette->colors[i].a = 0;
                    }

                    free_pngdat(&pdat);

                }
            }
        }
    }
    return surf;
}


void reset_video() {
    union REGS regs;
    regs.h.ah = 0x00;
    // regs.h.al = 0x03;
    regs.h.al = 0x07; // 80x25 black/white
    int86(VIDEO_INT, &regs, &regs);
}


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


SDL_Surface* SDL_CreateRGBSurface(Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask){
  return malloc_surface(flags, width, height, depth, Rmask, Gmask, Bmask, Amask);
}


//If 'x', 'y', 'w' and 'h' are all 0, SDL_UpdateRect will update the entire screen.
void SDL_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h){
    int xx;
    int yy;
    // char *VGA = (char *)0xA0000;
    char *VGA = (char *)VGA_START_ADDR;
    char* px = (char*)(screen->pixels);

    VGA += __djgpp_conventional_base;

    if (x == 0 && y == 0 && w == 0 && h == 0) {
        h = screen->h;
        w = screen->w;
    }

    for(yy=y; yy<y+h; yy++) {
      for(xx=x; xx<x+w; xx++) {
        VGA[yy*(screen->w)+xx] = px[yy*(screen->w)+xx];
      }
    }

}


// Returns 0 on success or a negative error code on failure
int SDL_FillRect(SDL_Surface * dst, const SDL_Rect * rect, Uint32 color) {
    int xx;
    int yy;

    char cval = (char)(color & 0xFF); // Just to be safe.
    char *px = (char*)(dst->pixels);

    for(yy=rect->y; yy<rect->y+rect->h; yy++) {
        for(xx=rect->x; xx<rect->x+rect->w; xx++) {
            *(px + (yy*(dst->w)+xx)) = cval;
        }
    }

   return 0;
}




void SDL_FreeSurface(SDL_Surface * surface) {
    if (surface) {
        if (surface->format) {
            if (surface->format->palette) {
                if (surface->format->palette->colors) {
                    free(surface->format->palette->colors);
                    surface->format->palette->colors = NULL;
                }
                free(surface->format->palette);
                surface->format->palette = NULL;
            }
            free(surface->format);
            surface->format = NULL;
        }
        if (surface->pixels) {
            free(surface->pixels);
            surface->pixels = NULL;
        }
        free(surface);
        surface = NULL;
    }
}


// Return elapsed time in milliseconds since starting. This should give
// run-time for a month before rollover. FreeDOS has 5/100 sec resolution.
// Uint32 is 2^32     = 4,294,967,295
// 31*24*60*60*100*10 = 2,678,400,000
Uint32 get_millis() {
  static int init_day = 0;
  static int max_day = 0;
  union REGS regs;
  int multiplier;
  int days;
  int hours;
  int minutes;
  int seconds;
  int hundreths;


  // Get date
  regs.h.ah = 0x2a; // function Get system date
  int86(DOS_INT, &regs, &regs);
  // year = regs.x.cx;  // (1980-2099)
  // month = regs.h.dh; // (1-12)
  days = regs.h.dl;     // (1-31)

  if (!init_day) {
    init_day = days;
    init_day--;
  }

  if (days > max_day) {
    max_day = days;
  }

  multiplier = max_day - init_day;
  if (days < init_day) {
       multiplier += days;
  }

  // Get time
  regs.h.ah = 0x2c; // function Get system time
  int86(DOS_INT, &regs, &regs);
  hours = regs.h.ch;     // (0-23)
  minutes = regs.h.cl;   // (0-59)
  seconds = regs.h.dh;   // (0-59)
  hundreths = regs.h.dl; // (0-99)

  //return multiplier * (((((((hours*60) + minutes)*60) + seconds)*100) + hundreths)*10);

  // FIXME? This seems to result in fewer eratic mouse movements.
  return (((((hours*60) + minutes)*60) + seconds)*100) + hundreths;
}


// Returns 0 on success or -1 on error.
int SDL_Init(int flags){
  union REGS regs;
  int is_mouse_avail = 0;

  (void)flags; // ignored

  TICKS_EPOCH = get_millis();

  __djgpp_nearptr_enable(); // Disable 640K memory protection.
                            // This allows writing to video memory.



  // resets mouse to default driver values:
  //    mouse is positioned to screen center
  //    mouse cursor is reset and hidden
  //    no interrupts are enabled (mask = 0)
  //    double speed threshold set to 64 mickeys per second
  //    horizontal mickey to pixel ratio (8 to 8)
  //    vertical mickey to pixel ratio (16 to 8)
  //    max width and height are set to maximum for video mode
  regs.x.ax = 0x0;
  int86(MOUSE_INT, &regs, &regs);
  is_mouse_avail = regs.x.ax;
  
  if (is_mouse_avail == 0xFFFF) {
    // Mouse is available.

    // Set mouse sensitivity.
    regs.x.ax = 0x1a;
    regs.x.bx = 20; // horz-rez (0-100) default: 50
    regs.x.cx = 20; // vert-rez (0-100) default: 50
    regs.x.dx = 0; // double-speed threashold
    int86(MOUSE_INT, &regs, &regs);
  }

  return 0;
}



// Returns a joystick identifier or NULL if an error occurred.
// (Joystick is currently not supported.)
SDL_Joystick * SDL_JoystickOpen(int device_index) {
  (void) device_index;
  return NULL;
}





void SDL_Delay(int ms) {
    union REGS regs;
    // CX:DX number of microseconds to wait
    // granularity is 976 microseconds
    long micro_secs = ms * 1000;
    int cx = micro_secs / 0xFFFF;
    int dx = micro_secs - (cx * 0xFFFF);
    regs.h.ah = 0x86;
    regs.x.cx = cx;
    regs.x.dx = dx;
    int86(BIOS15_INT, &regs, &regs);
}


// e.g. SDL_EnableKeyRepeat(250, 32);
void SDL_EnableKeyRepeat(int delay_ms, int interval_ms) {
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

void SDL_EnableUNICODE(int flag) {
  (void)flag;
  // Ignored
}


// Copied from SDL-1.2/src/video/SDL_pixels.c
// Match an RGB value to a particular palette index
Uint8 SDL_FindColor(SDL_Palette *pal, Uint8 r, Uint8 g, Uint8 b)
{
    // Do colorspace distance matching
    unsigned int smallest;
    unsigned int distance;
    int rd, gd, bd;
    int i;
    Uint8 pixel=0;

    smallest = ~0;
    for ( i=0; i<pal->ncolors; ++i ) {
        rd = pal->colors[i].r - r;
        gd = pal->colors[i].g - g;
        bd = pal->colors[i].b - b;
        distance = (rd*rd)+(gd*gd)+(bd*bd);
        if ( distance < smallest ) {
            pixel = i;
            if ( distance == 0 ) { // Perfect match!
                break;
            }
            smallest = distance;
        }
    }
    return(pixel);
}

// In Grafx2 SDL_SetModState() is only used to set modifier keys when using
// a joystick. Since a Joysticks is not supported, this function is basically
// useless and only exists to allow compiling to succeed.
void SDL_SetModState(SDLMod mod) {
  JOYSTICK_MODIFIERS = mod;
}

// Get the state of modifiers.
SDLMod SDL_GetModState(void){
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

// Return the number of milliseconds since the SDL library initialization.
// Due to DOS, resolution is 50 ms (5/100 second).
Uint32 SDL_GetTicks(void) {
  return get_millis() - TICKS_EPOCH;
}


// Returns array of SDL_Rect
SDL_Rect ** SDL_ListModes(SDL_PixelFormat *format, Uint32 flags) {
    (void)format;
    (void) flags;

    screen_rect.x = 0;
    screen_rect.y = 0;
    screen_rect.w = FDOS_SCREEN_WIDTH;
    screen_rect.h = FDOS_SCREEN_HEIGHT;

    screen_modes[0] = &screen_rect;
    screen_modes[1] = NULL; // Terminate list.

    return screen_modes;
}

// Locking a surface is not enforced. Always returns 0.
int SDL_LockSurface(SDL_Surface * surface){
    surface->locked++;
    return 0;
}

// Return the index of the color that most closely matches r, g, b.
Uint32 SDL_MapRGB(const SDL_PixelFormat * format, Uint8 r, Uint8 g, Uint8 b){
    return SDL_FindColor(format->palette, r, g, b);
}


void SDL_Quit(void) {
    union REGS regs;

    // Set video (Text mode)
    regs.h.ah = 0x00;
    regs.h.al = 0x03; // 80x25 16 color
    // regs.h.al = 0x07; // 80x25 black/white
    int86(VIDEO_INT, &regs, &regs);

    // Reset typematic rate
    regs.h.ah = 0x03; // set rate function
    regs.h.al = 0x00; // reset to default delay
    int86(KEYBOARD_INT, &regs, &regs);

    // Disable direct accett to lower 640K.
    __djgpp_nearptr_disable();
}


// Palettized (8-bit) screen surfaces with the SDL_HWPALETTE flag have two
// palettes, a logical palette that is used for mapping blits to/from the
// surface and a physical palette (that determines how the hardware will map
// the colors to the display). SDL_SetColors modifies both palettes
// (if present), and is equivalent to calling SDL_SetPalette with the flags set
// to (SDL_LOGPAL | SDL_PHYSPAL).
// Copied from ./src/video/SDL_video.c
int SDL_SetColors(SDL_Surface *screen, SDL_Color *colors, int firstcolor, int ncolors){
    return SDL_SetPalette(screen, SDL_LOGPAL | SDL_PHYSPAL, colors, firstcolor, ncolors);
}


// VGA only uses the lower six bits of each color byte. Shifting the value of
// the actual color value right by two bits will give approximately the same
// color when displayed by VGA hardware.
// Writing to the VGA_PALETTE_COLOR_PORT will automatically advance the index.
int SDL_SetPalette(SDL_Surface *surface, int flags, SDL_Color *colors, int firstcolor, int ncolors) {
    int i;

    if (firstcolor < 0 || firstcolor >= 256) {
        printf("Err: firstcolor must be between 0 and 255.\n");
        return -1;
    }
    if ( (ncolors != 1) && (ncolors != 256) ) {
        printf("Warn: ncolors must be 1 or 256\n");
        return -1;
    }

    if (flags & SDL_PHYSPAL) {
        if (ncolors == 1) {
            outp(VGA_PALETTE_INDEX_PORT, firstcolor); // Index palette to change
            outp(VGA_PALETTE_COLOR_PORT, colors[0].r >> 2);
            outp(VGA_PALETTE_COLOR_PORT, colors[0].g >> 2);
            outp(VGA_PALETTE_COLOR_PORT, colors[0].b >> 2);
        } else { //  ncolors == 256
            outp(VGA_PALETTE_INDEX_PORT, 0); // Signal output 256 colors.
            for(i=0; i<256; i++)
            {
                outp(VGA_PALETTE_COLOR_PORT, colors[i].r >> 2);
                outp(VGA_PALETTE_COLOR_PORT, colors[i].g >> 2);
                outp(VGA_PALETTE_COLOR_PORT, colors[i].b >> 2);
            }
        }
    }

    if (surface) {
        if (ncolors == 1) {
            surface->format->palette->colors[firstcolor].r = colors[0].r;
            surface->format->palette->colors[firstcolor].g = colors[0].g;
            surface->format->palette->colors[firstcolor].b = colors[0].b;
        } else { // ncolors == 256
            for(i=0; i<256; i++)
            {
                surface->format->palette->colors[i].r = colors[i].r;
                surface->format->palette->colors[i].g = colors[i].g;
                surface->format->palette->colors[i].b = colors[i].b;
            }
        }
    }

    return 0;
}


// The 'visible' parameter should be SDL_ENABLE or SDL_DISABLE.
// The value passed in will be returned.
int SDL_ShowCursor(int visible) {
    union REGS regs;
    if (visible) {
        regs.x.ax=0x1; // show;
    } else {
        regs.x.ax=0x2; // hide;
    }
    int86(MOUSE_INT, &regs, &regs);

   return visible;
}

// Locking surfaces is not enforced.
void SDL_UnlockSurface(SDL_Surface * surface){
    surface->locked--;
}


// Return 1 on success, otherwise 0.
int SDL_VideoModeOK(int width, int height, int bpp, Uint32 flags) {
   if (width != FDOS_SCREEN_WIDTH) return 0;
   if (height != FDOS_SCREEN_HEIGHT) return 0;
   if (bpp != 8) return 0;  // 2^8 ==256
   if (flags != SDL_FULLSCREEN) return 0;
   return 1;
}


void SDL_WarpMouse(Uint16 x, Uint16 y) {
    union REGS regs;
    if (x >= FDOS_SCREEN_WIDTH) x = FDOS_SCREEN_WIDTH - 1;
    if (y >= FDOS_SCREEN_HEIGHT) y = FDOS_SCREEN_HEIGHT - 1;
    regs.x.ax=0x4; // set mouse cursor position
    regs.x.cx=x;   // horz pos
    regs.x.dx=y;   // vert pos
    int86(MOUSE_INT, &regs, &regs);
}


void  SDL_WM_SetCaption(const char * s1, const char * s2) {
  (void)s1;
  (void)s2;
  // Ignored
}


void SDL_WM_SetIcon(SDL_Surface *icon, Uint8 *mask) {
  (void)icon;
  (void)mask;
  // Ignored
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


// This does not actually return a Unicode value (other than 0x01-0x7f).
// (DOS does not natively support Unicode.)
// Grafx2 uses the '.unicode' value when entering text in (e.g.) Text window.
int get_unicode_val(unsigned char ch) {
    if (ch > 0 && ch < 0x7f) {
        return ch;
    }
    return 0;
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


// Returns a surface of 320x200 with 8bpp, (regardless of 'width', 'height' or
// 'bpp' parameters) or NULL on failure; The 'flags' value is mostly ignored.
SDL_Surface *SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags) {
   union REGS regs;

    if (width != FDOS_SCREEN_WIDTH) {
        fprintf(stderr, "Ignoring width %d; only %d is supported.\n", width, FDOS_SCREEN_WIDTH);
    }
    if (height != FDOS_SCREEN_HEIGHT) {
        fprintf(stderr, "Ignoring height %d; only %d is supported.\n", height, FDOS_SCREEN_HEIGHT);
    }
    if (bpp != 8) {
        fprintf(stderr, "Ignoring bpp %d; only 8 is supported.\n", bpp);
    }
    if (flags & SDL_FULLSCREEN) {
        fprintf(stderr, "SDL_FULLSCREEN is not supported; removing flag.\n");
        flags &= ~SDL_FULLSCREEN;
    }
    // The SDL_RESIZABLE flag is also not supported, but we'll ignore it.

    // set video (Graphics mode)
    regs.h.ah = 0x00;
    regs.h.al = 0x13; // set 256 color mode
    int86(VIDEO_INT, &regs, &regs);

    return malloc_surface(flags, FDOS_SCREEN_WIDTH, FDOS_SCREEN_HEIGHT, 8, 0, 0, 0, 0);
}







// If set, the SD_COLORKEY defines a pixel value that will be treated as
// transparent when blitting.
int SDL_SetColorKey(SDL_Surface * surface, int flag, Uint32 key) {
    if (flag & SDL_SRCCOLORKEY) {
        surface->flags |= SDL_SRCCOLORKEY; // Enable
        surface->format->colorkey = key;
    } else if (flag == 0) {
        surface->flags &= ~SDL_SRCCOLORKEY; // Disable
        surface->format->colorkey = 0; // Set color index to 0.
    }
    return 0;
}


// Convert format of 'surface' to one that is suitable for displaying.
// When running on DOS, this simply returns a copy of 'surface'.
SDL_Surface *SDL_DisplayFormat(SDL_Surface *surface) {
    int i;
    SDL_Surface * surf = NULL;
    surf = malloc_surface(surface->flags, surface->w, surface->h, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);
    if (surf) {
        // copy pixels
        memcpy(surf->pixels, surface->pixels, surface->h * surface->w * surface->format->BytesPerPixel);

        // copy palette
        for (i=0; i<surface->format->palette->ncolors; i++) {
            surf->format->palette->colors[i].r = surface->format->palette->colors[i].r;
            surf->format->palette->colors[i].g = surface->format->palette->colors[i].g;
            surf->format->palette->colors[i].b = surface->format->palette->colors[i].b;
            surf->format->palette->colors[i].a = surface->format->palette->colors[i].a;
        }
    }
    return surf;
}


// Performs a blit from the source surface to the destination surface.
// Returns 0 if the blit is successful or a negative value on failure.
int SDL_BlitSurface(SDL_Surface* src, const SDL_Rect* src_rect, SDL_Surface* dst, SDL_Rect* dst_rect){
    int x, y;
    unsigned int src_offs;
    unsigned int dst_offs;
    unsigned char * src_ptr;
    unsigned char * dst_ptr;
    int do_copy = 0;
    unsigned char px = 0;

    if (src_rect->w != dst_rect->w) {
        printf("Err: width not equal\n");
        return -1;
    }
    if (src_rect->h !=  dst_rect->h) {
        printf("Err: height not equal\n");
        return -1;
    }
    if (src->format->BytesPerPixel != 1) {
        printf("Err: src BytesPerPixel not 1 %d\n", src->format->BytesPerPixel);
        return -1;
    }
    if (dst->format->BytesPerPixel != 1) {
        printf("Err: dst BytesPerPixel not equal %d\n", dst->format->BytesPerPixel);
        return -1;
    }

    src_offs = ((src_rect->y * src->w) + src_rect->x) * src->format->BytesPerPixel;
    dst_offs = ((dst_rect->y * dst->w) + dst_rect->x) * dst->format->BytesPerPixel;

    for (y=0; y<src_rect->h; y++) {
        for (x=0; x<src_rect->w; x++) {
            if (src_rect->x + x < src->w && src_rect->y + y < src->h &&
                dst_rect->x + x < dst->w && dst_rect->y + y < dst->h) {

                  src_ptr = (src->pixels + src_offs + (((y * src->w) + x) * src->format->BytesPerPixel));
                  dst_ptr = (dst->pixels + dst_offs + (((y * dst->w) + x) * dst->format->BytesPerPixel));

                  px = *(src_ptr);

                  // Per SDL documentation, if SDL_SRCCOLORKEY is set, only
                  // copy pixels that do not match colorkey.
                  do_copy = ((src->flags & SDL_SRCCOLORKEY) ? (px != src->format->colorkey) : 1);
                  if (do_copy) {
                      *(dst_ptr) = px;
                  }
           }
        }
    }
    return 0;
}

#endif
