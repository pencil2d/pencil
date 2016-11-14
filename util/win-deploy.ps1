
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

Copy-Item $SrcExePath $FolderName\Pencil2D.exe
Copy-Item "app\release\plugins" $FolderName

# Run windeployqt
$deployqt = $env:QTDIR + "\bin\windeployqt.exe"
Write-Host $deployqt

& $deployqt $FolderName\Pencil2D.exe

# attach VS2013 runtime
Copy-Item $env:windir\system32\msvcp140.dll $FolderName
Copy-Item $env:windir\system32\vcruntime140.dll $FolderName
Copy-Item $env:windir\system32\ucrtbase.dll $FolderName

$ZipName = $FolderName + ".zip"

[string]$ZipExe = "C:\Program Files\7-zip\7z.exe";
[Array]$arguments = "a", "-tzip", $ZipName, $FolderName;

& $ZipExe $arguments

Remove-Item -Recurse $FolderName