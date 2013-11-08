
INCLUDEPATH += . src \
            $$PWD/external/flash \
            $$PWD/graphics \
            $$PWD/graphics/bitmap \
            $$PWD/graphics/vector \
            $$PWD/interface \
            $$PWD/structure \
            $$PWD/tool \
            $$PWD/util \
            $$PWD/ui \
            $$PWD/managers

# Input
HEADERS +=  $$PWD/interfaces.h \
    $$PWD/graphics/bitmap/bitmapimage.h \
    $$PWD/graphics/vector/bezierarea.h \
    $$PWD/graphics/vector/beziercurve.h \
    $$PWD/graphics/vector/colourref.h \
    $$PWD/graphics/vector/gradient.h \
    $$PWD/graphics/vector/vectorimage.h \
    $$PWD/graphics/vector/vertexref.h \
    $$PWD/structure/layer.h \
    $$PWD/structure/layerbitmap.h \
    $$PWD/structure/layercamera.h \
    $$PWD/structure/layerimage.h \
    $$PWD/structure/layersound.h \
    $$PWD/structure/layervector.h \
    $$PWD/structure/object.h \
    $$PWD/interface/editor.h \
    $$PWD/interface/colorpalettewidget.h \
    $$PWD/interface/popupcolorpalettewidget.h \
    $$PWD/interface/preferences.h \
    $$PWD/interface/scribblearea.h \
    $$PWD/interface/timeline.h \
    $$PWD/interface/timecontrols.h \
    $$PWD/interface/toolset.h \
    $$PWD/interface/backupelement.h \
    $$PWD/interface/spinslider.h \
    $$PWD/interface/displayoptiondockwidget.h \
    $$PWD/interface/tooloptiondockwidget.h \
    $$PWD/tool/basetool.h \
    $$PWD/tool/pentool.h \
    $$PWD/tool/penciltool.h \
    $$PWD/tool/brushtool.h \
    $$PWD/tool/erasertool.h \
    $$PWD/tool/selecttool.h \
    $$PWD/tool/movetool.h \
    $$PWD/tool/handtool.h \
    $$PWD/tool/smudgetool.h \
    $$PWD/tool/polylinetool.h \
    $$PWD/tool/buckettool.h \
    $$PWD/tool/eyedroppertool.h \
    $$PWD/util/pencilsettings.h \
    $$PWD/util/fileformat.h \
    $$PWD/interface/shortcutspage.h \
    $$PWD/interface/mainwindow2.h \
    $$PWD/interface/timelinecells.h \
    $$PWD/graphics/vector/vectorselection.h \
    $$PWD/util/pencildef.h \
    $$PWD/interface/keycapturelineedit.h \
    $$PWD/structure/objectsaveloader.h \
    $$PWD/tool/strokemanager.h \
    $$PWD/tool/stroketool.h \
    $$PWD/util/blitrect.h \
    $$PWD/interface/colorwheel.h \
    $$PWD/interface/colorinspector.h \
    $$PWD/interface/colorgriditem.h \
    $$PWD/interface/colorgrid.h \
    $$PWD/interface/colorbox.h \
    $$PWD/interface/flowlayout.h \
    $$PWD/structure/keyframe.h \
    $$PWD/structure/camera.h \
    $$PWD/interface/recentfilemenu.h \
    $$PWD/util/util.h \
    $$PWD/managers/colormanager.h \
    $$PWD/managers/toolmanager.h \
    src/util/pencilerror.h

