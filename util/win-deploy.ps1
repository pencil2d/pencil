
cd $PSScriptRoot
cd ..

if ( !( Test-Path app\release\Pencil2D.exe ) )
{
    Write-Host "Can't find Pencil2D.exe!!"
    exit( 1 );
}
$SrcExePath = "app\release\Pencil2D.exe"

$today = Get-Date -Format "yyyyMMdd";
Write-Host "today is $today"

$FolderName = "Pencil2D-win-$today"
Remove-Item -Recurse $FolderName -ErrorAction SilentlyContinue
New-Item -ItemType Directory $FolderName

Copy-Item $SrcExePath $FolderName\Pecil2D.exe

$deployqt = $env:QTDIR + "\bin\windeployqt.exe"
Write-Host $deployqt

& $deployqt $FolderName\Pecil2D.exe

Copy-Item .\resources\translations\*.qm  $FolderName\translations

$ZipName = $FolderName + ".zip"

[string]$ZipExe = "C:\Program Files\7-zip\7z.exe";
[Array]$arguments = "a", "-tzip", $ZipName, $FolderName;

& $ZipExe $arguments