#-------------------------------------------------
#
# Pencil2D core library
#
#-------------------------------------------------

! include( ../common.pri ) { error( Could not find the common.pri file! ) }

QT += core widgets gui xml xmlpatterns multimedia svg

TEMPLATE = lib
CONFIG += qt staticlib console

RESOURCES += ../pencil.qrc

MOC_DIR = .moc
OBJECTS_DIR = .obj


INCLUDEPATH += . \
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
HEADERS +=  \
    graphics/bitmap/bitmapimage.h \
    graphics/vector/bezierarea.h \
    graphics/vector/beziercurve.h \
    graphics/vector/colourref.h \
    graphics/vector/vectorimage.h \
    graphics/vector/vectorselection.h \
    graphics/vector/vertexref.h \
    interface/backupelement.h \
    interface/editor.h \
    interface/flowlayout.h \
    interface/keycapturelineedit.h \
    interface/recentfilemenu.h \
    interface/scribblearea.h \
    interface/spinslider.h \
    interface/timecontrols.h \
    interface/timeline.h \
    interface/timelinecells.h \
    interface/basedockwidget.h \
    interface/backgroundwidget.h \
    managers/basemanager.h \
    managers/colormanager.h \
    managers/layermanager.h \
    managers/toolmanager.h \
    managers/playbackmanager.h \
    managers/viewmanager.h \
    managers/preferencemanager.h \
    managers/soundmanager.h \
    structure/camera.h \
    structure/keyframe.h \
    structure/layer.h \
    structure/layerbitmap.h \
    structure/layercamera.h \
    structure/layersound.h \
    structure/layervector.h \
    structure/keyframefactory.h \
    structure/soundclip.h \
    structure/object.h \
    structure/objectdata.h \
    structure/filemanager.h \
    tool/basetool.h \
    tool/brushtool.h \
    tool/buckettool.h \
    tool/erasertool.h \
    tool/eyedroppertool.h \
    tool/handtool.h \
    tool/movetool.h \
    tool/penciltool.h \
    tool/pentool.h \
    tool/polylinetool.h \
    tool/selecttool.h \
    tool/smudgetool.h \
    tool/strokemanager.h \
    tool/stroketool.h \
    util/blitrect.h \
    util/fileformat.h \
    util/pencildef.h \
    util/pencilerror.h \
    util/pencilsettings.h \
    util/util.h \
    util/log.h \
    canvasrenderer.h \
    soundplayer.h \
    movieexporter.h


SOURCES +=  graphics/bitmap/bitmapimage.cpp \
    graphics/vector/bezierarea.cpp \
    graphics/vector/beziercurve.cpp \
    graphics/vector/colourref.cpp \
    graphics/vector/vectorimage.cpp \
    graphics/vector/vectorselection.cpp \
    graphics/vector/vertexref.cpp \
    interface/editor.cpp \
    interface/flowlayout.cpp \
    interface/keycapturelineedit.cpp \
    interface/recentfilemenu.cpp \
    interface/scribblearea.cpp \
    interface/spinslider.cpp \
    interface/timecontrols.cpp \
    interface/timeline.cpp \
    interface/timelinecells.cpp \
    interface/basedockwidget.cpp \
    interface/backgroundwidget.cpp \
    managers/basemanager.cpp \
    managers/colormanager.cpp \
    managers/layermanager.cpp \
    managers/toolmanager.cpp \
    managers/preferencemanager.cpp \
    managers/playbackmanager.cpp \
    managers/viewmanager.cpp \
    structure/camera.cpp \
    structure/keyframe.cpp \
    structure/layer.cpp \
    structure/layerbitmap.cpp \
    structure/layercamera.cpp \
    structure/layersound.cpp \
    structure/layervector.cpp \
    structure/object.cpp \
    structure/keyframefactory.cpp \
    structure/soundclip.cpp \
    structure/objectdata.cpp \
    structure/filemanager.cpp \
    tool/basetool.cpp \
    tool/brushtool.cpp \
    tool/buckettool.cpp \
    tool/erasertool.cpp \
    tool/eyedroppertool.cpp \
    tool/handtool.cpp \
    tool/movetool.cpp \
    tool/penciltool.cpp \
    tool/pentool.cpp \
    tool/polylinetool.cpp \
    tool/selecttool.cpp \
    tool/smudgetool.cpp \
    tool/strokemanager.cpp \
    tool/stroketool.cpp \
    util/blitrect.cpp \
    util/fileformat.cpp \
    util/pencilerror.cpp \
    util/pencilsettings.cpp \
    util/util.cpp \
    canvasrenderer.cpp \
    soundplayer.cpp \
    managers/soundmanager.cpp \
    movieexporter.cpp

win32 {
    CONFIG -= flat
	#CONFIG += grouped

    INCLUDEPATH += external/win32
    SOURCES += external/win32/win32.cpp
}

macx {
    INCLUDEPATH += external/macosx
    SOURCES += external/macosx/macosx.cpp
}

unix:!macx {
    INCLUDEPATH += external/linux
    SOURCES += external/linux/linux.cpp
}


INCLUDEPATH += $$PWD/../3rdlib/quazip
DEPENDPATH += $$PWD/../3rdlib/quazip

INCLUDEPATH += $$PWD/../3rdlib/zlib
