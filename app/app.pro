#-------------------------------------------------
#
# Pencil2D GUI
#
#-------------------------------------------------

! include( ../common.pri ) { error( Could not find the common.pri file! ) }

QT += core widgets gui xml multimedia svg

TEMPLATE = app
TARGET = Pencil2D

CONFIG += qt

DESTDIR = ../bin

RESOURCES += \
    resource/app.qrc \
    ../translations/translations.qrc\
    ../pencil.qrc

INCLUDEPATH += \
    ../core_lib/graphics \
    ../core_lib/graphics/bitmap \
    ../core_lib/graphics/vector \
    ../core_lib/interface \
    ../core_lib/structure \
    ../core_lib/tool \
    ../core_lib/util \
    ../core_lib/ui \
    ../core_lib/managers

HEADERS += \
    mainwindow2.h \
    shortcutfilter.h \
    timeline2.h \
    actioncommands.h \
    preferencesdialog.h \
    shortcutspage.h \
    preview.h \
    colorbox.h \
    colorgrid.h \
    colorgriditem.h \
    colorinspector.h \
    colorpalettewidget.h \
    colorwheel.h \
	colordictionary.h \
    filedialogex.h \
    displayoptionwidget.h \
    pencilapplication.h \
    exportmoviedialog.h \
    app_util.h \
    errordialog.h \
    aboutdialog.h \
    toolbox.h \
    tooloptionwidget.h \
    importexportdialog.h \
    exportimagedialog.h \
    importimageseqdialog.h

SOURCES += \
    main.cpp \
    mainwindow2.cpp \
    shortcutfilter.cpp \
    timeline2.cpp \
    actioncommands.cpp \
    preferencesdialog.cpp \
    shortcutspage.cpp \
    preview.cpp \
    colorbox.cpp \
    colorgrid.cpp \
    colorgriditem.cpp \
    colorinspector.cpp \
    colorpalettewidget.cpp \
    colorwheel.cpp \
    filedialogex.cpp \
    displayoptionwidget.cpp \
    pencilapplication.cpp \
    exportmoviedialog.cpp \
    errordialog.cpp \
    aboutdialog.cpp \
    toolbox.cpp \
    tooloptionwidget.cpp \
    importexportdialog.cpp \
    exportimagedialog.cpp \
    importimageseqdialog.cpp

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
    ui/importimageseqoptions.ui

DEPENDPATH += .

VERSION = "0.6.0"
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

GIT {
    DEFINES += GIT_EXISTS \
    "GIT_CURRENT_SHA1=$$system(git --git-dir=.git --work-tree=. -C $$_PRO_FILE_PWD_/../ rev-parse HEAD)" \
    "GIT_TIMESTAMP=$$system(git --git-dir=.git --work-tree=. -C $$_PRO_FILE_PWD_/../ log -n 1 --pretty=format:"%cd" --date=format:"%Y-%m-%d_%H:%M:%S")"
}

NIGHTLY {
    DEFINES += NIGHTLY_BUILD
}


macx {
    RC_FILE = ../pencil.icns

    # Use custom Info.plist
    QMAKE_INFO_PLIST = ../Info.plist

    # Add file icons into the application bundle resources
    FILE_ICONS.files = ../icons/mac_pcl_icon.icns ../icons/mac_pclx_icon.icns
    FILE_ICONS.path = Contents/Resources
    QMAKE_BUNDLE_DATA += FILE_ICONS
}

win32 {
    CONFIG -= flat
    CONFIG += grouped
    RC_FILE = $$PWD/../pencil.rc
}

linux {
    target.path = $${PREFIX}/bin

    mimepackage.files = ../pencil2d.xml
    mimepackage.path = $${PREFIX}/share/mime/packages

    desktopentry.files = ../pencil2d.desktop
    desktopentry.path = $${PREFIX}/share/applications

    icon.files = ../pencil2d.png
    icon.path = $${PREFIX}/share/icons/hicolor/128x128/apps

    INSTALLS += target mimepackage desktopentry icon
}



# --- core_lib ---
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../core_lib/release/ -lcore_lib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../core_lib/debug/ -lcore_lib
else:unix: LIBS += -L$$OUT_PWD/../core_lib/ -lcore_lib

INCLUDEPATH += $$PWD/../core_lib
DEPENDPATH += $$PWD/../core_lib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core_lib/release/libcore_lib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core_lib/debug/libcore_lib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core_lib/release/core_lib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../core_lib/debug/core_lib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../core_lib/libcore_lib.a


# --- QuaZip ---
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../3rdlib/quazip/release/ -lquazip
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../3rdlib/quazip/debug/ -lquazip
else:unix: LIBS += -L$$OUT_PWD/../3rdlib/quazip/ -lquazip

INCLUDEPATH += $$PWD/../3rdlib/quazip
DEPENDPATH += $$PWD/../3rdlib/quazip

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3rdlib/quazip/release/libquazip.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3rdlib/quazip/debug/libquazip.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3rdlib/quazip/release/quazip.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3rdlib/quazip/debug/quazip.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../3rdlib/quazip/libquazip.a

# --- zlib ---
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../3rdlib/zlib/release/ -lzlib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../3rdlib/zlib/debug/ -lzlib
else:unix: LIBS += -L$$OUT_PWD/../3rdlib/zlib/ -lzlib

INCLUDEPATH += $$PWD/../3rdlib/zlib
DEPENDPATH += $$PWD/../3rdlib/zlib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3rdlib/zlib/release/libzlib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3rdlib/zlib/debug/libzlib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3rdlib/zlib/release/zlib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3rdlib/zlib/debug/zlib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../3rdlib/zlib/libzlib.a
