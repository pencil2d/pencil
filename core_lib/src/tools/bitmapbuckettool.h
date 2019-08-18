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

#ifndef BITMAPBUCKETTOOL_H
#define BITMAPBUCKETTOOL_H

#include "stroketool.h"

class Layer;
class VectorImage;

class BitmapBucketTool : public StrokeTool
{
    Q_OBJECT
public:
    explicit BitmapBucketTool(QObject* parent = nullptr);
    ToolType type() override;
    void loadSettings() override;
    void resetToDefault() override;
    QCursor cursor() override;

    void pointerPressEvent(PointerEvent*) override;
    void pointerMoveEvent(PointerEvent*) override;
    void pointerReleaseEvent(PointerEvent*) override;

    void setTolerance(const int tolerance) override;
    void setWidth(const qreal width) override;

    void paintBitmap(Layer* layer);
    void paintVector(Layer* layer);
    void drawStroke();

    void applyChanges();

private:

    VectorImage* vectorImage = nullptr;
};

#endif // BITMAPBUCKETTOOL_H
