/* vim:expandtab:ts=2 sw=2:
*/
/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2009 Pasi Kallinen
    Copyright 2008 Peter Gordon
    Copyright 2008 Franck Charlet
    Copyright 2007 Adrien Destugues
    Copyright 1996-2001 Sunset Design (Guillaume Dorme & Karl Maritaud)

    Grafx2 is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; version 2
    of the License.

    Grafx2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Grafx2; if not, see <http://www.gnu.org/licenses/>
*/
#define GLOBAL_VARIABLES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <SDL.h>
#include <SDL_image.h>

// There is no WM on the GP2X...
#ifndef __GP2X__
    #include <SDL_syswm.h>
#endif

#include "const.h"
#include "struct.h"
#include "global.h"
#include "graph.h"
#include "misc.h"
#include "init.h"
#include "buttons.h"
#include "engine.h"
#include "pages.h"
#include "loadsave.h"
#include "sdlscreen.h"
#include "errors.h"
#include "readini.h"
#include "saveini.h"
#include "io.h"
#include "text.h"
#include "setup.h"
#include "windows.h"
#include "brush.h"
#include "palette.h"
#include "realpath.h"

#if defined(__WIN32__)
    #include <windows.h>
    #include <shlwapi.h>
    #define chdir(dir) SetCurrentDirectory(dir)
#elif defined(__macosx__)
    #import <corefoundation/corefoundation.h>
    #import <sys/param.h>
#elif defined(__FreeBSD__)
    #import <sys/param.h>
#endif


#if defined (__WIN32__)
  // On Windows, SDL_putenv is not present in any compilable header.
  // It can be linked anyway, this declaration only avoids
  // a compilation warning.
  extern DECLSPEC int SDLCALL SDL_putenv(const char *variable);
#endif

//--- Affichage de la syntaxe, et de la liste des modes vidéos disponibles ---
void Display_syntax(void)
{
  int mode_index;
  printf("Syntax: grafx2 [<arguments>] [<picture1>] [<picture2>]\n\n");
  printf("<arguments> can be:]\n");
  printf("\t-? -h -H -help    for this help screen\n");
#if defined(FDOS)
  printf("\t-ls-alt -ls-ctrl  reassign LeftShift key as Alt or Ctrl key\n");
  printf("\t-rs-alt -rs-ctrl  reassign RightShift key as Alt or Ctrl key\n");
  printf("\t-cl-alt -cl-ctrl  reassign CapsLock key as Alt or Ctrl key\n");
  printf("\t-nl-alt -nl-ctrl  reassign NumLock key as Alt or Ctrl key\n");
#else
  printf("\t-wide             to emulate a video mode with wide pixels (2x1)\n");
  printf("\t-tall             to emulate a video mode with tall pixels (1x2)\n");
  printf("\t-double           to emulate a video mode with double pixels (2x2)\n");
  printf("\t-wide2            to emulate a video mode with double wide pixels (4x2)\n");
  printf("\t-tall2            to emulate a video mode with double tall pixels (2x4)\n");
  printf("\t-triple           to emulate a video mode with triple pixels (3x3)\n");
  printf("\t-quadruple        to emulate a video mode with quadruple pixels (4x4)\n");
#endif
  printf("\t-rgb n            to reduce RGB precision from 256 to n levels\n");
  printf("\t-skin <filename>  to use an alternate file with the menu graphics\n");
  printf("\t-mode <videomode> to set a video mode\n");
  printf("Arguments can be prefixed either by / - or --\n");
  printf("They can also be abbreviated.\n\n");
  printf("Available video modes:\n\n");
  for (mode_index = 0; mode_index < Nb_video_modes; mode_index += 12)
  {
    int k;
    for (k = 0; k < 6; k++)
    {
      if (mode_index + k >= Nb_video_modes) break;
      printf("%12s",Mode_label(mode_index + k));
    }
    puts("");
  }
}

// ---------------------------- Sortie impromptue ----------------------------
void Warning_function(const char *message, const char *filename, int line_number, const char *function_name)
{
  printf("Warning in file %s, line %d, function %s : %s\n", filename, line_number, function_name, message);
}


