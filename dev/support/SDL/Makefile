
STRIP = strip
DELCOMMAND = del
RMDIR = deltree /y
MKDIR = mkdirs      # 'mkdirs' exists in the 'util' directory
CC = gcc
RANLIB = ranlib


OBJDIR = ./obj


COPT = -W -Wall -Wdeclaration-after-statement -O -g -ggdb -I./include


OBJS = $(OBJDIR)/SDL.o $(OBJDIR)/SDL_cursor.o $(OBJDIR)/SDL_error.o $(OBJDIR)/SDL_events.o $(OBJDIR)/SDL_joystick.o $(OBJDIR)/SDL_keyboard.o $(OBJDIR)/SDL_pixels.o $(OBJDIR)/SDL_rwops.o $(OBJDIR)/SDL_string.o $(OBJDIR)/SDL_surface.o $(OBJDIR)/SDL_systimer.o $(OBJDIR)/SDL_video.o


.PHONY : libsdl.a


libsdl.a : $(OBJS)
	ar rc $@  $(OBJS)
	$(RANLIB) $@


clean :
	$(DELCOMMAND) libsdl.a
	$(RMDIR) $(OBJDIR)


$(OBJDIR)/%.o : %.c
	$(if $(wildcard $(OBJDIR)),,$(MKDIR) $(OBJDIR))
	$(CC) $(COPT) $(CFLAGS) -c $*.c -o $(OBJDIR)/$*.o
