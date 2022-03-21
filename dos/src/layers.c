/* vim:expandtab:ts=2 sw=2:
*/
/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2009 Yves Rizoud
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
#if defined(FDOS)
#include <stddef.h> // NULL
#endif
#include "const.h"
#include "struct.h"
#include "global.h"
#include "windows.h"
#include "engine.h"
#include "pages.h"
#include "sdlscreen.h"
#include "input.h"
#include "help.h"
#include "misc.h"

void Layer_activate(byte layer, short side)
{
  word old_layers;

  if (layer >= Main_backups->Pages->Nb_layers)
    return;
  
  // Keep a copy of which layers were visible
  old_layers = Main_layers_visible;
  
  #ifndef NOLAYERS
  
  if (side == RIGHT_SIDE)
  {
    // Right-click on current layer
    if (Main_current_layer == layer)
    {
      if (Main_layers_visible == (dword)(1<<layer))
      {
        // Set all layers visible
        Main_layers_visible = 0xFFFFFFFF;
      }
      else
      {
        // Set only this one visible
        Main_layers_visible = 1<<layer;
      }
    }
    else
    {
      // Right-click on an other layer : toggle its visibility
      Main_layers_visible ^= 1<<layer;
    }
  }
  else
  {
    // Left-click on any layer
    Main_current_layer = layer;
    Main_layers_visible |= 1<<layer;
  }
  #else
  // Handler for limited layers support: only allow one visible at a time
  if (side == LEFT_SIDE)
  {
    Main_current_layer = layer;
    Main_layers_visible = 1<<layer;
    
    Update_screen_targets();
  }
  #endif

  Hide_cursor();
  if (Main_layers_visible != old_layers)
    Redraw_layered_image();
  else
    Update_depth_buffer(); // Only need the depth buffer
  //Download_infos_page_main(Main_backups->Pages);
  //Update_FX_feedback(Config.FX_Feedback);
  Display_all_screen();
  Display_layerbar();
  Display_cursor();
}

void Button_Layer_add(void)
{
  Hide_cursor();

  if (Main_backups->Pages->Nb_layers < MAX_NB_LAYERS)
  {
    // Backup with unchanged layers
    Backup_layers(0);
    if (!Add_layer(Main_backups,Main_current_layer+1))
    {
      Update_depth_buffer();
      Display_all_screen();
      Display_layerbar();
      End_of_modification();
    }
  }

  Unselect_button(BUTTON_LAYER_ADD);
  Display_cursor();
}

void Button_Layer_remove(void)
{
  Hide_cursor();

  if (Main_backups->Pages->Nb_layers > 1)
  {
    // Backup with unchanged layers
    Backup_layers(0);
    if (!Delete_layer(Main_backups,Main_current_layer))
    {
      Update_screen_targets();
      Redraw_layered_image();
      
      Display_all_screen();
      Display_layerbar();
      End_of_modification();
    }
  }
  Unselect_button(BUTTON_LAYER_REMOVE);
  Display_cursor();
}

void Button_Layer_select(void)
{
  short layer;
  // Determine which button is clicked according to mouse position
  layer = (Mouse_X/Menu_factor_X - Menu_bars[MENUBAR_LAYERS].Skin_width)
    / Layer_button_width;

  // Safety required because the mouse cursor can have slided outside button.
  if (layer < 0)
    layer=0;
  else if (layer > Main_backups->Pages->Nb_layers-1)
    layer=Main_backups->Pages->Nb_layers-1;

  Layer_activate(layer, LEFT_SIDE);
}

void Button_Layer_toggle(void)
{
  short layer;
  // Determine which button is clicked according to mouse position
  layer = (Mouse_X/Menu_factor_X - Menu_bars[MENUBAR_LAYERS].Skin_width)
    / Layer_button_width;

  // Safety required because the mouse cursor can have slided outside button.
  if (layer < 0)
    layer=0;
  else if (layer > Main_backups->Pages->Nb_layers-1)
    layer=Main_backups->Pages->Nb_layers-1;
  
  Layer_activate(layer, RIGHT_SIDE);
}

static void Draw_transparent_color(byte color)
{
  char buf[4];
  Num2str(color, buf, 3);
  Print_in_window(63,39,buf,MC_Black,MC_Light);
  Window_rectangle(90,39,13,7,color);
}

static void Draw_transparent_background(byte background)
{
  Print_in_window(99,57,background?"X":" ",MC_Black,MC_Light);
}


