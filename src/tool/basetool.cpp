#include "editor.h"
#include "basetool.h"

#include "scribblearea.h"
#include "strokemanager.h"

QString BaseTool::TypeName(ToolType type)
{
    static QMap<ToolType, QString>* map = NULL;

    if ( map == NULL )
    {
        map = new QMap<ToolType, QString>();
        map->insert(PENCIL, "Pencil");
        map->insert(ERASER, "Eraser");
        map->insert(SELECT, "Select");
        map->insert(MOVE, "Move");
        map->insert(HAND, "Hand");
        map->insert(SMUDGE, "Smudge");
        map->insert(PEN, "Pen");
        map->insert(POLYLINE, "Polyline");
        map->insert(BUCKET, "Bucket");
        map->insert(EYEDROPPER, "Eyedropper");
        map->insert(BRUSH, "Brush");
    }

    return map->value( type );
}

BaseTool::BaseTool(QObject *parent) :
QObject(parent),
isAdjusting(false)
{
}


QCursor BaseTool::cursor()
{
    return Qt::ArrowCursor;
}

void BaseTool::initialize(Editor* editor, ScribbleArea *scribbleArea)
{
    if (editor == NULL)
    {
        qCritical("ERROR: editor is null!");
    }
    m_pEditor = editor;
    m_pScribbleArea = scribbleArea;
    m_pStrokeManager = scribbleArea->getStrokeManager();

    loadSettings();
}

void BaseTool::mousePressEvent( QMouseEvent* )
{

}

void BaseTool::mouseMoveEvent( QMouseEvent* )
{

}

void BaseTool::mouseReleaseEvent( QMouseEvent* )
{

}

void BaseTool::mouseDoubleClickEvent(QMouseEvent *event)
{
    mousePressEvent(event);
}

QCursor BaseTool::wswgCursor() //wysywig circular cursor (dynamic adjustments)
{
    qreal propWidth = properties.width;
    qreal propFeather = properties.feather;
    qreal width = propWidth + 0.5 * propFeather;

    if (width < 1) { width = 1; }
    qreal radius = width/2;
    qreal xyA = 1 + propFeather/2;
    qreal xyB = 1 + propFeather/8;
    qreal whA = qMax(0.0, propWidth-xyA - 1);
    qreal whB = qMax(0.0, width-propFeather/4 - 2);
    QPixmap pixmap(width, width);
    if (!pixmap.isNull())
    {
        pixmap.fill( QColor(255,255,255,0) );
        QPainter painter(&pixmap);
        painter.setPen( QColor(0,0,0,190) );
        painter.setBrush( Qt::NoBrush );
        painter.drawLine( QPointF(radius-2,radius), QPointF(radius+2,radius) );
        painter.drawLine( QPointF(radius,radius-2), QPointF(radius,radius+2) );
        painter.setRenderHints(QPainter::Antialiasing, true);
        painter.setPen( QColor(0,0,0,100) );
        painter.drawEllipse( QRectF( xyA, xyA, whA, whA) );
        painter.setPen( QColor(0,0,0,50) );
        painter.drawEllipse( QRectF(xyB, xyB, whB, whB) );
        painter.end();
    }
    return QCursor(pixmap);

}

void BaseTool::adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice)
{
    Q_UNUSED(pressure);
    Q_UNUSED(mouseDevice);
}

QPointF BaseTool::getCurrentPixel()
{
    return m_pStrokeManager->getCurrentPixel();
}

QPointF BaseTool::getCurrentPoint()
{
    return m_pScribbleArea->pixelToPoint(getCurrentPixel());
}

QPointF BaseTool::getLastPixel()
{
    return m_pStrokeManager->getLastPixel();
}

QPointF BaseTool::getLastPoint()
{
    return m_pScribbleArea->pixelToPoint(getLastPixel());
}

QPointF BaseTool::getLastPressPixel()
{
    return m_pStrokeManager->getLastPressPixel();
}

QPointF BaseTool::getLastPressPoint()
{
    return m_pScribbleArea->pixelToPoint(getLastPressPixel());
}
