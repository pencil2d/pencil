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

CONFIG += qt

DESTDIR = ../bin

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

HEADERS += \
    src/mainwindow2.h \
    src/shortcutfilter.h \
    src/timeline2.h \
    src/actioncommands.h \
    src/preferencesdialog.h \
    src/shortcutspage.h \
    src/preview.h \
    src/colorbox.h \
    src/colorinspector.h \
    src/colorpalettewidget.h \
    src/colorwheel.h \
    src/filedialogex.h \
    src/displayoptionwidget.h \
    src/pencilapplication.h \
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
    src/checkupdatesdialog.h

SOURCES += \
    src/main.cpp \
    src/mainwindow2.cpp \
    src/shortcutfilter.cpp \
    src/timeline2.cpp \
    src/actioncommands.cpp \
    src/preferencesdialog.cpp \
    src/shortcutspage.cpp \
    src/preview.cpp \
    src/colorbox.cpp \
    src/colorinspector.cpp \
    src/colorpalettewidget.cpp \
    src/colorwheel.cpp \
    src/filedialogex.cpp \
    src/displayoptionwidget.cpp \
    src/pencilapplication.cpp \
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
    src/checkupdatesdialog.cpp

FORMS += \
    ui/mainwindow2.ui \
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
    ui/toolboxwidget.ui



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

    LIBS += -framework AppKit
}

win32 {
    CONFIG -= flat
    RC_FILE = data/pencil2d.rc
}

linux {
    target.path = $${PREFIX}/bin

    bashcompletion.files = data/pencil2d
    bashcompletion.path = $${PREFIX}/share/bash-completion/completions

    zshcompletion.files = data/_pencil2d
    zshcompletion.path = $${PREFIX}/share/zsh/site-functions

    mimepackage.files = data/pencil2d.xml
    mimepackage.path = $${PREFIX}/share/mime/packages

    desktopentry.files = data/pencil2d.desktop
    desktopentry.path = $${PREFIX}/share/applications

    icon.files = data/pencil2d.png
    icon.path = $${PREFIX}/share/icons/hicolor/256x256/apps

    INSTALLS += bashcompletion zshcompletion target mimepackage desktopentry icon
}



# --- core_lib ---
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core_lib/release/ -lcore_lib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core_lib/debug/ -lcore_lib
else:unix: LIBS += -L$$OUT_PWD/../core_lib/ -lcore_lib

INCLUDEPATH += $$PWD/../core_lib/src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core_lib/release/libcore_lib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core_lib/debug/libcore_lib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core_lib/release/core_lib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core_lib/debug/core_lib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../core_lib/libcore_lib.a
