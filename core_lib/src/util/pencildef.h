/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef PENCILDEF_H
#define PENCILDEF_H

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define S__GIT_TIMESTAMP TOSTRING(GIT_TIMESTAMP)
#define S__GIT_COMMIT_HASH TOSTRING(GIT_CURRENT_SHA1)

enum ToolType : int
{
    INVALID_TOOL = -1,
    PENCIL = 0,
    ERASER,
    SELECT,
    MOVE,
    HAND,
    SMUDGE,
    PEN,
    POLYLINE,
    BUCKET,
    EYEDROPPER,
    BRUSH,
    TOOL_TYPE_COUNT
};

enum ToolPropertyType
{
    WIDTH,
    FEATHER,
    PRESSURE,
    INVISIBILITY,
    PRESERVEALPHA,
    BEZIER,
    USEFEATHER,
    VECTORMERGE,
    ANTI_ALIASING,
    FILL_MODE,
    STABILIZATION,
    TOLERANCE,
    FILLCONTOUR,
    SHOWSELECTIONINFO,
    USETOLERANCE,
    BUCKETFILLEXPAND,
    USEBUCKETFILLEXPAND,
    BUCKETFILLLAYERMODE,
    BUCKETFILLLAYERREFERENCEMODE,
};

enum BackgroundStyle
{

};

enum StabilizationLevel
{
    NONE,
    SIMPLE,
    STRONG
};

enum TimecodeTextLevel
{
    NOTEXT,
    FRAMES, // FF
    SMPTE,  // HH:MM:SS:FF
    SFF     // S:FF
};

enum class LayerVisibility
{
    CURRENTONLY = 0,
    RELATED = 1,
    ALL = 2,
    // If you are adding new enum values here, be sure to update the ++/-- operators below
};

inline LayerVisibility& operator++(LayerVisibility& vis)
{
    return vis = (vis == LayerVisibility::ALL) ? LayerVisibility::CURRENTONLY : static_cast<LayerVisibility>(static_cast<int>(vis)+1);
}

inline LayerVisibility& operator--(LayerVisibility& vis)
{
    return vis = (vis == LayerVisibility::CURRENTONLY) ? LayerVisibility::ALL : static_cast<LayerVisibility>(static_cast<int>(vis)-1);
}

// Max frames that can be imported and loaded onto the timeline
const static int MaxFramesBound = 9999;

