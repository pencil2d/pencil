
:: ------ Build Pencil2D x64------
echo [Build Pencil2D x64]

SET PATH=%PATH%;C:\Qt\5.7\msvc2015_64\bin
call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" amd64

pushd .
cd ..

rmdir /s /q bin 

qmake -tp vc -r "pencil.pro"

SET BUILDTYPE=/t:Build
SET SOLUTIONFILE="pencil.sln"
SET BUILDTARGET="/p:Configuration=Release;Platform=x64"

msbuild %SOLUTIONFILE% %BUILDTYPE% %BUILDTARGET% "/v:m"

cd bin
windeployqt Pencil2D.exe

popd 

echo "[Build Pencil2D Done]"
echo "[Deploy Pencil2D]"

powershell -File "win-deploy.ps1" 