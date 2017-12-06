/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef PENCILTOOL_H
#define PENCILTOOL_H

#include "stroketool.h"
#include <QColor>

class Layer;

class PencilTool : public StrokeTool
{
    Q_OBJECT
public:
    explicit PencilTool( QObject *parent = 0 );
    ToolType type() override { return PENCIL; }
    void loadSettings() override;
    QCursor cursor() override;

    void mousePressEvent( QMouseEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;

    void drawStroke();
    void paintAt( QPointF point );
    void paintVectorStroke(Layer* layer);
    void paintBitmapStroke();

    void adjustPressureSensitiveProperties( qreal pressure, bool mouseDevice ) override;

    void setWidth( const qreal width ) override;
    void setFeather( const qreal feather ) override;
    void setUseFeather( const bool useFeather ) override;
    void setInvisibility( const bool invisibility ) override;
    void setPressure( const bool pressure ) override;
    void setPreserveAlpha( const bool preserveAlpha ) override;
    void setInpolLevel(const int level) override;
    void setUseFillContour(const bool useFillContour) override;

private:
    QColor mCurrentPressuredColor { 0, 0, 0, 255 };
    QPointF mLastBrushPoint { 0, 0 };
    qreal mOpacity = 1.0f;
    QPointF mMouseDownPoint;
};

#endif // PENCILTOOL_H
