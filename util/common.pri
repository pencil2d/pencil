
VERSION = 0.6.6
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

PENCIL2D_NIGHTLY {
    DEFINES += PENCIL2D_NIGHTLY_BUILD
}

PENCIL2D_RELEASE {
    DEFINES += QT_NO_DEBUG_OUTPUT
    DEFINES += PENCIL2D_RELEASE_BUILD
}

CONFIG += c++11

win32-g++ {
    QMAKE_CXXFLAGS += -std=c++11
}

win32-msvc* {
    QMAKE_CXXFLAGS += /MP /utf-8 
    CONFIG(release) {
        QMAKE_CXXFLAGS += /Gy /GL 
        CONFIG += ltcg
        CONFIG += force_debug_info
    }
}

macx {
    QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
    LIBS += -lobjc -framework Carbon -framework AppKit
}

unix:!macx {
    QMAKE_CXXFLAGS += -std=c++11
    QMAKE_LINK = $$QMAKE_CXX
    QMAKE_LINK_SHLIB = $$QMAKE_CXX
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050600    # disables all the APIs deprecated before Qt 5.6.0
