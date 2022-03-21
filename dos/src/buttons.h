/* vim:expandtab:ts=2 sw=2:
*/
/*  Grafx2 - The Ultimate 256-color bitmap paint program

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

////////////////////////////////////////////////////////////////////////////
///@file buttons.h
/// Almost all the editor actions that are called by the menu are here.
////////////////////////////////////////////////////////////////////////////

#ifndef __BOUTONS_H_
#define __BOUTONS_H_

#include "struct.h"

void Stencil_update_color(byte color);
void Stencil_tag_color(byte color, byte tag_color);

/*!
    Displays an error message when there is no more memory for the requested
    operation.
*/
void Message_out_of_memory(void);

/*!
    Displays the splash screen at program startup.
*/
void Button_Message_initial(void);

/*!
    Changes brush shape.
    This function saves the current brush shape and swith to the default one
    (single pixel brush) for the filler and the color picker.
    These functions don't need (and will not work with) a custom brush.
*/
void Change_paintbrush_shape(byte shape);

// Boutons relatifs aux couleurs


/*!
    Callback for the palette scroller buttons left click.
    Scrolls the menubar palette one column to the left.
*/
void Button_Pal_left(void);

/*!
    Callback for the palette scroller buttons right click.
    Scrolls the menubar palette faster to the left.
*/
void Button_Pal_left_fast(void);

/*!
    Callback for the palette scroller buttons left click.
    Scrolls the menubar palette one column to the right.
*/
void Button_Pal_right(void);

/*!
    Callback for the palette scroller buttons right click.
    Scrolls the menubar palette faster to the right.
*/
void Button_Pal_right_fast(void);

/*!
    Callback for the palette color buttons left click.
    Selects the foreground drawing color when clicking on the menubar palette.
*/
void Button_Select_forecolor(void);

/*!
    Callback for the palette color buttons right click.
    Selects the background drawing color when clicking on the menubar palette.
*/
void Button_Select_backcolor(void);

// Boutons relatifs au pinceaux

/*!
    Callback for the brush button left click.
    Selects the monochrome brush mode when right clicking on the brush button.
*/
void Button_Brush_monochrome(void);

/*!
    Callback for the brush button right click.
    Displays the "Paintbrush menu".
*/
void Button_Paintbrush_menu(void);

// Boutons relatifs au mode de dessin à main levée

/*!
    Callback for the freehand draw button left click.
    Selects freehand drawing mode, depending on the current state of the freehand button.
*/
void Button_Draw(void);

/*!
    Callback for the freehand draw button right click.
    Cycles the drawing modes for the freehand tool.
*/
void Button_Draw_switch_mode(void);

// Dessin par ligne

/*!
    Callback for the lines button left click.
    Selects lines drawing mode, depending on the current state of the lines button.
*/
void Button_Lines(void);

/*!
    Callback for the lines button right click.
    Cycles the drawing modes for the lines tool.
*/
void Button_Lines_switch_mode(void);

// Button relatif au remplissage

/*!
    Callback for the fill button left click.
    Start the filling operation.
*/
void Button_Fill(void);

/*!
    Callback for the fill button right click.
    Start the color replace operation.
*/
void Button_Replace(void);

/*!
    Disable and release the fill button.
    Restores the cursor (custom brushes are disabled for the fill operation).
    Cleans the status bar if the color replacement tool put a preview color inside it.
*/
void Button_Unselect_fill(void);

// Spray

/*!
    Callback for the spray button left click.
    Start the spray operation.
*/
void Button_Airbrush(void);

/*!
    Callback for the spray button right click.
    Opens the spray's setup menu.
*/
void Button_Airbrush_menu(void);

// Courbes de Bézier

/*!
    Callback for the curves button left click.
    Start curve operation according to the selected mode.
*/
void Button_Curves(void);

/*!
    Callback for the curves button right click.
    Select the curve mode (1-point, 2-point)
*/
void Button_Curves_switch_mode(void);

// Boutons relatifs aux rectangles pleins et vides

/*!
    Callback for the empty rectangle button.
    Start the rectangle operation.
*/
void Button_Empty_rectangle(void);

/*!
    Callback for the filled rectangle button.
    Start the filled rectangle operation.
*/
void Button_Filled_rectangle(void);

// Boutons relatifs au texte

/*!
    Callback for the text button.
    Opens the text setup window.
*/
void Button_Text(void);

// Boutons relatifs aux dégradés

/*!
    Callback for the gradation button.
    Opens the "Gradation menu". 
*/
void Button_Gradients(void);

/*!
    Gets the informations from the gradations table and set the global vars for the current gradation.
    @param index index of the selected gradation
*/
void Load_gradient_data(int index);