SOURCES +=  $$PWD/graphics/bitmap/blur.cpp \
    $$PWD/graphics/bitmap/bitmapimage.cpp \
    $$PWD/graphics/vector/bezierarea.cpp \
    $$PWD/graphics/vector/beziercurve.cpp \
    $$PWD/graphics/vector/colourref.cpp \
    $$PWD/graphics/vector/gradient.cpp \
    $$PWD/graphics/vector/vectorimage.cpp \
    $$PWD/graphics/vector/vertexref.cpp \
    $$PWD/structure/layer.cpp \
    $$PWD/structure/layerbitmap.cpp \
    $$PWD/structure/layercamera.cpp \
    $$PWD/structure/layerimage.cpp \
    $$PWD/structure/layersound.cpp \
    $$PWD/structure/layervector.cpp \
    $$PWD/structure/object.cpp \
    $$PWD/interface/editor.cpp \
    $$PWD/interface/colorpalettewidget.cpp \
    $$PWD/interface/popupcolorpalettewidget.cpp \
    $$PWD/interface/preferences.cpp \
    $$PWD/interface/scribblearea.cpp \
    $$PWD/interface/timeline.cpp \
    $$PWD/interface/timecontrols.cpp \
    $$PWD/interface/toolset.cpp \
    $$PWD/interface/backupelement.cpp \
    $$PWD/interface/spinslider.cpp \
    $$PWD/interface/displayoptiondockwidget.cpp \
    $$PWD/interface/tooloptiondockwidget.cpp \
    $$PWD/tool/basetool.cpp \
    $$PWD/tool/pentool.cpp \
    $$PWD/tool/penciltool.cpp \
    $$PWD/tool/brushtool.cpp \
    $$PWD/tool/erasertool.cpp \
    $$PWD/tool/selecttool.cpp \
    $$PWD/tool/movetool.cpp \
    $$PWD/tool/handtool.cpp \
    $$PWD/tool/smudgetool.cpp \
    $$PWD/tool/polylinetool.cpp \
    $$PWD/tool/buckettool.cpp \
    $$PWD/tool/eyedroppertool.cpp \
    $$PWD/util/pencilsettings.cpp \
    $$PWD/util/fileformat.cpp \
    $$PWD/interface/shortcutspage.cpp \
    $$PWD/interface/mainwindow2.cpp \
    $$PWD/interface/timelinecells.cpp \
    $$PWD/graphics/vector/vectorselection.cpp \
    $$PWD/interface/keycapturelineedit.cpp \
    $$PWD/structure/objectsaveloader.cpp \
    $$PWD/tool/strokemanager.cpp \
    $$PWD/tool/stroketool.cpp \
    $$PWD/util/blitrect.cpp \
    $$PWD/interface/colorwheel.cpp \
    $$PWD/interface/colorinspector.cpp \
    $$PWD/interface/colorgriditem.cpp \
    $$PWD/interface/colorgrid.cpp \
    $$PWD/interface/colorbox.cpp \
    $$PWD/interface/flowlayout.cpp \
    $$PWD/structure/keyframe.cpp \
    $$PWD/structure/camera.cpp \
    $$PWD/interface/recentfilemenu.cpp \
    $$PWD/util/util.cpp \
    $$PWD/managers/colormanager.cpp \
    $$PWD/managers/toolmanager.cpp \
    src/util/pencilerror.cpp

# Track dependencies for all includes
DEPENDPATH *= $${INCLUDEPATH}

win32 {
    INCLUDEPATH += . libwin32
    SOURCES += $$PWD/external/win32/win32.cpp
    INCLUDEPATH += $$PWD/external/win32
    INCLUDEPATH += $$[QT_INSTALL_PREFIX]/src/3rdparty/zlib
    LIBS += -Llibwin32
    RC_FILE = $$PWD/../pencil.rc
}
macx {
    INCLUDEPATH +=  $$PWD/external/macosx
    LIBS += -lobjc -lz -framework AppKit -framework Carbon
    INCLUDEPATH += . libmacosx
    HEADERS += $$PWD/external/macosx/style.h
    SOURCES += $$PWD/external/macosx/macosx.cpp \
           $$PWD/external/macosx/style.cpp
    RC_FILE = $$PWD/../pencil.icns
}
linux-* {
    INCLUDEPATH += . liblinux
    INCLUDEPATH += $$PWD/external/linux
    SOURCES += $$PWD/external/linux/linux.cpp
    LIBS += -Lliblinux -lming -lpng -lz
    LIBS += -L/usr/local/zlib/lib
    INCLUDEPATH+=/usr/local/zlib/include
}


FORMS += \
    $$PWD/interface/mainwindow2.ui \
    $$PWD/interface/shortcutspage.ui \
    $$PWD/interface/colorinspector.ui

# QuaZip
DEFINES += QUAZIP_STATIC
include($$PWD/external/quazip.pri)