// shortcuts command code
#define CMD_NEW_FILE  "CmdNewFile"
#define CMD_OPEN_FILE "CmdOpenFile"
#define CMD_SAVE_FILE "CmdSaveFile"
#define CMD_SAVE_AS "CmdSaveAs"
#define CMD_IMPORT_IMAGE "CmdImportImage"
#define CMD_IMPORT_IMAGE_SEQ "CmdImportImageSequence"
#define CMD_IMPORT_MOVIE_VIDEO "CmdImportMovieVideo"
#define CMD_IMPORT_MOVIE_AUDIO "CmdImportMovieAudio"
#define CMD_IMPORT_PALETTE "CmdImportPalette"
#define CMD_IMPORT_SOUND "CmdImportSound"
#define CMD_EXPORT_IMAGE_SEQ "CmdExportImageSequence"
#define CMD_EXPORT_IMAGE "CmdExportImage"
#define CMD_EXPORT_MOVIE "CmdExportMovie"
#define CMD_EXPORT_PALETTE "CmdExportPalette"
#define CMD_EXPORT_SOUND "CmdExportSound"
#define CMD_UNDO "CmdUndo"
#define CMD_REDO "CmdRedo"
#define CMD_CUT "CmdCut"
#define CMD_COPY "CmdCopy"
#define CMD_PASTE "CmdPaste"
#define CMD_PASTE_FROM_PREVIOUS "CmdPasteFromPrevious"
#define CMD_SELECT_ALL "CmdSelectAll"
#define CMD_DESELECT_ALL "CmdDeselectAll"
#define CMD_CLEAR_FRAME "CmdClearFrame"
#define CMD_PREFERENCE "CmdPreferences"
#define CMD_RESET_WINDOWS "CmdResetWindows"
#define CMD_ZOOM_IN "CmdZoomIn"
#define CMD_ZOOM_OUT "CmdZoomOut"
#define CMD_ROTATE_CLOCK "CmdRotateClockwise"
#define CMD_ROTATE_ANTI_CLOCK "CmdRotateAntiClockwise"
#define CMD_RESET_ROTATION "CmdResetRotation"
#define CMD_RESET_ZOOM_ROTATE "CmdResetZoomRotate"
#define CMD_CENTER_VIEW "CmdCenterView"
#define CMD_ZOOM_400 "CmdZoom400"
#define CMD_ZOOM_300 "CmdZoom300"
#define CMD_ZOOM_200 "CmdZoom200"
#define CMD_ZOOM_100 "CmdZoom100"
#define CMD_ZOOM_50 "CmdZoom50"
#define CMD_ZOOM_33 "CmdZoom33"
#define CMD_ZOOM_25 "CmdZoom25"
#define CMD_FLIP_HORIZONTAL "CmdFlipHorizontal"
#define CMD_FLIP_VERTICAL "CmdFlipVertical"
#define CMD_PREVIEW "CmdPreview"
#define CMD_GRID "CmdGrid"
#define CMD_ONIONSKIN_PREV "CmdOnionSkinPrevious"
#define CMD_ONIONSKIN_NEXT "CmdOnionSkinNext"
#define CMD_TOGGLE_STATUS_BAR "CmdToggleStatusBar"
#define CMD_PLAY "CmdPlay"
#define CMD_LOOP "CmdLoop"
#define CMD_FLIP_INBETWEEN "CmdFlipInBetween"
#define CMD_FLIP_ROLLING "CmdFlipRolling"
#define CMD_GOTO_NEXT_FRAME "CmdGotoNextFrame"
#define CMD_GOTO_PREV_FRAME "CmdGotoPreviousFrame"
#define CMD_GOTO_NEXT_KEY_FRAME "CmdGotoNextKeyFrame"
#define CMD_GOTO_PREV_KEY_FRAME "CmdGotoPreviousKeyFrame"
#define CMD_ADD_FRAME "CmdAddFrame"
#define CMD_DUPLICATE_FRAME "CmdDuplicateFrame"
#define CMD_REMOVE_FRAME "CmdRemoveFrame"
#define CMD_REVERSE_SELECTED_FRAMES "CmdReverseSelectedFrames"
#define CMD_REMOVE_SELECTED_FRAMES "CmdRemoveSelectedFrames"
#define CMD_SELECTION_ADD_FRAME_EXPOSURE "CmdSelectionAddFrameExposure"
#define CMD_SELECTION_SUBTRACT_FRAME_EXPOSURE "CmdSelectionSubtractFrameExposure"
#define CMD_MOVE_FRAME_BACKWARD "CmdMoveFrameBackward"
#define CMD_MOVE_FRAME_FORWARD "CmdMoveFrameForward"
#define CMD_TOOL_MOVE "CmdToolMove"
#define CMD_TOOL_SELECT "CmdToolSelect"
#define CMD_TOOL_BRUSH "CmdToolBrush"
#define CMD_TOOL_POLYLINE "CmdToolPolyline"
#define CMD_TOOL_SMUDGE "CmdToolSmudge"
#define CMD_TOOL_PEN "CmdToolPen"
#define CMD_TOOL_HAND "CmdToolHand"
#define CMD_TOOL_PENCIL "CmdToolPencil"
#define CMD_TOOL_BUCKET "CmdToolBucket"
#define CMD_TOOL_EYEDROPPER "CmdToolEyedropper"
#define CMD_TOOL_ERASER "CmdToolEraser"
#define CMD_NEW_BITMAP_LAYER "CmdNewBitmapLayer"
#define CMD_NEW_VECTOR_LAYER "CmdNewVectorLayer"
#define CMD_NEW_SOUND_LAYER "CmdNewSoundLayer"
#define CMD_NEW_CAMERA_LAYER "CmdNewCameraLayer"
#define CMD_DELETE_CUR_LAYER "CmdDeleteCurrentLayer"
#define CMD_CURRENT_LAYER_VISIBILITY "CmdLayerVisibilityCurrentOnly"
#define CMD_RELATIVE_LAYER_VISIBILITY "CmdLayerVisibilityRelative"
#define CMD_ALL_LAYER_VISIBILITY "CmdLayerVisibilityAll"
#define CMD_HELP "CmdHelp"
#define CMD_TOGGLE_TOOLBOX "CmdToggleToolBox"
#define CMD_TOGGLE_TOOL_OPTIONS "CmdToggleToolOptions"
#define CMD_TOGGLE_COLOR_WHEEL "CmdToggleColorWheel"
#define CMD_TOGGLE_COLOR_INSPECTOR "CmdToggleColorInspector"
#define CMD_TOGGLE_COLOR_LIBRARY "CmdToggleColorLibrary"
#define CMD_TOGGLE_DISPLAY_OPTIONS "CmdToggleDisplayOptions"
#define CMD_TOGGLE_ONION_SKIN "CmdToggleOnionSkin"
#define CMD_TOGGLE_TIMELINE "CmdToggleTimeline"
#define CMD_INCREASE_SIZE "CmdIncreaseSize"
#define CMD_DECREASE_SIZE "CmdDecreaseSize"
#define CMD_EXIT "CmdExit"

