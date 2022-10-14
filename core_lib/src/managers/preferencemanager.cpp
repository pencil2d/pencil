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

#include "preferencemanager.h"

#include <QSettings>
#include <QDebug>

PreferenceManager::PreferenceManager(Editor* editor) : BaseManager(editor, __FUNCTION__)
{
}

PreferenceManager::~PreferenceManager()
{
}

bool PreferenceManager::init()
{
    loadPrefs();
    return true;
}

Status PreferenceManager::load(Object*)
{
    return Status::OK;
}

Status PreferenceManager::save(Object*)
{
    return Status::OK;
}

void PreferenceManager::loadPrefs()
{
    QSettings settings( PENCIL2D, PENCIL2D );

    // Display
    set(SETTING::GRID,                     settings.value(SETTING_SHOW_GRID,              false).toBool());
    set(SETTING::INVISIBLE_LINES,          settings.value(SETTING_INVISIBLE_LINES,        false).toBool());
    set(SETTING::OUTLINES,                 settings.value(SETTING_OUTLINES,               false).toBool());
    set(SETTING::OVERLAY_CENTER,           settings.value(SETTING_OVERLAY_CENTER,         false).toBool());
    set(SETTING::OVERLAY_THIRDS,           settings.value(SETTING_OVERLAY_THIRDS,         false).toBool());
    set(SETTING::OVERLAY_GOLDEN,           settings.value(SETTING_OVERLAY_GOLDEN,         false).toBool());
    set(SETTING::OVERLAY_SAFE,             settings.value(SETTING_OVERLAY_SAFE,           false).toBool());
    set(SETTING::OVERLAY_PERSPECTIVE1,     settings.value(SETTING_OVERLAY_PERSPECTIVE1,   false).toBool());
    set(SETTING::OVERLAY_PERSPECTIVE2,     settings.value(SETTING_OVERLAY_PERSPECTIVE2,   false).toBool());
    set(SETTING::OVERLAY_PERSPECTIVE3,     settings.value(SETTING_OVERLAY_PERSPECTIVE3,   false).toBool());
    set(SETTING::OVERLAY_ANGLE,            settings.value(SETTING_OVERLAY_ANGLE,          15).toInt());
    set(SETTING::ACTION_SAFE,              settings.value(SETTING_ACTION_SAFE,            5).toInt());
    set(SETTING::ACTION_SAFE_ON,           settings.value(SETTING_ACTION_SAFE_ON,         true).toBool());
    set(SETTING::OVERLAY_SAFE_HELPER_TEXT_ON, settings.value(SETTING_OVERLAY_SAFE_HELPER_TEXT_ON, true).toBool());
    set(SETTING::TITLE_SAFE,               settings.value(SETTING_TITLE_SAFE,             10).toInt());
    set(SETTING::TITLE_SAFE_ON,            settings.value(SETTING_TITLE_SAFE_ON,          true).toBool());

    // Grid
    set(SETTING::GRID_SIZE_W,              settings.value(SETTING_GRID_SIZE_W,            100).toInt());
    set(SETTING::GRID_SIZE_H,              settings.value(SETTING_GRID_SIZE_H,            100).toInt());

    // General
    set(SETTING::ANTIALIAS,                settings.value(SETTING_ANTIALIAS,              true).toBool());
    set(SETTING::TOOL_CURSOR,              settings.value(SETTING_TOOL_CURSOR,            true).toBool());
    set(SETTING::DOTTED_CURSOR,            settings.value(SETTING_DOTTED_CURSOR,          true).toBool());
    set(SETTING::HIGH_RESOLUTION,          settings.value(SETTING_HIGH_RESOLUTION,        true).toBool());
    set(SETTING::SHADOW,                   settings.value(SETTING_SHADOW,                 false).toBool());
    set(SETTING::QUICK_SIZING,             settings.value(SETTING_QUICK_SIZING,           true).toBool());
    set(SETTING::SHOW_SELECTION_INFO,      settings.value(SETTING_SHOW_SELECTION_INFO,    false).toBool());

    set(SETTING::ROTATION_INCREMENT,       settings.value(SETTING_ROTATION_INCREMENT,     15).toInt());

    set(SETTING::WINDOW_OPACITY,           settings.value(SETTING_WINDOW_OPACITY,         0).toInt());
    set(SETTING::SHOW_STATUS_BAR,          settings.value(SETTING_SHOW_STATUS_BAR,        true).toBool());
    set(SETTING::CURVE_SMOOTHING,          settings.value(SETTING_CURVE_SMOOTHING,        20).toInt());

    set(SETTING::BACKGROUND_STYLE,         settings.value(SETTING_BACKGROUND_STYLE,       "white").toString());

    set(SETTING::LAYOUT_LOCK,              settings.value(SETTING_LAYOUT_LOCK,            false).toBool());
    set(SETTING::FRAME_POOL_SIZE,          settings.value(SETTING_FRAME_POOL_SIZE,        1024).toInt());

    set(SETTING::FPS,                      settings.value(SETTING_FPS,                    12).toInt());
    set(SETTING::FIELD_W,                  settings.value(SETTING_FIELD_W,                800).toInt());
    set(SETTING::FIELD_H,                  settings.value(SETTING_FIELD_H,                600).toInt());
    set(SETTING::TIMECODE_TEXT,            settings.value(SETTING_TIMECODE_TEXT,          1).toInt());

    // Files
    set(SETTING::AUTO_SAVE,                settings.value(SETTING_AUTO_SAVE,              false).toBool());
    set(SETTING::AUTO_SAVE_NUMBER,         settings.value(SETTING_AUTO_SAVE_NUMBER,       256).toInt());
    set(SETTING::ASK_FOR_PRESET,           settings.value(SETTING_ASK_FOR_PRESET,         false).toBool());
    set(SETTING::LOAD_MOST_RECENT,         settings.value(SETTING_LOAD_MOST_RECENT,       false).toBool());
    set(SETTING::LOAD_DEFAULT_PRESET,      settings.value(SETTING_LOAD_DEFAULT_PRESET,    true).toBool());
    set(SETTING::DEFAULT_PRESET,           settings.value(SETTING_DEFAULT_PRESET,         0).toInt());

    // Timeline
    set(SETTING::SHORT_SCRUB,              settings.value(SETTING_SHORT_SCRUB,            false ).toBool());
    set(SETTING::FRAME_SIZE,               settings.value(SETTING_FRAME_SIZE,             12).toInt());
    set(SETTING::TIMELINE_SIZE,            settings.value(SETTING_TIMELINE_SIZE,          240).toInt());
    set(SETTING::DRAW_LABEL,               settings.value(SETTING_DRAW_LABEL,             false ).toBool());
    set(SETTING::LABEL_FONT_SIZE,          settings.value(SETTING_LABEL_FONT_SIZE,        12).toInt());

    set( SETTING::DRAW_ON_EMPTY_FRAME_ACTION, settings.value( SETTING_DRAW_ON_EMPTY_FRAME_ACTION,
                                                              KEEP_DRAWING_ON_PREVIOUS_KEY).toInt() );

    // Onion Skin
    set(SETTING::PREV_ONION,               settings.value(SETTING_PREV_ONION,             false).toBool());
    set(SETTING::NEXT_ONION,               settings.value(SETTING_NEXT_ONION,             false).toBool());
    set(SETTING::MULTILAYER_ONION,         settings.value(SETTING_MULTILAYER_ONION,       false).toBool());
    set(SETTING::ONION_BLUE,               settings.value(SETTING_ONION_BLUE,             false).toBool());
    set(SETTING::ONION_RED,                settings.value(SETTING_ONION_RED,              false).toBool());

    set(SETTING::ONION_MAX_OPACITY,        settings.value(SETTING_ONION_MAX_OPACITY,      50).toInt());
    set(SETTING::ONION_MIN_OPACITY,        settings.value(SETTING_ONION_MIN_OPACITY,      20).toInt());
    set(SETTING::ONION_PREV_FRAMES_NUM,    settings.value(SETTING_ONION_PREV_FRAMES_NUM,  5).toInt());
    set(SETTING::ONION_NEXT_FRAMES_NUM,    settings.value(SETTING_ONION_NEXT_FRAMES_NUM,  5).toInt());
    set(SETTING::ONION_WHILE_PLAYBACK,     settings.value(SETTING_ONION_WHILE_PLAYBACK,   0).toInt());
    set(SETTING::ONION_TYPE,               settings.value(SETTING_ONION_TYPE,             "relative").toString());
    set(SETTING::LAYER_VISIBILITY,         settings.value(SETTING_LAYER_VISIBILITY,       2).toInt());
    set(SETTING::LAYER_VISIBILITY_THRESHOLD, settings.value(SETTING_LAYER_VISIBILITY_THRESHOLD, 0.5f).toFloat());

    set(SETTING::FLIP_ROLL_MSEC,           settings.value(SETTING_FLIP_ROLL_MSEC,         100).toInt());
    set(SETTING::FLIP_ROLL_DRAWINGS,       settings.value(SETTING_FLIP_ROLL_DRAWINGS,     5).toInt());
    set(SETTING::FLIP_INBETWEEN_MSEC,      settings.value(SETTING_FLIP_INBETWEEN_MSEC,    100).toInt());
    set(SETTING::SOUND_SCRUB_ACTIVE,       settings.value(SETTING_SOUND_SCRUB_ACTIVE,     false).toBool());
    set(SETTING::SOUND_SCRUB_MSEC,         settings.value(SETTING_SOUND_SCRUB_MSEC,       100).toInt());

    set(SETTING::LANGUAGE,                 settings.value(SETTING_LANGUAGE).toString());
}

