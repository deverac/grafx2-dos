#!/bin/sh
set -e

mod_prefix () {
    OLD_PFX=$1
    NEW_PFX=$2
    for FIL in `ls ${OLD_PFX}*`; do
       NAM=$(echo $FIL | cut -c 8-)
       mv  "$FIL"  "${NEW_PFX}${NAM}"
    done
}


if [ -e g2 ]; then
  rm -rf g2
fi

if [ -e g2.zip ]; then
  rm -rf g2.zip
fi

mkdir g2

cd g2

    cp -r ../../dos ./grafx2
    rm ./grafx2/src/gfx2.ico


    # Rename lua scripts to have unique names on an 8.3 file system.
    cd ./grafx2/share/grafx2/scripts
        mod_prefix  bru_db_  bd_
        mod_prefix  pal_db_  ad_
        mod_prefix  pic_db_  id_
        mod_prefix  pic_ni_  in_
        mod_prefix  scn_db_  sd_

        # Add a note describing how prefixes have been modified.
        TXT="aa_pfx.lua"
        echo "-- This Lua script does nothing." > $TXT
        echo "-- The names of the Lua scripts in this directory have been" >> $TXT
        echo "-- shortened to fit 8.3 format. Prefixes have been changed" >> $TXT
        echo "-- in the following manner:" >> $TXT
        echo "--" >> $TXT
        echo "--   NEW     OLD" >> $TXT
        echo "--    bd    bru_db" >> $TXT
        echo "--    ad    pal_db" >> $TXT
        echo "--    id    pic_db" >> $TXT
        echo "--    in    pic_ni" >> TXT
        echo "--    sd    scn_db" >> TXT

        # Even after shortening prefixes, the following script names must be
        # renamed because the first eight characters are the same, which causes
        # conflicts on an 8.3 file system.
        mv bd_GrayscaleAvg.lua        bd_GrAvg.lua
        mv bd_GrayscaleDesat.lua      bd_GrDes.lua

        mv id_SierpinskyCarpet.lua    id_SirCa.lua
        mv id_SierpinskyTriangle.lua  id_SirTr.lua

        mv in_Colorspace12bit.lua     in_Clr12.lua
        mv in_Colorspace15bit.lua     in_Clr15.lua
        mv in_Colorspace18bit.lua     in_Clr18.lua

        mv in_Grid8.lua               in_G8.lua
        mv in_Grid8red.lua            in_G8red.lua

        mv in_PaletteX1.lua           in_PalX1.lua
        mv in_PaletteX8.lua           in_PalX8.lua

        mv sd_RemapImage2RGB.lua      sd_2RGB.lua
        mv sd_RemapImage2RGB_ed.lua   sd_2RGBe.lua
        mv sd_RemapImageTo3bitPal.lua sd_3bitP.lua
    cd ../../../..


    cp -r ../support/SDL ./SDL

    cp -r ../support/SDL_image ./SDL_image


    tar -xzf ../support/jpegsrc.v6b.tar.gz
    mv jpeg-6b libjpg6b
    cd ./libjpg6b
        mv jconfig.dj jconfig.h
        mv makefile.dj Makefile

        rm -f jconfig.bcc
        rm -f jconfig.cfg
        rm -f jconfig.mac
        rm -f jconfig.manx
        rm -f jconfig.mc6
        rm -f jconfig.sas
        rm -f jconfig.st
        rm -f jconfig.vc
        rm -f jconfig.vms
        rm -f jconfig.wat

        rm -f testimg.jpg
        rm -f testimgp.jpg
        rm -f testorig.jpg
        rm -f testprog.jpg

        rm -f makefile.ansi
        rm -f makefile.bcc
        rm -f makefile.cfg
        rm -f makefile.manx
        rm -f makefile.mc6
        rm -f makefile.mms
        rm -f makefile.sas
        rm -f makefile.unix
        rm -f makefile.vc
        rm -f makefile.vms
        rm -f makefile.wat

        rm -f coderules.doc
        rm -f install.doc
        # Needed    rm -f jconfig.doc
        rm -f libjpeg.doc
        rm -f structure.doc
        rm -f usage.doc
        rm -f wizard.doc

        rm -f ansi2knr.1
        rm -f cjpeg.1
        rm -f djpeg.1
        rm -f jpegtran.1
        rm -f rdjpgcom.1
        rm -f wrjpgcom.1

        rm -f README

        rm -f testing.ppm
        rm -f testimg.bmp

        rm -f config.guess
        rm -f config.sub
        rm -f configure
        rm -f install-sh
        rm -f ltconfig
        rm -f ltmain.sh

        rm -f testimg.ppm

        rm -f makcjpeg.st
        rm -f makdjpeg.st
        rm -f makeapps.ds
        rm -f makelib.ds
        rm -f makeproj.mac
        rm -f makljpeg.st
        rm -f maktjpeg.st
        rm -f makvms.opt

        rm -f filelist.doc
        rm -f change.log
        rm -f jmemdosa.asm
    cd ..


    tar -xzf ../support/libpng-1.2.59.tar.gz
    mv ./libpng-1.2.59 ./libpng12
    cp ./libpng12/scripts/makefile.dj2 ./libpng12/Makefile
    sed -i -e 's/rm -f /del /' ./libpng12/Makefile
    cd ./libpng12
        rm -rf ./contrib
        rm -rf ./projects
        rm -rf ./scripts

        rm -f ./aclocal.m4
        rm -f ./ag.diff
        rm -f ./ANNOUNCE
        rm -f ./autogen.sh
        rm -f ./CHANGES
        rm -f ./CMakeLists.txt
        rm -f ./compile
        rm -f ./config.guess
        rm -f ./config.h.in
        rm -f ./config.sub
        rm -f ./configure
        rm -f ./configure.ac
        rm -f ./depcomp
        rm -f ./example.c
        rm -f ./INSTALL
        rm -f ./install-sh
        rm -f ./KNOWNBUG
        rm -f ./libpng.3
        rm -f ./libpng-1.2.59.txt
        rm -f ./libpngpf.3
        rm -f ./ltmain.sh
        rm -f ./Makefile.am
        rm -f ./Makefile.in
        rm -f ./missing
        rm -f ./mkinstalldirs
        rm -f ./png.5
        rm -f ./pngbar.jpg
        rm -f ./pngbar.png
        rm -f ./pngnow.png
        rm -f ./pngtest.png
        rm -f ./README
        rm -f ./test-driver
        rm -f ./test-pngtest.sh
        rm -f ./TODO
        rm -f ./Y2KINFO
    cd ..


    tar -xzf ../support/tiff-v3.6.1.tar.gz
    mv tiff-v3.6.1 libtif36
    cd ./libtif36
        # Copy configuration for DJGPP
        mv ./contrib/dosdjgpp/port.h       ./libtiff/port.h
        mv ./contrib/dosdjgpp/Makefile.lib ./libtiff/Makefile
        # Update Makefile
        #     Avoid error if file does not exist
        sed -i -e 's/del tif_fx3s.c/if exist tif_fx3s.c del tif_fx3s.c/' ./libtiff/Makefile
        sed -i -e 's/del version.h/if exist version.h del version.h/' ./libtiff/Makefile
        #     Use RELEASE-DATE instead of tiff.alpha
        sed -i -e 's/ ..\/dist\/tiff.alpha/ ..\/RELEASE-DATE/' ./libtiff/Makefile
        #     Correct param to mkversion
        sed -i -e 's/ -a \${ALPHA} / -r ${ALPHA} /' ./libtiff/Makefile
        #     Add color.c
        sed -i -e 's/tif_codec.c \\/tif_codec.c tif_color.c \\/' ./libtiff/Makefile
        sed -i -e 's/tif_codec.o \\/tif_codec.o tif_color.o \\/' ./libtiff/Makefile
        #     Add tif_color.o rule
        sed -i -e 's/tif_compress.o:/tif_color.o: ${SRCDIR}\/tif_color.c\n\t${CC} -c ${CFLAGS} ${SRCDIR}\/tif_color.c\ntif_compress.o:/' ./libtiff/Makefile
        # Mangle symbol to avoid source code.
        sed -i -e 's/__GNUC__/__XXXGNUC__/' ./libtiff/tif_msdos.c

        rm -rf ./contrib
        rm -rf ./dist
        rm -rf ./html
        rm -rf ./man
        rm -rf ./port
        rm -rf ./tools

        rm -f ./libtiff/Makefile.in
        rm -f ./libtiff/Makefile.lcc
        rm -f ./libtiff/makefile.vc
        rm -f ./libtiff/libtiff.def

        rm -f config.guess
        rm -f config.site
        rm -f config.sub
        rm -f configure
        rm -f Makefile.in
        rm -f README
        rm -f test_pics.sh
        rm -f TODO
    cd ..


    tar -xzf ../support/lua-5.1.5.tar.gz
    mv ./lua-5.1.5 ./lua515
    # Remove comment which prevents successful compilation.
    sed -i -e 's/# DLL needs all object files//' ./lua515/src/Makefile
    cd ./lua515
        rm -rf ./doc
        rm -rf ./etc
        rm -rf ./test

        rm -f ./HISTORY
        rm -f ./INSTALL
        rm -f ./Makefile
        rm -f ./README
    cd ..


    # libpng expects '../zlib' to exist, so extract to a directory of that name.
    tar -xzf ../support/zlib-1.2.11.tar.gz
    mv ./zlib-1.2.11 ./zlib
    cp ./zlib/msdos/Makefile.dj2  ./zlib/Makefile
    cd ./zlib
        rm -rf ./amiga
        rm -rf ./contrib
        rm -rf ./doc
        rm -rf ./examples
        rm -rf ./msdos
        rm -rf ./nintendods
        rm -rf ./old
        rm -rf ./os400
        rm -rf ./qnx
        rm -rf ./test
        rm -rf ./watcom
        rm -rf ./win32

        rm -f ./CMakeLists.txt
        rm -f ./ChangeLog
        rm -f ./FAQ
        rm -f ./INDEX
        rm -f ./Makefile.in
        rm -f ./README
        rm -f ./configure
        rm -f ./make_vms.com
        rm -f ./treebuild.xml
        rm -f ./zconf.h.cmakein
        rm -f ./zconf.h.in
        rm -f ./zlib.3
        rm -f ./zlib.3.pdf
        rm -f ./zlib.map
        rm -f ./zlib.pc.in
        rm -f ./zlib.pc.cmakein
        rm -f ./zlib2ansi
    cd ..

 

    # Copy shim.c and shim.h (which the patch creates) to the libpng dir
    # and modify the Makefile appropriately to compile the example program.
    #cp ./grafx2/src/shim.c ./libpng12
    #cp ./grafx2/src/shim.h ./libpng12
  
    cp -r ../support/util .
    cp ../support/build.bat .
cd ..

7z a g2.zip g2

rm -rf g2
