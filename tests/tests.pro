#-------------------------------------------------
#
# Project created by QtCreator 2013-09-30T11:52:56
#
#-------------------------------------------------

QT += core widgets gui xml xmlpatterns multimedia svg testlib

TEMPLATE = app

TARGET = tests

CONFIG   += console
CONFIG   -= app_bundle

macx {
    QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
    LIBS += -lz
}

INCLUDEPATH += \
    ../pencil/graphics \
    ../pencil/graphics/bitmap \
    ../pencil/graphics/vector \
    ../pencil/interface \
    ../pencil/structure \
    ../pencil/tool \
    ../pencil/util \
    ../pencil/ui \
    ../pencil/managers

HEADERS += \
    AutoTest.h \
    test_objectsaveloader.h \
    test_layer.h \
    test_layermanager.h

SOURCES += \
    main.cpp \
    test_objectsaveloader.cpp \
    test_layer.cpp \
    test_layermanager.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../pencil/release/ -lpencil
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../pencil/debug/ -lpencil
else:unix: LIBS += -L$$OUT_PWD/../pencil/ -lpencil

INCLUDEPATH += $$PWD/../pencil
DEPENDPATH += $$PWD/../pencil

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../pencil/release/libpencil.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../pencil/debug/libpencil.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../pencil/release/pencil.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../pencil/debug/pencil.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../pencil/libpencil.a
