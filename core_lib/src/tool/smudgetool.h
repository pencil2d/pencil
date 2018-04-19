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
    ToolType type() override;
    uint toolMode;  // 0=normal/smooth 1=smudge - todo: move to basetool? could be useful
    void loadSettings() override;
    QCursor cursor() override;

    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    bool keyPressEvent(QKeyEvent *) override;
    bool keyReleaseEvent(QKeyEvent *) override;

    void adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice);
    void drawStroke();

    void setWidth( const qreal width );
    void setFeather( const qreal feather );
    void setPressure( const bool pressure );

protected:
    bool emptyFrameActionEnabled() override;

private:
    QPointF mLastBrushPoint;
};

#endif // SMUDGETOOL_H
