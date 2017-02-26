Param
(
    [bool]$win32 = 0
)

cd $PSScriptRoot
cd ..

# Create nightly build folder
$today = Get-Date -Format "yyyyMMdd";
Write-Host "today is $today"

if ( $win32 )
{
    $TheFolder = "Pencil2D-win32-$today"
}
else
{
    $TheFolder = "Pencil2D-win64-$today"
}

if ( Test-Path $TheFolder )
{
    Remove-Item -Recurse $TheFolder -ErrorAction Continue
}
New-Item -ItemType Directory $TheFolder

# Copy exe
Copy-Item -Recurse "bin\*" "$TheFolder"

Remove-Item "$TheFolder\*.pdb"
Remove-Item "$TheFolder\*.ilk"

# Copy plugins
New-Item -ItemType Directory "$TheFolder\plugins"
Copy-Item "C:\Bin\ffmpeg.exe" "$TheFolder\plugins\"

# Compress
$TheZip = "..\" + $TheFolder + ".zip"
[string]$7z = "C:\Program Files\7-zip\7z.exe";
[Array]$arguments = "a", "-tzip", $TheZip, $TheFolder;

& $7z $arguments

Remove-Item -Recurse $TheFolder