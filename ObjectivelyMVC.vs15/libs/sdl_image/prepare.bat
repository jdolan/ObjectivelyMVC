copy lib\x86\SDL2_image.lib ..\bin\Win32Release\SDL2_image.lib /Y
copy lib\x86\SDL2_image.lib ..\bin\Win32Debug\SDL2_image.lib /Y
copy lib\x64\SDL2_image.lib ..\bin\Win64Release\SDL2_image.lib /Y
copy lib\x64\SDL2_image.lib ..\bin\Win64Debug\SDL2_image.lib /Y

copy lib\x86\*.dll ..\bin\Win32Release\*.dll /Y
copy lib\x86\*.dll ..\bin\Win32Debug\*.dll /Y

copy lib\x64\*.dll ..\bin\Win64Release\*.dll /Y
copy lib\x64\*.dll ..\bin\Win64Debug\*.dll /Y

rename include SDL2