// ---------------------------- Sortie impromptue ----------------------------
void Error_function(int error_code, const char *filename, int line_number, const char *function_name)
{
  T_Palette temp_palette;
  int       index;
#if defined(FDOS)
  // Silence 'set but not used' compiler warning.
  (void)temp_palette;
  (void)index;
#endif
  printf("Error number %d occured in file %s, line %d, function %s.\n", error_code, filename,line_number,function_name);

  if (error_code==0)
  {
    // L'erreur 0 n'est pas une vraie erreur, elle fait seulement un flash rouge de l'écran pour dire qu'il y a un problème.
    // Toutes les autres erreurs déclenchent toujours une sortie en catastrophe du programme !
    memcpy(temp_palette,Main_palette,sizeof(T_Palette));
    for (index=0;index<=255;index++)
      temp_palette[index].R=255;
    Set_palette(temp_palette);
    SDL_Delay(500);
    Set_palette(Main_palette);
  }
  else
  {
    switch (error_code)
    {
      case ERROR_GUI_MISSING         : printf("Error: File containing the GUI graphics is missing!\n");
                                       printf("This program cannot run without this file.\n");
                                       break;
      case ERROR_GUI_CORRUPTED       : printf("Error: File containing the GUI graphics couldn't be parsed!\n");
                                       printf("This program cannot run without a correct version of this file.\n");
                                       break;
      case ERROR_INI_MISSING         : printf("Error: File gfx2def.ini is missing!\n");
                                       printf("This program cannot run without this file.\n");
                                       break;
      case ERROR_MEMORY              : printf("Error: Not enough memory!\n\n");
                                       printf("You should try exiting other programs to free some bytes for Grafx2.\n\n");
                                       break;
      case ERROR_FORBIDDEN_MODE      : printf("Error: The requested video mode has been disabled from the resolution menu!\n");
                                       printf("If you want to run the program in this mode, you'll have to start it with an\n");
                                       printf("enabled mode, then enter the resolution menu and enable the mode you want.\n");
                                       printf("Check also if the 'Default_video_mode' parameter in gfx2.ini is correct.\n");
                                       break;
      case ERROR_COMMAND_LINE     : printf("Error: Invalid parameter or file not found.\n\n");
                                       Display_syntax();
                                       break;
      case ERROR_SAVING_CFG     : printf("Error: Write error while saving settings!\n");
                                       printf("Settings have not been saved correctly, and the gfx2.cfg file may have been\n");
                                       printf("corrupt. If so, please delete it and Grafx2 will restore default settings.\n");
                                       break;
      case ERROR_MISSING_DIRECTORY : printf("Error: Directory you ran the program from not found!\n");
                                       break;
      case ERROR_INI_CORRUPTED       : printf("Error: File gfx2.ini is corrupt!\n");
                                       printf("It contains bad values at line %d.\n",Line_number_in_INI_file);
                                       printf("You can re-generate it by deleting the file and running GrafX2 again.\n");
                                       break;
      case ERROR_SAVING_INI     : printf("Error: Cannot rewrite file gfx2.ini!\n");
                                       break;
      case ERROR_SORRY_SORRY_SORRY  : printf("Error: Sorry! Sorry! Sorry! Please forgive me!\n");
                                       break;
    }

#if defined(FDOS)
    // When quitting, the screen is reset which clears the error message.
    printf("(Press ENTER to quit.)\n");
    getchar();
#endif
    SDL_Quit();
    exit(error_code);
  }
}

enum CMD_PARAMS
{
    CMDPARAM_HELP,
#if defined(FDOS)
    CMDPARAM_LEFTSHIFT_ALT,
    CMDPARAM_LEFTSHIFT_CTRL,
    CMDPARAM_RIGHTSHIFT_ALT,
    CMDPARAM_RIGHTSHIFT_CTRL,
    CMDPARAM_CAPSLOCK_ALT,
    CMDPARAM_CAPSLOCK_CTRL,
    CMDPARAM_NUMLOCK_ALT,
    CMDPARAM_NUMLOCK_CTRL,
#endif
    CMDPARAM_MODE,
    CMDPARAM_PIXELRATIO_TALL,
    CMDPARAM_PIXELRATIO_WIDE,
    CMDPARAM_PIXELRATIO_DOUBLE,
    CMDPARAM_PIXELRATIO_TRIPLE,
    CMDPARAM_PIXELRATIO_QUAD,
    CMDPARAM_PIXELRATIO_TALL2,
    CMDPARAM_PIXELRATIO_WIDE2,
    CMDPARAM_RGB,
    CMDPARAM_SKIN
};