// Boutons relatifs aux cercles (ellipses) dégradé(e)s

/*!
    Callback for the gradation circle button left click.
    Starts drawing a gradation circle.
*/
void Button_Grad_circle(void);

/*!
    Callback for the gradation circle right click.
    Starts drawing a gradation ellipsis.
*/
void Button_Grad_ellipse(void);

/*!
   Callback for the gradation rectangle button.
   Starts the gradation rectangle drawing operation.
*/
void Button_Grad_rectangle(void);

// Boutons relatifs aux cercles (ellipses) plein(e)s et vides

/*!
    Callback for the circle button left click.
    Starts drawing an empty circle
*/
void Button_Empty_circle(void);

/*!
    Callback for the circle button left click.
    Starts drawing an empty ellipsis
*/
void Button_Empty_ellipse(void);

/*!
    Callback for the filled circle button ledt click.
    Starts drawing a filled circle.
*/
void Button_Filled_circle(void);

/*!
    Callback for the filled circle right click.
    Starts drawing a filled ellipsis.
*/
void Button_Filled_ellipse(void);

// Boutons relatifs aux polygones vides et pleins

/*!
    Callback for the polyline button left click.
    Starts drawing a polygon.
*/
void Button_polygon(void);

/*!
    Callback for the polyline right click.
    Starts drawing a polyform.
*/
void Button_Polyform(void);

/*!
    Callback for the polyfill button left click.
    Starts drawing a filled polygon.
*/
void Button_Polyfill(void);

/*!
    Callback for the polyfill button right click.
    Starts drawing a filled polyform.
*/
void Button_Filled_polyform(void);

// Boutons d'ajustement de l'image

/*!
    Callback for the adjust picture button.
    Start the adjust picture operation.
*/
void Button_Adjust(void);

// Gestion du mode Shade

/*!
    Callback for the shade button (in the FX window).
    Toogle the shade mode.
*/
void Button_Shade_mode(void);

/*!
    Callback for the QSHade button (in the FX window).
    Toogle the Quick Shade effect.
*/
void Button_Quick_shade_mode(void);

/*!
    Callback for the Shade button (in the FX window) right click.
    Displays the shade setup menu.
*/
void Button_Shade_menu(void);

// Gestion du Stencil

/*!
    Callback for the Stencil button (in the FX window) left click.
    Toogle stencil mode.
*/
void Button_Stencil_mode(void);

/*!
    Callback for the Stencil button (in the FX window) right click.
    Displays the stencil setup menu.
*/
void Button_Stencil_menu(void);

// Gestion du Masque

/*!
    Callback for the Mask button (in the FX window) left click.
    Toogles the mask mode/
*/
void Button_Mask_mode(void);

/*!
    Callback for the Mask button (in the FX window) right click.
    Displays the mask setup menu.
*/
void Button_Mask_menu(void);

// Mode grille (Snap)

/*!
    Callback for the Grid button (in the FX window) left click.
    Toogle the grid.
*/

void Button_Snap_mode(void);

/*!
    Callback for the Grid button (in the FX window) right click.
    Displays the grid setup menu.
*/
void Button_Grid_menu(void);

/*!
    Callback to toggle the grid visible in the magnified view.
*/
void Button_Show_grid(void);

// Mode trame (Sieve)

/*!
    In the sieve window, copy one of the presets patterns to the current one.
    @param index Index of the pattern to copy
*/
void Copy_preset_sieve(byte index);

/*!
    In the sieve window, swaps black and white in the current pattern.
*/
void Invert_trame(void);

/*!
    Callback for the Sieve button (in the FX window) left click.
    Toogle sieve mode.
*/
void Button_Sieve_mode(void);

/*!
    Callback for the Sieve button (in the FX window) right click.
    Displays the sieve setup menu.
*/
void Button_Sieve_menu(void);

// Mode Smooth

/*!
    Callback for the smooth button (in the FX window) left click.
    Toogles smooth mode.
*/
void Button_Smooth_mode(void);

/*!
    Callback for the Smooth button (in the FX window) right click.
    Displays the smooth setup menu.
*/
void Button_Smooth_mode(void);

// Boutons relatifs au mode Colorize

/*!
    Computes the tables used by the transparency/colorize mode.
    These tables are used to match the drawing color*picture color to the color that is painted on screen.
*/
void Compute_colorize_table(void);

/*!
    Callback for the Tranparency button (in the FX window) left click.
    Toogles transparent drawing mode.
*/
void Button_Colorize_mode(void);

/*!
    Callback for the Transparency button (in the FX window) right click.
    Displays the tranparency setup menu.
*/
void Button_Colorize_menu(void);

