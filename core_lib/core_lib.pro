#-------------------------------------------------
#
# Pencil2D core library
#
#-------------------------------------------------

! include( ../util/common.pri ) { error( Could not find the common.pri file! ) }

QT += core widgets gui xml xmlpatterns multimedia svg

TEMPLATE = lib
CONFIG += qt staticlib precompile_header

RESOURCES += data/core_lib.qrc

MOC_DIR = .moc
OBJECTS_DIR = .obj
UI_DIR = .ui

INCLUDEPATH += src \
    src/graphics \
    src/graphics/bitmap \
    src/graphics/vector \
    src/interface \
    src/structure \
    src/tool \
    src/util \
    src/managers \
    src/external

PRECOMPILED_HEADER = src/corelib-pch.h

HEADERS +=  \
    src/corelib-pch.h \
    src/graphics/bitmap/bitmapbucket.h \
    src/graphics/bitmap/bitmapimage.h \
    src/graphics/vector/bezierarea.h \
    src/graphics/vector/beziercurve.h \
    src/graphics/vector/colorref.h \
    src/graphics/vector/vectorimage.h \
    src/graphics/vector/vectorselection.h \
    src/graphics/vector/vertexref.h \
    src/interface/backupelement.h \
    src/interface/camerapropertiesdialog.h \
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
    src/managers/overlaymanager.h \
    src/managers/clipboardmanager.h \
    src/managers/selectionmanager.h \
    src/managers/colormanager.h \
    src/managers/layermanager.h \
    src/managers/toolmanager.h \
    src/managers/playbackmanager.h \
    src/managers/viewmanager.h \
    src/managers/preferencemanager.h \
    src/managers/soundmanager.h \
    src/movieimporter.h \
    src/overlaypainter.h \
    src/structure/camera.h \
    src/structure/keyframe.h \
    src/structure/layer.h \
    src/structure/layerbitmap.h \
    src/structure/layercamera.h \
    src/structure/layersound.h \
    src/structure/layervector.h \
    src/structure/pegbaraligner.h \
    src/structure/soundclip.h \
    src/structure/object.h \
    src/structure/objectdata.h \
    src/structure/filemanager.h \
    src/tool/basetool.h \
    src/tool/brushtool.h \
    src/tool/buckettool.h \
    src/tool/erasertool.h \
    src/tool/eyedroppertool.h \
    src/tool/handtool.h \
    src/tool/movetool.h \
    src/tool/penciltool.h \
    src/tool/pentool.h \
    src/tool/polylinetool.h \
    src/tool/selecttool.h \
    src/tool/smudgetool.h \
    src/tool/strokemanager.h \
    src/tool/stroketool.h \
    src/util/blitrect.h \
    src/util/cameraeasingtype.h \
    src/util/camerafieldoption.h \
    src/util/colordictionary.h \
    src/util/fileformat.h \
    src/util/filetype.h \
    src/util/mathutils.h \
    src/util/pencildef.h \
    src/util/pencilerror.h \
    src/util/pencilsettings.h \
    src/util/util.h \
    src/util/log.h \
    src/util/movemode.h \
    src/util/pointerevent.h \
    src/canvaspainter.h \
    src/soundplayer.h \
    src/movieexporter.h \
    src/miniz.h \
    src/qminiz.h \
    src/activeframepool.h \
    src/external/platformhandler.h \
    src/selectionpainter.h


SOURCES +=  src/graphics/bitmap/bitmapimage.cpp \
    src/graphics/bitmap/bitmapbucket.cpp \
    src/graphics/vector/bezierarea.cpp \
    src/graphics/vector/beziercurve.cpp \
    src/graphics/vector/colorref.cpp \
    src/graphics/vector/vectorimage.cpp \
    src/graphics/vector/vectorselection.cpp \
    src/graphics/vector/vertexref.cpp \
    src/interface/backupelement.cpp \
    src/interface/camerapropertiesdialog.cpp \
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
    src/managers/overlaymanager.cpp \
    src/managers/clipboardmanager.cpp \
    src/managers/selectionmanager.cpp \
    src/managers/colormanager.cpp \
    src/managers/layermanager.cpp \
    src/managers/toolmanager.cpp \
    src/managers/preferencemanager.cpp \
    src/managers/playbackmanager.cpp \
    src/managers/viewmanager.cpp \
    src/managers/soundmanager.cpp \
    src/movieimporter.cpp \
    src/overlaypainter.cpp \
    src/structure/camera.cpp \
    src/structure/keyframe.cpp \
    src/structure/layer.cpp \
    src/structure/layerbitmap.cpp \
    src/structure/layercamera.cpp \
    src/structure/layersound.cpp \
    src/structure/layervector.cpp \
    src/structure/object.cpp \
    src/structure/pegbaraligner.cpp \
    src/structure/soundclip.cpp \
    src/structure/objectdata.cpp \
    src/structure/filemanager.cpp \
    src/tool/basetool.cpp \
    src/tool/brushtool.cpp \
    src/tool/buckettool.cpp \
    src/tool/erasertool.cpp \
    src/tool/eyedroppertool.cpp \
    src/tool/handtool.cpp \
    src/tool/movetool.cpp \
    src/tool/penciltool.cpp \
    src/tool/pentool.cpp \
    src/tool/polylinetool.cpp \
    src/tool/selecttool.cpp \
    src/tool/smudgetool.cpp \
    src/tool/strokemanager.cpp \
    src/tool/stroketool.cpp \
    src/util/blitrect.cpp \
    src/util/fileformat.cpp \
    src/util/pencilerror.cpp \
    src/util/pencilsettings.cpp \
    src/util/log.cpp \
    src/util/util.cpp \
    src/util/pointerevent.cpp \
    src/canvaspainter.cpp \
    src/soundplayer.cpp \
    src/movieexporter.cpp \
    src/miniz.cpp \
    src/qminiz.cpp \
    src/activeframepool.cpp \
    src/selectionpainter.cpp

FORMS += \
    ui/camerapropertiesdialog.ui

win32 {
    INCLUDEPATH += src/external/win32
    SOURCES += src/external/win32/win32.cpp
}

macx {
    INCLUDEPATH += src/external/macosx
    HEADERS += src/external/macosx/macosxnative.h
    SOURCES += src/external/macosx/macosx.cpp
    OBJECTIVE_SOURCES += src/external/macosx/macosxnative.mm
}

unix:!macx {
    INCLUDEPATH += src/external/linux
    SOURCES += src/external/linux/linux.cpp
}
