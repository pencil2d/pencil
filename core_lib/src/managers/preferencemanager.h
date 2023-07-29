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

#ifndef PREFERENCEMANAGER_H
#define PREFERENCEMANAGER_H

#include <bitset>
#include <QHash>
#include "basemanager.h"
#include "pencildef.h"
#include "preferencesdef.h"

class PreferenceManager : public BaseManager
{
    Q_OBJECT

public:
    explicit PreferenceManager(Editor* editor);
    ~PreferenceManager() override;

    virtual bool init() override;
    Status load(Object*) override;
    Status save(Object*) override;

    void loadPrefs();
    void set(SETTING option, QString value);
    void set(SETTING option, int value);
    void set(SETTING option, bool value);
    void set(SETTING option, float value);

    void turnOn(SETTING option);
    void turnOff(SETTING option);
    bool isOn(SETTING option);

    QString getString(SETTING option);
    int     getInt(SETTING option);
    float getFloat(SETTING option);

signals:
    void optionChanged(SETTING e);

private:
    QHash<int, QString> mStringSet;
    QHash<int, int> mIntegerSet;
    QHash<int, bool> mBooleanSet;
    QHash<int, float> mFloatingPointSet;
};

#endif // PREFERENCEMANAGER_H