void Button_Layer_menu(void)
{
  byte transparent_color = Main_backups->Pages->Transparent_color;
  byte transparent_background = Main_backups->Pages->Background_transparent;
  short clicked_button;
  byte color;
  byte click;

  Open_window(122,100,"Layers");

  Window_display_frame_in( 6, 21,110, 52);
  Print_in_window(14,18,"Transparency",MC_Dark,MC_Light);

  Print_in_window(11,38,"Color",MC_Black,MC_Light);
  Window_set_normal_button(54, 36, 56,13,"" , 0,1,KEY_NONE); // 1
  Draw_transparent_color(transparent_color);
  
  Print_in_window(11,57,"Background",MC_Black,MC_Light);
  Window_set_normal_button(95, 54, 15,13,"" , 0,1,KEY_NONE); // 2
  Draw_transparent_background(transparent_background);
  
  Window_set_normal_button( 7, 78, 51,14,"OK" , 0,1,SDLK_RETURN); // 3
  Window_set_normal_button(63, 78, 51,14,"Cancel", 0,1,KEY_ESC); // 4
  
  Update_window_area(0,0,Window_width, Window_height);

  do
  {
    
    clicked_button=Window_clicked_button();
    if (Is_shortcut(Key,0x100+BUTTON_HELP))
      Window_help(BUTTON_LAYER_MENU, NULL);
    switch(clicked_button)
    {
      case 1: // color
        Get_color_behind_window(&color,&click);
        if (click && transparent_color!=color)
        {
          transparent_color=color;
          Hide_cursor();
          Draw_transparent_color(transparent_color);
          Display_cursor();
          Wait_end_of_click();
        }
        break;
        
      case 2: // background
        transparent_background = !transparent_background;
        Hide_cursor();
        Draw_transparent_background(transparent_background);
        Display_cursor();
        break;
    }
  }
  while (clicked_button<3);

  // On exit
  Hide_cursor();
  Close_window();
  if (clicked_button==3)
  {
    // Accept changes
    if (Main_backups->Pages->Transparent_color != transparent_color ||
        Main_backups->Pages->Background_transparent != transparent_background)
    {
      Backup_layers(-1);
      Main_backups->Pages->Transparent_color = transparent_color;
      Main_backups->Pages->Background_transparent = transparent_background;
      Redraw_layered_image();
      Display_all_screen();
      End_of_modification();
    }
  }
  Unselect_button(BUTTON_LAYER_MENU);
  Display_cursor();
}

void Button_Layer_set_transparent(void)
{
  Hide_cursor();

  if (Main_backups->Pages->Transparent_color != Back_color)
  {
    Backup_layers(-1);
    Main_backups->Pages->Transparent_color = Back_color;
    
    Redraw_layered_image();
    Display_all_screen();
    End_of_modification();
  }

  Unselect_button(BUTTON_LAYER_COLOR);
  Display_cursor();
}

void Button_Layer_get_transparent(void)
{
  Hide_cursor();

  if (Main_backups->Pages->Transparent_color != Back_color)
  {
    Set_back_color(Main_backups->Pages->Transparent_color);
  }

  Unselect_button(BUTTON_LAYER_COLOR);
  Display_cursor();
}

void Button_Layer_merge(void)
{
  Hide_cursor();

  if (Main_current_layer>0)
  {
    // Backup layer below the current
    Backup_layers(1<<(Main_current_layer-1));
  
    Merge_layer();
    
    Update_screen_targets();
    Redraw_layered_image();
    Display_all_screen();
    Display_layerbar();
    End_of_modification();
  }
  
  Unselect_button(BUTTON_LAYER_MERGE);
  Display_cursor();
}

void Button_Layer_up(void)
{
  Hide_cursor();

  if (Main_current_layer < (Main_backups->Pages->Nb_layers-1))
  {
    byte * tmp;
    dword layer_flags;
    
    // Backup with unchanged layers
    Backup_layers(0);
    
    // swap
    tmp = Main_backups->Pages->Image[Main_current_layer];
    Main_backups->Pages->Image[Main_current_layer] = Main_backups->Pages->Image[Main_current_layer+1];
    Main_backups->Pages->Image[Main_current_layer+1] = tmp;
    
    // Swap visibility indicators
    layer_flags = (Main_layers_visible >> Main_current_layer) & 3;
    // Only needed if they are different.
    if (layer_flags == 1 || layer_flags == 2)
    {
      // One is on, the other is off. Negating them will
      // perform the swap.
      Main_layers_visible ^= (3 << Main_current_layer);
    }
    Main_current_layer++;
    
    Update_screen_targets();
    Redraw_layered_image();
    Display_all_screen();
    Display_layerbar();
    End_of_modification();
  }
  
  Unselect_button(BUTTON_LAYER_UP);
  Display_cursor();
}

void Button_Layer_down(void)
{
  Hide_cursor();
  
  if (Main_current_layer > 0)
  {
    byte * tmp;
    dword layer_flags;
  
    // Backup with unchanged layers
    Backup_layers(0);
    
    // swap
    tmp = Main_backups->Pages->Image[Main_current_layer];
    Main_backups->Pages->Image[Main_current_layer] = Main_backups->Pages->Image[Main_current_layer-1];
    Main_backups->Pages->Image[Main_current_layer-1] = tmp;
    
    // Swap visibility indicators
    layer_flags = (Main_layers_visible >> (Main_current_layer-1)) & 3;
    // Only needed if they are different.
    if (layer_flags == 1 || layer_flags == 2)
    {
      // Only needed if they are different.
      // One is on, the other is off. Negating them will
      // perform the swap.
      Main_layers_visible ^= (3 << (Main_current_layer-1));
    }
    Main_current_layer--;
    Update_screen_targets();
    Redraw_layered_image();
    Display_layerbar();
    Display_all_screen();
    End_of_modification();
  }
  
  Unselect_button(BUTTON_LAYER_DOWN);
  Display_cursor();
}
