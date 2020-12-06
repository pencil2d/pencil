!ifndef INSTALLFILES
  !error "Please set INSTALLFILES to the directory with the files to be installed"
!endif
!ifndef VCREDIST
  !error "Please set VCREDIST to the path to the Microsoft Visual C++ Redistributable"
!endif
!ifndef BITS
  !warning "BITS not defined, defaulting to 32"
  !define BITS 32
!endif
!ifndef VERSION
  !searchparse /file ..\..\common.pri 'VERSION = ' VERSION
  !warning "VERSION not defined, auto-detected ${VERSION} from app\app.pro"
!endif
!ifndef NIGHTLY_BUILD
  !define NAME "Pencil2D"
!else
  !define NAME "Pencil2D Nightly Build"
!endif
!define LOWERNAME "pencil2d"
!define PUBLISHER "Pencil2D Team"
!define DIRNAME "${NAME}"
!define REGNAME "${NAME}"
!define FORUMLINK "https://discuss.pencil2d.org/"
!define HELPLINK "https://www.pencil2d.org/doc/"
!define UPDATELINK "https://www.pencil2d.org/download/"
!define ABOUTLINK "https://www.pencil2d.org/"
!define PROGRAMBIN "pencil2d.exe"
