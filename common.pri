
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
    LIBS += -lpng -lz
    LIBS += -L/usr/local/zlib/lib
    LIBS += -L/usr/lib/x86_64-linux-gnu/
    QMAKE_CXXFLAGS += -std=c++11
}
