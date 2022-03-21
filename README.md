`grafx2.exe` - A port of [Grafx2](http://grafx2.chez.com/) to FreeDOS.

`grafx2.zip` is a FreeDOS package containing `grafx2.exe` (and sources).

To install: `fdinst install grafx2.zip`. To install with sources: `fdnpkg install-wsrc grafx2.zip`

Some keystrokes (e.g. Alt-Down, or Ctrl-3) are used as shortcuts in Grafx2, but
are not available in DOS and cannot be captured without writing a custom
keyboard driver. To work around this, the LeftShift, RightShift, CapsLock, and
NumLock can be reassigned to act as the Alt or Ctrl key by supplying a command
line parameter.

    /ls-alt    Use LeftShift key as Alt key
    /ls-ctrl   Use LeftShift key as Ctrl key
    /rs-alt    Use RightShift key as Alt key
    /rs-ctrl   Use RightShift key as Ctrl key
    /cl-alt    Use CapsLock key as Alt key
    /cl-ctrl   Use CapsLock key as Ctrl key
    /nl-alt    Use NumLock key as Alt key
    /nl-ctrl   Use NumLock key as Ctrl key

A key cannot be reassigned to act as both Alt and Ctrl, however multiple
assignments can be made. (e.g. `/rs-alt /nl-alt /cl-ctrl`)

When using the CapsLock on NumLock keys to simulate a key, their state is
ignored. It does not matter whether they are toggled on or off; it only matters
if they are pressed or not pressed. 

Reassigning keys is only needed if you plan to use a keyboard shortcut and the
keyboard shortcut is one that is not available in DOS.

Grafx2 can run under DOSBox, provided CWSDPMI.EXE is available. There are
several ways of running CWSDPMI; the easiest is probably just to copy the
CWSDPMI.EXE executable to the same directory that GRAFX2.EXE resides in.

### Bugs

This is an alpha-release. There are probably many bugs.

* The Ctrl-Break (or Ctrl-2, which generates the same scancode) will result in
`^C` being printed on the screen. This can be cleared by moving the mouse
over it. This is a difficult issue to fix.

* Mouse emulation via the keyboard (Alt + Up, Down, Left, Right) is not perfect.
Moving Left causes the mouse to move two pixels. If the cursor is on an odd
x-value, moving Up or Down will also cause the x position to decrease by one.

* In FreeDOS, the mouse will sometimes make small erratic movements. This is
probably due to a bug with the mouse driver. No such movements occur when
running in DOSBox.

* TrueType Fonts are not supported.

* Grafx2 can be driven by Lua scripts, however, some Lua scripts are useless.
e.g. Converting an image to RGB format is not supported. The useless scripts
are included only for illustrative purposes.

* Grafx2 does not work in DOSBox-X.

* Multi-byte characters in the names of contributors had to be replaced with
similar single-byte ASCII characters to avoid over-running buffers and crashing
the program.

### Building

Grafx2 is compiled with DJGPP on FreeDOS. Ensure DJGPP is on your PATH (e.g.
`PATH=c:\devel\djgpp\bin;%PATH%`) and configured (e.g.
`set DJGPP=c:\devel\djgpp\djgpp.env`).

    >build.bat clean     Remove generated files
    >build.bat grafx2    Build grafx2.exe
    >build.bat pkg       Build FreeDOS package grafx2.zip

A package suitable for building on FreeDOS can be created on Linux with the
`build-g2.sh` script. This script extracts library source files, removes
unneeded files, applies a patch to the original Grafx2 sources, and then
creates `g2.zip` containing all the modified source files.
