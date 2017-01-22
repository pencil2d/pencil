

cd $PSScriptRoot
cd ..

$msvc14_dir = "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin"
Push-Location $msvc14_dir
Invoke-BatchFile "vcvars64.bat"
Pop-Location

$env:Path += ";$env:QTDIR\bin";

& "qmake" ("-tp","vc","-r")

$Solution    = "pencil.sln"
$BuildType   = "/t:Rebuild"
$BuildTarget = "/p:Configuration=Release;Platform=x64"
$Quiet       = "/verbosity:minimal"
& "msbuild" $Solution, $BuildType, $BuildTarget $Quiet

# Create nightly build folder
$today = Get-Date -Format "yyyyMMdd";
Write-Host "today is $today"

$TheFolder = "Pencil2D-win64-$today"
if ( Test-Path $TheFolder )
{
    Remove-Item -Recurse $TheFolder -ErrorAction Continue
}
New-Item -ItemType Directory $TheFolder

# Copy exe
$AppExe = "app\release\Pencil2D.exe"
Copy-Item $AppExe $TheFolder\Pencil2D.exe

# Copy plugins
New-Item -ItemType Directory "$TheFolder\plugins"
Copy-Item "C:\Bin\ffmpeg.exe" "$TheFolder\plugins\"

# Run windeployqt 
& "windeployqt.exe" $TheFolder\Pencil2D.exe

# attach VS2015 runtime
Copy-Item $env:windir\system32\msvcp140.dll $TheFolder
Copy-Item $env:windir\system32\vcruntime140.dll $TheFolder
Copy-Item $env:windir\system32\ucrtbase.dll $TheFolder

# Compress
$TheZip = "..\" + $TheFolder + ".zip"
[string]$7z = "C:\Program Files\7-zip\7z.exe";
[Array]$arguments = "a", "-tzip", $TheZip, $TheFolder;

& $7z $arguments

Remove-Item -Recurse $TheFolder