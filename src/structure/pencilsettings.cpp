
#include <QSettings>
#include "pencilsettings.h"



PencilSettings::PencilSettings(QObject *parent) :
    QObject(parent)
{
    m_pSettings = new QSettings("pencil", "pencil");
}

PencilSettings* PencilSettings::get()
{

}
