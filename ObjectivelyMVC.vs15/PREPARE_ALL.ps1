Push-Location "libs\fontconfig\fontconfig\"
[Environment]::CurrentDirectory = $PWD
.\PREPARE.ps1
Pop-Location
[Environment]::CurrentDirectory = $PWD

Push-Location "libs\sdl\SDL2\"
[Environment]::CurrentDirectory = $PWD
.\PREPARE.ps1
Pop-Location
[Environment]::CurrentDirectory = $PWD

Push-Location "libs\sdl_image\SDL2\"
[Environment]::CurrentDirectory = $PWD
.\PREPARE.ps1
Pop-Location
[Environment]::CurrentDirectory = $PWD

Push-Location "libs\sdl_ttf\SDL2\"
[Environment]::CurrentDirectory = $PWD
.\PREPARE.ps1
Pop-Location
[Environment]::CurrentDirectory = $PWD