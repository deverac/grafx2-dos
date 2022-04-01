
STRIP = strip
DELCOMMAND = del
RMDIR = deltree /y
MKDIR = mkdirs      # 'mkdirs' exists in the 'util' directory
CC = gcc
RANLIB = ranlib


OBJDIR = ./obj


# gif   Grafx2 has internal support
# lbm   Grafx2 has internal support
# pcx   Grafx2 has internal support
# png   Grafx2 has internal support for uncompressed PNG
# webp  Grafx2 does not support (requires external webp lib)
# xv    Grafx2 does not support
SDLIMGCOPT = -DSDL_IMAGE_USE_COMMON_BACKEND -DLOAD_BMP -DLOAD_GIF -DLOAD_JPG -DLOAD_LBM -DLOAD_PCX -DLOAD_PNG -DLOAD_PNM -DLOAD_TGA -DLOAD_TIF -DLOAD_XCF -DLOAD_XPM -DLOAD_XV

#### Add SDL support
SDLCOPT = -I../SDL/include


#### Add ZLIB support
ZCOPT = -I../zlib


#### Add PNG support
PNGCOPT = -I../libpng12

#### Add TIF support
TIFCOPT = -I../libtif36/libtiff

#### Add JPG support
JPGCOPT = -I../libjpg6b

COPT = -W -Wall -Wdeclaration-after-statement -O -g -ggdb $(SDLIMGCOPT) -I. $(JPGCOPT) $(PNGCOPT) $(SDLCOPT) $(TIFCOPT) $(ZCOPT)


OBJS = $(OBJDIR)/IMG.o $(OBJDIR)/IMG_bmp.o $(OBJDIR)/IMG_gif.o $(OBJDIR)/IMG_jpg.o $(OBJDIR)/IMG_lbm.o $(OBJDIR)/IMG_pcx.o $(OBJDIR)/IMG_png.o $(OBJDIR)/IMG_pnm.o $(OBJDIR)/IMG_tga.o $(OBJDIR)/IMG_tif.o $(OBJDIR)/IMG_webp.o $(OBJDIR)/IMG_xcf.o $(OBJDIR)/IMG_xpm.o $(OBJDIR)/IMG_xv.o


.PHONY : libsdl_image.a


libsdl_image.a : $(OBJS)
	ar rc $@  $(OBJS)
	$(RANLIB) $@


clean :
	$(DELCOMMAND) libsdl_image.a
	$(RMDIR) $(OBJDIR)


$(OBJDIR)/%.o : %.c
	$(if $(wildcard $(OBJDIR) ),,$(MKDIR) $(OBJDIR))
	$(CC) $(COPT) $(CFLAGS) -c $*.c -o $(OBJDIR)/$*.o
