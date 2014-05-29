
CONFIG += c++11

win32 {
    INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtZlib
}

win32-g++ {
    QMAKE_CXXFLAGS += -std=c++11
}

win32-msvc* {
    QMAKE_CXXFLAGS += /MP
}

macx {
   QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
   LIBS += -lobjc -lz -framework AppKit -framework Carbon
}

linux-* {
    INCLUDEPATH += /usr/local/zlib/include
    LIBS += -L/usr/local/zlib/lib
    QMAKE_CXXFLAGS += -std=c++11
    QMAKE_LINK = $$QMAKE_CXX
    QMAKE_LINK_SHLIB = $$QMAKE_CXX
}

DEFINES += QUAZIP_BUILD
DEFINES += QUAZIP_STATIC
