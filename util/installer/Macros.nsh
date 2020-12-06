!macro SCRIPT_TO_FILE SCRIPT ARGS FILE
  # This macro serves as a helper to allow the installer to be cross-compiled.
  # The following line is a workaround that executes the batch file on Windows
  # and the bash file on unixoid systems. It looks little awkward because it
  # needs to be both valid Bash and valid Batch.
  !system 'if test neq -a set != tes; then scripts/${SCRIPT}.sh ${ARGS} ^> ${FILE}; exit 0; fi; : "" & scripts\${SCRIPT}.bat ${ARGS} > ${FILE}'
!macroend

!macro _LOAD_LANGUAGE EXTRA CODE LFID
  !insertmacro MUI_LANGUAGE "${LFID}"
  
  !tempfile _LANGFILE
  !insertmacro SCRIPT_TO_FILE "mki18n" "${CODE} ${LFID}" "${_LANGFILE}"

  !ifdef _LANGFILE_DEFAULT
    !insertmacro LANGFILE_INCLUDE_WITHDEFAULT "${_LANGFILE}" "${_LANGFILE_DEFAULT}"
  !else
    !insertmacro LANGFILE_INCLUDE "${_LANGFILE}"
  !endif

  !if ${EXTRA} == DEFAULT
    !define _LANGFILE_DEFAULT "${_LANGFILE}"
  !else
    !delfile "${_LANGFILE}"
  !endif
  !undef _LANGFILE

  !if ${EXTRA} == LAST
    !delfile "${_LANGFILE_DEFAULT}"
    !undef _LANGFILE_DEFAULT
  !endif
!macroend
!define LoadDefaultLanguage "!insertmacro _LOAD_LANGUAGE DEFAULT"
!define LoadLanguage "!insertmacro _LOAD_LANGUAGE -"
!define LoadLastLanguage "!insertmacro _LOAD_LANGUAGE LAST"

!macro MKFILETREE MODE DIRECTORY
  !tempfile MKFILETREE_FILE
  # The following line is a workaround that executes the batch file on Windows
  # and the bash file on unixoid systems. It looks little awkward because it
  # needs to be both valid Bash and valid Batch.
  !insertmacro SCRIPT_TO_FILE "mkfiletree" '${MODE} "${DIRECTORY}"' "${MKFILETREE_FILE}"
  !include "${MKFILETREE_FILE}"
  !delfile "${MKFILETREE_FILE}"
  !undef MKFILETREE_FILE
!macroend

!macro CREATE_INTERNET_SHORTCUT LINK TARGET
  DetailPrint "$(^CreateShortcut)${LINK}"
  ClearErrors
  WriteINIStr "${LINK}" InternetShortcut URL "${TARGET}"
  ${If} ${Errors}
    DetailPrint "$(^ErrorCreatingShortcut)${LINK}"
  ${EndIf}
!macroend
