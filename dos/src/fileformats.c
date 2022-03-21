/* vim:expandtab:ts=2 sw=2:
*/
/*  Grafx2 - The Ultimate 256-color bitmap paint program

    Copyright 2009 Petter Lindquist
    Copyright 2008 Yves Rizoud
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

///@file fileformats.c
/// Saving and loading different picture formats.

#ifndef __no_pnglib__
#include <png.h>
#endif

#include <stdlib.h>

#if defined(FDOS)
#include <string.h> // strlen
#endif

#include "errors.h"
#include "global.h"
#include "loadsave.h"
#include "misc.h"
#include "struct.h"
#include "io.h"
#include "windows.h" // Best_color()
#include "pages.h" // Add_layer()

//////////////////////////////////// IMG ////////////////////////////////////

// -- Tester si un fichier est au format IMG --------------------------------
void Test_IMG(T_IO_Context * context)
{
  FILE *file;              // Fichier du fichier
  char filename[MAX_PATH_CHARACTERS]; // Nom complet du fichier
  T_IMG_Header IMG_header;
  byte signature[6]={0x01,0x00,0x47,0x12,0x6D,0xB0};


  Get_full_filename(filename, context->File_name, context->File_directory);

  File_error=1;

  // Ouverture du fichier
  if ((file=fopen(filename, "rb")))
  {
    // Lecture et vérification de la signature
    if (Read_bytes(file,IMG_header.Filler1,sizeof(IMG_header.Filler1))
    && Read_word_le(file,&(IMG_header.Width))
    && Read_word_le(file,&(IMG_header.Height))
    && Read_bytes(file,IMG_header.Filler2,sizeof(IMG_header.Filler2))
    && Read_bytes(file,IMG_header.Palette,sizeof(IMG_header.Palette))
    )
    {
      if ( (!memcmp(IMG_header.Filler1,signature,6))
        && IMG_header.Width && IMG_header.Height)
        File_error=0;
    }
    // Fermeture du fichier
    fclose(file);
  }
}


// -- Lire un fichier au format IMG -----------------------------------------
void Load_IMG(T_IO_Context * context)
{
  char filename[MAX_PATH_CHARACTERS]; // Nom complet du fichier
  byte * buffer;
  FILE *file;
  word x_pos,y_pos;
  long width_read;
  long file_size;
  T_IMG_Header IMG_header;

#if defined(FDOS)
  (void) width_read; // Silence 'set but not used' compiler warning.
#endif

  Get_full_filename(filename, context->File_name, context->File_directory);
  File_error=0;

  if ((file=fopen(filename, "rb")))
  {
    file_size=File_length_file(file);

    if (Read_bytes(file,IMG_header.Filler1,sizeof(IMG_header.Filler1))
    && Read_word_le(file,&(IMG_header.Width))
    && Read_word_le(file,&(IMG_header.Height))
    && Read_bytes(file,IMG_header.Filler2,sizeof(IMG_header.Filler2))
    && Read_bytes(file,IMG_header.Palette,sizeof(IMG_header.Palette))
    )
    {

      buffer=(byte *)malloc(IMG_header.Width);

      Pre_load(context, IMG_header.Width,IMG_header.Height,file_size,FORMAT_IMG,PIXEL_SIMPLE,0);
      if (File_error==0)
      {
        memcpy(context->Palette,IMG_header.Palette,sizeof(T_Palette));
        Palette_loaded(context);

        context->Width=IMG_header.Width;
        context->Height=IMG_header.Height;
        width_read=IMG_header.Width;

        for (y_pos=0;(y_pos<context->Height) && (!File_error);y_pos++)
        {
          if (Read_bytes(file,buffer,context->Width))
          {
            for (x_pos=0; x_pos<context->Width;x_pos++)
              Set_pixel(context, x_pos,y_pos,buffer[x_pos]);
          }
          else
            File_error=2;
        }
      }

      free(buffer);
      buffer = NULL;
    }
    else
      File_error=1;

    fclose(file);
  }
  else
    File_error=1;
}

// -- Sauver un fichier au format IMG ---------------------------------------
void Save_IMG(T_IO_Context * context)
{
  char filename[MAX_PATH_CHARACTERS]; // Nom complet du fichier
  FILE *file;
  short x_pos,y_pos;
  T_IMG_Header IMG_header;
  byte signature[6]={0x01,0x00,0x47,0x12,0x6D,0xB0};

  Get_full_filename(filename, context->File_name, context->File_directory);

  File_error=0;

  // Ouverture du fichier
  if ((file=fopen(filename,"wb")))
  {
    memcpy(IMG_header.Filler1,signature,6);

    IMG_header.Width=context->Width;
    IMG_header.Height=context->Height;

    memset(IMG_header.Filler2,0,118);
    IMG_header.Filler2[4]=0xFF;
    IMG_header.Filler2[22]=64; // Lo(Longueur de la signature)
    IMG_header.Filler2[23]=0;  // Hi(Longueur de la signature)
    memcpy(IMG_header.Filler2+23,"GRAFX2 by SunsetDesign (IMG format taken from PV (c)W.Wiedmann)",64);

    memcpy(IMG_header.Palette,context->Palette,sizeof(T_Palette));

    if (Write_bytes(file,IMG_header.Filler1,sizeof(IMG_header.Filler1))
    && Write_word_le(file,IMG_header.Width)
    && Write_word_le(file,IMG_header.Height)
    && Write_bytes(file,IMG_header.Filler2,sizeof(IMG_header.Filler2))
    && Write_bytes(file,IMG_header.Palette,sizeof(IMG_header.Palette))
    )

    {
      Init_write_buffer();

      for (y_pos=0; ((y_pos<context->Height) && (!File_error)); y_pos++)
        for (x_pos=0; x_pos<context->Width; x_pos++)
          Write_one_byte(file,Get_pixel(context, x_pos,y_pos));

      End_write(file);
      fclose(file);

      if (File_error)
        remove(filename);
    }
    else // Error d'écriture (disque plein ou protégé)
    {
      fclose(file);
      remove(filename);
      File_error=1;
    }
  }
  else
  {
    fclose(file);
    remove(filename);
    File_error=1;
  }
}


//////////////////////////////////// LBM ////////////////////////////////////
#pragma pack(1)
typedef struct
{
  word  Width;
  word  Height;
  word  X_org;       // Inutile
  word  Y_org;       // Inutile
  byte  BitPlanes;
  byte  Mask;
  byte  Compression;
  byte  Pad1;       // Inutile
  word  Transp_col;
  byte  X_aspect;    // Inutile
  byte  Y_aspect;    // Inutile
  word  X_screen;
  word  Y_screen;
} T_LBM_Header;
#pragma pack()

byte * LBM_buffer;
FILE *LBM_file;

// -- Tester si un fichier est au format LBM --------------------------------

void Test_LBM(T_IO_Context * context)
{
  char filename[MAX_PATH_CHARACTERS];
  char  format[4];
  char  section[4];
  dword dummy;

  Get_full_filename(filename, context->File_name, context->File_directory);

  File_error=0;

  if ((LBM_file=fopen(filename, "rb")))
  {
    if (! Read_bytes(LBM_file,section,4))
      File_error=1;
    else
    if (memcmp(section,"FORM",4))
      File_error=1;
    else
    {
      Read_dword_be(LBM_file, &dummy);
                   //   On aurait pu vérifier que ce long est égal à la taille
                   // du fichier - 8, mais ça aurait interdit de charger des
                   // fichiers tronqués (et déjà que c'est chiant de perdre
                   // une partie du fichier il faut quand même pouvoir en
                   // garder un peu... Sinon, moi je pleure :'( !!! )
      if (! Read_bytes(LBM_file,format,4))
        File_error=1;
      else
      if ( (memcmp(format,"ILBM",4)) && (memcmp(format,"PBM ",4)) )
        File_error=1;
    }
    fclose(LBM_file);
  }
  else
    File_error=1;
}


// -- Lire un fichier au format LBM -----------------------------------------

  byte Image_HAM;

  // ---------------- Adapter la palette pour les images HAM ----------------
  void Adapt_palette_HAM(T_IO_Context * context)
  {
    short i,j,temp;
    byte  color;

    if (Image_HAM==6)
    {
      for (i=1; i<=14; i++)
      {
        // On recopie a palette de base
        memcpy(context->Palette+(i<<4),context->Palette,48);
        // On modifie les teintes de cette palette
        for (j=0; j<16; j++)
        {
          color=(i<<4)+j;
          if (i<=7)
          {
            if (i&1)
            {
              temp=context->Palette[j].R+16;
              context->Palette[color].R=(temp<63)?temp:63;
            }
            if (i&2)
            {
              temp=context->Palette[j].G+16;
              context->Palette[color].G=(temp<63)?temp:63;
            }
            if (i&4)
            {
              temp=context->Palette[j].B+16;
              context->Palette[color].B=(temp<63)?temp:63;
            }
          }
          else
          {
            if ((i-7)&1)
            {
              temp=context->Palette[j].R-16;
              context->Palette[color].R=(temp>=0)?temp:0;
            }
            if ((i-7)&2)
            {
              temp=context->Palette[j].G-16;
              context->Palette[color].G=(temp>=0)?temp:0;
            }
            if ((i-7)&4)
            {
              temp=context->Palette[j].B-16;
              context->Palette[color].B=(temp>=0)?temp:0;
            }
          }
        }
      }
      // Ici, il reste les 16 dernières couleurs à modifier
      for (i=240,j=0; j<16; i++,j++)
      {
        temp=context->Palette[j].R+8;
        context->Palette[i].R=(temp<63)?temp:63;
        temp=context->Palette[j].G+8;
        context->Palette[i].G=(temp<63)?temp:63;
        temp=context->Palette[j].B+8;
        context->Palette[i].B=(temp<63)?temp:63;
      }
    }
    else if (Image_HAM==8)
    {
      for (i=1; i<=3; i++)
      {
        // On recopie la palette de base
        memcpy(context->Palette+(i<<6),context->Palette,192);
        // On modifie les teintes de cette palette
        for (j=0; j<64; j++)
        {
          color=(i<<6)+j;
          switch (i)
          {
            case 1 :
              temp=context->Palette[j].R+16;
              context->Palette[color].R=(temp<63)?temp:63;
              break;
            case 2 :
              temp=context->Palette[j].G+16;
              context->Palette[color].G=(temp<63)?temp:63;
              break;
            default:
              temp=context->Palette[j].B+16;
              context->Palette[color].B=(temp<63)?temp:63;
          }
        }
      }
    }
    else // Image 64 couleurs sauvée en 32.
    {
      for (i=0; i<32; i++)
      {
        j=i+32;
        context->Palette[j].R=context->Palette[i].R>>1;
        context->Palette[j].G=context->Palette[i].G>>1;
        context->Palette[j].B=context->Palette[i].B>>1;
      }
    }
  }

  // ------------------------- Attendre une section -------------------------
  byte Wait_for(byte * expected_section)
  {
    // Valeur retournée: 1=Section trouvée, 0=Section non trouvée (erreur)
    dword Taille_section;
    byte section_read[4];

    if (! Read_bytes(LBM_file,section_read,4))
      return 0;
    while (memcmp(section_read,expected_section,4)) // Sect. pas encore trouvée
    {
      if (!Read_dword_be(LBM_file,&Taille_section))
        return 0;
      if (Taille_section&1)
        Taille_section++;
      if (fseek(LBM_file,Taille_section,SEEK_CUR))
        return 0;
      if (! Read_bytes(LBM_file,section_read,4))
        return 0;
    }
    return 1;
  }

// Les images ILBM sont stockés en bitplanes donc on doit trifouiller les bits pour
// en faire du chunky

byte Color_ILBM_line(word x_pos, word real_line_size, byte HBPm1)
{
  // Renvoie la couleur du pixel (ILBM) en x_pos.
  // CL sera le rang auquel on extrait les bits de la couleur
  byte cl = 7 - (x_pos & 7);
  int ax,bh,dx;
  byte bl=0;

  for(dx=HBPm1;dx>=0;dx--)
  {
  //CIL_Loop
    ax = (real_line_size * dx + x_pos) >> 3;
    bh = (LBM_buffer[ax] >> cl) & 1;

    bl = (bl << 1) + bh;
  }

  return bl;
}

byte HBPm1; // header.BitPlanes-1

  // ----------------------- Afficher une ligne ILBM ------------------------
  void Draw_ILBM_line(T_IO_Context *context, short y_pos, short real_line_size)
  {
    byte  color;
    byte  red,green,blue;
    byte  temp;
    short x_pos;

    if (Image_HAM<=1)                                               // ILBM
    {
      for (x_pos=0; x_pos<context->Width; x_pos++)
      {
        Set_pixel(context, x_pos,y_pos,Color_ILBM_line(x_pos,real_line_size, HBPm1));
      }
    }
    else
    {
      color=0;
      red=context->Palette[0].R;
      green =context->Palette[0].G;
      blue =context->Palette[0].B;
      if (Image_HAM==6)
      for (x_pos=0; x_pos<context->Width; x_pos++)         // HAM6
      {
        temp=Color_ILBM_line(x_pos,real_line_size, HBPm1);
        switch (temp & 0xF0)
        {
          case 0x10: // blue
            blue=(temp&0x0F)<<2;
            color=Best_color(red,green,blue);
            break;
          case 0x20: // red
            red=(temp&0x0F)<<2;
            color=Best_color(red,green,blue);
            break;
          case 0x30: // green
            green=(temp&0x0F)<<2;
            color=Best_color(red,green,blue);
            break;
          default:   // Nouvelle couleur
            color=temp;
            red=context->Palette[color].R;
            green =context->Palette[color].G;
            blue =context->Palette[color].B;
        }
        Set_pixel(context, x_pos,y_pos,color);
      }
      else
      for (x_pos=0; x_pos<context->Width; x_pos++)         // HAM8
      {
        temp=Color_ILBM_line(x_pos,real_line_size, HBPm1);
        switch (temp & 0x03)
        {
          case 0x01: // blue
            blue=temp>>2;
            color=Best_color(red,green,blue);
            break;
          case 0x02: // red
            red=temp>>2;
            color=Best_color(red,green,blue);
            break;
          case 0x03: // green
            green=temp>>2;
            color=Best_color(red,green,blue);
            break;
          default:   // Nouvelle couleur
            color=temp;
            red=context->Palette[color].R;
            green =context->Palette[color].G;
            blue =context->Palette[color].B;
        }
        Set_pixel(context, x_pos,y_pos,color);
      }
    }
  }


void Load_LBM(T_IO_Context * context)
{
  char filename[MAX_PATH_CHARACTERS];
  T_LBM_Header header;
  char  format[4];
  char  section[4];
  byte  temp_byte;
  short b256;
  dword nb_colors;
  dword image_size;
  short x_pos;
  short y_pos;
  short counter;
  short line_size;       // Taille d'une ligne en octets
  short real_line_size; // Taille d'une ligne en pixels
  byte  color;
  long  file_size;
  dword dummy;

  Get_full_filename(filename, context->File_name, context->File_directory);

  File_error=0;

  if ((LBM_file=fopen(filename, "rb")))
  {
    file_size=File_length_file(LBM_file);

    // On avance dans le fichier (pas besoin de tester ce qui l'a déjà été)
    Read_bytes(LBM_file,section,4);
    Read_dword_be(LBM_file,&dummy);
    Read_bytes(LBM_file,format,4);
    if (!Wait_for((byte *)"BMHD"))
      File_error=1;
    Read_dword_be(LBM_file,&dummy);

    // Maintenant on lit le header pour pouvoir commencer le chargement de l'image
    if ( (Read_word_be(LBM_file,&header.Width))
      && (Read_word_be(LBM_file,&header.Height))
      && (Read_word_be(LBM_file,&header.X_org))
      && (Read_word_be(LBM_file,&header.Y_org))
      && (Read_byte(LBM_file,&header.BitPlanes))
      && (Read_byte(LBM_file,&header.Mask))
      && (Read_byte(LBM_file,&header.Compression))
      && (Read_byte(LBM_file,&header.Pad1))
      && (Read_word_be(LBM_file,&header.Transp_col))
      && (Read_byte(LBM_file,&header.X_aspect))
      && (Read_byte(LBM_file,&header.Y_aspect))
      && (Read_word_be(LBM_file,&header.X_screen))
      && (Read_word_be(LBM_file,&header.Y_screen))
      && header.Width && header.Height)
    {
      if ( (header.BitPlanes) && (Wait_for((byte *)"CMAP")) )
      {
        Read_dword_be(LBM_file,&nb_colors);
        nb_colors/=3;

        if (((dword)1<<header.BitPlanes)!=nb_colors)
        {
          if ((nb_colors==32) && (header.BitPlanes==6))
          {              // Ce n'est pas une image HAM mais une image 64 coul.
            Image_HAM=1; // Sauvée en 32 coul. => il faut copier les 32 coul.
          }              // sur les 32 suivantes et assombrir ces dernières.
          else
          {
            if ((header.BitPlanes==6) || (header.BitPlanes==8))
              Image_HAM=header.BitPlanes;
            else
              /* File_error=1;*/  /* C'est censé être incorrect mais j'ai */
              Image_HAM=0;            /* trouvé un fichier comme ça, alors... */
          }
        }
        else
          Image_HAM=0;

        if ( (!File_error) && (nb_colors>=2) && (nb_colors<=256) )
        {
          HBPm1=header.BitPlanes-1;
          if (header.Mask==1)
            header.BitPlanes++;

          // Deluxe paint le fait... alors on le fait...
          Back_color=header.Transp_col;

          // On commence par passer la palette en 256 comme ça, si la nouvelle
          // palette a moins de 256 coul, la précédente ne souffrira pas d'un
          // assombrissement préjudiciable.
          if (Config.Clear_palette)
            memset(context->Palette,0,sizeof(T_Palette));
          else
            Palette_64_to_256(context->Palette);
          //   On peut maintenant charger la nouvelle palette
          if (Read_bytes(LBM_file,context->Palette,3*nb_colors))
          {
            Palette_256_to_64(context->Palette);
            if (Image_HAM)
              Adapt_palette_HAM(context);
            Palette_64_to_256(context->Palette);
            Palette_loaded(context);

            // On lit l'octet de padding du CMAP si la taille est impaire
            if (nb_colors&1)
              if (Read_byte(LBM_file,&temp_byte))
                File_error=2;

            if ( (Wait_for((byte *)"BODY")) && (!File_error) )
            {
              Read_dword_be(LBM_file,&image_size);
              //swab((char *)&header.Width ,(char *)&context->Width,2);
              //swab((char *)&header.Height,(char *)&context->Height,2);
              context->Width = header.Width;
              context->Height = header.Height;

              //swab((char *)&header.X_screen,(char *)&Original_screen_X,2);
              //swab((char *)&header.Y_screen,(char *)&Original_screen_Y,2);
              Original_screen_X = header.X_screen;
              Original_screen_Y = header.Y_screen;

              Pre_load(context, context->Width,context->Height,file_size,FORMAT_LBM,PIXEL_SIMPLE,0);
              if (File_error==0)
              {
                if (!memcmp(format,"ILBM",4))    // "ILBM": InterLeaved BitMap
                {
                  // Calcul de la taille d'une ligne ILBM (pour les images ayant des dimensions exotiques)
                  if (context->Width & 15)
                  {
                    real_line_size=( (context->Width+16) >> 4 ) << 4;
                    line_size=( (context->Width+16) >> 4 )*(header.BitPlanes<<1);
                  }
                  else
                  {
                    real_line_size=context->Width;
                    line_size=(context->Width>>3)*header.BitPlanes;
                  }

                  if (!header.Compression)
                  {                                           // non compressé
                    LBM_buffer=(byte *)malloc(line_size);
                    for (y_pos=0; ((y_pos<context->Height) && (!File_error)); y_pos++)
                    {
                      if (Read_bytes(LBM_file,LBM_buffer,line_size))
                        Draw_ILBM_line(context, y_pos,real_line_size);
                      else
                        File_error=2;
                    }
                    free(LBM_buffer);
                    LBM_buffer = NULL;
                  }
                  else
                  {                                               // compressé
                    /*Init_lecture();*/

                    LBM_buffer=(byte *)malloc(line_size);

                    for (y_pos=0; ((y_pos<context->Height) && (!File_error)); y_pos++)
                    {
                      for (x_pos=0; ((x_pos<line_size) && (!File_error)); )
                      {
                        if(Read_byte(LBM_file, &temp_byte)!=1)
                        {
                          File_error=2;
                          break;
                        }
                        // Si temp_byte > 127 alors il faut répéter 256-'temp_byte' fois la couleur de l'octet suivant
                        // Si temp_byte <= 127 alors il faut afficher directement les 'temp_byte' octets suivants
                        if (temp_byte>127)
                        {
                          if(Read_byte(LBM_file, &color)!=1)
                          {
                            File_error=2;
                            break;
                          }
                          b256=(short)(256-temp_byte);
                          for (counter=0; counter<=b256; counter++)
                            if (x_pos<line_size)
                              LBM_buffer[x_pos++]=color;
                            else
                              File_error=2;
                        }
                        else
                          for (counter=0; counter<=(short)(temp_byte); counter++)
                            if (x_pos>=line_size || Read_byte(LBM_file, &(LBM_buffer[x_pos++]))!=1)
                              File_error=2;
                      }
                      if (!File_error)
                        Draw_ILBM_line(context, y_pos,real_line_size);
                    }

                    free(LBM_buffer);
                    LBM_buffer = NULL;
                    /*Close_lecture();*/
                  }
                }
                else                               // "PBM ": Planar(?) BitMap
                {
                  real_line_size=context->Width+(context->Width&1);

                  if (!header.Compression)
                  {                                           // non compressé
                    LBM_buffer=(byte *)malloc(real_line_size);
                    for (y_pos=0; ((y_pos<context->Height) && (!File_error)); y_pos++)
                    {
                      if (Read_bytes(LBM_file,LBM_buffer,real_line_size))
                        for (x_pos=0; x_pos<context->Width; x_pos++)
                          Set_pixel(context, x_pos,y_pos,LBM_buffer[x_pos]);
                      else
                        File_error=2;
                    }
                    free(LBM_buffer);
                    LBM_buffer = NULL;
                  }
                  else
                  {                                               // compressé
                    /*Init_lecture();*/
                    for (y_pos=0; ((y_pos<context->Height) && (!File_error)); y_pos++)
                    {
                      for (x_pos=0; ((x_pos<real_line_size) && (!File_error)); )
                      {
                        if(Read_byte(LBM_file, &temp_byte)!=1)
                        {
                          File_error=2;
                          break;
                        }
                        if (temp_byte>127)
                        {
                          if(Read_byte(LBM_file, &color)!=1)
                          {
                            File_error=2;
                            break;
                          }
                          b256=256-temp_byte;
                          for (counter=0; counter<=b256; counter++)
                            Set_pixel(context, x_pos++,y_pos,color);
                        }
                        else
                          for (counter=0; counter<=temp_byte; counter++)
                          {
                            byte byte_read=0;
                            if(Read_byte(LBM_file, &byte_read)!=1)
                            {
                              File_error=2;
                              break;
                            }
                            Set_pixel(context, x_pos++,y_pos,byte_read);
                          }
                      }
                    }
                    /*Close_lecture();*/
                  }
                }
              }
            }
            else
              Set_file_error(2);
          }
          else
          {
            File_error=1;
          }
        }
        else
          Set_file_error(1);
      }
      else
        File_error=1;
    }
    else
      File_error=1;

    fclose(LBM_file);
  }
  else
    File_error=1;
}


