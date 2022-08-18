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

#ifndef PENTOOL_H
#define PENTOOL_H

#include "stroketool.h"

class Layer;

class PenTool : public StrokeTool
{
    Q_OBJECT
public:
    PenTool(QObject* parent = 0);
    ToolType type() override { return PEN; }
    void loadSettings() override;
    QCursor cursor() override;
    void resetToDefault() override;

    void pointerPressEvent(PointerEvent*) override;
    void pointerMoveEvent(PointerEvent*) override;
    void pointerReleaseEvent(PointerEvent*) override;

    void drawStroke();
    void paintAt(QPointF point);
    void paintVectorStroke(Layer *layer);
    void paintBitmapStroke();

    void setWidth(const qreal width) override;
    void setPressure(const bool pressure) override;
    void setAA(const int AA) override;
    void setStabilizerLevel(const int level) override;

private:
    QPointF mLastBrushPoint;
    QPointF mMouseDownPoint;
};

#endif // PENTOOL_H
