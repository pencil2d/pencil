Function .onInit
  !if ${BITS} = 64
    ${If} ${Runningx64}
      SetRegView 64
    ${EndIf}
  !endif

  !insertmacro MULTIUSER_INIT
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

Function onGUIInit
  Push $0
  #!insertmacro CHECK_FOR_EXISTING_INSTALLATION
  !if ${BITS} == 64
    ReadRegDWORD $0 HKLM Software\Microsoft\VisualStudio\14.0\VC\Runtimes\x64 Installed
  !else
    ReadRegDWORD $0 HKLM Software\Microsoft\VisualStudio\14.0\VC\Runtimes\x86 Installed
  !endif
  ${If} $0 <> 1
    !insertmacro SelectSection ${VCREDIST_SECTION_ID}
    ${If} $MultiUser.Privileges != "Power"
    ${AndIf} $MultiUser.Privileges != "Admin"
      StrCpy $0 "Microsoft Visual C++ Redistributable"
      MessageBox MB_ICONEXCLAMATION|MB_OKCANCEL $(MISSING_DEP_ADMIN_REQUIRED) /SD IDOK IDOK OK
      Pop $0
      Abort
      OK:
    ${EndIf}
  ${EndIf}
  Pop $0
FunctionEnd

Function onMultiUserPageLeave
  ${If} $MultiUser.InstallMode == CurrentUser
  ${AndIf} ${SectionIsSelected} ${VCREDIST_SECTION_ID}
    Push $0
    StrCpy $0 "Microsoft Visual C++ Redistributable"
    MessageBox MB_ICONEXCLAMATION|MB_OKCANCEL $(MISSING_DEP_ALLUSERS_REQUIRED) IDOK OK
    Pop $0
    Abort
    OK:
    Pop $0
  ${EndIf}
FunctionEnd

Function un.onInit
  !if ${BITS} = 64
    ${If} ${Runningx64}
      SetRegView 64
    ${EndIf}
  !endif

  !insertmacro MULTIUSER_UNINIT
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd
