Section Install INSTALL_SECTION_ID
  CreateDirectory "$INSTDIR"

  !insertmacro MKFILETREE install "${INSTALLFILES}"

  WriteUninstaller "$INSTDIR\uninstall.exe"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\${NAME}.lnk" "$INSTDIR\${PROGRAMBIN}"
    !insertmacro CREATE_INTERNET_SHORTCUT "$SMPROGRAMS\$StartMenuFolder\${NAME} Website.url" "${ABOUTLINK}"
    !insertmacro CREATE_INTERNET_SHORTCUT "$SMPROGRAMS\$StartMenuFolder\${NAME} Forum.url" "${FORUMLINK}"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Uninstall ${NAME}.lnk" "$INSTDIR\uninstall.exe"
  !insertmacro MUI_STARTMENU_WRITE_END

  WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${REGNAME}" "DisplayName" "${NAME}"
  WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${REGNAME}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${REGNAME}" "QuietUninstallString" '"$INSTDIR\uninstall.exe" /S'
  WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${REGNAME}" "InstallLocation" '"$INSTDIR"'
  WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${REGNAME}" "DisplayIcon" '"$INSTDIR\${PROGRAMBIN}"'
  WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${REGNAME}" "Publisher" "${PUBLISHER}"
  WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${REGNAME}" "HelpLink" "${HELPLINK}"
  WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${REGNAME}" "URLUpdateInfo" "${UPDATELINK}"
  WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${REGNAME}" "URLInfoAbout" "${ABOUTLINK}"
  WriteRegStr SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${REGNAME}" "DisplayVersion" "${VERSION}"
  SectionGetSize ${INSTALL_SECTION_ID} $0
  WriteRegDWORD SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${REGNAME}" "EstimatedSize" $0
  WriteRegDWORD SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${REGNAME}" "NoModify" 1
  WriteRegDWORD SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${REGNAME}" "NoRepair" 1
SectionEnd

Section un.Install
  !insertmacro MKFILETREE uninstall "${INSTALLFILES}"

  Delete "$INSTDIR\uninstall.exe"
  RMDir "$INSTDIR"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  Delete "$SMPROGRAMS\$StartMenuFolder\${NAME}.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\${NAME} Website.url"
  Delete "$SMPROGRAMS\$StartMenuFolder\${NAME} Forum.url"
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall ${NAME}.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"

  DeleteRegKey SHCTX "Software\Microsoft\Windows\CurrentVersion\Uninstall\${REGNAME}"
  DeleteRegKey SHCTX "Software\${REGNAME}"
SectionEnd
