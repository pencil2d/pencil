
:: ------ Build Pencil2D win32------
echo [Build Pencil2D win32]

SET PATH=%PATH%;C:\Qt\5.7\msvc2015\bin
call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" amd64_x86

pushd .
cd ..

rmdir /s /q bin 

qmake -tp vc -r "pencil.pro"

SET BUILDTYPE=/t:Build
SET SOLUTIONFILE="pencil.sln"
SET BUILDTARGET="/p:Configuration=Release;Platform=Win32"

msbuild %SOLUTIONFILE% %BUILDTYPE% %BUILDTARGET% "/v:m"

cd bin
windeployqt Pencil2D.exe

popd 

echo "[Build Pencil2D Done]"