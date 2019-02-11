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
#include "movemode.h"

class Layer;
class VectorImage;


class MoveTool : public BaseTool
{
    Q_OBJECT
public:
    explicit MoveTool(QObject* parent);
    ToolType type() override;
    void loadSettings() override;
    QCursor cursor() override;

    void pointerPressEvent(PointerEvent*) override;
    void pointerReleaseEvent(PointerEvent*) override;
    void pointerMoveEvent(PointerEvent*) override;

    bool leavingThisTool() override;
    bool switchingLayer() override;

private:
    void cancelChanges();
    void applyTransformation();
    void applySelectionChanges();
    void resetSelectionProperties();
    void paintTransformedSelection();
    void whichAnchorPoint();
    void setAnchorToLastPoint();
    void updateTransformation();

    int showTransformWarning();

    void beginInteraction(Qt::KeyboardModifiers keyMod, Layer* layer);
    void createVectorSelection(Qt::KeyboardModifiers keyMod, Layer* layer);
    void transformSelection(Qt::KeyboardModifiers keyMod, Layer* layer);
    void storeClosestVectorCurve(Layer* layer);

    void setCurveSelected(VectorImage* vectorImage, Qt::KeyboardModifiers keyMod);
    void setAreaSelected(VectorImage* vectorImage, Qt::KeyboardModifiers keyMod);

    bool transformHasBeenModified();
    bool shouldDeselect();

    QPointF maintainAspectRatio(qreal offsetX, qreal offsetY);
    Layer* currentPaintableLayer();

    QPointF anchorOriginPoint;
    Layer* mCurrentLayer = nullptr;
    qreal mRotatedAngle = 0.0;
};

#endif
