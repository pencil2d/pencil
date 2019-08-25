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
#include "bitmapbuckettool.h"

#include <QtMath>
#include "pointerevent.h"

#include "layer.h"
#include "layerbitmap.h"
#include "layermanager.h"
#include "colormanager.h"
#include "editor.h"
#include "scribblearea.h"


BitmapBucketTool::BitmapBucketTool(QObject* parent) : StrokeTool(parent)
{
}

ToolType BitmapBucketTool::type()
{
    return BUCKET;
}

void BitmapBucketTool::loadSettings()
{
    mPropertyEnabled[TOLERANCE] = true;
    mPropertyEnabled[WIDTH] = true;

    QSettings settings(PENCIL2D, PENCIL2D);

    properties.width = settings.value("fillThickness", 4.0).toDouble();
    properties.feather = 10;
    properties.stabilizerLevel = StabilizationLevel::NONE;
    properties.tolerance = settings.value("tolerance", 32.0).toDouble();
}

void BitmapBucketTool::resetToDefault()
{
    setWidth(4.0);
    setTolerance(32.0);
}

QCursor BitmapBucketTool::cursor()
{
    if (mEditor->preference()->isOn(SETTING::TOOL_CURSOR))
    {
        QPixmap pixmap(":icons/bucketTool.png");
        QPainter painter(&pixmap);
        painter.end();

        return QCursor(pixmap, 4, 20);
    }
    else
    {
        return Qt::CrossCursor;
    }
}

/**
 * @brief BrushTool::setWidth
 * @param width
 * set fill thickness
 */
void BitmapBucketTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("fillThickness", width);
    settings.sync();
}

void BitmapBucketTool::setTolerance(const int tolerance)
{
    // Set current property
    properties.tolerance = tolerance;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("tolerance", tolerance);
    settings.sync();
}

void BitmapBucketTool::pointerPressEvent(PointerEvent* event)
{
    startStroke();
    if (event->button() == Qt::LeftButton)
    {
        mScribbleArea->setAllDirty();
    }
    startStroke();
}

void BitmapBucketTool::pointerReleaseEvent(PointerEvent* event)
{
    Layer* layer = editor()->layers()->currentLayer();
    if (layer == nullptr) { return; }

    if (event->button() == Qt::LeftButton)
    {
        mEditor->backup(typeName());
        paintBitmap(layer);

    }
    endStroke();
}

void BitmapBucketTool::paintBitmap(Layer* layer)
{
    Layer* targetLayer = layer; // by default
    int layerNumber = editor()->layers()->currentLayerIndex(); // by default

    BitmapImage* targetImage = static_cast<LayerBitmap*>(targetLayer)->getLastBitmapImageAtFrame(editor()->currentFrame(), 0);

    QPoint point = QPoint(qFloor(getLastPoint().x()), qFloor(getLastPoint().y()));
    QRect cameraRect = mScribbleArea->getCameraRect().toRect();
    BitmapImage::floodFill(targetImage,
                           cameraRect,
                           point,
                           qPremultiply(mEditor->color()->frontColor().rgba()),
                           properties.tolerance);

    mScribbleArea->setModified(layerNumber, mEditor->currentFrame());
    mScribbleArea->setAllDirty();
}
