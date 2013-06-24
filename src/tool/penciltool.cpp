#include <QSettings>
#include <QPixmap>
#include "pencilsettings.h"
#include "penciltool.h"


PencilTool::PencilTool(QObject *parent) :
    BaseTool(parent)
{
}

ToolType PencilTool::type()
{
    return PENCIL;
}

void PencilTool::loadSettings()
{
    QSettings settings("pencil", "pencil");

    properties.width = settings.value("pencilWidth").toDouble();    
    properties.colourNumber = 0;
    properties.feather = -1;
    properties.opacity = 0.8;
    properties.pressure = 1;
    properties.invisibility = 1;
    properties.preserveAlpha = 0;

    if (properties.width == 0)
    {
        properties.width = 1;
        settings.setValue("pencilWidth", properties.width);
    }
}

QCursor PencilTool::cursor()
{
    if ( pencilSettings()->value( kSettingToolCursor ).toBool() )
    {
        return QCursor(QPixmap(":icons/pencil2.png"), 0, 16);
    }
    else
    {
        return Qt::CrossCursor;
    }
}
