Param(
  [string]$platform = "x64",  # x64/x86
  [string]$branch = "master",   # branch names: master, release
  [string]$upload = "no"        # yes/no
)

echo ">>> Upload?", $upload
echo ">>> Branch:", $branch
echo ">>> Platform:", $platform

$arch = switch ($platform) {
  "x86" {"win32"; break}
  "x64" {"win64"; break}
  default {"Unknown"; break}
}

$libcrypto = switch ($platform) {
  "x86" {"C:\OpenSSL-v111-Win32\bin\libcrypto-1_1.dll"; break}
  "x64" {"C:\OpenSSL-v111-Win64\bin\libcrypto-1_1-x64.dll"; break}
  default {""; break}
}

$libssl = switch ($platform) {
  "x86" {"C:\OpenSSL-v111-Win32\bin\libssl-1_1.dll"; break}
  "x64" {"C:\OpenSSL-v111-Win64\bin\libssl-1_1-x64.dll"; break}
  default {""; break}
}

[string]$ffmpegFileName = "ffmpeg-$arch.zip"
[string]$ffmpegUrl = "https://github.com/pencil2d/pencil2d-deps/releases/download/ffmpge-v4.1.1/$ffmpegFileName"


echo $PSScriptRoot
cd $PSScriptRoot
cd ../build

echo ">>> Current working directory:"
Get-Location # print the current working directory

New-Item -ItemType 'directory' -Path './bin/plugins' -ErrorAction Continue

echo ">>> Downloading ffmpeg: $ffmpegUrl"
 
wget -Uri $ffmpegUrl -OutFile "$ffmpegFileName" -ErrorAction Stop
Expand-Archive -Path "$ffmpegFileName" -DestinationPath "./bin/plugins" -ErrorAction Stop

echo ">>> Clean up ffmpeg"

Remove-Item -Path "./$ffmpegFileName"

Remove-Item -Path "./Pencil2D" -Recurse -ErrorAction SilentlyContinue
Copy-Item -Path "./bin" -Destination "./Pencil2D" -Recurse
Remove-Item -Path "./Pencil2D/*.pdb"
Remove-Item -Path "./Pencil2D/*.ilk"

echo ">>> Deploying Qt libraries"

& "windeployqt" @("Pencil2D/pencil2d.exe")

echo ">>> Copy OpenSSL DLLs"
Copy-Item $libcrypto -Destination "./Pencil2D"
Copy-Item $libssl -Destination "./Pencil2D"

echo ">>> Zipping bin folder"

Compress-Archive -Path "./Pencil2D" -DestinationPath "./Pencil2D.zip"

$today = Get-Date -Format "yyyy-MM-dd"
$zipFileName = "pencil2d-$arch-$today.zip"

echo ">>> Zip filename: $zipFileName"
Rename-Item -Path "./Pencil2D.zip" -NewName $zipFileName

echo ">>> Zip ok?"
Test-Path $zipFileName

cd $PSScriptRoot

if ($upload -ne "yes") {
  echo ">>> Done. No need to upload binaries."
  exit 0
}

echo ">>> Upload to Google drive"

$python3 = if (Test-Path env:PYTHON) { "$env:PYTHON\python.exe" } else { "python.exe" }

$GDriveFolderId = switch($platform) {
  "x86" {$env:WIN32_NIGHTLY_PARENT; break}
  "x64" {$env:WIN64_NIGHTLY_PARENT; break}
}

$fullPath = Convert-Path "..\build\$zipFileName"

& $python3 @("nightly-build-upload.py", $GDriveFolderId, $fullPath)

echo ">>> Done!"
