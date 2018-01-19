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

#ifndef SMUDGETOOL_H
#define SMUDGETOOL_H

#include "stroketool.h"

class SmudgeTool : public StrokeTool
{
    Q_OBJECT
public:
    explicit SmudgeTool(QObject *parent = 0);
    ToolType type();
    uint toolMode;  // 0=normal/smooth 1=smudge - todo: move to basetool? could be useful
    void loadSettings();
    QCursor cursor();

    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    bool keyPressEvent(QKeyEvent *);
    bool keyReleaseEvent(QKeyEvent *);

    void adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice);
    void drawStroke();

    void setWidth( const qreal width );
    void setFeather( const qreal feather );
    void setPressure( const bool pressure );

private:
    QPointF mLastBrushPoint;
};

#endif // SMUDGETOOL_H