void PreferenceManager::turnOn(SETTING option)
{
    set(option, true);
}

void PreferenceManager::turnOff(SETTING option)
{
    set(option, false);
}

bool PreferenceManager::isOn(SETTING option)
{
    int optionId = static_cast<int>(option);
    return mBooleanSet.value(optionId, false);
}

int PreferenceManager::getInt(SETTING option)
{
    int optionId = static_cast<int>(option);
    return mIntegerSet.value(optionId, -1);
}

float PreferenceManager::getFloat(SETTING option)
{
    int optionId = static_cast<int>(option);
    return mFloatingPointSet.value(optionId, -1);
}

QString PreferenceManager::getString(SETTING option)
{
    int optionId = static_cast<int>(option);
    if (mIntegerSet.contains(optionId))
    {
        return QString::number(mIntegerSet.value(optionId, -1));
    }
    else if (mBooleanSet.contains(optionId))
    {
        if (mBooleanSet.value(optionId, false))
        {
            return "true";
        }
        else
        {
            return "false";
        }
    }
    return mStringSet.value(optionId);
}

void PreferenceManager::set(SETTING option, QString value)
{
    QSettings settings(PENCIL2D, PENCIL2D);
    switch (option)
    {
    case SETTING::BACKGROUND_STYLE:
        settings.setValue(SETTING_BACKGROUND_STYLE, value);
        break;
    case SETTING::ONION_TYPE:
        settings.setValue(SETTING_ONION_TYPE, value);
        break;
    case SETTING::LANGUAGE:
        settings.setValue(SETTING_LANGUAGE, value);
        break;
    default:
        break;
    }

    int optionId = static_cast<int>(option);
    if (mStringSet[optionId] != value)
    {
        mStringSet[optionId] = value;
        emit optionChanged(option);
    }
}

