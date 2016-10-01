Write-Output "Downloading fontconfig.h..."
(New-Object System.Net.WebClient).DownloadFile("https://cgit.freedesktop.org/fontconfig/plain/fontconfig/fontconfig.h", "fontconfig.h")