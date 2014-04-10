
INCLUDEPATH += src/external/flash \
            src/graphics \
            src/graphics/bitmap \
            src/graphics/vector \
            src/interface \
            src/structure \
            src/tool \
            src/util \
            src/ui \
            src/managers

# Input
HEADERS +=  src/interfaces.h \
    src/graphics/bitmap/bitmapimage.h \
    src/graphics/vector/bezierarea.h \
    src/graphics/vector/beziercurve.h \
    src/graphics/vector/colourref.h \
    src/graphics/vector/vectorimage.h \
    src/graphics/vector/vertexref.h \
    src/structure/layer.h \
    src/structure/layerbitmap.h \
    src/structure/layercamera.h \
    src/structure/layerimage.h \
    src/structure/layersound.h \
    src/structure/layervector.h \
    src/structure/object.h \
    src/interface/editor.h \
    src/interface/colorpalettewidget.h \
    src/interface/popupcolorpalettewidget.h \
    src/interface/preferences.h \
    src/interface/scribblearea.h \
    src/interface/timeline.h \
    src/interface/timecontrols.h \
    src/interface/toolbox.h \
    src/interface/backupelement.h \
    src/interface/spinslider.h \
    src/interface/displayoptiondockwidget.h \
    src/interface/tooloptiondockwidget.h \
    src/tool/basetool.h \
    src/tool/pentool.h \
    src/tool/penciltool.h \
    src/tool/brushtool.h \
    src/tool/erasertool.h \
    src/tool/selecttool.h \
    src/tool/movetool.h \
    src/tool/handtool.h \
    src/tool/smudgetool.h \
    src/tool/polylinetool.h \
    src/tool/buckettool.h \
    src/tool/eyedroppertool.h \
    src/util/pencilsettings.h \
    src/util/fileformat.h \
    src/interface/shortcutspage.h \
    src/interface/mainwindow2.h \
    src/interface/timelinecells.h \
    src/graphics/vector/vectorselection.h \
    src/util/pencildef.h \
    src/interface/keycapturelineedit.h \
    src/structure/objectsaveloader.h \
    src/tool/strokemanager.h \
    src/tool/stroketool.h \
    src/util/blitrect.h \
    src/interface/colorwheel.h \
    src/interface/colorinspector.h \
    src/interface/colorgriditem.h \
    src/interface/colorgrid.h \
    src/interface/colorbox.h \
    src/interface/flowlayout.h \
    src/structure/keyframe.h \
    src/structure/camera.h \
    src/interface/recentfilemenu.h \
    src/util/util.h \
    src/managers/colormanager.h \
    src/managers/toolmanager.h \
    src/managers/layermanager.h \
    src/util/pencilerror.h \
    src/managers/basemanager.h


SOURCES +=  src/graphics/bitmap/blur.cpp \
    src/graphics/bitmap/bitmapimage.cpp \
    src/graphics/vector/bezierarea.cpp \
    src/graphics/vector/beziercurve.cpp \
    src/graphics/vector/colourref.cpp \
    src/graphics/vector/vectorimage.cpp \
    src/graphics/vector/vertexref.cpp \
    src/structure/layer.cpp \
    src/structure/layerbitmap.cpp \
    src/structure/layercamera.cpp \
    src/structure/layerimage.cpp \
    src/structure/layersound.cpp \
    src/structure/layervector.cpp \
    src/structure/object.cpp \
    src/interface/editor.cpp \
    src/interface/colorpalettewidget.cpp \
    src/interface/popupcolorpalettewidget.cpp \
    src/interface/preferences.cpp \
    src/interface/scribblearea.cpp \
    src/interface/timeline.cpp \
    src/interface/timecontrols.cpp \
    src/interface/toolbox.cpp \
    src/interface/backupelement.cpp \
    src/interface/spinslider.cpp \
    src/interface/displayoptiondockwidget.cpp \
    src/interface/tooloptiondockwidget.cpp \
    src/tool/basetool.cpp \
    src/tool/pentool.cpp \
    src/tool/penciltool.cpp \
    src/tool/brushtool.cpp \
    src/tool/erasertool.cpp \
    src/tool/selecttool.cpp \
    src/tool/movetool.cpp \
    src/tool/handtool.cpp \
    src/tool/smudgetool.cpp \
    src/tool/polylinetool.cpp \
    src/tool/buckettool.cpp \
    src/tool/eyedroppertool.cpp \
    src/util/pencilsettings.cpp \
    src/util/fileformat.cpp \
    src/interface/shortcutspage.cpp \
    src/interface/mainwindow2.cpp \
    src/interface/timelinecells.cpp \
    src/graphics/vector/vectorselection.cpp \
    src/interface/keycapturelineedit.cpp \
    src/structure/objectsaveloader.cpp \
    src/tool/strokemanager.cpp \
    src/tool/stroketool.cpp \
    src/util/blitrect.cpp \
    src/interface/colorwheel.cpp \
    src/interface/colorinspector.cpp \
    src/interface/colorgriditem.cpp \
    src/interface/colorgrid.cpp \
    src/interface/colorbox.cpp \
    src/interface/flowlayout.cpp \
    src/structure/keyframe.cpp \
    src/structure/camera.cpp \
    src/interface/recentfilemenu.cpp \
    src/util/util.cpp \
    src/managers/colormanager.cpp \
    src/managers/toolmanager.cpp \
    src/managers/layermanager.cpp \
    src/util/pencilerror.cpp \
    src/managers/basemanager.cpp

win32 {
    INCLUDEPATH += . libwin32
    SOURCES += src/external/win32/win32.cpp
    INCLUDEPATH += src/external/win32
    INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtZlib
    LIBS += -Llibwin32
    RC_FILE = src/../pencil.rc
}
win32-g++ {
    QMAKE_CXXFLAGS += -std=c++11
}

macx {
    INCLUDEPATH +=  src/external/macosx
    LIBS += -lobjc -lz -framework AppKit -framework Carbon
    INCLUDEPATH += . libmacosx
    HEADERS += src/external/macosx/style.h
    SOURCES += src/external/macosx/macosx.cpp \
           src/external/macosx/style.cpp
    RC_FILE = src/../pencil.icns
    QMAKE_CXXFLAGS += -std=c++11
}
linux-* {
    INCLUDEPATH += . liblinux
    INCLUDEPATH += src/external/linux
    SOURCES += src/external/linux/linux.cpp
    LIBS += -Lliblinux -lming -lpng -lz
    LIBS += -L/usr/local/zlib/lib
    INCLUDEPATH+=/usr/local/zlib/include
    QMAKE_CXXFLAGS += -std=c++11
}


FORMS += \
    src/interface/mainwindow2.ui \
    src/interface/shortcutspage.ui \
    src/interface/colorinspector.ui

# QuaZip
DEFINES += QUAZIP_STATIC
include(external/quazip.pri)
