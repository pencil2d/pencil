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

#ifndef BRUSHTOOL_H
#define BRUSHTOOL_H

#include "stroketool.h"


class BrushTool : public StrokeTool
{
    Q_OBJECT

public:
    explicit BrushTool(QObject* parent = 0);
    ToolType type() override;
    void loadSettings() override;
    void resetToDefault() override;
    QCursor cursor() override;

    void pointerMoveEvent(PointerEvent*) override;
    void pointerPressEvent(PointerEvent*) override;
    void pointerReleaseEvent(PointerEvent*) override;

    void drawStroke();
    void paintVectorStroke();
    void paintBitmapStroke();
    void paintAt(QPointF point);

    void setWidth(const qreal width) override;
    void setFeather(const qreal feather) override;
    void setUseFeather(const bool usingFeather) override;
    void setPressure(const bool pressure) override;
    void setInvisibility(const bool invisibility) override;
    void setAA(const int useAA) override;
    void setStabilizerLevel(const int level) override;

protected:
    QPointF mLastBrushPoint;
    QPointF mMouseDownPoint;

    QColor mCurrentPressuredColor;
    qreal mOpacity = 1.0;
};

#endif // BRUSHTOOL_H