// -- Sauver un fichier au format LBM ---------------------------------------

  byte LBM_color_list[129];
  word LBM_list_size;
  byte LBM_repetition_mode;

  // ------------- Ecrire les couleurs que l'on vient de traiter ------------
  void Transfer_colors(void)
  {
    byte index;

    if (LBM_list_size>0)
    {
      if (LBM_repetition_mode)
      {
        Write_one_byte(LBM_file,257-LBM_list_size);
        Write_one_byte(LBM_file,LBM_color_list[0]);
      }
      else
      {
        Write_one_byte(LBM_file,LBM_list_size-1);
        for (index=0; index<LBM_list_size; index++)
          Write_one_byte(LBM_file,LBM_color_list[index]);
      }
    }
    LBM_list_size=0;
    LBM_repetition_mode=0;
  }

  // - Compresion des couleurs encore plus performante que DP2e et que VPIC -
  void New_color(byte color)
  {
    byte last_color;
    byte second_last_color;

    switch (LBM_list_size)
    {
      case 0 : // Première couleur
        LBM_color_list[0]=color;
        LBM_list_size=1;
        break;
      case 1 : // Deuxième couleur
        last_color=LBM_color_list[0];
        LBM_repetition_mode=(last_color==color);
        LBM_color_list[1]=color;
        LBM_list_size=2;
        break;
      default: // Couleurs suivantes
        last_color      =LBM_color_list[LBM_list_size-1];
        second_last_color=LBM_color_list[LBM_list_size-2];
        if (last_color==color)  // On a une répétition de couleur
        {
          if ( (LBM_repetition_mode) || (second_last_color!=color) )
          // On conserve le mode...
          {
            LBM_color_list[LBM_list_size]=color;
            LBM_list_size++;
            if (LBM_list_size==128)
              Transfer_colors();
          }
          else // On est en mode <> et on a 3 couleurs qui se suivent
          {
            LBM_list_size-=2;
            Transfer_colors();
            LBM_color_list[0]=color;
            LBM_color_list[1]=color;
            LBM_color_list[2]=color;
            LBM_list_size=3;
            LBM_repetition_mode=1;
          }
        }
        else // La couleur n'est pas la même que la précédente
        {
          if (!LBM_repetition_mode)                 // On conserve le mode...
          {
            LBM_color_list[LBM_list_size++]=color;
            if (LBM_list_size==128)
              Transfer_colors();
          }
          else                                        // On change de mode...
          {
            Transfer_colors();
            LBM_color_list[LBM_list_size]=color;
            LBM_list_size++;
          }
        }
    }
  }


void Save_LBM(T_IO_Context * context)
{
  char filename[MAX_PATH_CHARACTERS];
  T_LBM_Header header;
  word x_pos;
  word y_pos;
  byte temp_byte;
  word real_width;
  int file_size;

  File_error=0;
  Get_full_filename(filename, context->File_name, context->File_directory);

  // Ouverture du fichier
  if ((LBM_file=fopen(filename,"wb")))
  {
    Write_bytes(LBM_file,"FORM",4);
    Write_dword_be(LBM_file,0); // On mettra la taille à jour à la fin

    Write_bytes(LBM_file,"PBM BMHD",8);
    Write_dword_be(LBM_file,20);

    // On corrige la largeur de l'image pour qu'elle soit multiple de 2
    real_width=context->Width+(context->Width&1);

    //swab((byte *)&real_width,(byte *)&header.Width,2);
    header.Width=context->Width;
    header.Height=context->Height;
    header.X_org=0;
    header.Y_org=0;
    header.BitPlanes=8;
    header.Mask=0;
    header.Compression=1;
    header.Pad1=0;
    header.Transp_col=Back_color;
    header.X_aspect=1;
    header.Y_aspect=1;
    header.X_screen = Screen_width;
    header.Y_screen = Screen_height;

    Write_word_be(LBM_file,header.Width);
    Write_word_be(LBM_file,header.Height);
    Write_word_be(LBM_file,header.X_org);
    Write_word_be(LBM_file,header.Y_org);
    Write_bytes(LBM_file,&header.BitPlanes,1);
    Write_bytes(LBM_file,&header.Mask,1);
    Write_bytes(LBM_file,&header.Compression,1);
    Write_bytes(LBM_file,&header.Pad1,1);
    Write_word_be(LBM_file,header.Transp_col);
    Write_bytes(LBM_file,&header.X_aspect,1);
    Write_bytes(LBM_file,&header.Y_aspect,1);
    Write_word_be(LBM_file,header.X_screen);
    Write_word_be(LBM_file,header.Y_screen);

    Write_bytes(LBM_file,"CMAP",4);
    Write_dword_be(LBM_file,sizeof(T_Palette));

    Write_bytes(LBM_file,context->Palette,sizeof(T_Palette));

    Write_bytes(LBM_file,"BODY",4);
    Write_dword_be(LBM_file,0); // On mettra la taille à jour à la fin

    Init_write_buffer();

    LBM_list_size=0;

    for (y_pos=0; ((y_pos<context->Height) && (!File_error)); y_pos++)
    {
      for (x_pos=0; ((x_pos<real_width) && (!File_error)); x_pos++)
        New_color(Get_pixel(context, x_pos,y_pos));

      if (!File_error)
        Transfer_colors();
    }

    End_write(LBM_file);
    fclose(LBM_file);

    if (!File_error)
    {
      file_size=File_length(filename);
      
      LBM_file=fopen(filename,"rb+");
      fseek(LBM_file,820,SEEK_SET);
      Write_dword_be(LBM_file,file_size-824);

      if (!File_error)
      {
        fseek(LBM_file,4,SEEK_SET);

        //   Si la taille de la section de l'image (taille fichier-8) est
        // impaire, on rajoute un 0 (Padding) à la fin.
        if ((file_size) & 1)
        {
          Write_dword_be(LBM_file,file_size-7);
          fseek(LBM_file,0,SEEK_END);
          temp_byte=0;
          if (! Write_bytes(LBM_file,&temp_byte,1))
            File_error=1;
        }
        else
          Write_dword_be(LBM_file,file_size-8);

        fclose(LBM_file);

        if (File_error)
          remove(filename);
      }
      else
      {
        File_error=1;
        fclose(LBM_file);
        remove(filename);
      }
    }
    else // Il y a eu une erreur lors du compactage => on efface le fichier
      remove(filename);
  }
  else
    File_error=1;
}



