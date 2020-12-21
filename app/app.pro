#-------------------------------------------------
#
# Pencil2D GUI
#
#-------------------------------------------------

! include( ../common.pri ) { error( Could not find the common.pri file! ) }

QT += core widgets gui xml multimedia svg network

TEMPLATE = app
TARGET = pencil2d
QMAKE_APPLICATION_BUNDLE_NAME = Pencil2D

CONFIG += qt precompile_header

DESTDIR = ../bin
MOC_DIR = .moc
OBJECTS_DIR = .obj
UI_DIR = .ui

RESOURCES += \
    data/app.qrc \
    ../translations/translations.qrc

INCLUDEPATH += \
    src \
    ../core_lib/src/graphics \
    ../core_lib/src/graphics/bitmap \
    ../core_lib/src/graphics/vector \
    ../core_lib/src/interface \
    ../core_lib/src/structure \
    ../core_lib/src/tool \
    ../core_lib/src/util \
    ../core_lib/ui \
    ../core_lib/src/managers \
    ../core_lib/src/external

PRECOMPILED_HEADER = src/app-pch.h

HEADERS += \
    src/app-pch.h \
    src/importlayersdialog.h \
    src/importpositiondialog.h \
    src/mainwindow2.h \
    src/onionskinwidget.h \
    src/predefinedsetmodel.h \
    src/pegbaralignmentdialog.h \
    src/shortcutfilter.h \
    src/timeline2.h \
    src/actioncommands.h \
    src/preferencesdialog.h \
    src/filespage.h \
    src/generalpage.h \
    src/shortcutspage.h \
    src/timelinepage.h \
    src/toolspage.h \
    src/preview.h \
    src/colorbox.h \
    src/colorinspector.h \
    src/colorpalettewidget.h \
    src/colorwheel.h \
    src/filedialog.h \
    src/displayoptionwidget.h \
    src/pencil2d.h \
    src/exportmoviedialog.h \
    src/app_util.h \
    src/errordialog.h \
    src/aboutdialog.h \
    src/toolbox.h \
    src/tooloptionwidget.h \
    src/importexportdialog.h \
    src/exportimagedialog.h \
    src/importimageseqdialog.h \
    src/spinslider.h \
    src/doubleprogressdialog.h \
    src/colorslider.h \
    src/checkupdatesdialog.h \
    src/presetdialog.h \
    src/commandlineparser.h \
    src/commandlineexporter.h

SOURCES += \
    src/importlayersdialog.cpp \
    src/importpositiondialog.cpp \
    src/main.cpp \
    src/mainwindow2.cpp \
    src/onionskinwidget.cpp \
    src/predefinedsetmodel.cpp \
    src/pegbaralignmentdialog.cpp \
    src/shortcutfilter.cpp \
    src/timeline2.cpp \
    src/actioncommands.cpp \
    src/preferencesdialog.cpp \
    src/filespage.cpp \
    src/generalpage.cpp \
    src/shortcutspage.cpp \
    src/timelinepage.cpp \
    src/toolspage.cpp \
    src/preview.cpp \
    src/colorbox.cpp \
    src/colorinspector.cpp \
    src/colorpalettewidget.cpp \
    src/colorwheel.cpp \
    src/filedialog.cpp \
    src/displayoptionwidget.cpp \
    src/pencil2d.cpp \
    src/exportmoviedialog.cpp \
    src/errordialog.cpp \
    src/aboutdialog.cpp \
    src/toolbox.cpp \
    src/tooloptionwidget.cpp \
    src/importexportdialog.cpp \
    src/exportimagedialog.cpp \
    src/importimageseqdialog.cpp \
    src/spinslider.cpp \
    src/doubleprogressdialog.cpp \
    src/colorslider.cpp \
    src/checkupdatesdialog.cpp \
    src/presetdialog.cpp \
    src/app_util.cpp \
    src/commandlineparser.cpp \
    src/commandlineexporter.cpp

