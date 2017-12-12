
VERSION = 0.6.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

CONFIG += c++11

win32-g++ {
    QMAKE_CXXFLAGS += -std=c++11
}

win32-msvc* {
    QMAKE_CXXFLAGS += /MP
}

macx {
   QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
   LIBS += -lobjc -framework AppKit -framework Carbon
}

unix:!macx {
    QMAKE_CXXFLAGS += -std=c++11
    QMAKE_LINK = $$QMAKE_CXX
    QMAKE_LINK_SHLIB = $$QMAKE_CXX
}

DEFINES += QUAZIP_BUILD
DEFINES += QUAZIP_STATIC