void PreferenceManager::set(SETTING option, float value)
{
    QSettings settings(PENCIL2D, PENCIL2D);
    switch(option)
    {
    case SETTING::LAYER_VISIBILITY_THRESHOLD:
        settings.setValue(SETTING_LAYER_VISIBILITY_THRESHOLD, value);
        break;
    default:
        Q_ASSERT(false);
        break;
    }


    int optionId = static_cast<int>(option);

    if (qFuzzyCompare(mFloatingPointSet[optionId], value) == false)
    {
        mFloatingPointSet[optionId] = value;
        emit optionChanged(option);
    }
}

void PreferenceManager::set(SETTING option, int value)
{
    QSettings settings(PENCIL2D, PENCIL2D);
    switch (option)
    {
    case SETTING::WINDOW_OPACITY:
        settings.setValue(SETTING_WINDOW_OPACITY, value);
        break;
    case SETTING::CURVE_SMOOTHING:
        settings.setValue(SETTING_CURVE_SMOOTHING, value);
        break;
    case SETTING::AUTO_SAVE_NUMBER:
        settings.setValue(SETTING_AUTO_SAVE_NUMBER, value);
        break;
    case SETTING::FRAME_SIZE:
        if (value < 4) { value = 4; }
        else if (value > 40) { value = 40; }
        settings.setValue(SETTING_FRAME_SIZE, value);
        break;
    case SETTING::TIMELINE_SIZE:
        if (value < 2) { value = 2; }
        settings.setValue(SETTING_TIMELINE_SIZE, value);
        break;
    case SETTING::LABEL_FONT_SIZE:
        if (value < 12) { value = 12; }
        settings.setValue(SETTING_LABEL_FONT_SIZE, value);
        break;
    case SETTING::ONION_MAX_OPACITY:
        settings.setValue(SETTING_ONION_MAX_OPACITY, value);
        break;
    case SETTING::ONION_MIN_OPACITY:
        settings.setValue(SETTING_ONION_MIN_OPACITY, value);
        break;
    case SETTING::ONION_PREV_FRAMES_NUM:
        settings.setValue(SETTING_ONION_PREV_FRAMES_NUM, value);
        break;
    case SETTING::ONION_NEXT_FRAMES_NUM:
        settings.setValue(SETTING_ONION_NEXT_FRAMES_NUM, value);
        break;
    case SETTING::OVERLAY_ANGLE:
        settings.setValue(SETTING_OVERLAY_ANGLE, value);
        break;
    case SETTING::FLIP_ROLL_MSEC :
        settings.setValue(SETTING_FLIP_ROLL_MSEC, value);
        break;
    case SETTING::FLIP_ROLL_DRAWINGS :
        settings.setValue(SETTING_FLIP_ROLL_DRAWINGS, value);
        break;
    case SETTING::FLIP_INBETWEEN_MSEC :
        settings.setValue(SETTING_FLIP_INBETWEEN_MSEC, value);
        break;
    case SETTING::SOUND_SCRUB_MSEC :
        settings.setValue(SETTING_SOUND_SCRUB_MSEC, value);
        break;
    case SETTING::GRID_SIZE_W:
        settings.setValue(SETTING_GRID_SIZE_W, value);
        break;
    case SETTING::TIMECODE_TEXT:
        settings.setValue(SETTING_TIMECODE_TEXT, value);
        break;
    case SETTING::GRID_SIZE_H:
        settings.setValue(SETTING_GRID_SIZE_H, value);
        break;
    case SETTING::ACTION_SAFE:
        settings.setValue(SETTING_ACTION_SAFE, value);
        break;
    case SETTING::TITLE_SAFE:
        settings.setValue(SETTING_TITLE_SAFE, value);
        break;
    case SETTING::FRAME_POOL_SIZE:
        settings.setValue(SETTING_FRAME_POOL_SIZE, value);
        break;
    case SETTING::DRAW_ON_EMPTY_FRAME_ACTION:
        settings.setValue( SETTING_DRAW_ON_EMPTY_FRAME_ACTION, value);
        break;
    case SETTING::ONION_WHILE_PLAYBACK:
        settings.setValue(SETTING_ONION_WHILE_PLAYBACK, value);
        break;
    case SETTING::ROTATION_INCREMENT:
        settings.setValue(SETTING_ROTATION_INCREMENT, value);
        break;
    case SETTING::FPS:
        settings.setValue(SETTING_FPS, value);
        break;
    case SETTING::FIELD_W:
        settings.setValue(SETTING_FIELD_W, value);
        break;
    case SETTING::FIELD_H:
        settings.setValue(SETTING_FIELD_H, value);
        break;
    case SETTING::LAYER_VISIBILITY:
        settings.setValue(SETTING_LAYER_VISIBILITY, value);
        break;
    case SETTING::DEFAULT_PRESET:
        settings.setValue(SETTING_DEFAULT_PRESET, value);
        break;
    default:
        Q_ASSERT(false);
        break;
    }

    int optionId = static_cast<int>(option);
    if (mIntegerSet[optionId] != value)
    {
        mIntegerSet[optionId] = value;
        emit optionChanged(option);
    }
}

