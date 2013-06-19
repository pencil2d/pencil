
#include <QSettings>
#include <QPixmap>
#include <QPainter>
#include "brushtool.h"

BrushTool::BrushTool(QObject *parent) :
    BaseTool(parent)
{
}

ToolType BrushTool::type()
{
    return BRUSH;
}

void BrushTool::loadSettings()
{
    QSettings settings("pencil", "pencil");

    properties.width = settings.value("brushWidth").toDouble();        
    properties.colourNumber = 1;
    properties.feather = settings.value("brushFeather").toDouble();    
    properties.opacity = 0.5;

    properties.pressure = ON;
    properties.invisibility = DISABLED;
    properties.preserveAlpha = OFF;

    if (properties.feather == 0)
    {
        properties.feather = 70;
        settings.setValue("brushFeather", properties.feather);
    }
    if (properties.width == 0)
    {
        properties.width = 48;
        settings.setValue("brushWidth", properties.width);
    }
}

QCursor BrushTool::cursor()
{
    qreal width = properties.width + 0.5 * properties.feather;
    QPixmap pixmap(width,width);
    if (!pixmap.isNull())
    {
        pixmap.fill( QColor(255,255,255,0) );
        QPainter painter(&pixmap);
        painter.setPen( QColor(0,0,0,190) );
        painter.setBrush( Qt::NoBrush );
        painter.drawLine( QPointF(width/2-2,width/2), QPointF(width/2+2,width/2) );
        painter.drawLine( QPointF(width/2,width/2-2), QPointF(width/2,width/2+2) );
        painter.setRenderHints(QPainter::Antialiasing, true);
        painter.setPen( QColor(0,0,0,100) );
        painter.drawEllipse( QRectF( 
            1 + properties.feather/2, 
            1 + properties.feather/2,
            qMax(0.0, properties.width - properties.feather/2-2),
            qMax(0.0, properties.width - properties.feather/2-2)) );
        painter.setPen( QColor(0,0,0,50) );
        painter.drawEllipse( QRectF(1+properties.feather/8,1+properties.feather/8,qMax(0.0,width-properties.feather/4-2),qMax(0.0,width-properties.feather/4-2)) );
        painter.end();
    }
    return QCursor(pixmap);

}
