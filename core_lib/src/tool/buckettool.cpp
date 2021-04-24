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
#include <QPointer>
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
    mPropertyEnabled[FILL_MODE] = true;

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
    properties.fillMode = settings.value(SETTING_FILL_MODE, 0).toInt();
}

void BucketTool::resetToDefault()
{
    setWidth(4.0);
    setTolerance(32.0);
    setFillMode(0);
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

void BucketTool::setTolerance(const int tolerance)
{
    // Set current property
    properties.tolerance = tolerance;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(SETTING_BUCKET_TOLERANCE, tolerance);
    settings.sync();
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

void BucketTool::setFillMode(int mode)
{
    // Set current property
    properties.fillMode = mode;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(SETTING_FILL_MODE, mode);
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
    int currentFrameIndex = mEditor->currentFrame();

    if (properties.bucketFillToLayerMode == 1) {

        bool foundLayerBelow = false;
        for (int i = targetLayerIndex; i >= 0; i--) {
            Layer* searchlayer = mEditor->layers()->getLayer(i);

            if (searchlayer == nullptr) { Q_ASSERT(true); }

            if (targetLayer != searchlayer && searchlayer->type() == Layer::BITMAP && searchlayer->visible()) {
                targetLayer = searchlayer;
                targetLayerIndex = i;
                foundLayerBelow = true;
                break;
            }
        }

        if (foundLayerBelow && targetLayer->addNewKeyFrameAt(currentFrameIndex)) {
            emit mEditor->updateTimeLine();
        }
    }

    mEditor->backup(targetLayerIndex, currentFrameIndex, typeName());

    if (targetLayer == nullptr || targetLayer->type() != Layer::BITMAP) {
        Q_ASSERT(true);
        return;
    }

    BitmapImage* targetImage = static_cast<LayerBitmap*>(targetLayer)->getLastBitmapImageAtFrame(currentFrameIndex, 0);

    if (targetImage == nullptr || !targetImage->isLoaded()) { return; } // Can happen if the first frame is deleted while drawing

    BitmapImage replaceImage = BitmapImage(targetImage->bounds(), Qt::transparent);
    BitmapImage referenceImage = *static_cast<LayerBitmap*>(referenceLayer)->getLastBitmapImageAtFrame(currentFrameIndex, 0);
    if (properties.bucketFillReferenceMode == 1) // All layers
    {
        referenceImage = flattenBitmapLayersToImage(&replaceImage);
    }

    QRgb multipliedColor = qPremultiply(mEditor->color()->frontColor().rgba());
    QRgb origColor = multipliedColor;

    if (properties.fillMode == 0 && qAlpha(multipliedColor) == 0)
     {
         // Filling in overlay mode with a fully transparent color has no
         // effect, so we can skip it in this case
         return;
     }
     else if (properties.fillMode == 1)
     {
         // Pass a fully opaque version of the new color to floodFill
         // This is required so we can fully mask out the existing data before
         // writing the new color.
         QColor tempColor;
         tempColor.setRgba(multipliedColor);
         tempColor.setAlphaF(1);
         multipliedColor = tempColor.rgba();
     }

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

    switch(properties.fillMode)
    {
    default:
    case 0: // Overlay mode
        // Write fill image on top of target image
        targetImage->paste(&replaceImage);
        break;
    case 1: // Replace mode
        if (qAlpha(origColor) == 255)
        {
            // When the new color is fully opaque, replace mode
            // behaves exactly like overlay mode, and origColor == fillColor
            targetImage->paste(&replaceImage);
        }
        else
        {
            // Clearly all pixels in the to-be-filled region from the target image
            targetImage->paste(&replaceImage, QPainter::CompositionMode_DestinationOut);
            // Reduce the opacity of the fill to match the new color
            BitmapImage properColor(targetImage->bounds(), QColor::fromRgba(origColor));
            properColor.paste(&replaceImage, QPainter::CompositionMode_DestinationIn);
            // Write reduced-opacity fill image on top of target image
            targetImage->paste(&properColor);
        }
        break;
    }

    targetImage->modification();

    mScribbleArea->setModified(targetLayerIndex, currentFrameIndex);
}

BitmapImage BucketTool::flattenBitmapLayersToImage(BitmapImage* boundsImage)
{
    BitmapImage flattenImage = BitmapImage(boundsImage->bounds(), Qt::transparent);
    int currentFrame = editor()->currentFrame();
    auto layerMan = mEditor->layers();
    for (int i = 0; i < layerMan->count(); i++) {
        Layer* layer = layerMan->getLayer(i);

        if (layer && layer->type() == Layer::BITMAP && layer->visible()) {
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
