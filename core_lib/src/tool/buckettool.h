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

#ifndef BUCKETTOOL_H
#define BUCKETTOOL_H

#include "stroketool.h"

class Layer;
class VectorImage;


class BucketTool : public StrokeTool
{
    Q_OBJECT
public:
    explicit BucketTool(QObject* parent = nullptr);
    ToolType type() override;
    void loadSettings() override;
    QCursor cursor() override;

    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

    void tabletPressEvent(QTabletEvent*) override;
    void tabletMoveEvent(QTabletEvent*) override;
    void tabletReleaseEvent(QTabletEvent*) override;

    void setTolerance(const int tolerance) override;
    void setWidth(const qreal width) override;

    void paintBitmap(Layer* layer);
    void paintVector(Layer* layer);
    void drawStroke();

    void applyChanges();

private:
    void pressEventInternal(Qt::MouseButton);
    void releaseEventInternal(Qt::MouseButton);
    void moveEventInternal(Qt::MouseButtons);
    

    VectorImage* vectorImage = nullptr;
};

#endif // BUCKETTOOL_H