//////////////////////////////////// BMP ////////////////////////////////////
typedef struct
{
    byte  Signature[2];   // ='BM' = 0x4D42
    dword Size_1;    // file size
    word  Reserved_1; // 0
    word  Reserved_2; // 0
    dword Offset; // Offset of bitmap data start

    dword Size_2; // 40
    dword Width;
    dword Height;
    word  Planes; // 1
    word  Nb_bits; // 1,4,8 ou 24
    dword Compression;
    dword Size_3;
    dword XPM;
    dword YPM;
    dword Nb_Clr;
    dword Clr_Imprt;
} T_BMP_Header;

// -- Tester si un fichier est au format BMP --------------------------------
void Test_BMP(T_IO_Context * context)
{
  char filename[MAX_PATH_CHARACTERS];
  FILE *file;
  T_BMP_Header header;

  File_error=1;
  Get_full_filename(filename, context->File_name, context->File_directory);

  if ((file=fopen(filename, "rb")))
  {
    if (Read_bytes(file,&(header.Signature),2) // "BM"
     && Read_dword_le(file,&(header.Size_1))
     && Read_word_le(file,&(header.Reserved_1))
     && Read_word_le(file,&(header.Reserved_2))
     && Read_dword_le(file,&(header.Offset))
     && Read_dword_le(file,&(header.Size_2))
     && Read_dword_le(file,&(header.Width))
     && Read_dword_le(file,&(header.Height))
     && Read_word_le(file,&(header.Planes))
     && Read_word_le(file,&(header.Nb_bits))
     && Read_dword_le(file,&(header.Compression))
     && Read_dword_le(file,&(header.Size_3))
     && Read_dword_le(file,&(header.XPM))
     && Read_dword_le(file,&(header.YPM))
     && Read_dword_le(file,&(header.Nb_Clr))
     && Read_dword_le(file,&(header.Clr_Imprt))
        )
     {

      if ( header.Signature[0]=='B' && header.Signature[1]=='M'
        && header.Size_2==40
        && header.Width && header.Height )
        File_error=0;
     }
    fclose(file);
  }
}

// Find the 8 important bits in a dword
byte Bitmap_mask(dword pixel, dword mask)
{
  byte result;
  int i;
  int bits_found;

  switch(mask)
  {
    // Shortcuts to quickly handle the common 24/32bit cases
    case 0x000000FF:
      return (pixel & 0x000000FF);
    case 0x0000FF00:
      return (pixel & 0x0000FF00)>>8;
    case 0x00FF0000:
      return (pixel & 0x00FF0000)>>16;
    case 0xFF000000:
      return (pixel & 0xFF000000)>>24;
  }
  // Uncommon : do it bit by bit.
  bits_found=0;
  result=0;
  // Process the mask from low to high bit
  for (i=0;i<32;i++)
  {
    // Found a bit in the mask
    if (mask & (1<<i))
    {
      if (pixel & 1<<i)
        result |= 1<<bits_found;
        
      bits_found++;
      
      if (bits_found>=8)
        return result;
    }
  }
  // Less than 8 bits in the mask: scale the result to 8 bits
  return result << (8-bits_found);
}

// -- Charger un fichier au format BMP --------------------------------------
void Load_BMP(T_IO_Context * context)
{
  char filename[MAX_PATH_CHARACTERS];
  FILE *file;
  T_BMP_Header header;
  byte * buffer;
  word  index;
  byte  local_palette[256][4]; // R,G,B,0
  word  nb_colors =  0;
  short x_pos;
  short y_pos;
  word  line_size;
  byte  a,b,c=0;
  long  file_size;

  Get_full_filename(filename, context->File_name, context->File_directory);

  File_error=0;

  if ((file=fopen(filename, "rb")))
  {
    file_size=File_length_file(file);

    if (Read_bytes(file,header.Signature,2)
     && Read_dword_le(file,&(header.Size_1))
     && Read_word_le(file,&(header.Reserved_1))
     && Read_word_le(file,&(header.Reserved_2))
     && Read_dword_le(file,&(header.Offset))
     && Read_dword_le(file,&(header.Size_2))
     && Read_dword_le(file,&(header.Width))
     && Read_dword_le(file,&(header.Height))
     && Read_word_le(file,&(header.Planes))
     && Read_word_le(file,&(header.Nb_bits))
     && Read_dword_le(file,&(header.Compression))
     && Read_dword_le(file,&(header.Size_3))
     && Read_dword_le(file,&(header.XPM))
     && Read_dword_le(file,&(header.YPM))
     && Read_dword_le(file,&(header.Nb_Clr))
     && Read_dword_le(file,&(header.Clr_Imprt))
    )
    {
      switch (header.Nb_bits)
      {
        case 1 :
        case 4 :
        case 8 :
          if (header.Nb_Clr)
            nb_colors=header.Nb_Clr;
          else
            nb_colors=1<<header.Nb_bits;
          break;
        default:
          File_error=1;
      }

      if (!File_error)
      {
        Pre_load(context, header.Width,header.Height,file_size,FORMAT_BMP,PIXEL_SIMPLE,0);
        if (File_error==0)
        {
          if (Read_bytes(file,local_palette,nb_colors<<2))
          {
            if (Config.Clear_palette)
              memset(context->Palette,0,sizeof(T_Palette));
            //   On peut maintenant transférer la nouvelle palette
            for (index=0; index<nb_colors; index++)
            {
              context->Palette[index].R=local_palette[index][2];
              context->Palette[index].G=local_palette[index][1];
              context->Palette[index].B=local_palette[index][0];
            }
            Palette_loaded(context);

            context->Width=header.Width;
            context->Height=header.Height;

            switch (header.Compression)
            {
              case 0 : // Pas de compression
                line_size=context->Width;
                x_pos=(32/header.Nb_bits); // x_pos sert de variable temporaire
                // On arrondit line_size au premier multiple de x_pos supérieur
                if (line_size % x_pos)
                  line_size=((line_size/x_pos)*x_pos)+x_pos;
                // On convertit cette taille en octets
                line_size=(line_size*header.Nb_bits)>>3;

                buffer=(byte *)malloc(line_size);
                for (y_pos=context->Height-1; ((y_pos>=0) && (!File_error)); y_pos--)
                {
                  if (Read_bytes(file,buffer,line_size))
                    for (x_pos=0; x_pos<context->Width; x_pos++)
                      switch (header.Nb_bits)
                      {
                        case 8 :
                          Set_pixel(context, x_pos,y_pos,buffer[x_pos]);
                          break;
                        case 4 :
                          if (x_pos & 1)
                            Set_pixel(context, x_pos,y_pos,buffer[x_pos>>1] & 0xF);
                          else
                            Set_pixel(context, x_pos,y_pos,buffer[x_pos>>1] >> 4 );
                          break;
                        case 1 :
                          if ( buffer[x_pos>>3] & (0x80>>(x_pos&7)) )
                            Set_pixel(context, x_pos,y_pos,1);
                          else
                            Set_pixel(context, x_pos,y_pos,0);
                      }
                  else
                    File_error=2;
                }
                free(buffer);
                buffer = NULL;
                break;

              case 1 : // Compression RLE 8 bits
                x_pos=0;
                y_pos=context->Height-1;

                /*Init_lecture();*/
                if(Read_byte(file, &a)!=1 || Read_byte(file, &b)!=1)
                  File_error=2;
                while (!File_error)
                {
                  if (a) // Encoded mode
                    for (index=1; index<=a; index++)
                      Set_pixel(context, x_pos++,y_pos,b);
                  else   // Absolute mode
                    switch (b)
                    {
                      case 0 : // End of line
                        x_pos=0;
                        y_pos--;
                        break;
                      case 1 : // End of bitmap
                        break;
                      case 2 : // Delta
                        if(Read_byte(file, &a)!=1 || Read_byte(file, &b)!=1)
                          File_error=2;
                        x_pos+=a;
                        y_pos-=b;
                        break;
                      default: // Nouvelle série
                        while (b)
                        {
                          if(Read_byte(file, &a)!=1)
                            File_error=2;
                          //Read_one_byte(file, &c);
                          Set_pixel(context, x_pos++,y_pos,a);
                          //if (--c)
                          //{
                          //  Set_pixel(context, x_pos++,y_pos,c);
                          //  b--;
                          //}
                          b--;
                        }
                        if (ftell(file) & 1) fseek(file, 1, SEEK_CUR);
                    }
                  if (a==0 && b==1)
                    break;
                  if(Read_byte(file, &a) !=1 || Read_byte(file, &b)!=1)
                  {
                    File_error=2;
                  }
                }
                /*Close_lecture();*/
                break;

              case 2 : // Compression RLE 4 bits
                x_pos=0;
                y_pos=context->Height-1;

                /*Init_lecture();*/
                if(Read_byte(file, &a)!=1 ||  Read_byte(file, &b) != 1)
                  File_error =2;
                while ( (!File_error) && ((a)||(b!=1)) )
                {
                  if (a) // Encoded mode (A fois les 1/2 pixels de B)
                    for (index=1; index<=a; index++)
                    {
                      if (index & 1)
                        Set_pixel(context, x_pos,y_pos,b>>4);
                      else
                        Set_pixel(context, x_pos,y_pos,b&0xF);
                      x_pos++;
                    }
                  else   // Absolute mode
                    switch (b)
                    {
                      case 0 : //End of line
                        x_pos=0;
                        y_pos--;
                        break;
                      case 1 : // End of bitmap
                        break;
                      case 2 : // Delta
                       if(Read_byte(file, &a)!=1 ||  Read_byte(file, &b)!=1)
                         File_error=2;
                        x_pos+=a;
                        y_pos-=b;
                        break;
                      default: // Nouvelle série (B 1/2 pixels bruts)
                        for (index=1; ((index<=b) && (!File_error)); index++,x_pos++)
                        {
                          if (index&1)
                          {
                            if(Read_byte(file, &c)!=1) File_error=2;
                            Set_pixel(context, x_pos,y_pos,c>>4);
                          }
                          else
                            Set_pixel(context, x_pos,y_pos,c&0xF);
                        }
                        //   On lit l'octet rendant le nombre d'octets pair, si
                        // nécessaire. Encore un truc de crétin "made in MS".
                        if ( ((b&3)==1) || ((b&3)==2) )
                        {
                          byte dummy;
                          if(Read_byte(file, &dummy)!=1) File_error=2;
                        }
                    }
                  if(Read_byte(file, &a)!=1 || Read_byte(file, &b)!=1) File_error=2;
                }
                /*Close_lecture();*/
            }
            fclose(file);
          }
          else
          {
            fclose(file);
            File_error=1;
          }
        }
      }
      else
      {
        // Image 16/24/32 bits
        dword red_mask;
        dword green_mask;
        dword blue_mask;
        if (header.Nb_bits == 16)
        {
          red_mask =   0x00007C00;
          green_mask = 0x000003E0;
          blue_mask =  0x0000001F;
        }
        else
        {
          red_mask = 0x00FF0000;
          green_mask = 0x0000FF00;
          blue_mask = 0x000000FF;
        }
        File_error=0;

        context->Width=header.Width;
        context->Height=header.Height;
        Pre_load(context,header.Width,header.Height,file_size,FORMAT_BMP,PIXEL_SIMPLE,1);
        if (File_error==0)
        {
          switch (header.Compression)
          {
            case 3: // BI_BITFIELDS
              if (!Read_dword_le(file,&red_mask) ||
                  !Read_dword_le(file,&green_mask) ||
                  !Read_dword_le(file,&blue_mask))
                File_error=2;
              break;
            default:
              break;
          }
          if (fseek(file, header.Offset, SEEK_SET))
            File_error=2;
        }
        if (File_error==0)
        {
          switch (header.Nb_bits)
          {
            // 24bit bitmap
            default:
            case 24:
              line_size=context->Width*3;
              x_pos=(line_size % 4); // x_pos sert de variable temporaire
              if (x_pos>0)
                line_size+=(4-x_pos);
    
              buffer=(byte *)malloc(line_size);
              for (y_pos=context->Height-1; ((y_pos>=0) && (!File_error)); y_pos--)
              {
                if (Read_bytes(file,buffer,line_size))
                  for (x_pos=0,index=0; x_pos<context->Width; x_pos++,index+=3)
                    Set_pixel_24b(context, x_pos,y_pos,buffer[index+2],buffer[index+1],buffer[index+0]);
                else
                  File_error=2;
              }
              break;

            // 32bit bitmap
            case 32:
              line_size=context->Width*4;
              buffer=(byte *)malloc(line_size);
              for (y_pos=context->Height-1; ((y_pos>=0) && (!File_error)); y_pos--)
              {
                if (Read_bytes(file,buffer,line_size))
                  for (x_pos=0; x_pos<context->Width; x_pos++)
                  {
                    dword pixel=*(((dword *)buffer)+x_pos);
                    Set_pixel_24b(context, x_pos,y_pos,Bitmap_mask(pixel,red_mask),Bitmap_mask(pixel,green_mask),Bitmap_mask(pixel,blue_mask));
                  }
                else
                  File_error=2;
              }
              break;

            // 16bit bitmap
            case 16:
              line_size=(context->Width*2) + (context->Width&1)*2;
              buffer=(byte *)malloc(line_size);
              for (y_pos=context->Height-1; ((y_pos>=0) && (!File_error)); y_pos--)
              {
                if (Read_bytes(file,buffer,line_size))
                  for (x_pos=0; x_pos<context->Width; x_pos++)
                  {
                    word pixel=*(((word *)buffer)+x_pos);
                    Set_pixel_24b(context, x_pos,y_pos,Bitmap_mask(pixel,red_mask),Bitmap_mask(pixel,green_mask),Bitmap_mask(pixel,blue_mask));
                  }
                else
                  File_error=2;
              }
              break;
            
          }
          free(buffer);
          buffer = NULL;
          fclose(file);
        }
      }
    }
    else
    {
      fclose(file);
      File_error=1;
    }
  }
  else
    File_error=1;
}


