#  Grafx2 - The Ultimate 256-color bitmap paint program
#  
#  Grafx2 is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; version 2
#  of the License.
#  
#  Grafx2 is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with Grafx2; if not, see <http://www.gnu.org/licenses/>

# FreeDOS considirations:
#
#   Deleting all object files in one command is too long for the FreeDOS command
#   line, so we delete the entire directory instead of the object files.
#
#   The FreeDOS 'del' does not accept '/' as dir separator, but 'deltree' does,
#   so we use 'deltree' to delete files or dirs.
#
#   Using the Makefile 'echo' command is troublesome. If the text to echo is not
#   quoted, Makefile interprets semi-colons as a command separator (and will
#   not echo them). If the text to echo is enclosed in quotes, when it is
#   passed to the FreeDOS shell, the FreeDOS 'echo' command will output the
#   quotes. Escaping semi-colons does not work. To work-aronud this problem,
#   the 'qecho' command can be used.


PROG_VER = 2.2  # Grafx2 version.
SVN_VER = 1404  # SVN version. Only for compatibility; it's mostly meaningless.


STRIP = strip
DELCOMMAND = del
RMDIR = deltree /y  # 'mkdirs' exists in the 'util' directory
MKDIR = mkdirs      # 'qecho' exists in the 'util' directory
ECHO = qecho
CC = gcc


OBJDIR = ../obj/dos
BINDIR = ../bin
BIN = $(BINDIR)/grafx2.exe



#### Mandatory for building for FreeDOS
DOSCOPT = -DFDOS

#### Enable SDL
SDLCOPT = -I../../SDL/include
SDLLINC = -L../../SDL
SDLLOPT = -lsdl


#### Enable SDL_image
SDLIMGCOPT = -I../../SDL_image
SDLIMGLINC = -L../../SDL_image
SDLIMGLOPT = -lsdl_image


#### Enable/Disable Joystick support (Joystick is not supported)
#JOYCOPT = -DUSE_JOYSTICK
JOYCOPT =


#### Enable/Disable Layers (Disabling layered editing speeds up rendering)
#LAYERCOPT = -DNOLAYERS
LAYERCOPT =


#### Enable/Disable Lua support
LUACOPT = -D__ENABLE_LUA__ -I../../lua515/src
LUALINC = -L../../lua515/src
LUALOPT = -llua


#### Enable/Disable PNG support
PNGCOPT = -I../../libpng12 -I../../zlib
PNGLINC = -L../../libpng12 -L../../zlib
PNGLOPT = -lpng -lz


#### Enable/Disable TTF support (True Type Fonts are not supported)
TTFCOPT = -DNOTTF
#TTFCOPT =


COPT = -W -Wall -Wdeclaration-after-statement -O -g -ggdb -I. $(DOSCOPT) $(JOYCOPT) $(LAYERCOPT) $(LUACOPT) $(PNGCOPT) $(TTFCOPT) $(SDLCOPT) $(SDLIMGCOPT)

LOPT = $(SDLLINC) $(LUALINC) $(PNGLINC) $(SDLIMGLINC)  $(SDLLOPT) $(LUALOPT) $(PNGLOPT) $(TTFLOPT) $(SDLIMGLOPT)


OBJ = $(OBJDIR)/main.o $(OBJDIR)/init.o $(OBJDIR)/graph.o $(OBJDIR)/sdlscreen.o  $(OBJDIR)/misc.o $(OBJDIR)/special.o $(OBJDIR)/buttons.o $(OBJDIR)/palette.o $(OBJDIR)/help.o $(OBJDIR)/operatio.o $(OBJDIR)/pages.o $(OBJDIR)/loadsave.o $(OBJDIR)/readline.o $(OBJDIR)/engine.o $(OBJDIR)/filesel.o $(OBJDIR)/op_c.o $(OBJDIR)/readini.o $(OBJDIR)/saveini.o $(OBJDIR)/shade.o $(OBJDIR)/keyboard.o $(OBJDIR)/io.o $(OBJDIR)/version.o $(OBJDIR)/text.o $(OBJDIR)/SFont.o $(OBJDIR)/setup.o $(OBJDIR)/pxsimple.o $(OBJDIR)/pxtall.o $(OBJDIR)/pxwide.o $(OBJDIR)/pxdouble.o $(OBJDIR)/pxtriple.o $(OBJDIR)/pxtall2.o $(OBJDIR)/pxwide2.o $(OBJDIR)/pxquad.o $(OBJDIR)/windows.o $(OBJDIR)/brush.o $(OBJDIR)/realpath.o $(OBJDIR)/mountlist.o $(OBJDIR)/input.o $(OBJDIR)/hotkeys.o $(OBJDIR)/transform.o $(OBJDIR)/pversion.o $(OBJDIR)/factory.o $(OBJDIR)/fileformats.o $(OBJDIR)/miscfileformats.o $(OBJDIR)/libraw2crtc.o $(OBJDIR)/brush_ops.o $(OBJDIR)/buttons_effects.o $(OBJDIR)/layers.o $(OBJDIR)/shim.o


.PHONY : all debug release clean version



all : $(BIN)

debug : $(BIN)

$(BIN) : $(OBJ)
	$(MKDIR) ../bin
	$(CC) $(OBJ) -o $(BIN) $(LOPT) $(LFLAGS)

version.c :
	$(ECHO) "char SVN_revision[]=\"$(SVN_VER)\";" > version.c

pversion.c :
	$(ECHO) "char Program_version[]=\"$(PROG_VER)\";" > pversion.c

delversion :
	$(DELCOMMAND) version.c
	
delpversion :
	$(DELCOMMAND) pversion.c

version : delversion delpversion version.c pversion.c $(OBJDIR)/version.o $(OBJDIR)/pversion.o all

release : version $(BIN)
	$(STRIP) $(BIN)

clean :
	$(RMDIR) $(OBJDIR)
	$(RMDIR) $(BINDIR)


$(OBJDIR)/%.o : %.c
	$(if $(wildcard $(OBJDIR)),,$(MKDIR) $(OBJDIR))
	$(CC) $(COPT) $(CFLAGS) -c $*.c -o $(OBJDIR)/$*.o
