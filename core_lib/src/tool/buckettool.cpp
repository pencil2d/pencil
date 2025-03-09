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
#include "buckettool.h"

#include <QPixmap>
#include <QPainter>
#include <QSettings>
#include "pointerevent.h"

#include "layer.h"
#include "layervector.h"
#include "layerbitmap.h"
#include "layercamera.h"
#include "layermanager.h"
#include "colormanager.h"
#include "toolmanager.h"
#include "viewmanager.h"
#include "vectorimage.h"
#include "editor.h"
#include "scribblearea.h"


BucketTool::BucketTool(QObject* parent) : StrokeTool(parent)
{
}

ToolType BucketTool::type()
{
    return BUCKET;
}

void BucketTool::loadSettings()
{
    mPropertyEnabled[TOLERANCE] = true;
    mPropertyEnabled[WIDTH] = true;
    mPropertyEnabled[FILL_MODE] = true;

    QSettings settings(PENCIL2D, PENCIL2D);

    QHash<int, PropertyInfo> info;

    info[BucketSettings::WIDTH_VALUE] = { 0.0, 100.0, 4.0 };
    info[BucketSettings::TOLERANCE_VALUE] = { 0, 100, 32 };
    info[BucketSettings::TOLERANCE_ON] = false;
    info[BucketSettings::FILLEXPAND_VALUE] = { 0, 25, 2 };
    info[BucketSettings::FILLEXPAND_ON] = true;
    info[BucketSettings::FILLLAYERREFERENCEMODE_VALUE] = { 0, 1, 0 };
    info[BucketSettings::FILLMODE_VALUE] = { 0, 2, 0 };

    properties.load(typeName(), settings, info);
}

QCursor BucketTool::cursor()
{
    if (mEditor->preference()->isOn(SETTING::TOOL_CURSOR))
    {
        return QCursor(QPixmap(":icons/general/cursor-bucket.svg"), -1, 17);
    }
    else
    {
        return QCursor(QPixmap(":icons/general/cross.png"), 10, 10);
    }
}

void BucketTool::pointerPressEvent(PointerEvent* event)
{
    mInterpolator.pointerPressEvent(event);
    startStroke(event->inputType());

    Layer* targetLayer = mEditor->layers()->currentLayer();

    if (targetLayer->type() != Layer::BITMAP) { return; }

    LayerCamera* layerCam = mEditor->layers()->getCameraLayerBelow(mEditor->currentLayerIndex());

    mBitmapBucket = BitmapBucket(mEditor,
                                 mEditor->color()->frontColor(),
                                 layerCam ? layerCam->getViewAtFrame(mEditor->currentFrame()).inverted().mapRect(layerCam->getViewRect()) : QRect(),
                                 getCurrentPoint(),
                                 properties);

    // Because we can change layer to on the fly, but we do not act reactively
    // on it, it's necessary to invalidate layer cache on press event.
    // Otherwise, the cache will be drawn until a move event has been initiated.
    mScribbleArea->invalidatePainterCaches();
}

void BucketTool::pointerMoveEvent(PointerEvent* event)
{
    mInterpolator.pointerMoveEvent(event);
    if (event->buttons() & Qt::LeftButton && event->inputType() == mCurrentInputType)
    {
        Layer* layer = mEditor->layers()->currentLayer();
        if (layer->type() == Layer::VECTOR)
        {
            drawStroke();
        }
        else if (layer->type() == Layer::BITMAP)
        {
            paintBitmap();
            mFilledOnMove = true;
        }
    }
}

void BucketTool::pointerReleaseEvent(PointerEvent* event)
{
    mInterpolator.pointerReleaseEvent(event);
    if (event->inputType() != mCurrentInputType) return;

    Layer* layer = editor()->layers()->currentLayer();
    if (layer == nullptr) { return; }

    if (event->button() == Qt::LeftButton)
    {
        // Backup of bitmap image is more complicated now and has therefore been moved to bitmap code
        if (layer->type() == Layer::VECTOR) {
            mEditor->backup(typeName());
            paintVector(layer);
        }
        else if (layer->type() == Layer::BITMAP && !mFilledOnMove)
        {
            paintBitmap();
        }
    }
    mFilledOnMove = false;

    endStroke();
}