// Save / Export
#define LAST_PCLX_PATH          "LastFilePath"

// Import
#define IMPORT_REPOSITION_TYPE      "ImportRepositionType"

// Settings Group/Key Name
#define PENCIL2D "Pencil"
#define SHORTCUTS_GROUP             "Shortcuts"
#define SETTING_AUTO_SAVE           "AutoSave"
#define SETTING_AUTO_SAVE_NUMBER    "AutosaveNumber"
#define SETTING_TOOL_CURSOR         "ToolCursors"
#define SETTING_DOTTED_CURSOR       "DottedCursors"
#define SETTING_HIGH_RESOLUTION     "HighResPosition"
#define SETTING_BACKGROUND_STYLE    "Background"
#define SETTING_WINDOW_OPACITY      "WindowOpacity"
#define SETTING_WINDOW_GEOMETRY     "WindowGeometry"
#define SETTING_WINDOW_STATE        "WindowState"
#define SETTING_SHOW_STATUS_BAR     "ShowStatusBar"
#define SETTING_CURVE_SMOOTHING     "CurveSmoothing"
#define SETTING_DISPLAY_EFFECT      "RenderEffect"
#define SETTING_SHORT_SCRUB         "ShortScrub"
#define SETTING_FPS                 "Fps"
#define SETTING_FIELD_W             "FieldW"
#define SETTING_FIELD_H             "FieldH"
#define SETTING_FRAME_SIZE          "FrameSize"
#define SETTING_TIMELINE_SIZE       "TimelineSize"
#define SETTING_LABEL_FONT_SIZE     "LabelFontSize"
#define SETTING_DRAW_LABEL          "DrawLabel"
#define SETTING_QUICK_SIZING        "QuickSizing"
#define SETTING_LAYOUT_LOCK         "LayoutLock"
#define SETTING_ROTATION_INCREMENT  "RotationIncrement"
#define SETTING_SHOW_SELECTION_INFO "ShowSelectionInfo"
#define SETTING_ASK_FOR_PRESET      "AskForPreset"
#define SETTING_LOAD_MOST_RECENT    "LoadMostRecent"
#define SETTING_LOAD_DEFAULT_PRESET "LoadDefaultPreset"
#define SETTING_DEFAULT_PRESET      "DefaultPreset"

