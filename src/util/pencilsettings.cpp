
#include <QStringList>
#include "pencilsettings.h"

// ==== Singleton ====
static QSettings* g_pSettings = NULL;


QSettings* pencilSettings()
{
    if ( g_pSettings == NULL )
    {
        g_pSettings = new QSettings("Pencil", "Pencil");

        if ( !g_pSettings->contains("InitPencilSetting") )
        {
            restoreToDefaultSetting();
            g_pSettings->setValue("InitPencilSetting", true);
        }
    }
    return g_pSettings;
}

void restoreToDefaultSetting() // TODO: finish reset list
{
    QSettings* s = g_pSettings;

    s->setValue("penWidth", 2.0);
    s->setValue("pencilWidth", 1.0);
    s->setValue("eraserWidth", 10.0);
    s->setValue("brushWidth", 15.0);
    s->setValue("brushFeather", 15.0);

    s->setValue("autosaveNumber", 15);
    s->setValue("toolCursors", true);

    s->sync();
    qDebug("restored default tools");

}


void restoreShortcutsToDefault()
{
    QSettings defaultKey(":resources/kb.ini", QSettings::IniFormat);

    pencilSettings()->beginGroup("shortcuts");
    pencilSettings()->remove("");
    foreach (QString pKey, defaultKey.allKeys())
    {
        pencilSettings()->setValue(pKey, defaultKey.value(pKey));
    }
    pencilSettings()->endGroup();
}
