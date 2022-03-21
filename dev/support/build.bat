@echo off


if "%1" == "clean"  goto %1
if "%1" == "grafx2" goto %1
if "%1" == "pkg"    goto %1

echo.
echo   Targets:
echo      clean    Clean generated files
echo      grafx2   Build grafx2.exe
echo      pkg      Build FreeDOS package
echo.
goto end


:clean
    rem Clean

    echo    Cleaning Grafx2
    cd grafx2
        cd src
            make clean
        cd ..
    cd ..

    echo    Cleaning LIBLUA
    cd lua515
        cd src
            make clean
        cd ..
    cd ..

    echo    Cleaning LIBPNG
    cd libpng12
        make clean
    cd ..

    echo    Cleaning LIBZ
    cd zlib
        make clean
    cd ..

    echo    Cleaning Util
    cd util
        make clean
    cd ..

    goto end


:grafx2
    rem 'mkdirs' and/or 'qecho' may be needed.
    PATH=%CD%\util;%PATH%


    rem Build utilities.
    echo.
    echo    Building Utils
    echo.
    cd util
        make all
    cd ..


    rem Build ZLIB library
    echo.
    echo    Building LIBZ
    echo.
    cd zlib
        make libz.a
    cd ..


    rem Build LIBPNG library (depends on ZLIB)
    echo.
    echo    Building LIBPNG
    echo.
    cd libpng12
        make libpng.a
    cd ..


    rem Build LUA library
    echo.
    echo    Building LIBLUA
    echo.
    cd lua515
        cd src
            make liblua.a
        cd ..
    cd ..


    rem Build Grafx2 application.
    echo.
    echo    Building Grafx2
    echo.
    cd grafx2
        cd src
            make
        cd ..
    cd ..
    if not exist .\grafx2\bin\grafx2.exe goto errbuild
    strip .\grafx2\bin\grafx2.exe

    goto end


:pkg
    rem Build FreeDOS package

    set PKG=pkg
    set GRP=apps
    set NAM=grafx2
    if exist %PKG%\NUL deltree /y %PKG% > NUL
    if exist %NAM%.zip del %NAM%.zip  > NUL

    mkdir %PKG%
    mkdir %PKG%\appinfo
    set LSM=%PKG%\appinfo\%NAM%.lsm
    echo Begin3> %LSM%
    echo Title:          %NAM%>> %LSM%
    echo Version:        2.2>> %LSM%
    echo Entered-date:   2022-03-21>> %LSM%
    echo Description:    A graphics editor.>> %LSM%
    echo Keywords:       freedos %NAM%>> %LSM%
    echo Author:         >> %LSM%
    echo Maintained-by:  >> %LSM%
    echo Primary-site:   >> %LSM%
    echo Alternate-site: >> %LSM%
    echo Original-site:  >> %LSM%
    echo Platforms:      DOS>> %LSM%
    echo Copying-policy: GPL>> %LSM%
    echo End>> %LSM%

    mkdir %PKG%\%GRP%
    mkdir %PKG%\source
    mkdir %PKG%\source\%NAM%
    if not exist %PKG%\source\%NAM%\NUL goto err1pkg



    rem Copy 'release' files.
    mkdir %PKG%\%GRP%\%NAM%
    xcopy /E .\grafx2\bin    %PKG%\%GRP%\%NAM%\bin\
    xcopy /E .\grafx2\doc    %PKG%\%GRP%\%NAM%\doc\
    xcopy /E .\grafx2\share  %PKG%\%GRP%\%NAM%\share\

    rem Copy 'source' files (also copies compiled files, if they exist).
    xcopy /E .\grafx2      %PKG%\source\%NAM%\grafx2\
    xcopy /E .\libpng12    %PKG%\source\%NAM%\libpng12\
    xcopy /E .\lua515      %PKG%\source\%NAM%\lua515\
    xcopy /E .\util        %PKG%\source\%NAM%\util\
    xcopy /E .\zlib        %PKG%\source\%NAM%\zlib\
    copy     .\build.bat   %PKG%\source\%NAM%\
    if exist %PKG%\source\%NAM%\grafx2\gfx2.cfg  del %PKG%\source\%NAM%\grafx2\gfx2.cfg
    if exist %PKG%\source\%NAM%\grafx2\gfx2.ini  del %PKG%\source\%NAM%\grafx2\gfx2.ini



    rem Delete compiled files.
    if exist %PKG%\source\%NAM%\grafx2\bin\NUL   deltree /y %PKG%\source\%NAM%\grafx2\bin
    if exist %PKG%\source\%NAM%\grafx2\obj\NUL   deltree /y %PKG%\source\%NAM%\grafx2\obj

    if exist %PKG%\source\%NAM%\libpng12\libpng.a  del %PKG%\source\%NAM%\libpng12\libpng.a
    del %PKG%\source\%NAM%\libpng12\*.o

    if exist %PKG%\source\%NAM%\lua515\src\liblua.a  del %PKG%\source\%NAM%\lua515\src\liblua.a
    del %PKG%\source\%NAM%\lua515\src\*.o

    del %PKG%\source\%NAM%\util\*.exe

    if exist %PKG%\source\%NAM%\zlib\libz.a  del %PKG%\source\%NAM%\zlib\libz.a
    del %PKG%\source\%NAM%\zlib\*.o
    del %PKG%\source\%NAM%\zlib\*.d



    rem Create Link
    mkdir %PKG%\links
    echo %GRP%\%NAM%\bin\grafx2.exe > %PKG%\links\grafx2.bat
    if not exist %PKG%\links\grafx2.bat goto err4pkg


    cd .\%PKG%
    rem  -9  Max compression
    rem  -r  Recurse into directories
    zip -9 -r ..\grafx2.zip *
    if not exist ..\grafx2.zip goto err4pkg
    cd ..

    echo.
    echo The grafx2.zip package has been created.
    echo.
    echo To install: fdnpkg install grafx2.zip
    echo         or: fdnpkg install-wsrc grafx2.zip
    echo  To remove: fdnpkg remove grafx2

    goto end



:errbuild
    echo Error building.
    goto end

:err1pkg
    echo Error creating directory structure. Building package failed.
    goto end

:err2pkg
    echo Error copying source files. Building package failed. 
    goto end

:err3pkg
    echo Error copying executable files. Building package failed.
    goto end

:err4pkg
    echo Error creating zip file. Building package failed.
    goto end

:end