// -- Sauvegarder un fichier au format BMP ----------------------------------
void Save_BMP(T_IO_Context * context)
{
  char filename[MAX_PATH_CHARACTERS];
  FILE *file;
  T_BMP_Header header;
  short x_pos;
  short y_pos;
  long line_size;
  word index;
  byte local_palette[256][4]; // R,G,B,0


  File_error=0;
  Get_full_filename(filename, context->File_name, context->File_directory);

  // Ouverture du fichier
  if ((file=fopen(filename,"wb")))
  {

    // Image width must be a multiple of 4 bytes
    line_size = context->Width;
    if (line_size & 3)
      line_size += (4 - (line_size & 3));
    
    header.Signature[0]  = 'B';
    header.Signature[1]  = 'M';
    header.Size_1   =(line_size*context->Height)+1078;
    header.Reserved_1   =0;
    header.Reserved_2   =0;
    header.Offset   =1078; // Size of header data (including palette)
    header.Size_2   =40; // Size of header
    header.Width    =context->Width;
    header.Height    =context->Height;
    header.Planes      =1;
    header.Nb_bits    =8;
    header.Compression=0;
    header.Size_3   =0;
    header.XPM        =0;
    header.YPM        =0;
    header.Nb_Clr     =0;
    header.Clr_Imprt  =0;

    if (Write_bytes(file,header.Signature,2)
     && Write_dword_le(file,header.Size_1)
     && Write_word_le(file,header.Reserved_1)
     && Write_word_le(file,header.Reserved_2)
     && Write_dword_le(file,header.Offset)
     && Write_dword_le(file,header.Size_2)
     && Write_dword_le(file,header.Width)
     && Write_dword_le(file,header.Height)
     && Write_word_le(file,header.Planes)
     && Write_word_le(file,header.Nb_bits)
     && Write_dword_le(file,header.Compression)
     && Write_dword_le(file,header.Size_3)
     && Write_dword_le(file,header.XPM)
     && Write_dword_le(file,header.YPM)
     && Write_dword_le(file,header.Nb_Clr)
     && Write_dword_le(file,header.Clr_Imprt))
    {
      //   Chez Bill, ils ont dit: "On va mettre les couleur dans l'ordre
      // inverse, et pour faire chier, on va les mettre sur une échelle de
      // 0 à 255 parce que le standard VGA c'est de 0 à 63 (logique!). Et
      // puis comme c'est pas assez débile, on va aussi y rajouter un octet
      // toujours à 0 pour forcer les gens à s'acheter des gros disques
      // durs... Comme ça, ça fera passer la pillule lorsqu'on sortira
      // Windows 95." ...
      for (index=0; index<256; index++)
      {
        local_palette[index][0]=context->Palette[index].B;
        local_palette[index][1]=context->Palette[index].G;
        local_palette[index][2]=context->Palette[index].R;
        local_palette[index][3]=0;
      }

      if (Write_bytes(file,local_palette,1024))
      {
        Init_write_buffer();

        // ... Et Bill, il a dit: "OK les gars! Mais seulement si vous rangez
        // les pixels dans l'ordre inverse, mais que sur les Y quand-même
        // parce que faut pas pousser."
        for (y_pos=context->Height-1; ((y_pos>=0) && (!File_error)); y_pos--)
          for (x_pos=0; x_pos<line_size; x_pos++)
                Write_one_byte(file,Get_pixel(context, x_pos,y_pos));

        End_write(file);
        fclose(file);

        if (File_error)
          remove(filename);
      }
      else
      {
        fclose(file);
        remove(filename);
        File_error=1;
      }

    }
    else
    {
      fclose(file);
      remove(filename);
      File_error=1;
    }
  }
  else
    File_error=1;
}


//////////////////////////////////// GIF ////////////////////////////////////
#pragma pack(1)
typedef struct
{
  word Width;   // Width of the complete image area
  word Height;  // Height of the complete image area
  byte Resol;   // Informations about the resolution (and other)
  byte Backcol; // Proposed background color
  byte Aspect;  // Informations about aspect ratio (and other)
} T_GIF_LSDB;   // Logical Screen Descriptor Block

typedef struct
{
  word Pos_X;         // X offset where the image should be pasted
  word Pos_Y;         // Y offset where the image should be pasted
  word Image_width;   // Width of image
  word Image_height;  // Height of image
  byte Indicator;     // Misc image information
  byte Nb_bits_pixel; // Nb de bits par pixel
} T_GIF_IDB;          // Image Descriptor Block
#pragma pack()

typedef struct
{
  // byte Block_identifier : 0x21
  // byte Function         : 0xF9
  // byte Block_size         // 4
  byte Packed_fields;     // 11100000 : Reserved
                          // 00011100 : Disposal method
                          // 00000010 : User input flag
                          // 00000001 : Transparent flag
  word Delay_time;        // Time for this frame to stay displayed
  byte Transparent_color; // Which color index acts as transparent
  //word Bloc_terminator; // 0x00
} T_GIF_GCE;              // Graphic Control Extension

// -- Tester si un fichier est au format GIF --------------------------------

void Test_GIF(T_IO_Context * context)
{
  char filename[MAX_PATH_CHARACTERS];
  char signature[6];
  FILE *file;


  File_error=1;
  Get_full_filename(filename, context->File_name, context->File_directory);

  if ((file=fopen(filename, "rb")))
  {
    if (
        (Read_bytes(file,signature,6)) &&
        ((!memcmp(signature,"GIF87a",6))||(!memcmp(signature,"GIF89a",6)))
       )
      File_error=0;

    fclose(file);
  }
}


// -- Lire un fichier au format GIF -----------------------------------------
// -- Lire un fichier au format GIF -----------------------------------------

// Définition de quelques variables globales au chargement du GIF87a
word GIF_nb_bits;        // Nb de bits composants un code complet
word GIF_remainder_bits;      // Nb de bits encore dispos dans GIF_last_byte
byte GIF_remainder_byte;      // Nb d'octets avant le prochain bloc de Raster Data
word GIF_current_code;    // Code traité (qui vient d'être lu en général)
byte GIF_last_byte;      // Octet de lecture des bits
word GIF_pos_X;          // Coordonnées d'affichage de l'image
word GIF_pos_Y;
word GIF_interlaced;     // L'image est entrelacée
word GIF_finished_interlaced_image; // L'image entrelacée est finie de charger
word GIF_pass;          // index de passe de l'image entrelacée
FILE *GIF_file;        // L'handle du fichier

// -- Lit le code à GIF_nb_bits suivant --

word GIF_get_next_code(void)
{
  word nb_bits_to_process=GIF_nb_bits;
  word nb_bits_processed  =0;
  word current_nb_bits;

  GIF_current_code=0;

  while (nb_bits_to_process)
  {
    if (GIF_remainder_bits==0) // Il ne reste plus de bits...
    {
      // Lire l'octet suivant:

      // Si on a atteint la fin du bloc de Raster Data
      if (GIF_remainder_byte==0)
        // Lire l'octet nous donnant la taille du bloc de Raster Data suivant
        if(Read_byte(GIF_file, &GIF_remainder_byte)!=1)
          File_error=2;

      if(Read_byte(GIF_file,&GIF_last_byte)!=1)
        File_error = 2;
      GIF_remainder_byte--;
      GIF_remainder_bits=8;
    }

    current_nb_bits=(nb_bits_to_process<=GIF_remainder_bits)?nb_bits_to_process:GIF_remainder_bits;

    GIF_current_code|=(GIF_last_byte & ((1<<current_nb_bits)-1))<<nb_bits_processed;
    GIF_last_byte>>=current_nb_bits;
    nb_bits_processed  +=current_nb_bits;
    nb_bits_to_process-=current_nb_bits;
    GIF_remainder_bits    -=current_nb_bits;
  }

  return GIF_current_code;
}

// -- Affiche un nouveau pixel --

void GIF_new_pixel(T_IO_Context * context, T_GIF_IDB *idb, byte color)
{
  Set_pixel(context, idb->Pos_X+GIF_pos_X, idb->Pos_Y+GIF_pos_Y,color);

  GIF_pos_X++;

  if (GIF_pos_X>=idb->Image_width)
  {
    GIF_pos_X=0;

    if (!GIF_interlaced)
      GIF_pos_Y++;
    else
    {
      switch (GIF_pass)
      {
        case 0 : GIF_pos_Y+=8;
                 break;
        case 1 : GIF_pos_Y+=8;
                 break;
        case 2 : GIF_pos_Y+=4;
                 break;
        default: GIF_pos_Y+=2;
      }

      if (GIF_pos_Y>=idb->Image_height)
      {
        switch(++GIF_pass)
        {
        case 1 : GIF_pos_Y=4;
                 break;
        case 2 : GIF_pos_Y=2;
                 break;
        case 3 : GIF_pos_Y=1;
                 break;
        case 4 : GIF_finished_interlaced_image=1;
        }
      }
    }
  }
}


