#ifndef PENCILSETTINGS_H
#define PENCILSETTINGS_H

#include <QObject>
#include <QSettings>
#include "pencildef.h"

QSettings& pencilSettings();
void restoreToDefaultSetting();

void restoreShortcutsToDefault();
void checkExistingShortcuts();

#endif // PENCILSETTINGS_H
