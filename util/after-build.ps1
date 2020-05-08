Param(
  [string]$platform = "amd64",  # amd64/x86
  [string]$branch = "master",   # branch names: master, release
  [string]$upload = "no"        # yes/no
)

echo ">>> Upload?", $upload
echo ">>> Branch:", $branch
echo ">>> Platform:", $platform

$arch = switch ($platform) {
  "x86" {"win32"; break}
  "amd64" {"win64"; break}
  default {"Unknown"; break}
}

[string]$ffmpegFileName = "ffmpeg-4.1.1-$arch-static"
[string]$ffmpegUrl = "https://ffmpeg.zeranoe.com/builds/$arch/static/$ffmpegFileName.zip"


echo $PSScriptRoot
cd $PSScriptRoot
cd ../build

echo ">>> Current working directory:"
Get-Location # print the current working directory

New-Item -ItemType 'directory' -Path './bin/plugins' -ErrorAction Continue

echo ">>> Downloading ffmpeg: $ffmpegUrl"
 
wget -Uri $ffmpegUrl -OutFile "$ffmpegFileName.zip" -ErrorAction Stop
Expand-Archive -Path "$ffmpegFileName.zip" -DestinationPath "."

echo ">>> Move ffmpeg.exe to plugin folder"

Move-Item -Path "./$ffmpegFileName/bin/ffmpeg.exe" -Destination "./bin/plugins"

echo ">>> Clean up ffmpeg"

Remove-Item -Path "./$ffmpegFileName.zip"
Remove-Item -Path "./$ffmpegFileName" -Recurse

Remove-Item -Path "./Pencil2D" -Recurse -ErrorAction SilentlyContinue
Copy-Item -Path "./bin" -Destination "./Pencil2D" -Recurse

echo ">>> Deploying Qt libraries"

& "windeployqt" @("Pencil2D/pencil2d.exe")

echo ">>> Zipping bin folder"

Remove-Item -Path "./Pencil2D/*.pdb"
Remove-Item -Path "./Pencil2D/*.ilk"
Compress-Archive -Path "./Pencil2D" -DestinationPath "./Pencil2D.zip"

$today = Get-Date -Format "yyyy-MM-dd"
$zipFileName = "pencil2d-$arch-$today.zip"

echo ">>> Zip filename: $zipFileName"
Rename-Item -Path "./Pencil2D.zip" -NewName $zipFileName

echo ">>> Zip ok?"
Test-Path $zipFileName

if ($upload -ne "yes") {
  echo ">>> Done. No need to upload binaries."
  exit 0
}

echo ">>> Upload to Google drive"
cd $PSScriptRoot

$python3 = if (Test-Path env:PYTHON) { "$env:PYTHON\python.exe" } else { "python.exe" }

$GDriveFolderId = switch($platform) {
  "x86" {$env:WIN32_NIGHTLY_PARENT; break}
  "amd64" {$env:WIN64_NIGHTLY_PARENT; break}
}

$fullPath = Convert-Path "..\build\$zipFileName"

& $python3 @("nightly-build-upload.py", $GDriveFolderId, $fullPath)

echo ">>> Done!"
