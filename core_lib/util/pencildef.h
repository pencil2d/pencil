#ifndef PENCILDEF_H
#define PENCILDEF_H


#define PENCIL_WINDOW_TITLE QString("Pencil2D - Nightly Build %1").arg( __DATE__ )

#define PENCIL_MOVIE_EXT \
    tr( "AVI (*.avi);;MPEG(*.mpg);;MOV(*.mov);;MP4(*.mp4);;SWF(*.swf);;FLV(*.flv);;WMV(*.wmv)" )

#define PENCIL_IMAGE_FILTER \
   tr( "PNG (*.png);;JPG(*.jpg *.jpeg);;TIFF(*.tiff);;TIF(*.tif);;BMP(*.bmp);;GIF(*.gif)" )


enum ToolType
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
    BRUSH
};

enum ToolPropertyType
{
    WIDTH,
    FEATHER,
    PRESSURE,
    INVISIBILITY,
    PRESERVEALPHA,
    BEZIER,
    USEFEATHER
};

enum BackgroundStyle
{

};

// shortcuts command code
#define CMD_NEW_FILE  "CmdNewFile"
#define CMD_OPEN_FILE "CmdOpenFile"
#define CMD_SAVE_FILE "CmdSaveFile"
#define CMD_SAVE_AS "CmdSaveAs"
#define CMD_PRINT "CmdPrint"
#define CMD_EXIT "CmdExit"
#define CMD_IMPORT_IMAGE "CmdImportImage"
#define CMD_IMPORT_IMAGE_SEQ "CmdImportImageSequence"
#define CMD_IMPORT_MOVIE "CmdImportMovie"
#define CMD_IMPORT_PALETTE "CmdImportPalette"
#define CMD_IMPORT_SOUND "CmdImportSound"
#define CMD_EXPORT_XSHEET "CmdExportXsheet"
#define CMD_EXPORT_IMAGE_SEQ "CmdExportImageSequence"
#define CMD_EXPORT_IMAGE "CmdExportImage"
#define CMD_EXPORT_MOVIE "CmdExportMovie"
#define CMD_EXPORT_PALETTE "CmdExportPalette"
#define CMD_EXPORT_SVG "CmdExportSvgImage"
#define CMD_EXPORT_SOUND "CmdExportSound"
#define CMD_UNDO "CmdUndo"
#define CMD_REDO "CmdRedo"
#define CMD_CUT "CmdCut"
#define CMD_COPY "CmdCopy"
#define CMD_PASTE "CmdPaste"
#define CMD_SELECT_ALL "CmdSelectAll"
#define CMD_DESELECT_ALL "CmdDeselectAll"
#define CMD_CLEAR_FRAME "CmdClearFrame"
#define CMD_PREFERENCE "CmdPreferences"
#define CMD_RESET_WINDOWS "CmdResetWindows"
#define CMD_ZOOM_IN "CmdZoomIn"
#define CMD_ZOOM_OUT "CmdZoomOut"
#define CMD_ROTATE_CLOCK "CmdRotateClockwise"
#define CMD_ROTATE_ANTI_CLOCK "CmdRotateAntiClosewise"
#define CMD_RESET_ZOOM_ROTATE "CmdResetZoomRotate"
#define CMD_FLIP_HORIZONTAL "CmdFlipHorizontal"
#define CMD_FLIP_VERTICAL "CmdFlipVertical"
#define CMD_PREVIEW "CmdPreview"
#define CMD_GRID "CmdGrid"
#define CMD_ONIONSKIN_PREV "CmdOnionSkinPrevious"
#define CMD_ONIONSKIN_NEXT "CmdOnionSkinNext"
#define CMD_PLAY "CmdPlay"
#define CMD_LOOP "CmdLoop"
#define CMD_GOTO_NEXT_FRAME "CmdGotoNextFrame"
#define CMD_GOTO_PREV_FRAME "CmdGotoPreviousFrame"
#define CMD_GOTO_NEXT_KEY_FRAME "CmdGotoNextKeyFrame"
#define CMD_GOTO_PREV_KEY_FRAME "CmdGotoPreviousKeyFrame"
#define CMD_ADD_FRAME "CmdAddFrame"
#define CMD_DUPLICATE_FRAME "CmdDuplicateFrame"
#define CMD_REMOVE_FRAME "CmdRemoveFrame"
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
#define CMD_TOGGLE_PALETTE "CmdTogglePalette"
#define CMD_NEW_BITMAP_LAYER "CmdNewBitmapLayer"
#define CMD_NEW_VECTOR_LAYER "CmdNewVectorLayer"
#define CMD_NEW_SOUND_LAYER "CmdNewSoundLayer"
#define CMD_NEW_CAMERA_LAYER "CmdNewCameraLayer"
#define CMD_DELETE_CUR_LAYER "CmdDeleteCurrentLayer"
#define CMD_HELP "CmdHelp"
#define CMD_TOGGLE_TOOLBOX "CmdToggleToolBox"
#define CMD_TOGGLE_TOOL_OPTIONS "CmdToggleToolOptions"
#define CMD_TOGGLE_COLOR_WHEEL "CmdToggleColorWheel"
#define CMD_TOGGLE_COLOR_LIBRARY "CmdToggleColorLibrary"
#define CMD_TOGGLE_DISPLAY_OPTIONS "CmdToggleDisplayOptions"
#define CMD_TOGGLE_TIMELINE "CmdToggleTimeline"
#define CMD_INCREASE_SIZE "CmdIncreaseSize"
#define CMD_DECREASE_SIZE "CmdDecreaseSize"

