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

#ifndef STROKETOOL_H
#define STROKETOOL_H

#include "basetool.h"

#include <QList>
#include <QPointF>


class StrokeTool : public BaseTool
{
    Q_OBJECT

public:
    explicit StrokeTool(QObject* parent = 0);
    
    void startStroke();
    void drawStroke();
    void endStroke();

    bool keyPressEvent(QKeyEvent* event) override;
    bool keyReleaseEvent(QKeyEvent* event) override;

protected:
    bool mFirstDraw = false;

    QList<QPointF> mStrokePoints;
    QList<qreal> mStrokePressures;

    qreal mCurrentWidth    = 0.0;
    qreal mCurrentPressure = 0.5;

private:
	QPointF mLastPixel = { 0, 0 };
};

#endif // STROKETOOL_H
