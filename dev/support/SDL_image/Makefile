
STRIP = strip
DELCOMMAND = del
RMDIR = deltree /y
MKDIR = mkdirs      # 'mkdirs' exists in the 'util' directory
CC = gcc
RANLIB = ranlib


OBJDIR = ./obj


#### Add SDL support
SDLCOPT = -I../SDL/include


#### Add ZLIB support
ZCOPT = -I../zlib


#### Add PNG support
PNGCOPT = -I../libpng12


COPT = -W -Wall -Wdeclaration-after-statement -O -g -ggdb -I.  $(PNGCOPT) $(SDLCOPT) $(ZCOPT)


OBJS = $(OBJDIR)/IMG.o $(OBJDIR)/readpng.o


.PHONY : libsdl_image.a


libsdl_image.a : $(OBJS)
	ar rc $@  $(OBJS)
	$(RANLIB) $@


clean :
	$(DELCOMMAND) libsdl_image.a
	$(RMDIR) $(OBJDIR)


$(OBJDIR)/%.o : %.c
	$(if $(wildcard $(OBJDIR)),,$(MKDIR) $(OBJDIR))
	$(CC) $(COPT) $(CFLAGS) -c $*.c -o $(OBJDIR)/$*.o
