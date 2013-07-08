
#include <QMap>
#include <QString>
#include <QSettings>
#include "keyboardmanager.h"



KeyboardManager::KeyboardManager(QObject *parent) :
    QObject(parent)
{
    m_pKeySettings = new QSettings(SHORTCUTS_INI, QSettings::IniFormat);
    //m_pActionMap = new QMap<QString, QString>();

}

void KeyboardManager::setShortcut(QString strCommand, QString strKeys)
{
    Q_UNUSED(strCommand);
    Q_UNUSED(strKeys);

}

