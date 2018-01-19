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

#ifndef MOVETOOL_H
#define MOVETOOL_H

#include "basetool.h"

class Layer;

class MoveTool : public BaseTool
{
    Q_OBJECT
public:
    explicit MoveTool( QObject* parent = 0 );
    ToolType type() override;
    void loadSettings() override;
    QCursor cursor() override;

    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;

    void leavingThisTool() override;
    void switchingLayers() override;

private:
    void cancelChanges();
    void applyChanges();
    void resetSelectionProperties();
    void paintTransformedSelection();
    void whichTransformationPoint();
    void transformSelection(qreal offsetX, qreal offsetY);
    void pressOperation(QMouseEvent* event, Layer *layer);
    void actionOnVector(QMouseEvent *event, Layer *layer);
    void storeClosestVectorCurve();
    QPointF maintainAspectRatio(qreal offsetX, qreal offsetY);
};

#endif
