#-------------------------------------------------
#
# Pencil2D core library
#
#-------------------------------------------------

! include( ../common.pri ) { error( Could not find the common.pri file! ) }

QT += core widgets gui xml xmlpatterns multimedia svg

TEMPLATE = lib
CONFIG += qt staticlib

RESOURCES += data/core_lib.qrc

MOC_DIR = .moc
OBJECTS_DIR = .obj


INCLUDEPATH += src \
    src/graphics \
    src/graphics/bitmap \
    src/graphics/vector \
    src/interface \
    src/structure \
    src/tools \
    src/util \
    ui \
    src/managers \
    src/external

# Input
HEADERS +=  \
    src/graphics/bitmap/bitmapimage.h \
    src/graphics/vector/bezierarea.h \
    src/graphics/vector/beziercurve.h \
    src/graphics/vector/colourref.h \
    src/graphics/vector/vectorimage.h \
    src/graphics/vector/vectorselection.h \
    src/graphics/vector/vertexref.h \
    src/interface/backupelement.h \
    src/interface/editor.h \
    src/interface/flowlayout.h \
    src/interface/recentfilemenu.h \
    src/interface/scribblearea.h \
    src/interface/timecontrols.h \
    src/interface/timeline.h \
    src/interface/timelinecells.h \
    src/interface/basedockwidget.h \
    src/interface/backgroundwidget.h \
    src/managers/basemanager.h \
    src/managers/selectionmanager.h \
    src/managers/colormanager.h \
    src/managers/layermanager.h \
    src/managers/toolmanager.h \
    src/managers/playbackmanager.h \
    src/managers/viewmanager.h \
    src/managers/preferencemanager.h \
    src/managers/soundmanager.h \
    src/structure/camera.h \
    src/structure/keyframe.h \
    src/structure/layer.h \
    src/structure/layerbitmap.h \
    src/structure/layercamera.h \
    src/structure/layersound.h \
    src/structure/layervector.h \
    src/structure/soundclip.h \
    src/structure/object.h \
    src/structure/objectdata.h \
    src/structure/filemanager.h \
    src/tools/basetool.h \
    src/tools/bitmapbrushtool.h \
    src/tools/bitmapbuckettool.h \
    src/tools/bitmaperasertool.h \
    src/tools/bitmapeyedroppertool.h \
    src/tools/handtool.h \
    src/tools/bitmapmovetool.h \
    src/tools/bitmappenciltool.h \
    src/tools/bitmappentool.h \
    src/tools/bitmappolylinetool.h \
    src/tools/bitmapselecttool.h \
    src/tools/bitmapsmudgetool.h \
    src/tools/notool.h \
    src/tools/strokemanager.h \
    src/tools/stroketool.h \
    src/tools/vectorbrushtool.h \
    src/tools/vectorbuckettool.h \
    src/tools/vectorerasertool.h \
    src/tools/vectoreyedroppertool.h \
    src/tools/vectormovetool.h \
    src/tools/vectorpenciltool.h \
    src/tools/vectorpentool.h \
    src/tools/vectorpolylinetool.h \
    src/tools/vectorselecttool.h \
    src/tools/vectorsmudgetool.h \
    src/util/blitrect.h \
    src/util/colordictionary.h \
    src/util/fileformat.h \
    src/util/mathutils.h \
    src/util/pencildef.h \
    src/util/pencilerror.h \
    src/util/pencilsettings.h \
    src/util/util.h \
    src/util/log.h \
    src/util/movemode.h \
    src/canvaspainter.h \
    src/soundplayer.h \
    src/movieexporter.h \
    src/miniz.h \
    src/qminiz.h \
    src/activeframepool.h \
    src/external/platformhandler.h \
    src/external/macosx/macosxnative.h \
    src/util/pointerevent.h \
    src/selectionpainter.h


SOURCES +=  src/graphics/bitmap/bitmapimage.cpp \
    src/graphics/vector/bezierarea.cpp \
    src/graphics/vector/beziercurve.cpp \
    src/graphics/vector/colourref.cpp \
    src/graphics/vector/vectorimage.cpp \
    src/graphics/vector/vectorselection.cpp \
    src/graphics/vector/vertexref.cpp \
    src/interface/editor.cpp \
    src/interface/flowlayout.cpp \
    src/interface/recentfilemenu.cpp \
    src/interface/scribblearea.cpp \
    src/interface/timecontrols.cpp \
    src/interface/timeline.cpp \
    src/interface/timelinecells.cpp \
    src/interface/basedockwidget.cpp \
    src/interface/backgroundwidget.cpp \
    src/managers/basemanager.cpp \
    src/managers/selectionmanager.cpp \
    src/managers/colormanager.cpp \
    src/managers/layermanager.cpp \
    src/managers/toolmanager.cpp \
    src/managers/preferencemanager.cpp \
    src/managers/playbackmanager.cpp \
    src/managers/viewmanager.cpp \
    src/structure/camera.cpp \
    src/structure/keyframe.cpp \
    src/structure/layer.cpp \
    src/structure/layerbitmap.cpp \
    src/structure/layercamera.cpp \
    src/structure/layersound.cpp \
    src/structure/layervector.cpp \
    src/structure/object.cpp \
    src/structure/soundclip.cpp \
    src/structure/objectdata.cpp \
    src/structure/filemanager.cpp \
    src/tools/basetool.cpp \
    src/tools/bitmapbrushtool.cpp \
    src/tools/bitmapbuckettool.cpp \
    src/tools/bitmaperasertool.cpp \
    src/tools/bitmapeyedroppertool.cpp \
    src/tools/handtool.cpp \
    src/tools/bitmapmovetool.cpp \
    src/tools/bitmappenciltool.cpp \
    src/tools/bitmappentool.cpp \
    src/tools/bitmappolylinetool.cpp \
    src/tools/bitmapselecttool.cpp \
    src/tools/bitmapsmudgetool.cpp \
    src/tools/notool.cpp \
    src/tools/strokemanager.cpp \
    src/tools/stroketool.cpp \
    src/tools/vectorbrushtool.cpp \
    src/tools/vectorbuckettool.cpp \
    src/tools/vectorerasertool.cpp \
    src/tools/vectoreyedroppertool.cpp \
    src/tools/vectormovetool.cpp \
    src/tools/vectorpenciltool.cpp \
    src/tools/vectorpentool.cpp \
    src/tools/vectorpolylinetool.cpp \
    src/tools/vectorselecttool.cpp \
    src/tools/vectorsmudgetool.cpp \
    src/util/blitrect.cpp \
    src/util/fileformat.cpp \
    src/util/pencilerror.cpp \
    src/util/pencilsettings.cpp \
    src/util/util.cpp \
    src/canvaspainter.cpp \
    src/soundplayer.cpp \
    src/managers/soundmanager.cpp \
    src/movieexporter.cpp \
    src/miniz.cpp \
    src/qminiz.cpp \
    src/activeframepool.cpp \
    src/util/pointerevent.cpp \
    src/selectionpainter.cpp

FORMS += \
    ui/camerapropertiesdialog.ui

win32 {
    CONFIG -= flat

    INCLUDEPATH += src/external/win32
    SOURCES += src/external/win32/win32.cpp
}

macx {
    INCLUDEPATH += src/external/macosx
    LIBS += -framework AppKit
    SOURCES += src/external/macosx/macosx.cpp
    OBJECTIVE_SOURCES += src/external/macosx/macosxnative.mm
}

unix:!macx {
    INCLUDEPATH += src/external/linux
    SOURCES += src/external/linux/linux.cpp
}
