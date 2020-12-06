@ECHO OFF

SETLOCAL EnableDelayedExpansion

IF "%2"=="" GOTO :USAGE
IF NOT EXIST %2\* GOTO :USAGE

IF "%1"=="install" (
  FORFILES /P "%2" /S /C "CMD /V:ON /C IF @isdir==TRUE ( SET LINE=CreateDirectory \"$INSTDIR\"@relpath & ECHO ^!LINE:\"\".=^! ) ELSE ( SET LINE=File \"/oname=$INSTDIR\"@relpath \"%2\"@relpath & ECHO ^!LINE:\"\".=^! )"
) ELSE IF "%1"=="uninstall" (
  SET I=0
  FOR /F "tokens=*" %%G IN ('FORFILES /P "%2" /S /C "CMD /V:ON /C IF @isdir==TRUE ( SET LINE=RMDir \"$INSTDIR\"@relpath & ECHO ^!LINE:\"\".=^! ) ELSE ( SET LINE=Delete \"$INSTDIR\"@relpath & ECHO ^!LINE:\"\".=^! )"') DO (
    SET /A I=!I! + 1
    SET LINE!I!=%%G
  )
  FOR /L %%G IN (!I!,-1,1) DO ECHO !LINE%%G!
) ELSE GOTO :USAGE

EXIT /B 0

:USAGE
ECHO Usage: %0 (install^|uninstall) DIRECTORY
EXIT /B 255
