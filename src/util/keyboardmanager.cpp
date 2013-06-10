
#include <QSettings>
#include "keyboardmanager.h"


#define SHORTCUTS_INI "shortcuts.ini"


KeyboardManager::KeyboardManager(QObject *parent) :
    QObject(parent)
{
    m_pKeySettings = new QSettings(SHORTCUTS_INI, QSettings::IniFormat);


}


void KeyboardManager::setShortcut(ACTION eAction, QString strKeys)
{

}
