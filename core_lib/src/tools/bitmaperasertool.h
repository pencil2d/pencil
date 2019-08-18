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

#ifndef BITMAPERASERTOOL_H
#define BITMAPERASERTOOL_H

#include "stroketool.h"

class BitmapEraserTool : public StrokeTool
{
    Q_OBJECT

public:
    explicit BitmapEraserTool( QObject* parent = 0 );
    ToolType type() override;
    void loadSettings() override;
    void resetToDefault() override;
    QCursor cursor() override;

    void pointerMoveEvent( PointerEvent* ) override;
    void pointerPressEvent( PointerEvent* ) override;
    void pointerReleaseEvent( PointerEvent* ) override;

    void drawStroke();
    void paintAt( QPointF point );
    void removeVectorPaint();
    void updateStrokes();

    void setWidth( const qreal width ) override;
    void setFeather( const qreal feather ) override;
    void setPressure( const bool pressure ) override;
    void setStabilizerLevel(const int level) override;

protected:
    QPointF mLastBrushPoint;
    QPointF mMouseDownPoint;
};

#endif // BITMAPERASERTOOL_H