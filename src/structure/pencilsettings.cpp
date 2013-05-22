
#include "pencilsettings.h"

// ==== Singleton ====
static QSettings* g_pSettings = NULL;

QSettings* pencilSettings()
{
    if ( g_pSettings == NULL )
    {
        g_pSettings = new QSettings("pencil", "pencil");
    }
    return g_pSettings;
}


