
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

# Run windeployqt
$deployqt = $env:QTDIR + "\bin\windeployqt.exe"
Write-Host $deployqt

& $deployqt $FolderName\Pecil2D.exe

# attach VS2013 runtime
Copy-Item $env:windir\system32\msvcp120.dll $FolderName
Copy-Item $env:windir\system32\msvcr120.dll $FolderName

Copy-Item .\resources\translations\*.qm  $FolderName\translations

$ZipName = $FolderName + ".zip"

[string]$ZipExe = "C:\Program Files\7-zip\7z.exe";
[Array]$arguments = "a", "-tzip", $ZipName, $FolderName;

& $ZipExe $arguments