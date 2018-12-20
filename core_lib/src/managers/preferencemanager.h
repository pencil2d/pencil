/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef PREFERENCEMANAGER_H
#define PREFERENCEMANAGER_H

#include <bitset>
#include <QHash>
#include "basemanager.h"
#include "pencildef.h"


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
    DOTTED_CURSOR,
    HIGH_RESOLUTION,
    WINDOW_OPACITY,
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
    GRID_SIZE_W,
    GRID_SIZE_H,
    QUICK_SIZING,
    MULTILAYER_ONION,
    LANGUAGE,
    LAYOUT_LOCK,
    DRAW_ON_EMPTY_FRAME_ACTION,
    FRAME_POOL_SIZE,
    COUNT, // COUNT must always be the last one.
};

// Actions for drawing on an empty frame.
enum DrawOnEmptyFrameAction
{
    CREATE_NEW_KEY,
    DUPLICATE_PREVIOUS_KEY,
    KEEP_DRAWING_ON_PREVIOUS_KEY
};

class PreferenceManager : public BaseManager
{
    Q_OBJECT

public:
    explicit PreferenceManager(Editor* editor);
    ~PreferenceManager();

    virtual bool init() override;
    Status load(Object*) override;
    Status save(Object*) override;

    void loadPrefs();
    void set(SETTING option, QString value);
    void set(SETTING option, int value);
    void set(SETTING option, bool value);

    void turnOn(SETTING option);
    void turnOff(SETTING option);
    bool isOn(SETTING option);

    QString getString(SETTING option);
    int     getInt(SETTING option);

Q_SIGNALS:
    void optionChanged(SETTING e);

private:
    QHash<int, QString> mStringSet;
    QHash<int, int> mIntegerSet;
    QHash<int, bool> mBooleanSet;
};

#endif // PREFERENCEMANAGER_H
