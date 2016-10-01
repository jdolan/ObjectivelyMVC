Write-Output "Downloading SDL_image..."
(New-Object System.Net.WebClient).DownloadFile("https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.0.1-VC.zip", "tmp.zip")
Write-Output "Extracting SDL_image..."
7z e tmp.zip -aos "SDL2_image-2.0.1\include\*.h"
Remove-Item "tmp.*"