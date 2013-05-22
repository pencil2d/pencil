
#include <QSettings>
#include <QPixmap>
#include <QPainter>
#include "erasertool.h"


EraserTool::EraserTool(QObject *parent) :
    BaseTool(parent)
{
}

ToolType EraserTool::type()
{
    return ERASER;
}

void EraserTool::loadSettings()
{
    QSettings settings("pencil", "pencil");
    properties.width = settings.value("eraserWidth").toDouble();    
    properties.feather = 0;
    properties.pressure = 1;
    properties.invisibility = -1;
    properties.preserveAlpha = 0;

    if (properties.width == 0)
    {
        properties.width = 24;
        settings.setValue("eraserWidth", properties.width);
    }
}

QCursor EraserTool::cursor()
{
    QPixmap pixmap( properties.width, properties.width );
    pixmap.fill( QColor(255,255,255,0) );

    QPainter painter(&pixmap);
    painter.setPen( QColor(0,0,0,190) );
    painter.setBrush( QColor(255,255,255,100) );
    painter.drawLine( QPointF(properties.width/2 - 2, properties.width/2), QPointF(properties.width/2+2, properties.width/2) );
    painter.drawLine( QPointF(properties.width/2, properties.width/2-2), QPointF(properties.width/2, properties.width/2+2) );
    painter.setRenderHints(QPainter::Antialiasing, true);
    painter.setPen( QColor(0,0,0,100) );
    painter.drawEllipse( QRectF(1, 1, properties.width - 2, properties.width - 2) );
    painter.end();

    return QCursor(pixmap);
}
