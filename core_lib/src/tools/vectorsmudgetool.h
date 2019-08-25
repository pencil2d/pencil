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

#ifndef VECTORSMUDGETOOL_H
#define VECTORSMUDGETOOL_H

#include "stroketool.h"

class VectorSmudgeTool : public StrokeTool
{
    Q_OBJECT
public:
    explicit VectorSmudgeTool(QObject* parent = 0);
    ToolType type() override { return SMUDGE; }
    uint toolMode;  // 0=normal/smooth 1=smudge - todo: move to basetool? could be useful
    void loadSettings() override;
    void resetToDefault() override;
    QCursor cursor() override;

    void pointerPressEvent(PointerEvent *) override;
    void pointerReleaseEvent(PointerEvent *) override;
    void pointerMoveEvent(PointerEvent *) override;

    bool keyPressEvent(QKeyEvent *) override;
    bool keyReleaseEvent(QKeyEvent *) override;

    void setWidth( const qreal width ) override;
    void setPressure( const bool pressure ) override;

protected:
    bool emptyFrameActionEnabled() override;

private:

    QPointF offsetFromPressPos();

    QPointF mLastBrushPoint;
};

#endif // VECTORSMUDGETOOL_H
