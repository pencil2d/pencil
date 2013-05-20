#include "basetool.h"


QString typeName(ToolType type)
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
