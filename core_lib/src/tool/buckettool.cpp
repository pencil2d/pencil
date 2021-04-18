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
#include <QtMath>
#include <QSettings>
#include "pointerevent.h"

#include "layer.h"
#include "layervector.h"
#include "layerbitmap.h"
#include "layermanager.h"
#include "colormanager.h"
#include "strokemanager.h"
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

    QSettings settings(PENCIL2D, PENCIL2D);

    properties.width = settings.value("fillThickness", 4.0).toDouble();
    properties.feather = 10;
    properties.stabilizerLevel = StabilizationLevel::NONE;
    properties.useAA = DISABLED;
    properties.tolerance = settings.value(SETTING_BUCKET_TOLERANCE, 32.0).toDouble();
    properties.toleranceEnabled = settings.value(SETTING_BUCKET_TOLERANCE_ON, false).toBool();

    properties.bucketFillExpand = settings.value(SETTING_BUCKET_FILL_EXPAND, 2.0).toInt();
    properties.bucketFillExpandEnabled = settings.value(SETTING_BUCKET_FILL_EXPAND_ON, true).toBool();
    properties.bucketFillToLayerMode = settings.value(SETTING_BUCKET_FILL_TO_LAYER_MODE, 0).toInt();
    properties.bucketFillReferenceMode = settings.value(SETTING_BUCKET_FILL_REFERENCE_MODE, 0).toInt();
}

void BucketTool::resetToDefault()
{
    setWidth(4.0);
    setTolerance(32.0);
    setFillExpand(2);
    setFillExpandEnabled(true);
    setFillToLayer(0);
    setToleranceEnabled(false);
    setFillReferenceMode(0);
}

QCursor BucketTool::cursor()
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
        return QCursor(QPixmap(":icons/cross.png"), 10, 10);
    }
}

/**
 * @brief BrushTool::setWidth
 * @param width
 * set fill thickness
 */
void BucketTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("fillThickness", width);
    settings.sync();
}

void BucketTool::setTolerance(const int tolerance)
{
    // Set current property
    properties.tolerance = tolerance;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(SETTING_BUCKET_TOLERANCE, tolerance);
    settings.sync();
}

void BucketTool::setToleranceEnabled(const bool enabled)
{
    properties.toleranceEnabled = enabled;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(SETTING_BUCKET_TOLERANCE_ON, enabled);
    settings.sync();
}

void BucketTool::setFillExpandEnabled(const bool enabled)
{
    properties.bucketFillExpandEnabled = enabled;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(SETTING_BUCKET_FILL_EXPAND_ON, enabled);
    settings.sync();
}

void BucketTool::setFillExpand(const int fillExpandValue)
{
    properties.bucketFillExpand = fillExpandValue;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(SETTING_BUCKET_FILL_EXPAND, fillExpandValue);
    settings.sync();
}

void BucketTool::setFillToLayer(int layerMode)
{
    properties.bucketFillToLayerMode = layerMode;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(SETTING_BUCKET_FILL_TO_LAYER_MODE, layerMode);
    settings.sync();
}

void BucketTool::setFillReferenceMode(int referenceMode)
{
    properties.bucketFillReferenceMode = referenceMode;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(SETTING_BUCKET_FILL_REFERENCE_MODE, referenceMode);
    settings.sync();
}

void BucketTool::pointerPressEvent(PointerEvent* event)
{
    startStroke(event->inputType());
}

void BucketTool::pointerMoveEvent(PointerEvent* event)
{
    if (event->buttons() & Qt::LeftButton && event->inputType() == mCurrentInputType)
    {
        Layer* layer = mEditor->layers()->currentLayer();
        if (layer->type() == Layer::VECTOR)
        {
            drawStroke();
        }
    }
}

void BucketTool::pointerReleaseEvent(PointerEvent* event)
{
    if (event->inputType() != mCurrentInputType) return;

    Layer* layer = editor()->layers()->currentLayer();
    if (layer == nullptr) { return; }

    if (event->button() == Qt::LeftButton)
    {
        // Backup of bitmap image is more complicated now and has therefore been moved to bitmap code
        if (layer->type() != Layer::BITMAP) {
            mEditor->backup(typeName());
        }

        switch (layer->type())
        {
        case Layer::BITMAP: paintBitmap(layer); break;
        case Layer::VECTOR: paintVector(layer); break;
        default:
            break;
        }
    }
    endStroke();
}

bool BucketTool::startAdjusting(Qt::KeyboardModifiers modifiers, qreal argStep)
{
    mQuickSizingProperties.clear();
    if (mEditor->layers()->currentLayer()->type() == Layer::VECTOR)
    {
        mQuickSizingProperties.insert(Qt::ShiftModifier, WIDTH);
    }
    else
    {
        mQuickSizingProperties.insert(Qt::ControlModifier, TOLERANCE);
    }
    return BaseTool::startAdjusting(modifiers, argStep);
}