FORMS += \
    ui/importimageseqpreview.ui \
    ui/importlayersdialog.ui \
    ui/importpositiondialog.ui \
    ui/mainwindow2.ui \
    ui/onionskin.ui \
    ui/pegbaralignmentdialog.ui \
    ui/timeline2.ui \
    ui/shortcutspage.ui \
    ui/colorinspector.ui \
    ui/colorpalette.ui \
    ui/displayoption.ui \
    ui/errordialog.ui \
    ui/importexportdialog.ui \
    ui/exportmovieoptions.ui \
    ui/exportimageoptions.ui \
    ui/importimageseqoptions.ui \
    ui/tooloptions.ui \
    ui/aboutdialog.ui \
    ui/doubleprogressdialog.ui \
    ui/preferencesdialog.ui \
    ui/generalpage.ui \
    ui/timelinepage.ui \
    ui/filespage.ui \
    ui/toolspage.ui \
    ui/toolboxwidget.ui \
    ui/presetdialog.ui

GIT {
    DEFINES += GIT_EXISTS \
    "GIT_CURRENT_SHA1=$$system(git --git-dir=.git --work-tree=. -C $$_PRO_FILE_PWD_/../ rev-parse HEAD)" \
    "GIT_TIMESTAMP=$$system(git --git-dir=.git --work-tree=. -C $$_PRO_FILE_PWD_/../ log -n 1 --pretty=format:"%cd" --date=format:"%Y-%m-%d_%H:%M:%S")"
}

macx {
    RC_FILE = data/pencil2d.icns

    # Use custom Info.plist
    QMAKE_INFO_PLIST = data/Info.plist

    # Add file icons into the application bundle resources
    FILE_ICONS.files = data/icons/mac_pcl_icon.icns data/icons/mac_pclx_icon.icns
    FILE_ICONS.path = Contents/Resources
    QMAKE_BUNDLE_DATA += FILE_ICONS

    QMAKE_TARGET_BUNDLE_PREFIX += org.pencil2d
}

win32 {
    RC_FILE = data/pencil2d.rc
}

unix:!macx {
    target.path = $${PREFIX}/bin

    bashcompletion.files = data/pencil2d
    bashcompletion.path = $${PREFIX}/share/bash-completion/completions

    zshcompletion.files = data/_pencil2d
    zshcompletion.path = $${PREFIX}/share/zsh/site-functions

    metainfo.files = data/org.pencil2d.Pencil2D.metainfo.xml
    metainfo.path = $${PREFIX}/share/metainfo

    mimepackage.files = data/org.pencil2d.Pencil2D.xml
    mimepackage.path = $${PREFIX}/share/mime/packages

    desktopentry.files = data/org.pencil2d.Pencil2D.desktop
    desktopentry.path = $${PREFIX}/share/applications

    icon.files = data/org.pencil2d.Pencil2D.png
    icon.path = $${PREFIX}/share/icons/hicolor/256x256/apps

    INSTALLS += bashcompletion zshcompletion target metainfo mimepackage desktopentry icon
}

# --- core_lib ---

INCLUDEPATH += $$PWD/../core_lib/src

CONFIG(debug,debug|release) BUILDTYPE = debug
CONFIG(release,debug|release) BUILDTYPE = release

win32-msvc* {
    LIBS += -L$$OUT_PWD/../core_lib/$$BUILDTYPE/ -lcore_lib
    PRE_TARGETDEPS += $$OUT_PWD/../core_lib/$$BUILDTYPE/core_lib.lib
}

win32-g++{
    LIBS += -L$$OUT_PWD/../core_lib/$$BUILDTYPE/ -lcore_lib
    PRE_TARGETDEPS += $$OUT_PWD/../core_lib/$$BUILDTYPE/libcore_lib.a
}

# --- mac os and linux
unix {
  LIBS += -L$$OUT_PWD/../core_lib/ -lcore_lib
  PRE_TARGETDEPS += $$OUT_PWD/../core_lib/libcore_lib.a
}
