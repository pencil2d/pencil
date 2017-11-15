/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "pencilsettings.h"

#include <QStringList>
#include <QDebug>


// ==== Singleton ====

QSettings& pencilSettings()
{
    static QSettings settings(PENCIL2D, PENCIL2D);

    if ( !settings.contains("InitPencilSetting") )
    {
        restoreToDefaultSetting();
        settings.setValue("InitPencilSetting", true);
    }
    return settings;
}

void restoreToDefaultSetting() // TODO: finish reset list
{
    QSettings s(PENCIL2D, PENCIL2D);

    s.setValue("penWidth", 2.0);
    s.setValue("pencilWidth", 4.0);
    s.setValue("polyLineWidth", 1.5);
    s.setValue("eraserWidth", 10.0);
    s.setValue("brushWidth", 15.0);
    s.setValue("brushFeather", 15.0);
    s.setValue("brushUseFeather", true);

    s.setValue(SETTING_AUTO_SAVE_NUMBER, 15);
    s.setValue(SETTING_TOOL_CURSOR, true);

    s.sync();
    qDebug("restored default tools");
}

void checkExistingShortcuts()
{
    QSettings defaultKey(":resources/kb.ini", QSettings::IniFormat);

    QSettings curSetting( PENCIL2D, PENCIL2D );
    foreach (QString pShortcutsKey, defaultKey.allKeys())
    {
        if ( ! curSetting.contains( pShortcutsKey ) )
        {
            curSetting.setValue(pShortcutsKey, defaultKey.value(pShortcutsKey));
        }
    }

    curSetting.beginGroup(SHORTCUTS_GROUP);
    defaultKey.beginGroup(SHORTCUTS_GROUP);
    foreach (QString pKey, curSetting.allKeys())
    {
        if ( !defaultKey.contains(pKey) )
        {
            curSetting.remove(pKey);
        }
    }
    defaultKey.endGroup();
    curSetting.endGroup();
    curSetting.sync();
}

void restoreShortcutsToDefault()
{
    QSettings defaultKey(":resources/kb.ini", QSettings::IniFormat);

    QSettings curSetting( PENCIL2D, PENCIL2D );
    curSetting.remove("shortcuts");

    foreach (QString pShortcutsKey, defaultKey.allKeys())
    {
        curSetting.setValue(pShortcutsKey, defaultKey.value(pShortcutsKey));
    }
}