void Load_GIF(T_IO_Context * context)
{
  char filename[MAX_PATH_CHARACTERS];
  char signature[6];

  word * alphabet_stack;     // Pile de décodage d'une chaîne
  word * alphabet_prefix;  // Table des préfixes des codes
  word * alphabet_suffix;  // Table des suffixes des codes
  word   alphabet_free;     // Position libre dans l'alphabet
  word   alphabet_max;      // Nombre d'entrées possibles dans l'alphabet
  word   alphabet_stack_pos; // Position dans la pile de décodage d'un chaîne

  T_GIF_LSDB LSDB;
  T_GIF_IDB IDB;
  T_GIF_GCE GCE;

  word nb_colors;       // Nombre de couleurs dans l'image
  word color_index; // index de traitement d'une couleur
  byte size_to_read; // Nombre de données à lire      (divers)
  byte block_identifier;  // Code indicateur du type de bloc en cours
  word initial_nb_bits;   // Nb de bits au début du traitement LZW
  word special_case=0;       // Mémoire pour le cas spécial
  word old_code=0;       // Code précédent
  word byte_read;         // Sauvegarde du code en cours de lecture
  word value_clr;        // Valeur <=> Clear tables
  word value_eof;        // Valeur <=> End d'image
  long file_size;
  int number_LID; // Nombre d'images trouvées dans le fichier
  short current_layer = 0;

  /////////////////////////////////////////////////// FIN DES DECLARATIONS //


  number_LID=0;
  
  Get_full_filename(filename, context->File_name, context->File_directory);

  if ((GIF_file=fopen(filename, "rb")))
  {
    file_size=File_length_file(GIF_file);
    if ( (Read_bytes(GIF_file,signature,6)) &&
         ( (memcmp(signature,"GIF87a",6)==0) ||
           (memcmp(signature,"GIF89a",6)==0) ) )
    {

      // Allocation de mémoire pour les tables & piles de traitement:
      alphabet_stack   =(word *)malloc(4096*sizeof(word));
      alphabet_prefix=(word *)malloc(4096*sizeof(word));
      alphabet_suffix=(word *)malloc(4096*sizeof(word));

      if (Read_word_le(GIF_file,&(LSDB.Width))
      && Read_word_le(GIF_file,&(LSDB.Height))
      && Read_byte(GIF_file,&(LSDB.Resol))
      && Read_byte(GIF_file,&(LSDB.Backcol))
      && Read_byte(GIF_file,&(LSDB.Aspect))
        )
      {
        // Lecture du Logical Screen Descriptor Block réussie:

        Original_screen_X=LSDB.Width;
        Original_screen_Y=LSDB.Height;

        Pre_load(context, LSDB.Width,LSDB.Height,file_size,FORMAT_GIF,PIXEL_SIMPLE,0);
        context->Width=LSDB.Width;
        context->Height=LSDB.Height;

        // Palette globale dispo = (LSDB.Resol  and $80)
        // Profondeur de couleur =((LSDB.Resol  and $70) shr 4)+1
        // Nombre de bits/pixel  = (LSDB.Resol  and $07)+1
        // Ordre de Classement   = (LSDB.Aspect and $80)

        nb_colors=(1 << ((LSDB.Resol & 0x07)+1));
        initial_nb_bits=(LSDB.Resol & 0x07)+2;

        if (LSDB.Resol & 0x80)
        {
          // Palette globale dispo:

          if (Config.Clear_palette)
            memset(context->Palette,0,sizeof(T_Palette));

          //   On peut maintenant charger la nouvelle palette:
          if (!(LSDB.Aspect & 0x80))
            // Palette dans l'ordre:
            for(color_index=0;color_index<nb_colors;color_index++)
            {
              Read_byte(GIF_file,&(context->Palette[color_index].R));
              Read_byte(GIF_file,&(context->Palette[color_index].G));
              Read_byte(GIF_file,&(context->Palette[color_index].B));
            }
          else
          {
            // Palette triée par composantes:
            for (color_index=0;color_index<nb_colors;color_index++)
              Read_byte(GIF_file,&(context->Palette[color_index].R));
            for (color_index=0;color_index<nb_colors;color_index++)
              Read_byte(GIF_file,&(context->Palette[color_index].G));
            for (color_index=0;color_index<nb_colors;color_index++)
              Read_byte(GIF_file,&(context->Palette[color_index].B));
          }
        }

        // On lit un indicateur de block
        Read_byte(GIF_file,&block_identifier);
        while (block_identifier!=0x3B && !File_error)
        {
          switch (block_identifier)
          {
            case 0x21: // Bloc d'extension
            {
              byte function_code;
              // Lecture du code de fonction:
              Read_byte(GIF_file,&function_code);   
              // Lecture de la taille du bloc:
              Read_byte(GIF_file,&size_to_read);
              while (size_to_read!=0 && !File_error)
              {
                switch(function_code)
                {
                  case 0xFE: // Comment Block Extension
                    // On récupère le premier commentaire non-vide, 
                    // on jette les autres.
                    if (context->Comment[0]=='\0')
                    {
                      int nb_char_to_keep=Min(size_to_read,COMMENT_SIZE);
                      
                      Read_bytes(GIF_file,context->Comment,nb_char_to_keep);
                      context->Comment[nb_char_to_keep+1]='\0';
                      // Si le commentaire etait trop long, on fait avance-rapide
                      // sur la suite.
                      if (size_to_read>nb_char_to_keep)
                        fseek(GIF_file,size_to_read-nb_char_to_keep,SEEK_CUR);
                    }
                    break;
                  case 0xF9: // Graphics Control Extension
                    // Prévu pour la transparence
                    if ( Read_byte(GIF_file,&(GCE.Packed_fields))
                      && Read_word_le(GIF_file,&(GCE.Delay_time))
                      && Read_byte(GIF_file,&(GCE.Transparent_color)))
                    {
                      if (GCE.Packed_fields & 1)
                      {
                        if (number_LID == 0)
                          context->Background_transparent = 1;
                        context->Transparent_color= GCE.Transparent_color;
                      }
                      else
                      {
                        if (number_LID == 0)
                          context->Background_transparent = 0;
                        context->Transparent_color = 0; // Reset transparent color
                      }
                    
                    }
                    else
                      File_error=2;
                    break;

                  default:
                    // On saute le bloc:
                    fseek(GIF_file,size_to_read,SEEK_CUR);
                    break;
                }
                // Lecture de la taille du bloc suivant:
                Read_byte(GIF_file,&size_to_read);
              }
            }
            break;
            case 0x2C: // Local Image Descriptor
            {
              // Si on a deja lu une image, c'est une GIF animée ou bizarroide, on sort.
              if (number_LID!=0)
              {
                // This a second layer/frame, or more.
                // Attempt to add a layer to current image
                current_layer++;
                Set_layer(context, current_layer);
              }
              number_LID++;
              
              // lecture de 10 derniers octets
              if ( Read_word_le(GIF_file,&(IDB.Pos_X))
                && Read_word_le(GIF_file,&(IDB.Pos_Y))
                && Read_word_le(GIF_file,&(IDB.Image_width))
                && Read_word_le(GIF_file,&(IDB.Image_height))
                && Read_byte(GIF_file,&(IDB.Indicator))
                && Read_byte(GIF_file,&(IDB.Nb_bits_pixel))
                && IDB.Image_width && IDB.Image_height)
              {
    
                // Palette locale dispo = (IDB.Indicator and $80)
                // Image entrelacée     = (IDB.Indicator and $40)
                // Ordre de classement  = (IDB.Indicator and $20)
                // Nombre de bits/pixel = (IDB.Indicator and $07)+1 (si palette locale dispo)
    
                if (IDB.Indicator & 0x80)
                {
                  // Palette locale dispo
    
                  nb_colors=(1 << ((IDB.Indicator & 0x07)+1));
                  initial_nb_bits=(IDB.Indicator & 0x07)+2;
    
                  if (!(IDB.Indicator & 0x40))
                    // Palette dans l'ordre:
                    for(color_index=0;color_index<nb_colors;color_index++)
                    {   
                      Read_byte(GIF_file,&(context->Palette[color_index].R));
                      Read_byte(GIF_file,&(context->Palette[color_index].G));
                      Read_byte(GIF_file,&(context->Palette[color_index].B));
                    }
                  else
                  {
                    // Palette triée par composantes:
                    for (color_index=0;color_index<nb_colors;color_index++)
                      Read_byte(GIF_file,&(context->Palette[color_index].R));
                    for (color_index=0;color_index<nb_colors;color_index++)
                      Read_byte(GIF_file,&(context->Palette[color_index].G));
                    for (color_index=0;color_index<nb_colors;color_index++)
                      Read_byte(GIF_file,&(context->Palette[color_index].B));
                  }
                }
    
                Palette_loaded(context);
    
                value_clr   =nb_colors+0;
                value_eof   =nb_colors+1;
                alphabet_free=nb_colors+2;
                GIF_nb_bits  =initial_nb_bits;
                alphabet_max      =((1 <<  GIF_nb_bits)-1);
                GIF_interlaced    =(IDB.Indicator & 0x40);
                GIF_pass         =0;
    
                /*Init_lecture();*/
    
                File_error=0;
                GIF_finished_interlaced_image=0;
    
                //////////////////////////////////////////// DECOMPRESSION LZW //
    
                GIF_pos_X=0;
                GIF_pos_Y=0;
                alphabet_stack_pos=0;
                GIF_last_byte    =0;
                GIF_remainder_bits    =0;
                GIF_remainder_byte    =0;

                while ( (GIF_get_next_code()!=value_eof) && (!File_error) )
                {
                  if (GIF_current_code<=alphabet_free)
                  {
                    if (GIF_current_code!=value_clr)
                    {
                      if (alphabet_free==(byte_read=GIF_current_code))
                      {
                        GIF_current_code=old_code;
                        alphabet_stack[alphabet_stack_pos++]=special_case;
                      }
    
                      while (GIF_current_code>value_clr)
                      {
                        alphabet_stack[alphabet_stack_pos++]=alphabet_suffix[GIF_current_code];
                        GIF_current_code=alphabet_prefix[GIF_current_code];
                      }
    
                      special_case=alphabet_stack[alphabet_stack_pos++]=GIF_current_code;
    
                      do
                        GIF_new_pixel(context, &IDB, alphabet_stack[--alphabet_stack_pos]);
                      while (alphabet_stack_pos!=0);
    
                      alphabet_prefix[alphabet_free  ]=old_code;
                      alphabet_suffix[alphabet_free++]=GIF_current_code;
                      old_code=byte_read;
    
                      if (alphabet_free>alphabet_max)
                      {
                        if (GIF_nb_bits<12)
                          alphabet_max      =((1 << (++GIF_nb_bits))-1);
                      }
                    }
                    else // Code Clear rencontré
                    {
                      GIF_nb_bits       =initial_nb_bits;
                      alphabet_max      =((1 <<  GIF_nb_bits)-1);
                      alphabet_free     =nb_colors+2;
                      special_case       =GIF_get_next_code();
                      old_code       =GIF_current_code;
                      GIF_new_pixel(context, &IDB, GIF_current_code);
                    }
                  }
                  else
                    File_error=2;
                } // Code End-Of-Information ou erreur de fichier rencontré
    
                /*Close_lecture();*/
    
                if (File_error>=0)
                if ( /* (GIF_pos_X!=0) || */
                     ( ( (!GIF_interlaced) && (GIF_pos_Y!=IDB.Image_height) ) ||
                       (  (GIF_interlaced) && (!GIF_finished_interlaced_image) )
                     ) )
                  File_error=2;
              } // Le fichier contenait un IDB
              else
                File_error=2;
            }
            default:
            break;
          }
          // Lecture du code de fonction suivant:
          Read_byte(GIF_file,&block_identifier);
        }
      } // Le fichier contenait un LSDB
      else
        File_error=1;

      // Libération de la mémoire utilisée par les tables & piles de traitement:
      free(alphabet_suffix);
      free(alphabet_prefix);
      free(alphabet_stack);
      alphabet_suffix = alphabet_prefix = alphabet_stack = NULL;
    } // Le fichier contenait au moins la signature GIF87a ou GIF89a
    else
      File_error=1;

    fclose(GIF_file);

  } // Le fichier était ouvrable
  else
    File_error=1;
}


// -- Sauver un fichier au format GIF ---------------------------------------

  int  GIF_stop;         // "On peut arrêter la sauvegarde du fichier"
  byte GIF_buffer[256];   // buffer d'écriture de bloc de données compilées

  // -- Vider le buffer GIF dans le buffer KM --

  void GIF_empty_buffer(void)
  {
    word index;

    if (GIF_remainder_byte)
    {
      GIF_buffer[0]=GIF_remainder_byte;

      for (index=0;index<=GIF_remainder_byte;index++)
        Write_one_byte(GIF_file,GIF_buffer[index]);

      GIF_remainder_byte=0;
    }
  }

  // -- Ecrit un code à GIF_nb_bits --

  void GIF_set_code(word Code)
  {
    word nb_bits_to_process=GIF_nb_bits;
    word nb_bits_processed  =0;
    word current_nb_bits;

    while (nb_bits_to_process)
    {
      current_nb_bits=(nb_bits_to_process<=(8-GIF_remainder_bits))?nb_bits_to_process:(8-GIF_remainder_bits);

      GIF_last_byte|=(Code & ((1<<current_nb_bits)-1))<<GIF_remainder_bits;
      Code>>=current_nb_bits;
      GIF_remainder_bits    +=current_nb_bits;
      nb_bits_processed  +=current_nb_bits;
      nb_bits_to_process-=current_nb_bits;

      if (GIF_remainder_bits==8) // Il ne reste plus de bits à coder sur l'octet courant
      {
        // Ecrire l'octet à balancer:
        GIF_buffer[++GIF_remainder_byte]=GIF_last_byte;

        // Si on a atteint la fin du bloc de Raster Data
        if (GIF_remainder_byte==255)
          // On doit vider le buffer qui est maintenant plein
          GIF_empty_buffer();

        GIF_last_byte=0;
        GIF_remainder_bits=0;
      }
    }
  }


  // -- Lire le pixel suivant --

  byte GIF_next_pixel(T_IO_Context *context, T_GIF_IDB *idb)
  {
    byte temp;

    temp=Get_pixel(context, GIF_pos_X,GIF_pos_Y);

    if (++GIF_pos_X>=idb->Image_width)
    {
      GIF_pos_X=0;
      if (++GIF_pos_Y>=idb->Image_height)
        GIF_stop=1;
    }

    return temp;
  }



void Save_GIF(T_IO_Context * context)
{
  char filename[MAX_PATH_CHARACTERS];

  word * alphabet_prefix;  // Table des préfixes des codes
  word * alphabet_suffix;  // Table des suffixes des codes
  word * alphabet_daughter;    // Table des chaînes filles (plus longues)
  word * alphabet_sister;    // Table des chaînes soeurs (même longueur)
  word   alphabet_free;     // Position libre dans l'alphabet
  word   alphabet_max;      // Nombre d'entrées possibles dans l'alphabet
  word   start;            // Code précédent (sert au linkage des chaînes)
  int    descend;          // Booléen "On vient de descendre"

  T_GIF_LSDB LSDB;
  T_GIF_IDB IDB;


  byte block_identifier;  // Code indicateur du type de bloc en cours
  word current_string;   // Code de la chaîne en cours de traitement
  byte current_char;         // Caractère à coder
  word index;            // index de recherche de chaîne
  short current_layer;

  /////////////////////////////////////////////////// FIN DES DECLARATIONS //
  
  File_error=0;
  
  Get_full_filename(filename, context->File_name, context->File_directory);

  if ((GIF_file=fopen(filename,"wb")))
  {
    // On écrit la signature du fichier
    if (Write_bytes(GIF_file,"GIF89a",6))
    {
      // La signature du fichier a été correctement écrite.

      // Allocation de mémoire pour les tables
      alphabet_prefix=(word *)malloc(4096*sizeof(word));
      alphabet_suffix=(word *)malloc(4096*sizeof(word));
      alphabet_daughter  =(word *)malloc(4096*sizeof(word));
      alphabet_sister  =(word *)malloc(4096*sizeof(word));

      // On initialise le LSDB du fichier
      if (Config.Screen_size_in_GIF)
      {
        LSDB.Width=Screen_width;
        LSDB.Height=Screen_height;
      }
      else
      {
        LSDB.Width=context->Width;
        LSDB.Height=context->Height;
      }
      LSDB.Resol  =0x97;          // Image en 256 couleurs, avec une palette
      LSDB.Backcol=context->Transparent_color;
      LSDB.Aspect =0;             // Palette normale

      // On sauve le LSDB dans le fichier

      if (Write_word_le(GIF_file,LSDB.Width) &&
          Write_word_le(GIF_file,LSDB.Height) &&
          Write_byte(GIF_file,LSDB.Resol) &&
          Write_byte(GIF_file,LSDB.Backcol) &&
          Write_byte(GIF_file,LSDB.Aspect) )
      {
        // Le LSDB a été correctement écrit.

        // On sauve la palette
        if (Write_bytes(GIF_file,context->Palette,768))
        {
          // La palette a été correctement écrite.

          //   Le jour où on se servira des blocks d'extensions pour placer
          // des commentaires, on le fera ici.

          // Ecriture de la transparence
          //Write_bytes(GIF_file,"\x21\xF9\x04\x01\x00\x00\xNN\x00",8);

          // "Netscape" animation extension
          //  Write_bytes(GIF_file,"\x21\xFF\x0BNETSCAPE2.0\x03\xLL\xSS\xSS\x00",19);
          // LL : 01 to loop
          // SSSS : number of loops
            
          // Ecriture du commentaire
          if (context->Comment[0])
          {
            Write_bytes(GIF_file,"\x21\xFE",2);
            Write_byte(GIF_file,strlen(context->Comment));
            Write_bytes(GIF_file,context->Comment,strlen(context->Comment)+1);
          }
          
          // Loop on all layers
          for (current_layer=0; 
            current_layer < context->Nb_layers && !File_error;
            current_layer++)
          {
            // Write a Graphic Control Extension
            char GCE_block[] = "\x21\xF9\x04\x04\x05\x00\x00\x00";
            // 'Default' values:
            //    Disposal method "Do not dispose"
            //    Duration 5/100s (minimum viable value for current web browsers)
            
            if (current_layer > 0 || context->Background_transparent)
              GCE_block[3] |= 1; // Transparent color flag
            GCE_block[6] = context->Transparent_color;
            
            Set_layer(context, current_layer);
            
            if (current_layer == context->Nb_layers -1)
            {
              // "Infinite" delay for last frame
              GCE_block[4] = 255;
              GCE_block[5] = 255;
            }
            if (Write_bytes(GIF_file,GCE_block,8))
            {
            
              // On va écrire un block indicateur d'IDB et l'IDB du fichier
              block_identifier=0x2C;
              IDB.Pos_X=0;
              IDB.Pos_Y=0;
              IDB.Image_width=context->Width;
              IDB.Image_height=context->Height;
              IDB.Indicator=0x07;    // Image non entrelacée, pas de palette locale.
              IDB.Nb_bits_pixel=8; // Image 256 couleurs;
    
              if ( Write_byte(GIF_file,block_identifier) &&
                   Write_word_le(GIF_file,IDB.Pos_X) &&
                   Write_word_le(GIF_file,IDB.Pos_Y) &&
                   Write_word_le(GIF_file,IDB.Image_width) &&
                   Write_word_le(GIF_file,IDB.Image_height) &&
                   Write_byte(GIF_file,IDB.Indicator) &&
                   Write_byte(GIF_file,IDB.Nb_bits_pixel))
              {
                //   Le block indicateur d'IDB et l'IDB ont étés correctements
                // écrits.
    
                Init_write_buffer();
                GIF_pos_X=0;
                GIF_pos_Y=0;
                GIF_last_byte=0;
                GIF_remainder_bits=0;
                GIF_remainder_byte=0;
    
                index=4096;
                File_error=0;
                GIF_stop=0;
    
                // Réintialisation de la table:
                alphabet_free=258;
                GIF_nb_bits  =9;
                alphabet_max =511;
                GIF_set_code(256);
                for (start=0;start<4096;start++)
                {
                  alphabet_daughter[start]=4096;
                  alphabet_sister[start]=4096;
                }
    
                ////////////////////////////////////////////// COMPRESSION LZW //
    
                start=current_string=GIF_next_pixel(context, &IDB);
                descend=1;
    
                do
                {
                  current_char=GIF_next_pixel(context, &IDB);
    
                  //   On regarde si dans la table on aurait pas une chaîne
                  // équivalente à current_string+Caractere
    
                  while ( (index<alphabet_free) &&
                          ( (current_string!=alphabet_prefix[index]) ||
                            (current_char      !=alphabet_suffix[index]) ) )
                  {
                    descend=0;
                    start=index;
                    index=alphabet_sister[index];
                  }
    
                  if (index<alphabet_free)
                  {
                    //   On sait ici que la current_string+Caractere se trouve
                    // en position index dans les tables.
    
                    descend=1;
                    start=current_string=index;
                    index=alphabet_daughter[index];
                  }
                  else
                  {
                    // On fait la jonction entre la current_string et l'actuelle
                    if (descend)
                      alphabet_daughter[start]=alphabet_free;
                    else
                      alphabet_sister[start]=alphabet_free;
    
                    // On rajoute la chaîne current_string+Caractere à la table
                    alphabet_prefix[alphabet_free  ]=current_string;
                    alphabet_suffix[alphabet_free++]=current_char;
    
                    // On écrit le code dans le fichier
                    GIF_set_code(current_string);
    
                    if (alphabet_free>0xFFF)
                    {
                      // Réintialisation de la table:
                      GIF_set_code(256);
                      alphabet_free=258;
                      GIF_nb_bits  =9;
                      alphabet_max =511;
                      for (start=0;start<4096;start++)
                      {
                        alphabet_daughter[start]=4096;
                        alphabet_sister[start]=4096;
                      }
                    }
                    else if (alphabet_free>alphabet_max+1)
                    {
                      // On augmente le nb de bits
    
                      GIF_nb_bits++;
                      alphabet_max=(1<<GIF_nb_bits)-1;
                    }
    
                    // On initialise la current_string et le reste pour la suite
                    index=alphabet_daughter[current_char];
                    start=current_string=current_char;
                    descend=1;
                  }
                }
                while ((!GIF_stop) && (!File_error));
    
                if (!File_error)
                {
                  // On écrit le code dans le fichier
                  GIF_set_code(current_string); // Dernière portion d'image
    
                  //   Cette dernière portion ne devrait pas poser de problèmes
                  // du côté GIF_nb_bits puisque pour que GIF_nb_bits change de
                  // valeur, il faudrait que la table de chaîne soit remplie or
                  // c'est impossible puisqu'on traite une chaîne qui se trouve
                  // déjà dans la table, et qu'elle n'a rien d'inédit. Donc on
                  // ne devrait pas avoir à changer de taille, mais je laisse
                  // quand même en remarque tout ça, au cas où il subsisterait
                  // des problèmes dans certains cas exceptionnels.
                  //
                  // Note: de toutes façons, ces lignes en commentaires ont étés
                  //      écrites par copier/coller du temps où la sauvegarde du
                  //      GIF déconnait. Il y a donc fort à parier qu'elles ne
                  //      sont pas correctes.
    
                  /*
                  if (current_string==alphabet_max)
                  {
                    if (alphabet_max==0xFFF)
                    {
                      // On balargue un Clear Code
                      GIF_set_code(256);
    
                      // On réinitialise les données LZW
                      alphabet_free=258;
                      GIF_nb_bits  =9;
                      alphabet_max =511;
                    }
                    else
                    {
                      GIF_nb_bits++;
                      alphabet_max=(1<<GIF_nb_bits)-1;
                    }
                  }
                  */
    
                  GIF_set_code(257);             // Code de End d'image
                  if (GIF_remainder_bits!=0)
                    GIF_set_code(0);             // Code bidon permettant de s'assurer que tous les bits du dernier code aient bien étés inscris dans le buffer GIF
                  GIF_empty_buffer();         // On envoie les dernières données du buffer GIF dans le buffer KM
                  End_write(GIF_file);   // On envoie les dernières données du buffer KM  dans le fichier
    
                  // On écrit un \0
                  if (! Write_byte(GIF_file,'\x00'))
                    File_error=1;
                  
                  }
      
                } // On a pu écrire l'IDB
              else
                File_error=1;
            }
            else
              File_error=1;
          }
          
          // After writing all layers
          if (!File_error)
          {
            // On écrit un GIF TERMINATOR, exigé par SVGA et SEA.
            if (! Write_byte(GIF_file,'\x3B'))
              File_error=1;
          }

        } // On a pu écrire la palette
        else
          File_error=1;

      } // On a pu écrire le LSDB
      else
        File_error=1;

      // Libération de la mémoire utilisée par les tables
      free(alphabet_sister);
      free(alphabet_daughter);
      free(alphabet_suffix);
      free(alphabet_prefix);

    } // On a pu écrire la signature du fichier
    else
      File_error=1;

    fclose(GIF_file);
    if (File_error)
      remove(filename);

  } // On a pu ouvrir le fichier en écriture
  else
    File_error=1;

}



