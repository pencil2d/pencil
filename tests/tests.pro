#-------------------------------------------------
#
# Unit Test Project of Pencil2D
#
#-------------------------------------------------

! include( ../common.pri ) { error( Could not find the common.pri file! ) }

QT += core widgets gui xml xmlpatterns multimedia svg testlib

TEMPLATE = app

TARGET = tests

CONFIG   += console
CONFIG   -= app_bundle

MOC_DIR = .moc
OBJECTS_DIR = .obj

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
    catch.hpp \
    AutoTest.h \
    test_object.h

SOURCES += \
    main.cpp \
    test_layer.cpp \
    test_layermanager.cpp \
    test_object.cpp \
    test_filemanager.cpp \
    test_bitmapimage.cpp \
    test_viewmanager.cpp

# --- CoreLib ---
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

