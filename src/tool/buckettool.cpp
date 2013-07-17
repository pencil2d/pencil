#include <QPixmap>
#include <QPainter>
#include "pencilsettings.h"
#include "buckettool.h"

BucketTool::BucketTool(QObject *parent) :
    BaseTool(parent)
{
}


ToolType BucketTool::type()
{
    return BUCKET;
}

void BucketTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
}

QCursor BucketTool::cursor()
{
    if ( pencilSettings()->value( kSettingToolCursor ).toBool() )
    {
        QPixmap pixmap(":icons/bucketTool.png");
        QPainter painter(&pixmap);
        painter.setPen( Qt::blue );   // FIXED: need to get current color
        painter.drawLine( QPoint(5, 16), QPoint(5, 18) );
        painter.end();

        return QCursor(pixmap, 4, 20);
    }
    else
    {
        return Qt::CrossCursor;
    }
}
