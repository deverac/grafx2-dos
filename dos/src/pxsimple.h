/* vim:expandtab:ts=2 sw=2:
*/
/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2008 Yves Rizoud
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

//////////////////////////////////////////////////////////////////////////////
///@file pxsimple.h
/// Renderer for simple pixels (1x1). This is the normal one.
//////////////////////////////////////////////////////////////////////////////

#include "struct.h"

  void Pixel_simple                      (word x,word y,byte color);
  byte Read_pixel_simple                  (word x,word y);
  void Block_simple                      (word start_x,word start_y,word width,word height,byte color);
  void Pixel_preview_normal_simple       (word x,word y,byte color);
  void Pixel_preview_magnifier_simple        (word x,word y,byte color);
  void Horizontal_XOR_line_simple      (word x_pos,word y_pos,word width);
  void Vertical_XOR_line_simple        (word x_pos,word y_pos,word height);
  void Display_brush_color_simple        (word x_pos,word y_pos,word x_offset,word y_offset,word width,word height,byte transp_color,word brush_width);
  void Display_brush_mono_simple         (word x_pos,word y_pos,word x_offset,word y_offset,word width,word height,byte transp_color,byte color,word brush_width);
  void Clear_brush_simple                (word x_pos,word y_pos,word x_offset,word y_offset,word width,word height,byte transp_color,word image_width);
  void Remap_screen_simple               (word x_pos,word y_pos,word width,word height,byte * conversion_table);
  void Display_part_of_screen_simple (word width,word height,word image_width);
  void Display_line_on_screen_simple   (word x_pos,word y_pos,word width,byte * line);
  void Read_line_screen_simple       (word x_pos,word y_pos,word width,byte * line);
  void Display_part_of_screen_scaled_simple(word width,word height,word image_width,byte * buffer);
  void Display_brush_color_zoom_simple   (word x_pos,word y_pos,word x_offset,word y_offset,word width,word end_y_pos,byte transp_color,word brush_width,byte * buffer);
  void Display_brush_mono_zoom_simple    (word x_pos,word y_pos,word x_offset,word y_offset,word width,word end_y_pos,byte transp_color,byte color,word brush_width,byte * buffer);
  void Clear_brush_scaled_simple           (word x_pos,word y_pos,word x_offset,word y_offset,word width,word end_y_pos,byte transp_color,word image_width,byte * buffer);
  void Display_brush_simple             (byte * brush, word x_pos,word y_pos,word x_offset,word y_offset,word width,word height,byte transp_color,word brush_width);

void Display_transparent_mono_line_on_screen_simple(
        word x_pos, word y_pos, word width, byte* line, 
        byte transp_color, byte color);
void Display_transparent_line_on_screen_simple(word x_pos,word y_pos,word width,byte* line,byte transp_color);