struct {
    const char *param;
    int id;
} cmdparams[] = {
    {"?", CMDPARAM_HELP},
    {"h", CMDPARAM_HELP},
    {"H", CMDPARAM_HELP},
    {"help", CMDPARAM_HELP},
#if defined(FDOS)
    {"ls-alt", CMDPARAM_LEFTSHIFT_ALT},
    {"ls-ctrl", CMDPARAM_LEFTSHIFT_CTRL},
    {"rs-alt", CMDPARAM_RIGHTSHIFT_ALT},
    {"rs-ctrl", CMDPARAM_RIGHTSHIFT_CTRL},
    {"cl-alt", CMDPARAM_CAPSLOCK_ALT},
    {"cl-ctrl", CMDPARAM_CAPSLOCK_CTRL},
    {"nl-alt", CMDPARAM_NUMLOCK_ALT},
    {"nl-ctrl", CMDPARAM_NUMLOCK_CTRL},
#endif
    {"mode", CMDPARAM_MODE},
    {"tall", CMDPARAM_PIXELRATIO_TALL},
    {"wide", CMDPARAM_PIXELRATIO_WIDE},
    {"double", CMDPARAM_PIXELRATIO_DOUBLE},
    {"triple", CMDPARAM_PIXELRATIO_TRIPLE},
    {"quadruple", CMDPARAM_PIXELRATIO_QUAD},
    {"tall2", CMDPARAM_PIXELRATIO_TALL2},
    {"wide2", CMDPARAM_PIXELRATIO_WIDE2},
    {"rgb", CMDPARAM_RGB},
    {"skin", CMDPARAM_SKIN}
};

#define ARRAY_SIZE(x) (int)(sizeof(x) / sizeof(x[0]))

