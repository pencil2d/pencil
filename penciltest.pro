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

include(pencil.pri)

SOURCES += tests/test_objectsaveloader.cpp \
    tests/main.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    tests/AutoTest.h \
    tests/test_objectsaveloader.h
