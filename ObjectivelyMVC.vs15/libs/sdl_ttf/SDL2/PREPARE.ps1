If (!(Test-Path tmp.zip)) {
    Write-Output "Downloading SDL_ttf..."
    (New-Object System.Net.WebClient).DownloadFile("https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-2.0.14-VC.zip", "tmp.zip")
}

Write-Output "Extracting SDL_ttf..."
7z e tmp.zip -aos "SDL2_ttf-2.0.14\include\*.h"
7z e tmp.zip -aos "SDL2_ttf-2.0.14\lib\x86\*" -o"..\lib\Win32\"
7z e tmp.zip -aos "SDL2_ttf-2.0.14\lib\x64\*" -o"..\lib\x64\"