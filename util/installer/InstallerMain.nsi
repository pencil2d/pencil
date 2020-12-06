!include Defines.nsh
!if ${BITS} = 64
  !include x64.nsh
!endif

Name "${NAME}"
BrandingText "${NAME} ${VERSION}"
OutFile "${LOWERNAME}-win${BITS}-${VERSION}-install.exe"
SetCompressor /SOLID lzma
Unicode true

!include MultiUserSetup.nsh
!include MUISetup.nsh
!include Macros.nsh
!include Languages.nsh
!include InstallVCRedist.nsh
!include InstallPencil2D.nsh
!include Init.nsh
