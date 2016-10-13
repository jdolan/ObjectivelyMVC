If (!(Test-Path tmp.zip)) {
    Write-Output "Downloading SDL..."
    (New-Object System.Net.WebClient).DownloadFile("https://www.libsdl.org/release/SDL2-devel-2.0.4-VC.zip", "tmp.zip")
}

Write-Output "Extracting SDL..."
7z e tmp.zip -aos "SDL2-2.0.4\include\*.h"