// --------------------- Analyse de la ligne de commande ---------------------
int Analyze_command_line(int argc, char * argv[], char *main_filename, char *main_directory, char *spare_filename, char *spare_directory)
{
  char *buffer ;
  int index;
  int file_in_command_line;

  file_in_command_line = 0;
  Resolution_in_command_line = 0;
  
  Current_resolution = Config.Default_resolution;
  
  for (index = 1; index<argc; index++)
  {
    char *s = argv[index];
    int is_switch = ((strchr(s,'/') == s) || (strchr(s,'-') == s) || (strstr(s,"--") == s));
    int tmpi;
    int paramtype = -1;
    if (is_switch)
    {
      int param_matches = 0;
      int param_match = -1;
      if (*s == '-')
      {
        s++;
        if (*s == '-')
          s++;
      }
      else
        s++;
  
      for (tmpi = 0; tmpi < ARRAY_SIZE(cmdparams); tmpi++)
      {
        if (!strcmp(s, cmdparams[tmpi].param))
        {
          paramtype = cmdparams[tmpi].id;
          break;
        }
        else if (strstr(cmdparams[tmpi].param, s))
        {
          param_matches++;
          param_match = cmdparams[tmpi].id;
        }
      }
      if (paramtype == -1 && param_matches == 1)
        paramtype = param_match;

    }
    switch (paramtype)
    {
      case CMDPARAM_HELP:
        Display_syntax();
        exit(0);
#if defined(FDOS)
      case CMDPARAM_LEFTSHIFT_ALT:   set_fake_modifiers(FAKE_MOD_LS_ALT ); break;
      case CMDPARAM_LEFTSHIFT_CTRL:  set_fake_modifiers(FAKE_MOD_LS_CTRL); break;
      case CMDPARAM_RIGHTSHIFT_ALT:  set_fake_modifiers(FAKE_MOD_RS_ALT ); break;
      case CMDPARAM_RIGHTSHIFT_CTRL: set_fake_modifiers(FAKE_MOD_RS_CTRL); break;
      case CMDPARAM_CAPSLOCK_ALT:    set_fake_modifiers(FAKE_MOD_CL_ALT ); break;
      case CMDPARAM_CAPSLOCK_CTRL:   set_fake_modifiers(FAKE_MOD_CL_CTRL); break;
      case CMDPARAM_NUMLOCK_ALT:     set_fake_modifiers(FAKE_MOD_NL_ALT ); break;
      case CMDPARAM_NUMLOCK_CTRL:    set_fake_modifiers(FAKE_MOD_NL_CTRL); break;
#endif
      case CMDPARAM_MODE:
        index++;
        if (index<argc)
        {
          Resolution_in_command_line = 1;
          Current_resolution = Convert_videomode_arg(argv[index]);
          if (Current_resolution == -1)
          {
            Error(ERROR_COMMAND_LINE);
            Display_syntax();
            exit(0);
          }
          if ((Video_mode[Current_resolution].State & 0x7F) == 3)
          {
            Error(ERROR_FORBIDDEN_MODE);
            exit(0);
          }
        }
        else
        {
          Error(ERROR_COMMAND_LINE);
          Display_syntax();
          exit(0);
        }
        break;
      case CMDPARAM_PIXELRATIO_TALL:
        Pixel_ratio = PIXEL_TALL;
        break;
      case CMDPARAM_PIXELRATIO_WIDE:
        Pixel_ratio = PIXEL_WIDE;
        break;
      case CMDPARAM_PIXELRATIO_DOUBLE:
        Pixel_ratio = PIXEL_DOUBLE;
        break;
      case CMDPARAM_PIXELRATIO_TRIPLE:
        Pixel_ratio = PIXEL_TRIPLE;
        break;
      case CMDPARAM_PIXELRATIO_QUAD:
        Pixel_ratio = PIXEL_QUAD;
        break;
      case CMDPARAM_PIXELRATIO_TALL2:
        Pixel_ratio = PIXEL_TALL2;
        break;
      case CMDPARAM_PIXELRATIO_WIDE2:
        Pixel_ratio = PIXEL_WIDE2;
        break;
      case CMDPARAM_RGB:
        // echelle des composants RGB
        index++;
        if (index<argc)
        {
          int scale;
          scale = atoi(argv[index]);
          if (scale < 2 || scale > 256)
          {
            Error(ERROR_COMMAND_LINE);
            Display_syntax();
            exit(0);
          }
          Set_palette_RGB_scale(scale);
        }
        else
        {
          Error(ERROR_COMMAND_LINE);
          Display_syntax();
          exit(0);
        }
        break;
      case CMDPARAM_SKIN:
        // GUI skin file
        index++;
        if (index<argc)
        {
          strcpy(Config.Skin_file,argv[index]);
        }
        else
        {
          Error(ERROR_COMMAND_LINE);
          Display_syntax();
          exit(0);
        }
        break;
      default:
        // Si ce n'est pas un paramètre, c'est le nom du fichier à ouvrir
        if (file_in_command_line > 1)
        {
          // Il y a déjà 2 noms de fichiers et on vient d'en trouver un 3ème
          Error(ERROR_COMMAND_LINE);
          Display_syntax();
          exit(0);
        }
        else if (File_exists(argv[index]))
        {
          file_in_command_line ++;
          buffer = Realpath(argv[index], NULL);
        
          if (file_in_command_line == 1)
          {
            // Separate path from filename
            Extract_path(main_directory, buffer);
            Extract_filename(main_filename, buffer);
          }
          else
          {
            // Separate path from filename
            Extract_path(spare_directory, buffer);
            Extract_filename(spare_filename, buffer);
          }
          free(buffer);
          buffer = NULL;
        }
        else
        {
          Error(ERROR_COMMAND_LINE);
          Display_syntax();
          exit(0);
        }
        break;
    }
  }
  return file_in_command_line;
}

