del bin /S /Q

mkdir bin
mkdir bin\Win32Debug\
mkdir bin\Win32Release\
mkdir bin\Win64Debug\
mkdir bin\Win64Release\

cd sdl\
call prepare.bat
cd ..\sdl_image\
call prepare.bat
cd ..\sdl_ttf\
call prepare.bat