#-------------------------------------------------
#
# Project created by QtCreator 2013-09-30T11:52:56
#
#-------------------------------------------------

QT       += core gui widgets xml xmlpatterns phonon svg testlib

TARGET = pencil_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include($$PWD/../src/pencil.pri)

HEADERS += \
    AutoTest.h \
    test_objectsaveloader.h

SOURCES += \
    test_objectsaveloader.cpp \
    main.cpp
    
DEFINES += SRCDIR=\\\"$$PWD/\\\"

    
RESOURCES += $$PWD/../pencil.qrc