// Set bool value
//
void PreferenceManager::set(SETTING option, bool value)
{
    QSettings settings(PENCIL2D, PENCIL2D);
    switch (option)
    {
    case SETTING::SHOW_STATUS_BAR:
        settings.setValue(SETTING_SHOW_STATUS_BAR, value);
        break;
    case SETTING::ANTIALIAS:
        settings.setValue(SETTING_ANTIALIAS, value);
        break;
    case SETTING::GRID:
        settings.setValue(SETTING_SHOW_GRID, value);
        break;
    case SETTING::OVERLAY_CENTER:
        settings.setValue(SETTING_OVERLAY_CENTER, value);
        break;
    case SETTING::OVERLAY_THIRDS:
        settings.setValue(SETTING_OVERLAY_THIRDS, value);
        break;
    case SETTING::OVERLAY_GOLDEN:
        settings.setValue(SETTING_OVERLAY_GOLDEN, value);
        break;
    case SETTING::OVERLAY_SAFE:
        settings.setValue(SETTING_OVERLAY_SAFE, value);
        break;
    case SETTING::OVERLAY_PERSPECTIVE1:
        settings.setValue(SETTING_OVERLAY_PERSPECTIVE1, value);
        break;
    case SETTING::OVERLAY_PERSPECTIVE2:
        settings.setValue(SETTING_OVERLAY_PERSPECTIVE2, value);
        break;
    case SETTING::OVERLAY_PERSPECTIVE3:
        settings.setValue(SETTING_OVERLAY_PERSPECTIVE3, value);
        break;
    case SETTING::ACTION_SAFE_ON:
        settings.setValue(SETTING_ACTION_SAFE_ON, value);
        break;
    case SETTING::TITLE_SAFE_ON:
        settings.setValue(SETTING_TITLE_SAFE_ON, value);
        break;
    case SETTING::OVERLAY_SAFE_HELPER_TEXT_ON:
        settings.setValue(SETTING_OVERLAY_SAFE_HELPER_TEXT_ON, value);
        break;
    case SETTING::SHADOW:
        settings.setValue(SETTING_SHADOW, value);
        break;
    case SETTING::PREV_ONION:
        settings.setValue(SETTING_PREV_ONION, value);
        break;
    case SETTING::NEXT_ONION:
        settings.setValue(SETTING_NEXT_ONION, value);
        break;
    case SETTING::MULTILAYER_ONION:
        settings.setValue(SETTING_MULTILAYER_ONION, value);
        break;
    case SETTING::INVISIBLE_LINES:
        settings.setValue(SETTING_INVISIBLE_LINES, value);
        break;
    case SETTING::OUTLINES:
        settings.setValue(SETTING_OUTLINES, value);
        break;
    case SETTING::ONION_BLUE:
        settings.setValue(SETTING_ONION_BLUE, value);
        break;
    case SETTING::ONION_RED:
        settings.setValue(SETTING_ONION_RED, value);
        break;
    case SETTING::TOOL_CURSOR:
        settings.setValue(SETTING_TOOL_CURSOR, value);
        break;
    case SETTING::DOTTED_CURSOR:
        settings.setValue(SETTING_DOTTED_CURSOR, value);
        break;
    case SETTING::HIGH_RESOLUTION:
        settings.setValue(SETTING_HIGH_RESOLUTION, value);
        break;
    case SETTING::AUTO_SAVE:
        settings.setValue(SETTING_AUTO_SAVE, value);
        break;
    case SETTING::SHORT_SCRUB:
        settings.setValue(SETTING_SHORT_SCRUB, value);
        break;
    case SETTING::DRAW_LABEL:
        settings.setValue(SETTING_DRAW_LABEL, value);
        break;
    case SETTING::QUICK_SIZING:
        settings.setValue(SETTING_QUICK_SIZING, value);
        break;
    case SETTING::LAYOUT_LOCK:
        settings.setValue(SETTING_LAYOUT_LOCK, value);
        break;
    case SETTING::SHOW_SELECTION_INFO:
        settings.setValue(SETTING_SHOW_SELECTION_INFO, value);
        break;
    case SETTING::SOUND_SCRUB_ACTIVE:
        settings.setValue(SETTING_SOUND_SCRUB_ACTIVE, value);
        break;
    case SETTING::ASK_FOR_PRESET:
        settings.setValue(SETTING_ASK_FOR_PRESET, value);
        break;
    case SETTING::LOAD_MOST_RECENT:
        settings.setValue(SETTING_LOAD_MOST_RECENT, value);
        break;
    case SETTING::LOAD_DEFAULT_PRESET:
        settings.setValue(SETTING_LOAD_DEFAULT_PRESET, value);
        break;
    default:
        Q_ASSERT(false);
        break;
    }

    int optionId = static_cast<int>(option);
    if (mBooleanSet[optionId] != value)
    {
        mBooleanSet[optionId] = value;
        emit optionChanged(option);
    }
}
