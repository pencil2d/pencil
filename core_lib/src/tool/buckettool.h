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

#ifndef BUCKETTOOL_H
#define BUCKETTOOL_H

#include "stroketool.h"

#include "bitmapimage.h"
#include "bitmapbucket.h"

class Layer;
class VectorImage;

class BucketTool : public StrokeTool
{
    Q_OBJECT
public:
    explicit BucketTool(QObject* parent = nullptr);
    ToolType type() override;
    ToolCategory category() override { return STROKETOOL; }

    void loadSettings() override;
    QCursor cursor() override;

    void pointerPressEvent(PointerEvent*) override;
    void pointerMoveEvent(PointerEvent*) override;
    void pointerReleaseEvent(PointerEvent*) override;

    void paintBitmap();
    void paintVector(Layer* layer);
    void drawStroke();

    void applyChanges();

    void setWidth(qreal width) override;
    void setTolerance(int tolerance);
    void setFillExpand(int fillExpandValue);
    void setFillReferenceMode(int referenceMode);
    void setFillMode(int mode);
    void setToleranceON(bool isON);
    void setFillExpandON(bool isON);

    ToolSettings* getProperties() override { return &properties; }

private:

    BitmapBucket mBitmapBucket;
    VectorImage* vectorImage = nullptr;

    bool mFilledOnMove = false;

    BucketSettings properties;
};

#endif // BUCKETTOOL_H
