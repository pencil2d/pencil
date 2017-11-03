
TEMPLATE = lib
CONFIG += qt warn_on
QT -= gui
!win32:VERSION = 1.0.0

DEFINES += QUAZIP_BUILD
DEFINES += QUAZIP_STATIC
CONFIG += staticlib

INCLUDEPATH += ../zlib

QMAKE_CXXFLAGS += -std=c++11 -w
QMAKE_CFLAGS += -w

# Input
include(quazip.pri)

unix:!symbian {
    target.path=$$PREFIX/lib
    INSTALLS += target

    OBJECTS_DIR=.obj
    MOC_DIR=.moc
}

win32 {
    headers.path=$$PREFIX/include/quazip
    headers.files=$$HEADERS
    target.path=$$PREFIX/lib
    INSTALLS += headers target
    # workaround for qdatetime.h macro bug
    DEFINES += NOMINMAX
}


