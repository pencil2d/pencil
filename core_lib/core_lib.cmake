# Core Library Sources
# This file is included by the root CMakeLists.txt
# It defines source file lists that will be compiled into the main executable

# Set sources
set(CORE_LIB_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/canvascursorpainter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/corelib-pch.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/bitmap/bitmapbucket.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/bitmap/bitmapimage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/bitmap/tile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/bitmap/tiledbuffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/vector/bezierarea.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/vector/beziercurve.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/vector/colorref.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/vector/vectorimage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/vector/vectorselection.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/vector/vertexref.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/interface/editor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/interface/flowlayout.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/interface/legacybackupelement.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/interface/recentfilemenu.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/interface/scribblearea.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/interface/backgroundwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/interface/toolboxlayout.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/interface/undoredocommand.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/basemanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/overlaymanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/clipboardmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/selectionmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/colormanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/layermanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/toolmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/playbackmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/undoredomanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/viewmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/preferencemanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/soundmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/movieimporter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/onionskinsubpainter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/overlaypainter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/camerapainter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/camera.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/keyframe.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/layer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/layerbitmap.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/layercamera.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/layersound.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/layervector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/pegbaraligner.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/soundclip.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/object.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/objectdata.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/filemanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/basetool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/brushtool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/buckettool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/cameratool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/erasertool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/eyedroppertool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/handtool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/movetool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/penciltool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/pentool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/polylinetool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/selecttool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/smudgetool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/strokeinterpolator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/stroketool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/blitrect.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/cameraeasingtype.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/camerafieldoption.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/colordictionary.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/fileformat.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/filetype.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/importimageconfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/mathutils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/onionskinpainteroptions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/onionskinpaintstate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/painterutils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/pencildef.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/pencilerror.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/pencilsettings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/preferencesdef.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/transform.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/util.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/log.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/movemode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/pointerevent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/canvaspainter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/soundplayer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/movieexporter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/miniz.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/qminiz.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/activeframepool.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/external/platformhandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/selectionpainter.h
)

set(CORE_LIB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/bitmap/bitmapimage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/canvascursorpainter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/bitmap/bitmapbucket.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/bitmap/tile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/bitmap/tiledbuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/vector/bezierarea.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/vector/beziercurve.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/vector/colorref.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/vector/vectorimage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/vector/vectorselection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/vector/vertexref.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/interface/editor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/interface/flowlayout.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/interface/legacybackupelement.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/interface/recentfilemenu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/interface/scribblearea.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/interface/backgroundwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/interface/toolboxlayout.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/interface/undoredocommand.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/basemanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/overlaymanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/clipboardmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/selectionmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/colormanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/layermanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/toolmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/preferencemanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/playbackmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/undoredomanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/viewmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers/soundmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/movieimporter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/keyframe.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/layer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/layerbitmap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/layercamera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/layersound.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/layervector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/object.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/pegbaraligner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/soundclip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/objectdata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure/filemanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/basetool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/brushtool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/buckettool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/cameratool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/erasertool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/eyedroppertool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/handtool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/movetool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/penciltool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/pentool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/polylinetool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/selecttool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/smudgetool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/strokeinterpolator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool/stroketool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/blitrect.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/cameraeasingtype.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/fileformat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/pencilerror.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/pencilsettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/transform.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util/pointerevent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/canvaspainter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/overlaypainter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/onionskinsubpainter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/camerapainter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/soundplayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/movieexporter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/miniz.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/qminiz.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/activeframepool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/selectionpainter.cpp
)

# Platform-specific sources
if(WIN32)
    list(APPEND CORE_LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/external/win32/win32.cpp)
endif()

if(APPLE)
    list(APPEND CORE_LIB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/external/macosx/macosxnative.h)
    list(APPEND CORE_LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/external/macosx/macosx.cpp)
    list(APPEND CORE_LIB_OBJCXX_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/external/macosx/macosxnative.mm)
    enable_language(OBJCXX)
endif()

if(UNIX AND NOT APPLE)
    list(APPEND CORE_LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/external/linux/linux.cpp)
endif()

# Resources
set(CORE_LIB_RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/data/core_lib.qrc
)

# Include directories for core library
set(CORE_LIB_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/bitmap
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/graphics/vector
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/interface
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/structure
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/tool
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/util
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/external
)

# Add platform-specific include directories
if(WIN32)
    list(APPEND CORE_LIB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/external/win32)
endif()

if(APPLE)
    list(APPEND CORE_LIB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/external/macosx)
endif()

if(UNIX AND NOT APPLE)
    list(APPEND CORE_LIB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/src/external/linux)
endif()
