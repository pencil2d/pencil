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

RESOURCES += \
    ../pencil.qrc \
    resource/app.qrc

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
    exportimageseqdialog.h \
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
    filedialogex.h \
    displayoptionwidget.h
    # popupcolorpalettewidget.h

SOURCES += \
    main.cpp \
    mainwindow2.cpp \
    shortcutfilter.cpp \
    exportimageseqdialog.cpp \
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
    displayoptionwidget.cpp
    # spopupcolorpalettewidget.cpp

FORMS += \
    ui/mainwindow2.ui \
    ui/exportimageseqdialog.ui \
    ui/timeline2.ui \
    ui/shortcutspage.ui \
    ui/colorinspector.ui \
    ui/colorpalette.ui \
    ui/displayoption.ui

DEPENDPATH += .

macx {
    RC_FILE = ../pencil.icns
}

win32 {
    CONFIG -= flat
    CONFIG += grouped
    RC_FILE = $$PWD/../pencil.rc
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
