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
        map->insert(EDIT, "Edit");
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
    QObject(parent)
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