//////////////////////////////////// PCX ////////////////////////////////////
#pragma pack(1)
typedef struct
  {
    byte Manufacturer;       // |_ Il font chier ces cons! Ils auraient pu
    byte Version;            // |  mettre une vraie signature!
    byte Compression;        // L'image est-elle compressée?
    byte Depth;              // Nombre de bits pour coder un pixel (inutile puisqu'on se sert de Plane)
    word X_min;              // |_ Coin haut-gauche   |
    word Y_min;              // |  de l'image         |_ (Crétin!)
    word X_max;              // |_ Coin bas-droit     |
    word Y_max;              // |  de l'image         |
    word X_dpi;              // |_ Densité de |_ (Presque inutile parce que
    word Y_dpi;              // |  l'image    |  aucun moniteur n'est pareil!)
    byte Palette_16c[48];    // Palette 16 coul (inutile pour 256c) (débile!)
    byte Reserved;           // Ca me plait ça aussi!
    byte Plane;              // 4 => 16c , 1 => 256c , ...
    word Bytes_per_plane_line;// Doit toujours être pair
    word Palette_info;       // 1 => color , 2 => Gris (ignoré à partir de la version 4)
    word Screen_X;           // |_ Dimensions de
    word Screen_Y;           // |  l'écran d'origine
    byte Filler[54];         // Ca... J'adore!
  } T_PCX_Header;
#pragma pack()

T_PCX_Header PCX_header;

// -- Tester si un fichier est au format PCX --------------------------------

void Test_PCX(T_IO_Context * context)
{
  char filename[MAX_PATH_CHARACTERS];
  FILE *file;

  File_error=0;
  Get_full_filename(filename, context->File_name, context->File_directory);

  if ((file=fopen(filename, "rb")))
  {
    if (Read_byte(file,&(PCX_header.Manufacturer)) &&
        Read_byte(file,&(PCX_header.Version)) &&
        Read_byte(file,&(PCX_header.Compression)) &&
        Read_byte(file,&(PCX_header.Depth)) &&
        Read_word_le(file,&(PCX_header.X_min)) &&
        Read_word_le(file,&(PCX_header.Y_min)) &&
        Read_word_le(file,&(PCX_header.X_max)) &&
        Read_word_le(file,&(PCX_header.Y_max)) &&
        Read_word_le(file,&(PCX_header.X_dpi)) &&
        Read_word_le(file,&(PCX_header.Y_dpi)) &&
        Read_bytes(file,&(PCX_header.Palette_16c),48) &&        
        Read_byte(file,&(PCX_header.Reserved)) &&
        Read_byte(file,&(PCX_header.Plane)) &&
        Read_word_le(file,&(PCX_header.Bytes_per_plane_line)) &&
        Read_word_le(file,&(PCX_header.Palette_info)) &&
        Read_word_le(file,&(PCX_header.Screen_X)) &&
        Read_word_le(file,&(PCX_header.Screen_Y)) &&
        Read_bytes(file,&(PCX_header.Filler),54) )
    {
    
      //   Vu que ce header a une signature de merde et peu significative, il
      // va falloir que je teste différentes petites valeurs dont je connais
      // l'intervalle. Grrr!
      if ( (PCX_header.Manufacturer!=10)
        || (PCX_header.Compression>1)
        || ( (PCX_header.Depth!=1) && (PCX_header.Depth!=2) && (PCX_header.Depth!=4) && (PCX_header.Depth!=8) )
        || ( (PCX_header.Plane!=1) && (PCX_header.Plane!=2) && (PCX_header.Plane!=4) && (PCX_header.Plane!=8) && (PCX_header.Plane!=3) )
        || (PCX_header.X_max<PCX_header.X_min)
        || (PCX_header.Y_max<PCX_header.Y_min)
        || (PCX_header.Bytes_per_plane_line&1) )
        File_error=1;
    }
    else
      File_error=1;

    fclose(file);
  }
}


// -- Lire un fichier au format PCX -----------------------------------------

  // -- Afficher une ligne PCX codée sur 1 seul plan avec moins de 256 c. --
  void Draw_PCX_line(T_IO_Context *context, short y_pos, byte depth)
  {
    short x_pos;
    byte  color;
    byte  reduction=8/depth;
    byte  byte_mask=(1<<depth)-1;
    byte  reduction_minus_one=reduction-1;

    for (x_pos=0; x_pos<context->Width; x_pos++)
    {
      color=(LBM_buffer[x_pos/reduction]>>((reduction_minus_one-(x_pos%reduction))*depth)) & byte_mask;
      Set_pixel(context, x_pos,y_pos,color);
    }
  }

