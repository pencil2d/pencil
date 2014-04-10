
QT += core widgets gui xml multimedia svg

TEMPLATE = app

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

SOURCES += main.cpp

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