// ------------------------ Initialiser le programme -------------------------
// Returns 0 on fail
int Init_program(int argc,char * argv[])
{
  int temp;
  int starting_videomode;
  static char program_directory[MAX_PATH_CHARACTERS];
  T_Gui_skin *gfx;
  int file_in_command_line;
  static char main_filename [MAX_PATH_CHARACTERS];
  static char main_directory[MAX_PATH_CHARACTERS];
  static char spare_filename [MAX_PATH_CHARACTERS];
  static char spare_directory[MAX_PATH_CHARACTERS];
  
  

  // On crée dès maintenant les descripteurs des listes de pages pour la page
  // principale et la page de brouillon afin que leurs champs ne soient pas
  // invalide lors des appels aux multiples fonctions manipulées à
  // l'initialisation du programme.
  Main_backups=(T_List_of_pages *)malloc(sizeof(T_List_of_pages));
  Spare_backups=(T_List_of_pages *)malloc(sizeof(T_List_of_pages));
  Init_list_of_pages(Main_backups);
  Init_list_of_pages(Spare_backups);

  // Determine the executable directory
  Set_program_directory(argv[0],program_directory);
  // Choose directory for data (read only)
  Set_data_directory(program_directory,Data_directory);
  // Choose directory for settings (read/write)
  Set_config_directory(program_directory,Config_directory);

  // On détermine le répertoire courant:
  getcwd(Main_current_directory,256);

  // On en profite pour le mémoriser dans le répertoire principal:
  strcpy(Initial_directory,Main_current_directory);

  // On initialise les données sur le nom de fichier de l'image de brouillon:
  strcpy(Spare_current_directory,Main_current_directory);
  
  Main_fileformat=DEFAULT_FILEFORMAT;
  Spare_fileformat    =Main_fileformat;
  
  strcpy(Brush_current_directory,Main_current_directory);
  strcpy(Brush_file_directory,Main_current_directory);
  strcpy(Brush_filename       ,"NO_NAME.GIF");
  Brush_fileformat    =Main_fileformat;

  // On initialise ce qu'il faut pour que les fileselects ne plantent pas:
  
  Main_fileselector_position=0; // Au début, le fileselect est en haut de la liste des fichiers
  Main_fileselector_offset=0; // Au début, le fileselect est en haut de la liste des fichiers
  Main_format=FORMAT_ALL_IMAGES;
  Main_current_layer=0;
  Main_layers_visible=0xFFFFFFFF;
  Spare_current_layer=0;
  Spare_layers_visible=0xFFFFFFFF;
  
  Spare_fileselector_position=0;
  Spare_fileselector_offset=0;
  Spare_format=FORMAT_ALL_IMAGES;
  Brush_fileselector_position=0;
  Brush_fileselector_offset=0;
  Brush_format=FORMAT_ALL_IMAGES;

  // On initialise les commentaires des images à des chaînes vides
  Main_comment[0]='\0';
  Brush_comment[0]='\0';

  // On initialise d'ot' trucs
  Main_offset_X=0;
  Main_offset_Y=0;
  Old_main_offset_X=0;
  Old_main_offset_Y=0;
  Main_separator_position=0;
  Main_X_zoom=0;
  Main_separator_proportion=INITIAL_SEPARATOR_PROPORTION;
  Main_magnifier_mode=0;
  Main_magnifier_factor=DEFAULT_ZOOM_FACTOR;
  Main_magnifier_height=0;
  Main_magnifier_width=0;
  Main_magnifier_offset_X=0;
  Main_magnifier_offset_Y=0;
  Spare_offset_X=0;
  Spare_offset_Y=0;
  Old_spare_offset_X=0;
  Old_spare_offset_Y=0;
  Spare_separator_position=0;
  Spare_X_zoom=0;
  Spare_separator_proportion=INITIAL_SEPARATOR_PROPORTION;
  Spare_magnifier_mode=0;
  Spare_magnifier_factor=DEFAULT_ZOOM_FACTOR;
  Spare_magnifier_height=0;
  Spare_magnifier_width=0;
  Spare_magnifier_offset_X=0;
  Spare_magnifier_offset_Y=0;
  Keyboard_click_allowed = 0;
  
#if defined(FDOS)
  // Allow spacebar to act as mouse button.
  Keyboard_click_allowed = 1;
#endif

  Main_safety_backup_prefix = 'a';
  Spare_safety_backup_prefix = 'b';
  Main_time_of_safety_backup = 0;
  Spare_time_of_safety_backup = 0;
  

  // SDL
  if(SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) < 0)
  {
    // The program can't continue without that anyway
    printf("Couldn't initialize SDL.\n");
    return(0);
  }

  Joystick = SDL_JoystickOpen(0);
  SDL_EnableKeyRepeat(250, 32);
  SDL_EnableUNICODE(SDL_ENABLE);
  SDL_WM_SetCaption("GrafX2","GrafX2");

  {
    // Routine pour définir l'icone.
    char icon_path[MAX_PATH_CHARACTERS];
    SDL_Surface * icon;
    sprintf(icon_path, "%s%s", Data_directory, "gfx2.gif");
    icon = IMG_Load(icon_path);
    if (icon && icon->w == 32 && icon->h == 32)
    {
      Uint32 pink;
      pink = SDL_MapRGB(icon->format, 255, 0, 255);
      
      if (icon->format->BitsPerPixel == 8)
      {
        SDL_SetColorKey(icon, SDL_SRCCOLORKEY, pink);
        SDL_WM_SetIcon(icon,NULL);
      }
      else
      {
        byte *icon_mask;
        int x,y;
        
        icon_mask=malloc(128);
        memset(icon_mask,0,128);
        for (y=0;y<32;y++)
          for (x=0;x<32;x++)
            if ((Uint32)Get_SDL_pixel_hicolor(icon, x, y) != pink)
              icon_mask[(y*32+x)/8] |=0x80>>(x&7);
        SDL_WM_SetIcon(icon,icon_mask);
        free(icon_mask);
        icon_mask = NULL;
      }

      SDL_FreeSurface(icon);
    }
  }
  
  // Texte
  Init_text();

  // On initialise tous les modes vidéo
  Set_all_video_modes();
  Pixel_ratio=PIXEL_SIMPLE;
  // On initialise les données sur l'état du programme:
  // Donnée sur la sortie du programme:
  Quit_is_required=0;
  Quitting=0;
  // Données sur l'état du menu:
  Menu_is_visible=1;
  // Données sur les couleurs et la palette:
  First_color_in_palette=0;
  // Données sur le curseur:
  Cursor_shape=CURSOR_SHAPE_TARGET;
  Cursor_hidden=0;
  // Données sur le pinceau:
  Paintbrush_X=0;
  Paintbrush_Y=0;
  Paintbrush_shape=PAINTBRUSH_SHAPE_ROUND;
  Paintbrush_hidden=0;

  // On initialise tout ce qui concerne les opérations et les effets
  Operation_stack_size=0;
  Selected_freehand_mode=OPERATION_CONTINUOUS_DRAW;
  Selected_line_mode         =OPERATION_LINE;
  Selected_curve_mode        =OPERATION_3_POINTS_CURVE;
  Effect_function=No_effect;
    // On initialise les infos de la loupe:
  Main_magnifier_mode=0;
  Main_magnifier_factor=DEFAULT_ZOOM_FACTOR;
  Main_separator_proportion=INITIAL_SEPARATOR_PROPORTION;
  Spare_separator_proportion=INITIAL_SEPARATOR_PROPORTION;
    // On initialise les infos du mode smear:
  Smear_mode=0;
  Smear_brush_width=PAINTBRUSH_WIDTH;
  Smear_brush_height=PAINTBRUSH_HEIGHT;
    // On initialise les infos du mode smooth:
  Smooth_mode=0;
    // On initialise les infos du mode shade:
  Shade_mode=0;     // Les autres infos du Shade sont chargées avec la config
  Quick_shade_mode=0; // idem
    // On initialise les infos sur les dégradés:
  Gradient_pixel =Display_pixel; // Les autres infos sont chargées avec la config
    // On initialise les infos de la grille:
  Snap_mode=0;
  Snap_width=8;
  Snap_height=8;
  Snap_offset_X=0;
  Snap_offset_Y=0;
    // On initialise les infos du mode Colorize:
  Colorize_mode=0;          // Mode colorize inactif par défaut
  Colorize_opacity=50;      // Une interpolation de 50% par défaut
  Colorize_current_mode=0; // Par défaut, la méthode par interpolation
  Compute_colorize_table();
    // On initialise les infos du mode Tiling:
  Tiling_mode=0;  //   Pas besoin d'initialiser les décalages car ça se fait
                  // en prenant une brosse (toujours mis à 0).
    // On initialise les infos du mode Mask:
  Mask_mode=0;

    // Infos du Spray
  Airbrush_mode=1; // Mode Mono
  Airbrush_size=31;
  Airbrush_delay=1;
  Airbrush_mono_flow=10;
  memset(Airbrush_multi_flow,0,256);
  srand(time(NULL)); // On randomize un peu tout ça...

  // Initialisation des boutons
  Init_buttons();
  // Initialisation des opérations
  Init_operations();

  // Initialize the brush container
  Init_brush_container();

  Windows_open=0;
  
  // Charger la configuration des touches
  Set_config_defaults();

  switch(Load_CFG(1))
  {
    case ERROR_CFG_MISSING:
      // Pas un problème, on a les valeurs par défaut.
      break;
    case ERROR_CFG_CORRUPTED:
      DEBUG("Corrupted CFG file.",0);
      break;
    case ERROR_CFG_OLD:
      DEBUG("Unknown CFG file version, not loaded.",0);
      break;
  }
  // Charger la configuration du .INI
  temp=Load_INI(&Config);
  if (temp)
    Error(temp);

  Compute_menu_offsets();

  file_in_command_line=Analyze_command_line(argc, argv, main_filename, main_directory, spare_filename, spare_directory);

  Current_help_section=0;
  Help_position=0;

  // Load sprites, palette etc.
  gfx = Load_graphics(Config.Skin_file);
  if (gfx == NULL)
  {
    gfx = Load_graphics("skin_DPaint.png");
    if (gfx == NULL)
    {
      printf("%s", Gui_loading_error_message);
      Error(ERROR_GUI_MISSING);
    }
  }
  Set_current_skin(Config.Skin_file, gfx);
  // Override colors
  // Gfx->Default_palette[MC_Black]=Fav_menu_colors[0]=Config.Fav_menu_colors[0];
  // Gfx->Default_palette[MC_Dark] =Fav_menu_colors[1]=Config.Fav_menu_colors[1];
  // Gfx->Default_palette[MC_Light]=Fav_menu_colors[2]=Config.Fav_menu_colors[2];
  // Gfx->Default_palette[MC_White]=Fav_menu_colors[3]=Config.Fav_menu_colors[3];