void Load_PCX(T_IO_Context * context)
{
  char filename[MAX_PATH_CHARACTERS];
  FILE *file;
  
  short line_size;
  short real_line_size; // width de l'image corrigée
  short width_read;
  short x_pos;
  short y_pos;
  byte  byte1;
  byte  byte2;
  byte  index;
  dword nb_colors;
  long  file_size;
  byte  palette_CGA[9]={ 84,252,252,  252, 84,252,  252,252,252};

  long  position;
  long  image_size;
  byte * buffer;

  Get_full_filename(filename, context->File_name, context->File_directory);

  File_error=0;

  if ((file=fopen(filename, "rb")))
  {
    file_size=File_length_file(file);   
    if (Read_byte(file,&(PCX_header.Manufacturer)) &&
        Read_byte(file,&(PCX_header.Version)) &&
        Read_byte(file,&(PCX_header.Compression)) &&
        Read_byte(file,&(PCX_header.Depth)) &&
        Read_word_le(file,&(PCX_header.X_min)) &&
        Read_word_le(file,&(PCX_header.Y_min)) &&
        Read_word_le(file,&(PCX_header.X_max)) &&
        Read_word_le(file,&(PCX_header.Y_max)) &&
        Read_word_le(file,&(PCX_header.X_dpi)) &&
        Read_word_le(file,&(PCX_header.Y_dpi)) &&
        Read_bytes(file,&(PCX_header.Palette_16c),48) &&        
        Read_byte(file,&(PCX_header.Reserved)) &&
        Read_byte(file,&(PCX_header.Plane)) &&
        Read_word_le(file,&(PCX_header.Bytes_per_plane_line)) &&
        Read_word_le(file,&(PCX_header.Palette_info)) &&
        Read_word_le(file,&(PCX_header.Screen_X)) &&
        Read_word_le(file,&(PCX_header.Screen_Y)) &&
        Read_bytes(file,&(PCX_header.Filler),54) )
    {
      
      context->Width=PCX_header.X_max-PCX_header.X_min+1;
      context->Height=PCX_header.Y_max-PCX_header.Y_min+1;

      Original_screen_X=PCX_header.Screen_X;
      Original_screen_Y=PCX_header.Screen_Y;

      if (PCX_header.Plane!=3)
      {
        Pre_load(context, context->Width,context->Height,file_size,FORMAT_PCX,PIXEL_SIMPLE,0);
        if (File_error==0)
        {
          // On prépare la palette à accueillir les valeurs du fichier PCX
          if (Config.Clear_palette)
            memset(context->Palette,0,sizeof(T_Palette));
          nb_colors=(dword)(1<<PCX_header.Plane)<<(PCX_header.Depth-1);

          if (nb_colors>4)
            memcpy(context->Palette,PCX_header.Palette_16c,48);
          else
          {
            context->Palette[1].R=0;
            context->Palette[1].G=0;
            context->Palette[1].B=0;
            byte1=PCX_header.Palette_16c[3]>>5;
            if (nb_colors==4)
            { // Pal. CGA "alakon" (du Turc Allahkoum qui signifie "à la con" :))
              memcpy(context->Palette+1,palette_CGA,9);
              if (!(byte1&2))
              {
                context->Palette[1].B=84;
                context->Palette[2].B=84;
                context->Palette[3].B=84;
              }
            } // Palette monochrome (on va dire que c'est du N&B)
            else
            {
              context->Palette[1].R=252;
              context->Palette[1].G=252;
              context->Palette[1].B=252;
            }
          }

          //   On se positionne à la fin du fichier - 769 octets pour voir s'il y
          // a une palette.
          if ( (PCX_header.Depth==8) && (PCX_header.Version>=5) && (file_size>(256*3)) )
          {
            fseek(file,file_size-((256*3)+1),SEEK_SET);
            // On regarde s'il y a une palette après les données de l'image
            if (Read_byte(file,&byte1))
              if (byte1==12) // Lire la palette si c'est une image en 256 couleurs
              {
                int index;
                // On lit la palette 256c que ces crétins ont foutue à la fin du fichier
                for(index=0;index<256;index++)
                  if ( ! Read_byte(file,&(context->Palette[index].R))
                   || ! Read_byte(file,&(context->Palette[index].G))
                   || ! Read_byte(file,&(context->Palette[index].B)) )
                  {
                    File_error=2;
                    DEBUG("ERROR READING PCX PALETTE !",index);
                    break;
                  }
              }
          }
          Palette_loaded(context);

          //   Maintenant qu'on a lu la palette que ces crétins sont allés foutre
          // à la fin, on retourne juste après le header pour lire l'image.
          fseek(file,128,SEEK_SET);
          if (!File_error)
          {
            line_size=PCX_header.Bytes_per_plane_line*PCX_header.Plane;
            real_line_size=(short)PCX_header.Bytes_per_plane_line<<3;
            //   On se sert de données LBM car le dessin de ligne en moins de 256
            // couleurs se fait comme avec la structure ILBM.
            Image_HAM=0;
            HBPm1=PCX_header.Plane-1;
            LBM_buffer=(byte *)malloc(line_size);

            // Chargement de l'image
            if (PCX_header.Compression)  // Image compressée
            {
              /*Init_lecture();*/
  
              image_size=(long)PCX_header.Bytes_per_plane_line*context->Height;

              if (PCX_header.Depth==8) // 256 couleurs (1 plan)
              {
                for (position=0; ((position<image_size) && (!File_error));)
                {
                  // Lecture et décompression de la ligne
                  if(Read_byte(file,&byte1) !=1) File_error=2;
                  if (!File_error)
                  {
                    if ((byte1&0xC0)==0xC0)
                    {
                      byte1-=0xC0;               // facteur de répétition
                      if(Read_byte(file,&byte2)!=1) File_error = 2; // octet à répéter
                      if (!File_error)
                      {
                        for (index=0; index<byte1; index++,position++)
                          if (position<image_size)
                            Set_pixel(context, position%line_size,
                                                position/line_size,
                                                byte2);
                          else
                            File_error=2;
                      }
                    }
                    else
                    {
                      Set_pixel(context, position%line_size,
                                          position/line_size,
                                          byte1);
                      position++;
                    }
                  }
                }
              }
              else                 // couleurs rangées par plans
              {
                for (y_pos=0; ((y_pos<context->Height) && (!File_error)); y_pos++)
                {
                  for (x_pos=0; ((x_pos<line_size) && (!File_error)); )
                  {
                    if(Read_byte(file,&byte1)!=1) File_error = 2;
                    if (!File_error)
                    {
                      if ((byte1&0xC0)==0xC0)
                      {
                        byte1-=0xC0;               // facteur de répétition
                        if(Read_byte(file,&byte2)!=1) File_error=2; // octet à répéter
                        if (!File_error)
                        {
                          for (index=0; index<byte1; index++)
                            if (x_pos<line_size)
                              LBM_buffer[x_pos++]=byte2;
                            else
                              File_error=2;
                        }
                        else
                          Set_file_error(2);
                      }
                      else
                        LBM_buffer[x_pos++]=byte1;
                    }
                  }
                  // Affichage de la ligne par plan du buffer
                  if (PCX_header.Depth==1)
                    Draw_ILBM_line(context, y_pos,real_line_size);
                  else
                    Draw_PCX_line(context, y_pos,PCX_header.Depth);
                }
              }

              /*Close_lecture();*/
            }
            else                     // Image non compressée
            {
              for (y_pos=0;(y_pos<context->Height) && (!File_error);y_pos++)
              {
                if ((width_read=Read_bytes(file,LBM_buffer,line_size)))
                {
                  if (PCX_header.Plane==1)
                    for (x_pos=0; x_pos<context->Width;x_pos++)
                      Set_pixel(context, x_pos,y_pos,LBM_buffer[x_pos]);
                  else
                  {
                    if (PCX_header.Depth==1)
                      Draw_ILBM_line(context, y_pos,real_line_size);
                    else
                      Draw_PCX_line(context, y_pos,PCX_header.Depth);
                  }
                }
                else
                  File_error=2;
              }
            }

            free(LBM_buffer);
            LBM_buffer = NULL;
          }
        }
      }
      else
      {
        // Image 24 bits!!!

        Pre_load(context,context->Width,context->Height,file_size,FORMAT_PCX,PIXEL_SIMPLE,1);

        if (File_error==0)
        {
          line_size=PCX_header.Bytes_per_plane_line*3;
          buffer=(byte *)malloc(line_size);

          if (!PCX_header.Compression)
          {
            for (y_pos=0;(y_pos<context->Height) && (!File_error);y_pos++)
            {
              if (Read_bytes(file,buffer,line_size))
              {
                for (x_pos=0; x_pos<context->Width; x_pos++)
                  Set_pixel_24b(context, x_pos,y_pos,buffer[x_pos+(PCX_header.Bytes_per_plane_line*0)],buffer[x_pos+(PCX_header.Bytes_per_plane_line*1)],buffer[x_pos+(PCX_header.Bytes_per_plane_line*2)]);
              }
              else
                File_error=2;
            }
          }
          else
          {
            /*Init_lecture();*/

            for (y_pos=0,position=0;(y_pos<context->Height) && (!File_error);)
            {
              // Lecture et décompression de la ligne
              if(Read_byte(file,&byte1)!=1) File_error=2;
              if (!File_error)
              {
                if ((byte1 & 0xC0)==0xC0)
                {
                  byte1-=0xC0;               // facteur de répétition
                  if(Read_byte(file,&byte2)!=1) File_error=2; // octet à répéter
                  if (!File_error)
                  {
                    for (index=0; (index<byte1) && (!File_error); index++)
                    {
                      buffer[position++]=byte2;
                      if (position>=line_size)
                      {
                        for (x_pos=0; x_pos<context->Width; x_pos++)
                          Set_pixel_24b(context, x_pos,y_pos,buffer[x_pos+(PCX_header.Bytes_per_plane_line*0)],buffer[x_pos+(PCX_header.Bytes_per_plane_line*1)],buffer[x_pos+(PCX_header.Bytes_per_plane_line*2)]);
                        y_pos++;
                        position=0;
                      }
                    }
                  }
                }
                else
                {
                  buffer[position++]=byte1;
                  if (position>=line_size)
                  {
                    for (x_pos=0; x_pos<context->Width; x_pos++)
                      Set_pixel_24b(context, x_pos,y_pos,buffer[x_pos+(PCX_header.Bytes_per_plane_line*0)],buffer[x_pos+(PCX_header.Bytes_per_plane_line*1)],buffer[x_pos+(PCX_header.Bytes_per_plane_line*2)]);
                    y_pos++;
                    position=0;
                  }
                }
              }
            }
            if (position!=0)
              File_error=2;

            /*Close_lecture();*/
          }
          free(buffer);
          buffer = NULL;
        }
      }
    }
    else
    {
      File_error=1;
    }

    fclose(file);
  }
  else
    File_error=1;
}


// -- Ecrire un fichier au format PCX ---------------------------------------

void Save_PCX(T_IO_Context * context)
{
  char filename[MAX_PATH_CHARACTERS];
  FILE *file;

  short line_size;
  short x_pos;
  short y_pos;
  byte  counter;
  byte  last_pixel;
  byte  pixel_read;



  Get_full_filename(filename, context->File_name, context->File_directory);

  File_error=0;

  if ((file=fopen(filename,"wb")))
  {

    PCX_header.Manufacturer=10;
    PCX_header.Version=5;
    PCX_header.Compression=1;
    PCX_header.Depth=8;
    PCX_header.X_min=0;
    PCX_header.Y_min=0;
    PCX_header.X_max=context->Width-1;
    PCX_header.Y_max=context->Height-1;
    PCX_header.X_dpi=0;
    PCX_header.Y_dpi=0;
    memcpy(PCX_header.Palette_16c,context->Palette,48);
    PCX_header.Reserved=0;
    PCX_header.Plane=1;
    PCX_header.Bytes_per_plane_line=(context->Width&1)?context->Width+1:context->Width;
    PCX_header.Palette_info=1;
    PCX_header.Screen_X=Screen_width;
    PCX_header.Screen_Y=Screen_height;
    memset(PCX_header.Filler,0,54);

    if (Write_bytes(file,&(PCX_header.Manufacturer),1) &&
        Write_bytes(file,&(PCX_header.Version),1) &&
        Write_bytes(file,&(PCX_header.Compression),1) &&
        Write_bytes(file,&(PCX_header.Depth),1) &&
        Write_word_le(file,PCX_header.X_min) &&
        Write_word_le(file,PCX_header.Y_min) &&
        Write_word_le(file,PCX_header.X_max) &&
        Write_word_le(file,PCX_header.Y_max) &&
        Write_word_le(file,PCX_header.X_dpi) &&
        Write_word_le(file,PCX_header.Y_dpi) &&
        Write_bytes(file,&(PCX_header.Palette_16c),sizeof(PCX_header.Palette_16c)) &&        
        Write_bytes(file,&(PCX_header.Reserved),1) &&
        Write_bytes(file,&(PCX_header.Plane),1) &&
        Write_word_le(file,PCX_header.Bytes_per_plane_line) &&
        Write_word_le(file,PCX_header.Palette_info) &&
        Write_word_le(file,PCX_header.Screen_X) &&
        Write_word_le(file,PCX_header.Screen_Y) &&
        Write_bytes(file,&(PCX_header.Filler),sizeof(PCX_header.Filler)) )
    {
      line_size=PCX_header.Bytes_per_plane_line*PCX_header.Plane;
     
      Init_write_buffer();
     
      for (y_pos=0; ((y_pos<context->Height) && (!File_error)); y_pos++)
      {
        pixel_read=Get_pixel(context, 0,y_pos);
     
        // Compression et écriture de la ligne
        for (x_pos=0; ((x_pos<line_size) && (!File_error)); )
        {
          x_pos++;
          last_pixel=pixel_read;
          pixel_read=Get_pixel(context, x_pos,y_pos);
          counter=1;
          while ( (counter<63) && (x_pos<line_size) && (pixel_read==last_pixel) )
          {
            counter++;
            x_pos++;
            pixel_read=Get_pixel(context, x_pos,y_pos);
          }
      
          if ( (counter>1) || (last_pixel>=0xC0) )
            Write_one_byte(file,counter|0xC0);
          Write_one_byte(file,last_pixel);
      
        }
      }
      
      // Ecriture de l'octet (12) indiquant que la palette arrive
      if (!File_error)
        Write_one_byte(file,12);
      
      End_write(file);
      
      // Ecriture de la palette
      if (!File_error)
      {
        if (! Write_bytes(file,context->Palette,sizeof(T_Palette)))
          File_error=1;
      }
    }
    else
      File_error=1;
       
    fclose(file);
       
    if (File_error)
      remove(filename);
       
  }    
  else 
    File_error=1;
}      


//////////////////////////////////// SCx ////////////////////////////////////
typedef struct
{
  byte Filler1[4];
  word Width;
  word Height;
  byte Filler2;
  byte Planes;
} T_SCx_Header;

// -- Tester si un fichier est au format SCx --------------------------------
void Test_SCx(T_IO_Context * context)
{
  FILE *file;              // Fichier du fichier
  char filename[MAX_PATH_CHARACTERS]; // Nom complet du fichier
  //byte Signature[3];
  T_SCx_Header SCx_header;


  Get_full_filename(filename, context->File_name, context->File_directory);

  File_error=1;

  // Ouverture du fichier
  if ((file=fopen(filename, "rb")))
  {
    // Lecture et vérification de la signature
    if (Read_bytes(file,SCx_header.Filler1,sizeof(SCx_header.Filler1))
    && Read_word_le(file, &(SCx_header.Width))
    && Read_word_le(file, &(SCx_header.Height))
    && Read_byte(file, &(SCx_header.Filler2))
    && Read_byte(file, &(SCx_header.Planes))
    )
    {
      if ( (!memcmp(SCx_header.Filler1,"RIX",3))
        && SCx_header.Width && SCx_header.Height)
      File_error=0;
    }
    // Fermeture du fichier
    fclose(file);
  }
}


// -- Lire un fichier au format SCx -----------------------------------------
void Load_SCx(T_IO_Context * context)
{
  char filename[MAX_PATH_CHARACTERS]; // Nom complet du fichier
  FILE *file;
  word x_pos,y_pos;
  long size,real_size;
  long file_size;
  T_SCx_Header SCx_header;
  T_Palette SCx_Palette;

  Get_full_filename(filename, context->File_name, context->File_directory);

  File_error=0;

  if ((file=fopen(filename, "rb")))
  {
    file_size=File_length_file(file);

    if (Read_bytes(file,SCx_header.Filler1,sizeof(SCx_header.Filler1))
    && Read_word_le(file, &(SCx_header.Width))
    && Read_word_le(file, &(SCx_header.Height))
    && Read_byte(file, &(SCx_header.Filler2))
    && Read_byte(file, &(SCx_header.Planes))
    )
    {
      Pre_load(context, SCx_header.Width,SCx_header.Height,file_size,FORMAT_SCx,PIXEL_SIMPLE,0);
      if (File_error==0)
      {
        if (!SCx_header.Planes)
          size=sizeof(T_Palette);
        else
          size=sizeof(T_Components)*(1<<SCx_header.Planes);

        if (Read_bytes(file,SCx_Palette,size))
        {
          if (Config.Clear_palette)
            memset(context->Palette,0,sizeof(T_Palette));

          Palette_64_to_256(SCx_Palette);
          memcpy(context->Palette,SCx_Palette,size);
          Palette_loaded(context);

          context->Width=SCx_header.Width;
          context->Height=SCx_header.Height;

          if (!SCx_header.Planes)
          { // 256 couleurs (raw)
            LBM_buffer=(byte *)malloc(context->Width);

            for (y_pos=0;(y_pos<context->Height) && (!File_error);y_pos++)
            {
              if (Read_bytes(file,LBM_buffer,context->Width))
                for (x_pos=0; x_pos<context->Width;x_pos++)
                  Set_pixel(context, x_pos,y_pos,LBM_buffer[x_pos]);
              else
                File_error=2;
            }
          }
          else
          { // moins de 256 couleurs (planar)
            size=((context->Width+7)>>3)*SCx_header.Planes;
            real_size=(size/SCx_header.Planes)<<3;
            LBM_buffer=(byte *)malloc(size);
            HBPm1=SCx_header.Planes-1;
            Image_HAM=0;

            for (y_pos=0;(y_pos<context->Height) && (!File_error);y_pos++)
            {
              if (Read_bytes(file,LBM_buffer,size))
                Draw_ILBM_line(context, y_pos,real_size);
              else
                File_error=2;
            }
          }
          free(LBM_buffer);
          LBM_buffer = NULL;
        }
        else
          File_error=1;
      }
    }
    else
      File_error=1;

    fclose(file);
  }
  else
    File_error=1;
}

