If (!(Test-Path tmp.zip)) {
    Write-Output "Downloading SDL..."
    (New-Object System.Net.WebClient).DownloadFile("https://www.libsdl.org/release/SDL2-devel-2.0.5-VC.zip", "tmp.zip")
}

Write-Output "Extracting SDL..."
7z e tmp.zip -aos "SDL2-2.0.5\include\*.h"
7z e tmp.zip -aos "SDL2-2.0.5\lib\x86\SDL2.*" -o"..\lib\Win32\"
7z e tmp.zip -aos "SDL2-2.0.5\lib\x64\SDL2.*" -o"..\lib\x64\"