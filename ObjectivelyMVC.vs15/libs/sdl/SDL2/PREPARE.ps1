If (!(Test-Path tmp1.zip)) {
    Write-Output "Downloading SDL..."
    (New-Object System.Net.WebClient).DownloadFile("https://www.libsdl.org/release/SDL2-devel-2.0.7-VC.zip", "tmp1.zip")
}

Write-Output "Extracting SDL..."
7z e tmp1.zip -aoa "SDL2-2.0.7\include\*.h"
7z e tmp1.zip -aoa "SDL2-2.0.7\lib\x86\SDL2.*" -o"..\lib\Win32\"
7z e tmp1.zip -aoa "SDL2-2.0.7\lib\x64\SDL2.*" -o"..\lib\x64\"