void BucketTool::paintBitmap()
{
    mBitmapBucket.paint(getCurrentPoint(), [this](BucketState progress, int layerIndex, int frameIndex)
    {
        if (progress == BucketState::WillFillTarget)
        {
            mEditor->backup(layerIndex, frameIndex, typeName());
        }
        else if (progress == BucketState::DidFillTarget)
        {
            mEditor->setModified(layerIndex, frameIndex);
        }
    });
}

void BucketTool::paintVector(Layer* layer)
{
    mScribbleArea->clearDrawingBuffer();

    VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
    if (vectorImage == nullptr) { return; } // Can happen if the first frame is deleted while drawing

    if (!vectorImage->isPathFilled())
    {
        vectorImage->fillSelectedPath(mEditor->color()->frontColorNumber());
    }

    vectorImage->applyWidthToSelection(properties.thickness());
    vectorImage->applyColorToSelectedCurve(mEditor->color()->frontColorNumber());
    vectorImage->applyColorToSelectedArea(mEditor->color()->frontColorNumber());

    applyChanges();

    mEditor->setModified(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame());
}

void BucketTool::applyChanges()
{
    mScribbleArea->applyTransformedSelection();
}

void BucketTool::drawStroke()
{
    StrokeTool::drawStroke();

    if (properties.stabilizerLevel() != mInterpolator.getStabilizerLevel())
    {
        mInterpolator.setStabilizerLevel(properties.stabilizerLevel());
    }

    QList<QPointF> p = mInterpolator.interpolateStroke();

    Layer* layer = mEditor->layers()->currentLayer();

    if (layer->type() == Layer::VECTOR)
    {
        mCurrentWidth = 30;
        QColor pathColor = qPremultiply(mEditor->color()->frontColor().rgba());

        QPen pen(pathColor,
                 mCurrentWidth * mEditor->view()->scaling(),
                 Qt::NoPen,
                 Qt::RoundCap,
                 Qt::RoundJoin);

        if (p.size() == 4)
        {
            QPainterPath path(p[0]);
            path.cubicTo(p[1], p[2], p[3]);
            mScribbleArea->drawPath(path, pen, Qt::NoBrush, QPainter::CompositionMode_Source);
        }
    }
}

void BucketTool::setWidth(qreal width)
{
    properties.setBaseValue(BucketSettings::WIDTH_VALUE, width);
    editor()->tools()->toolPropertyChanged(type(), ToolPropertyType::WIDTH);
}

void BucketTool::setTolerance(int tolerance)
{
    properties.setBaseValue(BucketSettings::TOLERANCE_VALUE, tolerance);
    editor()->tools()->toolPropertyChanged(type(), ToolPropertyType::TOLERANCE);
}

void BucketTool::setToleranceON(bool isON)
{
    properties.setBaseValue(BucketSettings::TOLERANCE_ON, isON);
    editor()->tools()->toolPropertyChanged(type(), ToolPropertyType::USETOLERANCE);
}

void BucketTool::setFillExpand(int fillExpandValue)
{
    properties.setBaseValue(BucketSettings::FILLEXPAND_VALUE, fillExpandValue);
    editor()->tools()->toolPropertyChanged(type(), ToolPropertyType::BUCKETFILLEXPAND);
}

void BucketTool::setFillExpandON(bool isON)
{
    properties.setBaseValue(BucketSettings::FILLEXPAND_ON, isON);
    editor()->tools()->toolPropertyChanged(type(), ToolPropertyType::USEBUCKETFILLEXPAND);
}

void BucketTool::setFillReferenceMode(int referenceMode)
{
    properties.setBaseValue(BucketSettings::FILLLAYERREFERENCEMODE_VALUE, referenceMode);
    editor()->tools()->toolPropertyChanged(type(), ToolPropertyType::BUCKETFILLLAYERREFERENCEMODE);
}

void BucketTool::setFillMode(int mode)
{
    properties.setBaseValue(BucketSettings::FILLMODE_VALUE, mode);
    editor()->tools()->toolPropertyChanged(type(), ToolPropertyType::FILL_MODE);
}