// -- Sauver un fichier au format SCx ---------------------------------------
void Save_SCx(T_IO_Context * context)
{
  char filename[MAX_PATH_CHARACTERS]; // Nom complet du fichier
  FILE *file;
  short x_pos,y_pos;
  T_SCx_Header SCx_header;
  byte last_char;
 
  last_char=strlen(context->File_name)-1;
  if (context->File_name[last_char]=='?')
  {
    if (context->Width<=320)
      context->File_name[last_char]='I';
    else
    {
      if (context->Width<=360)
        context->File_name[last_char]='Q';
      else
      {
        if (context->Width<=640)
          context->File_name[last_char]='F';
        else
        {
          if (context->Width<=800)
            context->File_name[last_char]='N';
          else
            context->File_name[last_char]='O';
        }
      }
    }
  }
  Get_full_filename(filename, context->File_name, context->File_directory);

  File_error=0;

  // Ouverture du fichier
  if ((file=fopen(filename,"wb")))
  {
    T_Palette palette_64;
    memcpy(palette_64,context->Palette,sizeof(T_Palette));
    Palette_256_to_64(palette_64);
    
    memcpy(SCx_header.Filler1,"RIX3",4);
    SCx_header.Width=context->Width;
    SCx_header.Height=context->Height;
    SCx_header.Filler2=0xAF;
    SCx_header.Planes=0x00;

    if (Write_bytes(file,SCx_header.Filler1,sizeof(SCx_header.Filler1))
    && Write_word_le(file, SCx_header.Width)
    && Write_word_le(file, SCx_header.Height)
    && Write_byte(file, SCx_header.Filler2)
    && Write_byte(file, SCx_header.Planes)
    && Write_bytes(file,&palette_64,sizeof(T_Palette))
    )
    {
      Init_write_buffer();

      for (y_pos=0; ((y_pos<context->Height) && (!File_error)); y_pos++)
        for (x_pos=0; x_pos<context->Width; x_pos++)
          Write_one_byte(file,Get_pixel(context, x_pos,y_pos));

      End_write(file);
      fclose(file);

      if (File_error)
        remove(filename);
    }
    else // Error d'écriture (disque plein ou protégé)
    {
      fclose(file);
      remove(filename);
      File_error=1;
    }
  }
  else
  {
    fclose(file);
    remove(filename);
    File_error=1;
  }
}


//////////////////////////////////// PNG ////////////////////////////////////

#ifndef __no_pnglib__

// -- Tester si un fichier est au format PNG --------------------------------
void Test_PNG(T_IO_Context * context)
{
  FILE *file;             // Fichier du fichier
  char filename[MAX_PATH_CHARACTERS]; // Nom complet du fichier
  byte png_header[8];
  
  Get_full_filename(filename, context->File_name, context->File_directory);
  
  File_error=1;

  // Ouverture du fichier
  if ((file=fopen(filename, "rb")))
  {
    // Lecture du header du fichier
    if (Read_bytes(file,png_header,8))
    {
      if ( !png_sig_cmp(png_header, 0, 8))
        File_error=0;
    }
    fclose(file);
  }
}
  
png_bytep * Row_pointers;
// -- Lire un fichier au format PNG -----------------------------------------
void Load_PNG(T_IO_Context * context)
{
  FILE *file;             // Fichier du fichier
  char filename[MAX_PATH_CHARACTERS]; // Nom complet du fichier
  byte png_header[8];  
  byte row_pointers_allocated;
  png_bytep trans;
  int num_trans;
  png_color_16p trans_values;
 
  png_structp png_ptr;
  png_infop info_ptr;

  Get_full_filename(filename, context->File_name, context->File_directory);

  File_error=0;
  
  if ((file=fopen(filename, "rb")))
  {
    // Load header (8 first bytes)
    if (Read_bytes(file,png_header,8))
    {
      // Do we recognize a png file signature ?
      if ( !png_sig_cmp(png_header, 0, 8))
      {
        // Prepare internal PNG loader
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (png_ptr)
        {
          // Prepare internal PNG loader
          info_ptr = png_create_info_struct(png_ptr);
          if (info_ptr)
          {
            png_byte color_type;
            png_byte bit_depth;
            
            // Setup a return point. If a pnglib loading error occurs
            // in this if(), the else will be executed.
            if (!setjmp(png_jmpbuf(png_ptr)))
            {
              png_init_io(png_ptr, file);
              // Inform pnglib we already loaded the header.
              png_set_sig_bytes(png_ptr, 8);
            
              // Load file information
              png_read_info(png_ptr, info_ptr);
              color_type = info_ptr->color_type;
              bit_depth = info_ptr->bit_depth;
              
              // If it's any supported file
              // (Note: As of writing this, this test covers every possible 
              // image format of libpng)
              if (color_type == PNG_COLOR_TYPE_PALETTE
               || color_type == PNG_COLOR_TYPE_GRAY
               || color_type == PNG_COLOR_TYPE_GRAY_ALPHA
               || color_type == PNG_COLOR_TYPE_RGB
               || color_type == PNG_COLOR_TYPE_RGB_ALPHA
              )
              {
                int num_text;
                png_text *text_ptr;
                
                int unit_type;
                png_uint_32 res_x;
                png_uint_32 res_y;

                // Comment (tEXt)
                context->Comment[0]='\0'; // Clear the previous comment
                if ((num_text=png_get_text(png_ptr, info_ptr, &text_ptr, NULL)))
                {
                  while (num_text--)
                  {
                    if (!strcmp(text_ptr[num_text].key,"Title"))
                    {
                      int size;
                      size = Min(text_ptr[num_text].text_length, COMMENT_SIZE);
                      strncpy(context->Comment, text_ptr[num_text].text, size);
                      context->Comment[size]='\0';
                      break; // Skip all others tEXt chunks
                    }
                  }
                }
                // Pixel Ratio (pHYs)
                if (png_get_pHYs(png_ptr, info_ptr, &res_x, &res_y, &unit_type))
                {
                  // Ignore unit, and use the X/Y ratio as a hint for
                  // WIDE or TALL pixels
                  if (res_x>0 && res_y>0)
                  {
                    if (res_y/res_x>1)
                    {
                      context->Ratio=PIXEL_WIDE;
                    }
                    else if (res_x/res_y>1)
                    {
                      context->Ratio=PIXEL_TALL;
                    }
                  }
                }
                if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGB_ALPHA)
                  Pre_load(context,info_ptr->width,info_ptr->height,File_length_file(file),FORMAT_PNG,PIXEL_SIMPLE,1);
                else
                  Pre_load(context, info_ptr->width,info_ptr->height,File_length_file(file),FORMAT_PNG,context->Ratio,0);

                if (File_error==0)
                {
                  int x,y;
                  png_colorp palette;
                  int num_palette;

                  // 16-bit images
                  if (bit_depth == 16)
                  {
                    // Reduce to 8-bit
                    png_set_strip_16(png_ptr);
                  }
                  else if (bit_depth < 8)
                  {
                    // Inform libpng we want one byte per pixel,
                    // even though the file was less than 8bpp
                    png_set_packing(png_ptr);
                  }
                    
                  // Images with alpha channel
                  if (color_type & PNG_COLOR_MASK_ALPHA)
                  {
                    // Tell libpng to ignore it
                    png_set_strip_alpha(png_ptr);
                  }

                  // Greyscale images : 
                  if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
                  {
                    // Map low bpp greyscales to full 8bit (0-255 range)
                    if (bit_depth < 8)
                    {
                      #if (PNG_LIBPNG_VER_MAJOR <= 1) && (PNG_LIBPNG_VER_MINOR < 4)
                        // Works well with png 1.2.8, but deprecated in 1.4 ...
                        png_set_gray_1_2_4_to_8(png_ptr);
                      #else
                        // ...where this seems to replace it:
                        png_set_expand_gray_1_2_4_to_8(png_ptr);
                      #endif
                    }
                    
                    // Create greyscale palette
                    for (x=0;x<256;x++)
                    {
                      context->Palette[x].R=x;
                      context->Palette[x].G=x;
                      context->Palette[x].B=x;
                    } 
                  }
                  else if (color_type == PNG_COLOR_TYPE_PALETTE) // Palette images
                  {
                    
                    if (bit_depth < 8)
                    {
                      // Clear unused colors
                      if (Config.Clear_palette)
                        memset(context->Palette,0,sizeof(T_Palette));
                    }
                    // Load the palette
                    png_get_PLTE(png_ptr, info_ptr, &palette,
                       &num_palette);
                    for (x=0;x<num_palette;x++)
                    {
                      context->Palette[x].R=palette[x].red;
                      context->Palette[x].G=palette[x].green;
                      context->Palette[x].B=palette[x].blue;
                    }
                    free(palette);
                    palette = NULL;
                  }
                  if (color_type != PNG_COLOR_TYPE_RGB && color_type != PNG_COLOR_TYPE_RGB_ALPHA)
                  {
                    Palette_loaded(context);
                  }
                  // Transparency (tRNS)
                  if (png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, &trans_values))
                  {
                    int i;
                    for (i=0; i<num_trans; i++)
                    {
                      if (trans[i]==0)
                      {
                        context->Transparent_color = i;
                        context->Background_transparent = 1;
                        break;
                      }
                    }
                  }
                  
                  context->Width=info_ptr->width;
                  context->Height=info_ptr->height;
                  
                  png_set_interlace_handling(png_ptr);
                  png_read_update_info(png_ptr, info_ptr);

                  // Allocate row pointers
                  Row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * context->Height);
                  row_pointers_allocated = 0;

                  /* read file */
                  if (!setjmp(png_jmpbuf(png_ptr)))
                  {
                    if (color_type == PNG_COLOR_TYPE_GRAY
                    ||  color_type == PNG_COLOR_TYPE_GRAY_ALPHA
                    ||  color_type == PNG_COLOR_TYPE_PALETTE
                    )
                    {
                      // 8bpp
                      
                      for (y=0; y<context->Height; y++)
                        Row_pointers[y] = (png_byte*) malloc(info_ptr->rowbytes);
                      row_pointers_allocated = 1;
                      
                      png_read_image(png_ptr, Row_pointers);
                      
                      for (y=0; y<context->Height; y++)
                        for (x=0; x<context->Width; x++)
                          Set_pixel(context, x, y, Row_pointers[y][x]);
                    }
                    else
                    {
                      switch (context->Type)
                      {
                        case CONTEXT_PREVIEW:
                          // 24bpp
                        
                          // It's a preview
                          // Unfortunately we need to allocate loads of memory
                          for (y=0; y<context->Height; y++)
                            Row_pointers[y] = (png_byte*) malloc(info_ptr->rowbytes);
                          row_pointers_allocated = 1;
                          
                          png_read_image(png_ptr, Row_pointers);
                          
                          for (y=0; y<context->Height; y++)
                            for (x=0; x<context->Width; x++)
                              Set_pixel_24b(context, x, y, Row_pointers[y][x*3],Row_pointers[y][x*3+1],Row_pointers[y][x*3+2]);
                          break;
                        case CONTEXT_MAIN_IMAGE:
                        case CONTEXT_BRUSH:
                        case CONTEXT_SURFACE:
                          // It's loading an actual image
                          // We'll save memory and time by writing directly into
                          // our pre-allocated 24bit buffer
                          for (y=0; y<context->Height; y++)
                            Row_pointers[y] = (png_byte*) (&(context->Buffer_image_24b[y * context->Width]));
                          png_read_image(png_ptr, Row_pointers);
                          break;
                        
                      }
                    }
                  }
                  else
                    File_error=2;
                    
                  /* cleanup heap allocation */
                  if (row_pointers_allocated)
                  {
                    for (y=0; y<context->Height; y++) {
                      free(Row_pointers[y]);
                      Row_pointers[y] = NULL;
                    }
                      
                  }
                  free(Row_pointers);
                  Row_pointers = NULL;
                }
                else
                  File_error=2;
              }
              else
               // Unsupported image type
               File_error=1;
            }
            else
             File_error=1;
          }
          else
            File_error=1;
        }
      }
      /*Close_lecture();*/
    }
    else // Lecture header impossible: Error ne modifiant pas l'image
      File_error=1;

    fclose(file);
  }
  else // Ouv. fichier impossible: Error ne modifiant pas l'image
    File_error=1;
}

void Save_PNG(T_IO_Context * context)
{
  char filename[MAX_PATH_CHARACTERS];
  FILE *file;
  int y;
  byte * pixel_ptr;
  png_structp png_ptr;
  png_infop info_ptr;
  
  Get_full_filename(filename, context->File_name, context->File_directory);
  File_error=0;
  Row_pointers = NULL;
  
  // Ouverture du fichier
  if ((file=fopen(filename,"wb")))
  {
    /* initialisation */
    if ((png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL))
      && (info_ptr = png_create_info_struct(png_ptr)))
    {
  
      if (!setjmp(png_jmpbuf(png_ptr)))
      {    
        png_init_io(png_ptr, file);
      
        /* en-tete */
        if (!setjmp(png_jmpbuf(png_ptr)))
        {
          png_set_IHDR(png_ptr, info_ptr, context->Width, context->Height,
            8, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

          png_set_PLTE(png_ptr, info_ptr, (png_colorp)context->Palette, 256);
          {
            // Commentaires texte PNG
            // Cette partie est optionnelle
            png_text text_ptr[2] = {
              {-1, "Software", "Grafx2", 6},
              {-1, "Title", NULL, 0}
            };
            int nb_text_chunks=1;
            if (context->Comment[0])
            {
              text_ptr[1].text=context->Comment;
              text_ptr[1].text_length=strlen(context->Comment);
              nb_text_chunks=2;
            }
            png_set_text(png_ptr, info_ptr, text_ptr, nb_text_chunks);
          }
          if (context->Background_transparent)
          {
            // Transparency
            byte opacity[256];
            // Need to fill a segment with '255', up to the transparent color
            // which will have a 0. This piece of data (1 to 256 bytes)
            // will be stored in the file.
            memset(opacity, 255,context->Transparent_color);
            opacity[context->Transparent_color]=0;
            png_set_tRNS(png_ptr, info_ptr, opacity, (int)1 + context->Transparent_color,0);
          }
          switch(Pixel_ratio)
          {
            case PIXEL_WIDE:
            case PIXEL_WIDE2:
              png_set_pHYs(png_ptr, info_ptr, 3000, 6000, PNG_RESOLUTION_METER);
              break;
            case PIXEL_TALL:
            case PIXEL_TALL2:
              png_set_pHYs(png_ptr, info_ptr, 6000, 3000, PNG_RESOLUTION_METER);
              break;
            default:
              break;
          }          
          png_write_info(png_ptr, info_ptr);

          /* ecriture des pixels de l'image */
          Row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * context->Height);
          pixel_ptr = context->Target_address;
          for (y=0; y<context->Height; y++)
            Row_pointers[y] = (png_byte*)(pixel_ptr+y*context->Pitch);

          if (!setjmp(png_jmpbuf(png_ptr)))
          {
            png_write_image(png_ptr, Row_pointers);
          
            /* cloture png */
            if (!setjmp(png_jmpbuf(png_ptr)))
            {          
              png_write_end(png_ptr, NULL);
            }
            else
              File_error=1;
          }
          else
            File_error=1;
        }
        else
          File_error=1;
      }
      else
      {
        File_error=1;
      }
      png_destroy_write_struct(&png_ptr, &info_ptr);
    }
    else
      File_error=1;
    // fermeture du fichier
    fclose(file);
  }

  //   S'il y a eu une erreur de sauvegarde, on ne va tout de même pas laisser
  // ce fichier pourri trainait... Ca fait pas propre.
  if (File_error)
    remove(filename);
  
  if (Row_pointers)
  {
    free(Row_pointers);
    Row_pointers=NULL;
  }
}
#endif  // __no_pnglib__

