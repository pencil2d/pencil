
#include "pencilsettings.h"

// ==== Singleton ====
static QSettings* g_pSettings = NULL;

void checkSettings(QSettings* pSettings)
{
    if ( !pSettings->contains(kSettingToolCursor) )
    {
        pSettings->setValue( kSettingToolCursor, true );
    }
}

QSettings* pencilSettings()
{
    if ( g_pSettings == NULL )
    {
        g_pSettings = new QSettings("pencil", "pencil");
        checkSettings(g_pSettings);
    }
    return g_pSettings;
}


