#include <QStringList>
#include <QDebug>
#include "pencilsettings.h"

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