// Save / Export
#define LAST_FILE_PATH          "LastFilePath"

// Settings Group/Key Name
#define PENCIL2D "Pencil"
#define SHORTCUTS_GROUP             "Shortcuts"
#define SETTING_AUTO_SAVE           "AutoSave"
#define SETTING_AUTO_SAVE_NUMBER    "AutosaveNumber"
#define SETTING_TOOL_CURSOR         "ToolCursors"
#define SETTING_HIGH_RESOLUTION     "HighResPosition"
#define SETTING_BACKGROUND_STYLE    "Background"
#define SETTING_WINDOW_OPACITY      "WindowOpacity"
#define SETTING_WINDOW_GEOMETRY     "WindowGeometry"
#define SETTING_WINDOW_STATE        "WindowState"
#define SETTING_CURVE_SMOOTHING     "CurveSmoothing"
#define SETTING_DISPLAY_EFFECT      "RenderEffect"
#define SETTING_SHORT_SCRUB         "ShortScrub"
#define SETTING_FRAME_SIZE          "FrameSize"
#define SETTING_TIMELINE_SIZE       "TimelineSize"
#define SETTING_LABEL_FONT_SIZE     "LabelFontSize"
#define SETTING_DRAW_LABEL          "DrawLabel"
#define SETTING_QUICK_SIZING        "QuickSizing"

#define SETTING_ANTIALIAS       "Antialiasing"
#define SETTING_SHOW_GRID       "ShowGrid"
#define SETTING_COUNT           "Count"
#define SETTING_SHADOW          "Shadow"
#define SETTING_PREV_ONION      "PrevOnion"
#define SETTING_NEXT_ONION      "NextOnion"
#define SETTING_AXIS            "Axis"
#define SETTING_CAMERABORDER    "CameraBorder"
#define SETTING_INVISIBLE_LINES "InvisibleLines"
#define SETTING_OUTLINES        "Outlines"
#define SETTING_ONION_BLUE      "OnionBlue"
#define SETTING_ONION_RED       "OnionRed"
#define SETTING_MIRROR_H        "MirrorH"
#define SETTING_MIRROR_V        "MirrorV"

#define SETTING_GRID_SIZE       "GridSize"

#define SETTING_ONION_MAX_OPACITY       "OnionMaxOpacity"
#define SETTING_ONION_MIN_OPACITY       "OnionMinOpacity"
#define SETTING_ONION_PREV_FRAMES_NUM   "OnionPrevFramesNum"
#define SETTING_ONION_NEXT_FRAMES_NUM   "OnionNextFramesNum"
#define SETTING_ONION_TYPE              "OnionType"

#endif // PENCILDEF_H
