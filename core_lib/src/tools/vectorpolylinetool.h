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

#ifndef VECTORPOLYLINETOOL_H
#define VECTORPOLYLINETOOL_H

#include <QPointF>

#include "basetool.h"

class VectorPolylineTool : public BaseTool
{
    Q_OBJECT
public:
    explicit VectorPolylineTool(QObject* parent = 0);
    ToolType type() override { return POLYLINE; }
    void loadSettings() override;
    QCursor cursor() override;
    void resetToDefault() override;

    void pointerPressEvent(PointerEvent*) override;
    void pointerReleaseEvent(PointerEvent*) override;
    void pointerMoveEvent(PointerEvent* event) override;
    void pointerDoubleClickEvent(PointerEvent*) override;

    bool keyPressEvent(QKeyEvent* event) override;

    void clearToolData() override;

    void setWidth(const qreal width) override;

private:
    QList<QPointF> mPoints;

    void drawPolyline(QList<QPointF> points, QPointF endPoint);
    void cancelPolyline();
    void endPolyline(QList<QPointF> points);
};

#endif // VECTORPOLYLINETOOL_H
