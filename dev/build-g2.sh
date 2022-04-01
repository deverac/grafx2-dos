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
    rm -rf ./libpng12/contrib
    rm -rf ./libpng12/projects
    rm -rf ./libpng12/scripts
    rm -f ./libpng12/aclocal.m4
    rm -f ./libpng12/ag.diff
    rm -f ./libpng12/ANNOUNCE
    rm -f ./libpng12/autogen.sh
    rm -f ./libpng12/CHANGES
    rm -f ./libpng12/CMakeLists.txt
    rm -f ./libpng12/compile
    rm -f ./libpng12/config.guess
    rm -f ./libpng12/config.h.in
    rm -f ./libpng12/config.sub
    rm -f ./libpng12/configure
    rm -f ./libpng12/configure.ac
    rm -f ./libpng12/depcomp
    rm -f ./libpng12/example.c
    rm -f ./libpng12/INSTALL
    rm -f ./libpng12/install-sh
    rm -f ./libpng12/KNOWNBUG
    rm -f ./libpng12/libpng.3
    rm -f ./libpng12/libpng-1.2.59.txt
    rm -f ./libpng12/libpngpf.3
    rm -f ./libpng12/ltmain.sh
    rm -f ./libpng12/Makefile.am
    rm -f ./libpng12/Makefile.in
    rm -f ./libpng12/missing
    rm -f ./libpng12/mkinstalldirs
    rm -f ./libpng12/png.5
    rm -f ./libpng12/pngbar.jpg
    rm -f ./libpng12/pngbar.png
    rm -f ./libpng12/pngnow.png
    rm -f ./libpng12/pngtest.png
    rm -f ./libpng12/README
    rm -f ./libpng12/test-driver
    rm -f ./libpng12/test-pngtest.sh
    rm -f ./libpng12/TODO
    rm -f ./libpng12/Y2KINFO


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
    rm -rf ./lua515/doc
    rm -rf ./lua515/etc
    rm -rf ./lua515/test
    rm -f ./lua515/HISTORY
    rm -f ./lua515/INSTALL
    rm -f ./lua515/Makefile
    rm -f ./lua515/README


    # libpng expects '../zlib' to exist, so extract to a directory of that name.
    tar -xzf ../support/zlib-1.2.11.tar.gz
    mv ./zlib-1.2.11 ./zlib
    cp ./zlib/msdos/Makefile.dj2  ./zlib/Makefile
    rm -rf ./zlib/amiga
    rm -rf ./zlib/contrib
    rm -rf ./zlib/doc
    rm -rf ./zlib/examples
    rm -rf ./zlib/msdos
    rm -rf ./zlib/nintendods
    rm -rf ./zlib/old
    rm -rf ./zlib/os400
    rm -rf ./zlib/qnx
    rm -rf ./zlib/test
    rm -rf ./zlib/watcom
    rm -rf ./zlib/win32
    rm -f ./zlib/CMakeLists.txt
    rm -f ./zlib/ChangeLog
    rm -f ./zlib/FAQ
    rm -f ./zlib/INDEX
    rm -f ./zlib/Makefile.in
    rm -f ./zlib/README
    rm -f ./zlib/configure
    rm -f ./zlib/make_vms.com
    rm -f ./zlib/treebuild.xml
    rm -f ./zlib/zconf.h.cmakein
    rm -f ./zlib/zconf.h.in
    rm -f ./zlib/zlib.3
    rm -f ./zlib/zlib.3.pdf
    rm -f ./zlib/zlib.map
    rm -f ./zlib/zlib.pc.in
    rm -f ./zlib/zlib.pc.cmakein
    rm -f ./zlib/zlib2ansi

 

    # Copy shim.c and shim.h (which the patch creates) to the libpng dir
    # and modify the Makefile appropriately to compile the example program.
    #cp ./grafx2/src/shim.c ./libpng12
    #cp ./grafx2/src/shim.h ./libpng12
  
    cp -r ../support/util .
    cp ../support/build.bat .
cd ..

7z a g2.zip g2

rm -rf g2
