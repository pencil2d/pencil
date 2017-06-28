/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef STROKETOOL_H
#define STROKETOOL_H

#include "basetool.h"

#include <QList>
#include <QPointF>
#include <QPair>

typedef QPair<QPointF, QPointF> QSegment;

class StrokeTool : public BaseTool
{
    Q_OBJECT

public:
    explicit StrokeTool(QObject *parent = 0);
    
    void startStroke();
    void drawStroke();
    void endStroke();

    bool keyPressEvent(QKeyEvent *event) override;

    bool keyReleaseEvent(QKeyEvent *event) override;

protected:
    bool mFirstDraw = false;

    QList<QPointF> mStrokePoints;
    QList<qreal> mStrokePressures;

    qreal mCurrentWidth;
    qreal mCurrentPressure;

private:
	QPointF mLastPixel = { 0, 0 };
};

#endif // STROKETOOL_H