//  Compute_optimal_menu_colors(Gfx->Default_palette);
    
  // Infos sur les trames (Sieve)
  Sieve_mode=0;
  Copy_preset_sieve(0);

  // Font
  if (!(Menu_font=Load_font(Config.Font_file)))
    if (!(Menu_font=Load_font("font_DPaint.png")))
      {
        printf("Unable to open the default font file: %s\n", "font_DPaint.png");
        Error(ERROR_GUI_MISSING);
      }

  memcpy(Main_palette, Gfx->Default_palette, sizeof(T_Palette));

  Fore_color=Best_color_nonexcluded(255,255,255);
  Back_color=Best_color_nonexcluded(0,0,0);

  // Allocation de mémoire pour la brosse
  if (!(Brush         =(byte *)malloc(   1*   1))) Error(ERROR_MEMORY);
  if (!(Smear_brush   =(byte *)malloc(MAX_PAINTBRUSH_SIZE*MAX_PAINTBRUSH_SIZE))) Error(ERROR_MEMORY);

  // Pinceau
  if (!(Paintbrush_sprite=(byte *)malloc(MAX_PAINTBRUSH_SIZE*MAX_PAINTBRUSH_SIZE))) Error(ERROR_MEMORY);
  *Paintbrush_sprite=1;
  Paintbrush_width=1;
  Paintbrush_height=1;

  starting_videomode=Current_resolution;
  Horizontal_line_buffer=NULL;
  Screen_width=Screen_height=Current_resolution=0;

  Init_mode_video(
    Video_mode[starting_videomode].Width,
    Video_mode[starting_videomode].Height,
    Video_mode[starting_videomode].Fullscreen,
    Pixel_ratio);

