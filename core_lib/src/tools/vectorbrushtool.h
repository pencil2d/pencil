/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef VECTORBRUSHTOOL_H
#define VECTORBRUSHTOOL_H

#include "stroketool.h"


class VectorBrushTool : public StrokeTool
{
    Q_OBJECT

public:
    explicit VectorBrushTool(QObject* parent = 0);
    ToolType type() override;
    void loadSettings() override;
    void resetToDefault() override;
    QCursor cursor() override;

    void pointerMoveEvent(PointerEvent*) override;
    void pointerPressEvent(PointerEvent*) override;
    void pointerReleaseEvent(PointerEvent*) override;

    void drawStroke();
    void paintVectorStroke();

    void setWidth(const qreal width) override;
    void setPressure(const bool pressure) override;
    void setInvisibility(const bool invisibility) override;
    void setStabilizerLevel(const int level) override;

protected:
    QPointF mLastBrushPoint;
    QPointF mMouseDownPoint;

    QColor mCurrentPressuredColor;
    qreal mOpacity = 1.0;
};

#endif // VECTORBRUSHTOOL_H
