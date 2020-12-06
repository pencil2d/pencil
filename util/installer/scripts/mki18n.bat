@ECHO OFF

IF "%2"=="" (
  ECHO Usage: %0 CODE LFID
  EXIT /B 255
)

ECHO !insertmacro LANGFILE_EXT %2
IF EXIST "%~dp0..\translations\%1.ini" (
  FOR /F "delims== tokens=1*" %%G IN (%~dp0..\translations\%1.ini) DO ECHO ${LangFileString} %%G "%%H"
)

EXIT /B 0
