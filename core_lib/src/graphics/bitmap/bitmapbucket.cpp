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
#include "bitmapbucket.h"

#include <QtMath>
#include <QDebug>

#include "editor.h"
#include "layermanager.h"

#include "layerbitmap.h"

BitmapBucket::BitmapBucket()
{
}

BitmapBucket::BitmapBucket(Editor* editor,
                           QColor color,
                           QRectF maxFillRegion,
                           QPointF fillPoint,
                           Properties properties):
    mEditor(editor),
    mBucketStartPoint(fillPoint),
    mMaxFillRegion(maxFillRegion),
    mProperties(properties)

{
    Layer* initialLayer = editor->layers()->currentLayer();
    int initialLayerIndex = mEditor->currentLayerIndex();
    int frameIndex = mEditor->currentFrame();

    mBucketColor = qPremultiply(color.rgba());

    mTargetFillToLayer = initialLayer;
    mTargetFillToLayerIndex = initialLayerIndex;

    if (properties.bucketFillToLayerMode == 1) {

        auto result = findBitmapLayerBelow(initialLayer, initialLayerIndex);
        mTargetFillToLayer = result.first;
        mTargetFillToLayerIndex = result.second;
    }

    if (mTargetFillToLayer == nullptr) {
        Q_ASSERT(true);
        return;
    }

    mReferenceImage = *static_cast<LayerBitmap*>(initialLayer)->getLastBitmapImageAtFrame(frameIndex, 0);
    if (properties.bucketFillReferenceMode == 1) // All layers
    {
        mReferenceImage = flattenBitmapLayersToImage();
    }

    const QPoint point = QPoint(qFloor(fillPoint.x()), qFloor(fillPoint.y()));

    if (properties.bucketFillReferenceMode == 1) {
        mReferenceColor = mReferenceImage.constScanLine(point.x(), point.y());
    } else {
        BitmapImage* image = static_cast<LayerBitmap*>(mTargetFillToLayer)->getLastBitmapImageAtFrame(frameIndex, 0);
        mReferenceColor = image->constScanLine(point.x(), point.y());
    }
}

bool BitmapBucket::shouldFill(QPointF checkPoint) const
{
    const QPoint point = QPoint(qFloor(checkPoint.x()), qFloor(checkPoint.y()));

    if (mProperties.fillMode == 0 && qAlpha(mBucketColor) == 0)
    {
        // Filling in overlay mode with a fully transparent color has no
        // effect, so we can skip it in this case
        return false;
    }

    if (mTargetFillToLayer == nullptr) {
        Q_ASSERT(true);
        return false;
    }

    BitmapImage* targetImage = static_cast<LayerBitmap*>(mTargetFillToLayer)->getLastBitmapImageAtFrame(mEditor->currentFrame(), 0);
    // Can happen if the first frame is deleted while drawing
    if (targetImage == nullptr || !targetImage->isLoaded()) { return false; }

    BitmapImage referenceImage = mReferenceImage;

    QRgb preFill = 0;
    if (mProperties.bucketFillReferenceMode == 1) // All layers
    {
        referenceImage = mReferenceImage;

        // If the target image has been modified during move, we need to update the flattened reference image
        // but we know that only the target image could have been modified here, so no need to
        // look through all layers again, just paste the current target image.
        // but because of alpha blending, we need to do an additional scanline
        // to get the color prior to pasting
        preFill = referenceImage.constScanLine(point.x(), point.y());
        if (targetImage->isModified()) {
            referenceImage.paste(targetImage);
        }
    }

    QRgb pixelColor = referenceImage.constScanLine(point.x(), point.y());
    QRgb targetPixelColor = targetImage->constScanLine(point.x(), point.y());

    if (mProperties.fillMode == 2 && pixelColor != 0)
    {
        // don't try to fill because we won't be able to see it anyway...
        return false;
    }


    // Ensures that while dragging, only a pixel of either transparent or the same color as reference
    // color will be filled.
    if (mProperties.bucketFillReferenceMode == 1) {
        if (mReferenceColor != pixelColor)
        {
            // Special case for filling with alpha
            if ((mReferenceColor == targetPixelColor && targetPixelColor == preFill) || pixelColor == 0) {
                return true;
            }
            return false;
        }
        return true;
    }

    // Ensures that while dragging, only a pixel that is transparent or not the same color as target
    // will be be allowed to go through otherwise it will return false
    if (mProperties.bucketFillReferenceMode == 0) {
        if (targetPixelColor != pixelColor)
        {
            if (mReferenceColor == targetPixelColor && mReferenceColor != 0 && targetPixelColor != 0) {
                return true;
            }
            return false;
        }

        // Avoid spilling when target and reference is same layer
        if (mReferenceColor != targetPixelColor) {
            return false;
        }
    }

    return true;
}

