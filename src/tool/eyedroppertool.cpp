#include <QPainter>
#include <QPixmap>
#include <QBitmap>
#include "eyedroppertool.h"

EyedropperTool::EyedropperTool(QObject *parent) :
    BaseTool(parent)
{
}

ToolType EyedropperTool::type()
{
    return EYEDROPPER;
}

void EyedropperTool::loadSettings()
{

}

QCursor EyedropperTool::cursor()
{
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::white);
    
    QPainter painter(&pixmap);
    painter.drawLine(5,0,5,10);
    painter.drawLine(0,5,10,5);
    painter.end();

    QPixmap mask(32,32);
    mask.fill(Qt::color0);
    
    painter.begin(&mask);
    painter.setBrush(Qt::color1);
    painter.setPen(Qt::color1);
    painter.drawLine(5,0,5,10);
    painter.drawLine(0,5,10,5);
    painter.drawRect(10,10,20,20);
    painter.end();
    pixmap.setMask(QBitmap(mask));

    return QCursor(pixmap, 5, 5);
}