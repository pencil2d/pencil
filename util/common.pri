
# Development branch build number is always 0.0.0.0
# Nightly build version number is 99.0.0.BuildNumber
# Release build version number is the git branch name plus the build number.

isEmpty(VERSION) {
    VERSION = 0.0.0.0
}

message("App Version: $$VERSION")

DEFINES    += APP_VERSION=\\\"$$VERSION\\\"
RC_DEFINES += APP_VERSION=\\\"$$VERSION\\\"
RC_DEFINES += APP_VERSION_RC=$$replace(VERSION, "\.", ",")

PENCIL2D_NIGHTLY {
    DEFINES += PENCIL2D_NIGHTLY_BUILD
}

PENCIL2D_RELEASE {
    DEFINES += QT_NO_DEBUG_OUTPUT
    DEFINES += PENCIL2D_RELEASE_BUILD
}

CONFIG += strict_c strict_c++
greaterThan(QT_MAJOR_VERSION, 5) {
    CONFIG += c++17
} else {
    CONFIG += c++11
}

# utf8_source is only for Qt 5, it is the default since Qt 6
CONFIG += msvc_mp utf8_source

win32-msvc*:CONFIG(release,debug|release): CONFIG += force_debug_info
win32:!WIN_LEGACY: DEFINES += _WIN32_WINNT=0x0601
WIN_LEGACY {
    QMAKE_LFLAGS += /SUBSYSTEM:CONSOLE,5.01
    QMAKE_CXX += /D_USING_V110_SDK71_
    DEFINES += _WIN32_WINNT=0x0501
}

macx: LIBS += -lobjc -framework Carbon -framework AppKit

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050600    # disables all the APIs deprecated before Qt 5.6.0
