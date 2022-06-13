/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef SelectionPainter_H
#define SelectionPainter_H

#include "QRectF"
#include "QPolygonF"

class QPainter;
class Object;
class BaseTool;

struct TransformParameters
{
    QPolygonF lastSelectionPolygonF;
    QPolygonF currentSelectionPolygonF;
};

class SelectionPainter
{
public:
    SelectionPainter();

    void paint(QPainter& painter, const Object* object, int layerIndex, BaseTool* tool,
               TransformParameters& transformParameters, QPolygonF original, QPolygonF currentNotMapped);

    void paint(QPainter& painter, const Object* object, int layerIndex, BaseTool* tool, TransformParameters& transformParameters);
};

#endif // SelectionPainter_H
