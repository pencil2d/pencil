
#include "editor.h"
#include "basetool.h"



QString BaseTool::typeName(ToolType type)
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