void BitmapBucket::paint(QPointF updatedPoint, std::function<void(BucketState, int, int)> state)
{
    Layer* targetLayer = mTargetFillToLayer;
    int targetLayerIndex = mEditor->currentLayerIndex();
    QRgb multipliedColor = mBucketColor;

    const QPoint point = QPoint(qFloor(updatedPoint.x()), qFloor(updatedPoint.y()));
    const QRect cameraRect = mMaxFillRegion.toRect();
    const int tolerance = mProperties.toleranceEnabled ? static_cast<int>(mProperties.tolerance) : 0;
    const int currentFrameIndex = mEditor->currentFrame();
    const QRgb origColor = multipliedColor;

    if (!shouldFill(updatedPoint)) { return; }

    if (mProperties.bucketFillToLayerMode == 1)
    {
        targetLayer = mTargetFillToLayer;
        targetLayerIndex = mTargetFillToLayerIndex;
    }

    BitmapImage* targetImage = static_cast<LayerBitmap*>(targetLayer)->getLastBitmapImageAtFrame(currentFrameIndex, 0);

    if (targetImage == nullptr || !targetImage->isLoaded()) { return; } // Can happen if the first frame is deleted while drawing

    BitmapImage referenceImage = mReferenceImage;

    if (mProperties.bucketFillReferenceMode == 1) // All layers
    {
        referenceImage = mReferenceImage;

        // If the target image has been modified during move, we need to update the flattened reference image
        // but we know that only the target image could have been modified here, so no need to
        // look through all layers again, just paste the current target image.
        if (targetImage->isModified()) {
            referenceImage.paste(targetImage);
        }
    }

    if (mProperties.fillMode == 1)
    {
        // Pass a fully opaque version of the new color to floodFill
        // This is required so we can fully mask out the existing data before
        // writing the new color.
        QColor tempColor;
        tempColor.setRgba(multipliedColor);
        tempColor.setAlphaF(1);
        multipliedColor = tempColor.rgba();
    }

    state(BucketState::WillFill, targetLayerIndex, currentFrameIndex);

    BitmapImage replaceImage = BitmapImage(targetImage->bounds(), Qt::transparent);

    BitmapImage::floodFill(&replaceImage,
                           &referenceImage,
                           cameraRect,
                           point,
                           multipliedColor,
                           tolerance);

    if (mProperties.bucketFillExpandEnabled)
    {
        BitmapImage::expandFill(&replaceImage,
                                multipliedColor,
                                mProperties.bucketFillExpand);
    }

    if (mProperties.fillMode == 0)
    {
        targetImage->paste(&replaceImage);
    }
    else if (mProperties.fillMode == 2) {
        targetImage->paste(&replaceImage, QPainter::CompositionMode_DestinationOver);
    }
    else
    {
        // fill mode replace
        targetImage->paste(&replaceImage, QPainter::CompositionMode_DestinationOut);
        // Reduce the opacity of the fill to match the new color
        BitmapImage properColor(replaceImage.bounds(), QColor::fromRgba(origColor));
        properColor.paste(&replaceImage, QPainter::CompositionMode_DestinationIn);
        // Write reduced-opacity fill image on top of target image
        targetImage->paste(&properColor);
    }

    targetImage->modification();

    state(BucketState::DidFill, targetLayerIndex, currentFrameIndex);
}

BitmapImage BitmapBucket::flattenBitmapLayersToImage()
{
    BitmapImage flattenImage = BitmapImage();
    int currentFrame = mEditor->currentFrame();
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

std::pair<Layer*, int> BitmapBucket::findBitmapLayerBelow(Layer* layer, int layerIndex) const
{
    Layer* targetLayer = layer;
    bool foundLayerBelow = false;
    int layerBelowIndex = layerIndex;
    for (int i = layerIndex-1; i >= 0; i--) {
        Layer* searchlayer = mEditor->layers()->getLayer(i);

        if (searchlayer == nullptr) { Q_ASSERT(true); }

        if (searchlayer->type() == Layer::BITMAP && searchlayer->visible()) {
            targetLayer = searchlayer;
            foundLayerBelow = true;
            layerBelowIndex = i;
            break;
        }
    }

    if (foundLayerBelow && targetLayer->addNewKeyFrameAt(mEditor->currentFrame())) {
        emit mEditor->updateTimeLine();
    }
    return std::make_pair(targetLayer, layerBelowIndex);
}
