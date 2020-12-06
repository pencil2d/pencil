Var VCREDIST_BASE
Section /o -VCRedist VCREDIST_SECTION_ID
  AllowSkipFiles off
  ${GetFileName} "${VCREDIST}" $VCREDIST_BASE
  DetailPrint "$(INSTALL)Microsoft Visual C++ Redistributable"
  SetDetailsPrint none
  File "/oname=$PLUGINSDIR\$VCREDIST_BASE" "${VCREDIST}"
  ExecWait '"$PLUGINSDIR\$VCREDIST_BASE" /install /quiet /norestart'
  SetDetailsPrint both
  ${If} ${Errors}
    Abort "$(ERROR_INSTALLING)Microsoft Visual C++ Redistributable"
  ${EndIf}
  AllowSkipFiles on
SectionEnd

# Could also be required by other applications installed after Pencil2D, therefore no uninstallation
