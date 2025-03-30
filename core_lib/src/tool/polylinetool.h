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

#ifndef POLYLINETOOL_H
#define POLYLINETOOL_H

#include <QPointF>

#include "stroketool.h"

class PolylineTool : public StrokeTool
{
    Q_OBJECT
public:
    explicit PolylineTool(QObject* parent = 0);

    ToolType type() const override;
    ToolCategory category() const override { return STROKETOOL; }

    void createSettings(ToolSettings *) override;
    void loadSettings() override;
    QCursor cursor() override;

    void setUseBezier(bool useBezier);
    void setClosePath(bool closePath);

    void pointerPressEvent(PointerEvent*) override;
    void pointerReleaseEvent(PointerEvent*) override;
    void pointerMoveEvent(PointerEvent* event) override;
    void pointerDoubleClickEvent(PointerEvent*) override;

    bool keyPressEvent(QKeyEvent* event) override;
    bool keyReleaseEvent(QKeyEvent* event) override;

    void clearToolData() override;

    bool leavingThisTool() override;

    bool isActive() const override;

    ToolSettings* settings() override { return mSettings; }

signals:
    void bezierPathEnabledChanged(bool useBezier);
    void closePathChanged(bool closePath);


private:
    PolylineSettings* mSettings = nullptr;
    QList<QPointF> mPoints;
    bool mClosedPathOverrideEnabled = false;

    void drawPolyline(QList<QPointF> points, QPointF endPoint);
    void removeLastPolylineSegment();
    void cancelPolyline();
    void endPolyline(QList<QPointF> points);

};

#endif // POLYLINETOOL_H