void BucketTool::paintBitmap(Layer* layer)
{
    Layer* referenceLayer = layer; // by default
    Layer* targetLayer = layer;

    QPoint point = QPoint(qFloor(getLastPoint().x()), qFloor(getLastPoint().y()));
    QRect cameraRect = mScribbleArea->getCameraRect().toRect();
    int tolerance = properties.toleranceEnabled ? static_cast<int>(properties.tolerance) : 0;

    int targetLayerIndex = mEditor->currentLayerIndex();
    if (properties.bucketFillToLayerMode == 1) {

        for (int i = mEditor->currentLayerIndex(); i >= 0; i--) {
            Layer* searchlayer = mEditor->layers()->getLayer(i);

            if (searchlayer == nullptr) { Q_ASSERT(true); }

            if (targetLayer != searchlayer && searchlayer->type() == Layer::BITMAP) {
                targetLayer = searchlayer;
                targetLayerIndex = i;
                break;
            }
        }
    }

    mEditor->backup(targetLayerIndex, mEditor->currentFrame(), typeName());

    if (targetLayer == nullptr || targetLayer->type() != Layer::BITMAP) {
        Q_ASSERT(true);
        return;
    }

    BitmapImage* targetImage = static_cast<LayerBitmap*>(targetLayer)->getLastBitmapImageAtFrame(editor()->currentFrame(), 0);

    if (targetImage == nullptr || !targetImage->isLoaded()) { return; } // Can happen if the first frame is deleted while drawing

    BitmapImage replaceImage = BitmapImage(targetImage->bounds(), Qt::transparent);
    BitmapImage referenceImage = *static_cast<LayerBitmap*>(referenceLayer)->getLastBitmapImageAtFrame(editor()->currentFrame(), 0);
    if (properties.bucketFillReferenceMode == 1) // All layers
    {
        referenceImage = flattenBitmapLayersToImage(&replaceImage);
    }

    QRgb multipliedColor = qPremultiply(mEditor->color()->frontColor().rgba());

    BitmapImage::floodFill(&replaceImage,
                           &referenceImage,
                           cameraRect,
                           point,
                           multipliedColor,
                           tolerance);

    if (properties.bucketFillExpandEnabled) {
        BitmapImage::expandFill(&replaceImage,
                                multipliedColor,
                                properties.bucketFillExpand);
    }

    targetImage->paste(&replaceImage, QPainter::CompositionMode_DestinationOver);
    targetImage->modification();

    mScribbleArea->setModified(targetLayerIndex, mEditor->currentFrame());
}

BitmapImage BucketTool::flattenBitmapLayersToImage(BitmapImage* boundsImage)
{
    BitmapImage flattenImage = BitmapImage(boundsImage->bounds(), Qt::transparent);
    int currentFrame = editor()->currentFrame();
    auto layerMan = mEditor->layers();
    for (int i = 0; i < layerMan->count(); i++) {
        Layer* layer = layerMan->getLayer(i);

        if (layer && layer->type() == Layer::BITMAP) {
            BitmapImage* image = static_cast<LayerBitmap*>(layer)->getLastBitmapImageAtFrame(currentFrame);
            flattenImage.paste(image);
        }
    }
    return flattenImage;
}

void BucketTool::paintVector(Layer* layer)
{
    mScribbleArea->clearBitmapBuffer();

    VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
    if (vectorImage == nullptr) { return; } // Can happen if the first frame is deleted while drawing

    if (!vectorImage->isPathFilled())
    {
        vectorImage->fillSelectedPath(mEditor->color()->frontColorNumber());
    }

    vectorImage->applyWidthToSelection(properties.width);
    vectorImage->applyColorToSelectedCurve(mEditor->color()->frontColorNumber());
    vectorImage->applyColorToSelectedArea(mEditor->color()->frontColorNumber());

    applyChanges();

    mScribbleArea->setModified(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame());
}

void BucketTool::applyChanges()
{
    mScribbleArea->applyTransformedSelection();
}

void BucketTool::drawStroke()
{
    StrokeTool::drawStroke();

    if (properties.stabilizerLevel != strokeManager()->getStabilizerLevel())
    {
        strokeManager()->setStabilizerLevel(properties.stabilizerLevel);
    }

    QList<QPointF> p = strokeManager()->interpolateStroke();

    Layer* layer = mEditor->layers()->currentLayer();

    if (layer->type() == Layer::VECTOR)
    {
        mCurrentWidth = 30;
        int rad = qRound((mCurrentWidth / 2 + 2) * mEditor->view()->scaling());

        QColor pathColor = qPremultiply(mEditor->color()->frontColor().rgba());
        //pathColor.setAlpha(255);

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
            mScribbleArea->refreshVector(path.boundingRect().toRect(), rad);
        }
    }
}