#define SETTING_ANTIALIAS        "Antialiasing"
#define SETTING_SHOW_GRID        "ShowGrid"
#define SETTING_COUNT            "Count"
#define SETTING_SHADOW           "Shadow"
#define SETTING_PREV_ONION       "PrevOnion"
#define SETTING_NEXT_ONION       "NextOnion"
#define SETTING_MULTILAYER_ONION "MultilayerOnion"
#define SETTING_AXIS             "Axis"
#define SETTING_CAMERABORDER     "CameraBorder"
#define SETTING_INVISIBLE_LINES  "InvisibleLines"
#define SETTING_OUTLINES         "Outlines"
#define SETTING_ONION_BLUE       "OnionBlue"
#define SETTING_ONION_RED        "OnionRed"

#define SETTING_FRAME_POOL_SIZE  "FramePoolSizeInMB"
#define SETTING_GRID_SIZE_W      "GridSizeW"
#define SETTING_GRID_SIZE_H      "GridSizeH"
#define SETTING_OVERLAY_CENTER   "OverlayCenter"
#define SETTING_OVERLAY_THIRDS   "OverlayThirds"
#define SETTING_OVERLAY_GOLDEN   "OverlayGolden"
#define SETTING_OVERLAY_SAFE     "OverlaySafe"
#define SETTING_OVERLAY_PERSPECTIVE1 "OverlayPerspective1"
#define SETTING_OVERLAY_PERSPECTIVE2 "OverlayPerspective2"
#define SETTING_OVERLAY_PERSPECTIVE3 "OverlayPerspective3"
#define SETTING_OVERLAY_ANGLE    "OverlayAngle"
#define SETTING_TITLE_SAFE_ON    "TitleSafeOn"
#define SETTING_TITLE_SAFE       "TitleSafe"
#define SETTING_ACTION_SAFE_ON   "ActionSafeOn"
#define SETTING_ACTION_SAFE      "ActionSafe"
#define SETTING_OVERLAY_SAFE_HELPER_TEXT_ON "OverlaySafeHelperTextOn"
#define SETTING_TIMECODE_TEXT    "TimecodeText"

#define SETTING_ONION_MAX_OPACITY       "OnionMaxOpacity"
#define SETTING_ONION_MIN_OPACITY       "OnionMinOpacity"
#define SETTING_ONION_PREV_FRAMES_NUM   "OnionPrevFramesNum"
#define SETTING_ONION_NEXT_FRAMES_NUM   "OnionNextFramesNum"
#define SETTING_ONION_WHILE_PLAYBACK    "OnionWhilePlayback"
#define SETTING_ONION_TYPE              "OnionType"
#define SETTING_FLIP_ROLL_MSEC          "FlipRoll"
#define SETTING_FLIP_ROLL_DRAWINGS      "FlipRollDrawings"
#define SETTING_FLIP_INBETWEEN_MSEC     "FlipInbetween"
#define SETTING_SOUND_SCRUB_ACTIVE      "SoundScrubActive"
#define SETTING_SOUND_SCRUB_MSEC        "SoundScrubMsec"

// Ideally this should also BucketTolerance eg.. but for compatibility sake, i'm not changing it now
#define SETTING_BUCKET_TOLERANCE "Tolerance"
#define SETTING_BUCKET_TOLERANCE_ON "BucketToleranceEnabled"
#define SETTING_BUCKET_FILL_EXPAND "BucketFillExpand"
#define SETTING_BUCKET_FILL_EXPAND_ON "BucketFillExpandEnabled"
#define SETTING_BUCKET_FILL_TO_LAYER_MODE "BucketFillToLayerMode"
#define SETTING_BUCKET_FILL_REFERENCE_MODE "BucketFillReferenceMode"

#define SETTING_FILL_MODE "FillMode"

#define SETTING_LAYER_VISIBILITY "LayerVisibility"
#define SETTING_LAYER_VISIBILITY_THRESHOLD "LayerVisibilityThreshold"

#define SETTING_DRAW_ON_EMPTY_FRAME_ACTION  "DrawOnEmptyFrameAction"

#define SETTING_LANGUAGE        "Language"

#endif // PENCILDEF_H
