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

#ifndef PREFERENCESDEF_H
#define PREFERENCESDEF_H

enum class SETTING
{
    ANTIALIAS,
    GRID,
    SHADOW,
    PREV_ONION,
    NEXT_ONION,
    INVISIBLE_LINES,
    OUTLINES,
    ONION_BLUE,
    ONION_RED,
    TOOL_CURSOR,
    CANVAS_CURSOR,
    HIGH_RESOLUTION,
    WINDOW_OPACITY,
    SHOW_STATUS_BAR,
    CURVE_SMOOTHING,
    BACKGROUND_STYLE,
    AUTO_SAVE,
    AUTO_SAVE_NUMBER,
    SHORT_SCRUB,
    FPS,
    FIELD_W,
    FIELD_H,
    FRAME_SIZE,
    TIMELINE_SIZE,
    LABEL_FONT_SIZE,
    DRAW_LABEL,
    ONION_MAX_OPACITY,
    ONION_MIN_OPACITY,
    ONION_PREV_FRAMES_NUM,
    ONION_NEXT_FRAMES_NUM,
    ONION_WHILE_PLAYBACK,
    ONION_TYPE,
    FLIP_ROLL_MSEC,
    FLIP_ROLL_DRAWINGS,
    FLIP_INBETWEEN_MSEC,
    SOUND_SCRUB_ACTIVE,
    SOUND_SCRUB_MSEC,
    LAYER_VISIBILITY,
    LAYER_VISIBILITY_THRESHOLD,
    GRID_SIZE_W,
    GRID_SIZE_H,
    OVERLAY_CENTER,
    OVERLAY_THIRDS,
    OVERLAY_GOLDEN,
    OVERLAY_SAFE,
    OVERLAY_PERSPECTIVE1,
    OVERLAY_PERSPECTIVE2,
    OVERLAY_PERSPECTIVE3,
    OVERLAY_ANGLE,
    OVERLAY_SAFE_HELPER_TEXT_ON,
    ACTION_SAFE_ON,
    ACTION_SAFE,
    TIMECODE_TEXT,
    TITLE_SAFE_ON,
    TITLE_SAFE,
    QUICK_SIZING,
    INVERT_DRAG_ZOOM_DIRECTION,
    INVERT_SCROLL_ZOOM_DIRECTION,
    MULTILAYER_ONION,
    LANGUAGE,
    LAYOUT_LOCK,
    DRAW_ON_EMPTY_FRAME_ACTION,
    FRAME_POOL_SIZE,
    ROTATION_INCREMENT,
    SHOW_SELECTION_INFO,
    ASK_FOR_PRESET,
    LOAD_MOST_RECENT,
    LOAD_DEFAULT_PRESET,
    DEFAULT_PRESET,
    COUNT, // COUNT must always be the last one.
};

// Actions for drawing on an empty frame.
enum DrawOnEmptyFrameAction
{
    CREATE_NEW_KEY,
    DUPLICATE_PREVIOUS_KEY,
    KEEP_DRAWING_ON_PREVIOUS_KEY
};

#endif // PREFERENCESDEF_H
