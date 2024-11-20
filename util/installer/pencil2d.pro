TEMPLATE = lib
# Flags based on WiX example project
CONFIG += shared static_runtime exceptions_off optimize_size
QMAKE_CXXFLAGS_RELEASE += -guard:cf -Gy -Oi
QMAKE_LFLAGS_RELEASE += /GUARD:CF
DEFINES += _WIN32_MSI=500 _WIN32_WINNT=0x0600
QT -= core gui
INCLUDEPATH += WixToolset.DUtil/build/native/include \
               WixToolset.BALUtil/build/native/include \
               WixToolset.BootstrapperCore.Native/build/native/include
equals(QMAKE_TARGET.arch, "x86") {
    LIBS += "-L$$PWD/WixToolset.DUtil/build/native/v14/x86" \
            "-L$$PWD/WixToolset.BALUtil/build/native/v14/x86"
}
equals(QMAKE_TARGET.arch, "x86_64") {
    LIBS += "-L$$PWD/WixToolset.DUtil/build/native/v14/x64" \
            "-L$$PWD/WixToolset.BALUtil/build/native/v14/x64"
}
LIBS += User32.lib Advapi32.lib Ole32.lib OleAut32.lib Version.lib balutil.lib dutil.lib
SOURCES += pencil2d.cpp
DEF_FILE = pencil2d.def
