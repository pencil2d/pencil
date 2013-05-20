
#include <QSettings>
#include "pentool.h"


PenTool::PenTool()
{

}

ToolType PenTool::type()
{
    return PEN;
}

void PenTool::loadSettings()
{
    QSettings settings("Pencil","Pencil");
    properties.width = settings.value("penWidth").toDouble();
    properties.colour = Qt::black;
    properties.colourNumber = 0;
    properties.feather = 0;
    properties.opacity = 1;
    properties.pressure = 1;
    properties.invisibility = 0;
    properties.preserveAlpha = 0;

    if ( properties.width <= 0 )
    {
        properties.width = 1.5;
        settings.setValue("penWidth", properties.width);
    }

    if ( properties.feather < 0 )
    {
        properties.feather = 0;
    }
}