#if defined(FDOS)
  // Set mouse co-ords to center of screen.
  SDL_WarpMouse(
    Video_mode[starting_videomode].Width / 2,
    Video_mode[starting_videomode].Height / 2
  );
  mod_prev_mouse_position();
#endif

  // Windows only: move back the window to its original position.
  #if defined(__WIN32__)
  if (!Video_mode[starting_videomode].Fullscreen)
  {
    if (Config.Window_pos_x != 9999 && Config.Window_pos_y != 9999)
    {
      //RECT r;
      static SDL_SysWMinfo pInfo;
      SDL_VERSION(&pInfo.version);
      SDL_GetWMInfo(&pInfo);
      //GetWindowRect(pInfo.window, &r);
      SetWindowPos(pInfo.window, 0, Config.Window_pos_x, Config.Window_pos_y, 0, 0, SWP_NOSIZE);
    }
  }
  #endif
  
  Main_image_width=Screen_width/Pixel_width;
  Main_image_height=Screen_height/Pixel_height;
  Spare_image_width=Screen_width/Pixel_width;
  Spare_image_height=Screen_height/Pixel_height;
  
  // Allocation de mémoire pour les différents écrans virtuels (et brosse)
  if (Init_all_backup_lists(Screen_width,Screen_height)==0)
    Error(ERROR_MEMORY);

  // Nettoyage de l'écran virtuel (les autres recevront celui-ci par copie)
  memset(Main_screen,0,Main_image_width*Main_image_height);

  // Initialisation de diverses variables par calcul:
  Compute_magnifier_data();
  Compute_limits();
  Compute_paintbrush_coordinates();

  // On affiche le menu:
  Display_menu();
  Display_paintbrush_in_menu();
  Display_sprite_in_menu(BUTTON_PAL_LEFT,18+(Config.Palette_vertical!=0));

  // On affiche le curseur pour débutter correctement l'état du programme:
  Display_cursor();

  Spare_image_is_modified=0;
  Main_image_is_modified=0;

  // Gestionnaire de signaux, quand il ne reste plus aucun espoir
  Init_sighandler();

  // Le programme débute en mode de dessin à la main
  Select_button(BUTTON_DRAW,LEFT_SIDE);

  // On initialise la brosse initiale à 1 pixel blanc:
  Brush_width=1;
  Brush_height=1;
  Capture_brush(0,0,0,0,0);
  *Brush=MC_White;
  
  // Test de recuperation de fichiers sauvés
  switch (Check_recovery())
  {
    T_IO_Context context;

    default:    
      // Some files were loaded from last crash-exit.
      // Do not load files from command-line, nor show splash screen.
      Compute_optimal_menu_colors(Main_palette);
      Display_all_screen();
      Display_menu();
      Display_cursor();
      Verbose_message("Images recovered",
        "Grafx2 has recovered images from\n"
        "last session, before a crash or\n"
        "shutdown. Browse the history using\n"
        "the Undo/Redo button, and when\n"
        "you find a state that you want to\n"
        "save, use the 'Save as' button to\n"
        "save the image.\n"
        "Some backups can be present in\n"
        "the spare page too.\n");
      break;
  
    case -1: // Unable to write lock file
      Verbose_message("Warning", 
        "Safety backups (every minute) are\n"
        "disabled because Grafx2 is running\n"
        "from a read-only device, or other\n"
        "instances are running.");
      break;

    case 0:
    
      switch (file_in_command_line)
      {
        case 0:
          if (Config.Opening_message)
            Button_Message_initial();
          break;
  
        case 2:
          // Load this file
          Init_context_layered_image(&context, spare_filename, spare_directory);
          Load_image(&context);
          Destroy_context(&context);
          Redraw_layered_image();
          End_of_modification();
  
          Button_Page();
          // no break ! proceed with the other file now
        case 1:
          Init_context_layered_image(&context, main_filename, main_directory);
          Load_image(&context);
          Destroy_context(&context);
          Redraw_layered_image();
          End_of_modification();
          
          Hide_cursor();
          Compute_optimal_menu_colors(Main_palette);
          Display_all_screen();
          Display_menu();
          Display_cursor();
          Resolution_in_command_line = 0;
          break;
    
        default:
          break;
      }
  }
  return(1);
}

