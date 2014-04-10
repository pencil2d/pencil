

QT += core widgets gui xml xmlpatterns multimedia svg

TEMPLATE = lib
TARGET = pencil
CONFIG += qt debug console depend_includepath staticlib

RESOURCES += ../pencil.qrc

TRANSLATIONS += ../pencil.ts \
                ../pencil2d_it.ts

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}
updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
isEmpty(vcproj):updateqm.variable_out = PRE_TARGETDEPS
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm
PRE_TARGETDEPS += compiler_updateqm_make_all


INCLUDEPATH += external/flash \
            graphics \
            graphics/bitmap \
            graphics/vector \
            interface \
            structure \
            tool \
            util \
            ui \
            managers

# Input
HEADERS +=  interfaces.h \
    graphics/bitmap/bitmapimage.h \
    graphics/vector/bezierarea.h \
    graphics/vector/beziercurve.h \
    graphics/vector/colourref.h \
    graphics/vector/vectorimage.h \
    graphics/vector/vertexref.h \
    structure/layer.h \
    structure/layerbitmap.h \
    structure/layercamera.h \
    structure/layerimage.h \
    structure/layersound.h \
    structure/layervector.h \
    structure/object.h \
    interface/editor.h \
    interface/colorpalettewidget.h \
    interface/popupcolorpalettewidget.h \
    interface/preferences.h \
    interface/scribblearea.h \
    interface/timeline.h \
    interface/timecontrols.h \
    interface/toolbox.h \
    interface/backupelement.h \
    interface/spinslider.h \
    interface/displayoptiondockwidget.h \
    interface/tooloptiondockwidget.h \
    tool/basetool.h \
    tool/pentool.h \
    tool/penciltool.h \
    tool/brushtool.h \
    tool/erasertool.h \
    tool/selecttool.h \
    tool/movetool.h \
    tool/handtool.h \
    tool/smudgetool.h \
    tool/polylinetool.h \
    tool/buckettool.h \
    tool/eyedroppertool.h \
    util/pencilsettings.h \
    util/fileformat.h \
    interface/shortcutspage.h \
    interface/mainwindow2.h \
    interface/timelinecells.h \
    graphics/vector/vectorselection.h \
    util/pencildef.h \
    interface/keycapturelineedit.h \
    structure/objectsaveloader.h \
    tool/strokemanager.h \
    tool/stroketool.h \
    util/blitrect.h \
    interface/colorwheel.h \
    interface/colorinspector.h \
    interface/colorgriditem.h \
    interface/colorgrid.h \
    interface/colorbox.h \
    interface/flowlayout.h \
    structure/keyframe.h \
    structure/camera.h \
    interface/recentfilemenu.h \
    util/util.h \
    managers/colormanager.h \
    managers/toolmanager.h \
    managers/layermanager.h \
    util/pencilerror.h \
    managers/basemanager.h


SOURCES +=  graphics/bitmap/blur.cpp \
    graphics/bitmap/bitmapimage.cpp \
    graphics/vector/bezierarea.cpp \
    graphics/vector/beziercurve.cpp \
    graphics/vector/colourref.cpp \
    graphics/vector/vectorimage.cpp \
    graphics/vector/vertexref.cpp \
    structure/layer.cpp \
    structure/layerbitmap.cpp \
    structure/layercamera.cpp \
    structure/layerimage.cpp \
    structure/layersound.cpp \
    structure/layervector.cpp \
    structure/object.cpp \
    interface/editor.cpp \
    interface/colorpalettewidget.cpp \
    interface/popupcolorpalettewidget.cpp \
    interface/preferences.cpp \
    interface/scribblearea.cpp \
    interface/timeline.cpp \
    interface/timecontrols.cpp \
    interface/toolbox.cpp \
    interface/backupelement.cpp \
    interface/spinslider.cpp \
    interface/displayoptiondockwidget.cpp \
    interface/tooloptiondockwidget.cpp \
    tool/basetool.cpp \
    tool/pentool.cpp \
    tool/penciltool.cpp \
    tool/brushtool.cpp \
    tool/erasertool.cpp \
    tool/selecttool.cpp \
    tool/movetool.cpp \
    tool/handtool.cpp \
    tool/smudgetool.cpp \
    tool/polylinetool.cpp \
    tool/buckettool.cpp \
    tool/eyedroppertool.cpp \
    util/pencilsettings.cpp \
    util/fileformat.cpp \
    interface/shortcutspage.cpp \
    interface/mainwindow2.cpp \
    interface/timelinecells.cpp \
    graphics/vector/vectorselection.cpp \
    interface/keycapturelineedit.cpp \
    structure/objectsaveloader.cpp \
    tool/strokemanager.cpp \
    tool/stroketool.cpp \
    util/blitrect.cpp \
    interface/colorwheel.cpp \
    interface/colorinspector.cpp \
    interface/colorgriditem.cpp \
    interface/colorgrid.cpp \
    interface/colorbox.cpp \
    interface/flowlayout.cpp \
    structure/keyframe.cpp \
    structure/camera.cpp \
    interface/recentfilemenu.cpp \
    util/util.cpp \
    managers/colormanager.cpp \
    managers/toolmanager.cpp \
    managers/layermanager.cpp \
    util/pencilerror.cpp \
    managers/basemanager.cpp


FORMS += \
    interface/mainwindow2.ui \
    interface/shortcutspage.ui \
    interface/colorinspector.ui


win32 {
    INCLUDEPATH += . libwin32
    SOURCES += external/win32/win32.cpp
    INCLUDEPATH += external/win32
    INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtZlib
    LIBS += -Llibwin32
    RC_FILE = ../pencil.rc
}
win32-g++ {
    QMAKE_CXXFLAGS += -std=c++11
}

macx {
    INCLUDEPATH +=  external/macosx
    LIBS += -lobjc -lz -framework AppKit -framework Carbon
    INCLUDEPATH += . libmacosx
    HEADERS += external/macosx/style.h
    SOURCES += external/macosx/macosx.cpp \
           external/macosx/style.cpp
    RC_FILE = ../pencil.icns
    QMAKE_CXXFLAGS += -std=c++11
}
linux-* {
    INCLUDEPATH += . liblinux
    INCLUDEPATH += external/linux
    SOURCES += external/linux/linux.cpp
    LIBS += -Lliblinux -lming -lpng -lz
    LIBS += -L/usr/local/zlib/lib
    INCLUDEPATH+=/usr/local/zlib/include
    QMAKE_CXXFLAGS += -std=c++11
}


# QuaZip
DEFINES += QUAZIP_STATIC
include(external/quazip.pri)
