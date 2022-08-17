#-------------------------------------------------
#
# Unit Test Project of Pencil2D
#
#-------------------------------------------------

! include( ../util/common.pri ) { error( Could not find the common.pri file! ) }

QT += core widgets gui xml xmlpatterns multimedia svg testlib

TEMPLATE = app

TARGET = tests

CONFIG += console
CONFIG -= app_bundle

MOC_DIR = .moc
OBJECTS_DIR = .obj
DESTDIR = bin

RESOURCES += data/tests.qrc

INCLUDEPATH += \
    ../core_lib/src/graphics \
    ../core_lib/src/graphics/bitmap \
    ../core_lib/src/graphics/vector \
    ../core_lib/src/interface \
    ../core_lib/src/structure \
    ../core_lib/src/tool \
    ../core_lib/src/util \
    ../core_lib/ui \
    ../core_lib/src/managers

HEADERS += \
    src/catch.hpp

SOURCES += \
    src/main.cpp \
    src/test_colormanager.cpp \
    src/test_layer.cpp \
    src/test_layermanager.cpp \
    src/test_object.cpp \
    src/test_filemanager.cpp \
    src/test_bitmapimage.cpp \
    src/test_bitmapbucket.cpp \
    src/test_vectorimage.cpp \
    src/test_viewmanager.cpp

# --- core_lib ---

INCLUDEPATH += $$PWD/../core_lib/src

CONFIG(debug,debug|release) BUILDTYPE = debug
CONFIG(release,debug|release) BUILDTYPE = release

win32-msvc* {
    LIBS += -L$$OUT_PWD/../core_lib/$$BUILDTYPE/ -lcore_lib
    PRE_TARGETDEPS += $$OUT_PWD/../core_lib/$$BUILDTYPE/core_lib.lib
}

win32-g++ {
    LIBS += -L$$OUT_PWD/../core_lib/$$BUILDTYPE/ -lcore_lib
    PRE_TARGETDEPS += $$OUT_PWD/../core_lib/$$BUILDTYPE/libcore_lib.a
}

# --- mac os and linux
unix {
    LIBS += -L$$OUT_PWD/../core_lib/ -lcore_lib
    PRE_TARGETDEPS += $$OUT_PWD/../core_lib/libcore_lib.a
}