// Boutons relatifs au mode Tiling

/*!
    Callback for the Tiling button (in the FX window) left click.
    Toogles tiling mode.
*/
void Button_Tiling_mode(void);

/*!
    Callback for the Tiling button (in the FX window) right click.
    Displays the tiling setup menu.
*/
void Button_Tiling_menu(void);

/*!
    Callback for the command that turns off all drawaing effects.
*/
void Effects_off(void);

// Menu des effets

/*!
    Callback for the effects button click.
    Displays the effect selection menu.
*/
void Button_Effects(void);

// Prise de brosse

/*!
    Callback for the brush button left click.
    Start the brush picking operation.
*/
void Button_Brush(void);

/*!
    Callback for the brush button right click.
    Activates the last captured custom brush.
*/
void Button_Restore_brush(void);

/*!
    Disables the custom brush and set back a regular one.
*/
void Button_Unselect_brush(void);
// Prise de brosse au lasso

/*!
    Callback for the freehand brush pick button left click.
    Starts freehand brush picking operation.
*/
void Button_Lasso(void);

/*!
    Disables the custom freehand brush and set back a regular one.
*/
void Button_Unselect_lasso(void);

// Button relatifs à la pipette

/*!
    Starts the color picking operation.
*/
void Button_Colorpicker(void);

/*!
    Disables the color picker button and get back to the previously selected drawing mode.
*/
void Button_Unselect_colorpicker(void);

/*!
    Swap fore- and background colors.
*/
void Button_Invert_foreback(void);

// Mode loupe

/*!
    Enters magnify mode.
*/
void Button_Magnify(void);

/*!
    Displays magnify menu.
*/
void Button_Magnify_menu(void);

/*!
    Exit magnify mode.
*/
void Button_Unselect_magnifier(void);

// Les différents effets sur la brosse

/*!
    Display the Brush effects window.
*/
void Button_Brush_FX(void);

// Boutons relatifs aux différentes pages

/*!
    Swap main and spare drawing pages.
*/
void Button_Page(void);

/*!
    Copy main page to spare page.
*/
void Button_Copy_page(void);

/*!
    Copy only pixel data from main page to spare page (no palette copy).
*/
void Copy_image_only(void);

/*!
    Kill (free from memory) the current page.
*/
void Button_Kill(void);

// Boutons relatifs aux changements de résolution et de taille d'image

/*!
    Display the screenmode menu.
*/
void Button_Resolution(void);

/*!
    Set the screen to the "safe resolution" (320x200 pixel window).
*/
void Button_Safety_resolution(void);

// Boutons relatifs aux chargements et sauvegardes

/*!
    Opens the load file dialog.
*/
void Button_Load(void);

/*!
    Reload current picture from disk.
*/
void Button_Reload(void);

/*!
    Open the save file dialog.
*/
void Button_Save(void);

/*!
    Saves the current file without asking for a new name.
*/
void Button_Autosave(void);

// Réglage des paramètres de l'utilisateur

/*!
    Display the setting menu.
*/
void Button_Settings(void);

/*!
     Display the skin selector window.
*/
void Button_Skins(void);

// Annulation de la dernière modification

/*!
    Undo the last modification to the picture.
*/
void Button_Undo(void);

/*!
    Redo an operation that has been undone.
*/
void Button_Redo(void);

// Boutons relatifs aux effacements d'images

/*!
    Clear the whole screen with black (color index 0).
*/
void Button_Clear(void);

/*!
    Clear the screen with the selected backcolor.
*/
void Button_Clear_with_backcolor(void);

// Quitter le programme

/*!
    Quits the program. Display a requester to save the changes to the picture before exiting if the pic was modified since last save.
*/
void Button_Quit(void);

// Cacher le menu

/*!
    Hides the menubar.
*/
void Button_Hide_menu(void);

/*!
    Shows a dropdown panel where you can choose which toolbars are visible
*/
void Button_Toggle_toolbar(void);

/*!
    Hides all toolbars (except status) or shows them again
*/
void Button_Toggle_all_toolbars(void);


/*!
    Load picture from file.
*/
void Load_picture(byte image);

/*!
    Save picture to file.
*/
void Save_picture(byte image);


/*!
    Generic color tagging menu, for various effects.
*/
void Menu_tag_colors(char * window_title, byte * table, byte * mode, byte can_cancel, const char *help_section, word close_shortcut );


/*!
    Display the menu for the smooth effect.
*/
void Button_Smooth_menu(void);


/*!
    Toogles the smear mode.
*/
void Button_Smear_mode(void);

void Button_Brush_container(void);

#endif