// ------------------------- Fermeture du programme --------------------------
void Program_shutdown(void)
{
  int      return_code;

  // Windows only: Recover the window position.
  #if defined(__WIN32__)
  {
    RECT r;
    static SDL_SysWMinfo pInfo;
    
    SDL_GetWMInfo(&pInfo);
    GetWindowRect(pInfo.window, &r);

    Config.Window_pos_x = r.left;
    Config.Window_pos_y = r.top;
  }
  #else
  // All other targets: irrelevant dimensions.
  // Do not attempt to force them back on next program run.
    Config.Window_pos_x = 9999;
    Config.Window_pos_y = 9999;
  #endif

  // Remove the safety backups, this is normal exit
  Delete_safety_backups();

  // On libère le buffer de gestion de lignes
  free(Horizontal_line_buffer);
  Horizontal_line_buffer = NULL;

  // On libère le pinceau spécial
  free(Paintbrush_sprite);
  Paintbrush_sprite = NULL;

  // On libère les différents écrans virtuels et brosse:
  free(Brush);
  Brush = NULL;
  Set_number_of_backups(0);

  // Free the skin (Gui graphics) data
  free(Gfx);
  Gfx=NULL;

  // On prend bien soin de passer dans le répertoire initial:
  if (chdir(Initial_directory)!=-1)
  {
    // On sauvegarde les données dans le .CFG et dans le .INI
    if (Config.Auto_save)
    {
      return_code=Save_CFG();
      if (return_code)
        Error(return_code);
      return_code=Save_INI(&Config);
      if (return_code)
        Error(return_code);
    }
  }
  else
    Error(ERROR_MISSING_DIRECTORY);
    
  SDL_Quit();
}


// -------------------------- Procédure principale ---------------------------
int main(int argc,char * argv[])
{

  if(!Init_program(argc,argv))
  {
    Program_shutdown();
    return 0;
  }
  
  Main_handler();

  Program_shutdown();
  return 0;
}
