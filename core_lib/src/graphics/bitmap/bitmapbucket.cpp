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
                           QRect maxFillRegion,
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

    mTolerance = mProperties.toleranceEnabled ? static_cast<int>(mProperties.tolerance) : 0;

    if (properties.bucketFillToLayerMode == 1)
    {
        auto result = findBitmapLayerBelow(initialLayer, initialLayerIndex);
        mTargetFillToLayer = result.first;
        mTargetFillToLayerIndex = result.second;
    }
    Q_ASSERT(mTargetFillToLayer);

    mReferenceImage = *static_cast<BitmapImage*>(initialLayer->getLastKeyFrameAtPosition(frameIndex));
    if (properties.bucketFillReferenceMode == 1) // All layers
    {
        mReferenceImage = flattenBitmapLayersToImage();
    }
    const QPoint point = QPoint(qFloor(fillPoint.x()), qFloor(fillPoint.y()));
    mStartReferenceColor = mReferenceImage.constScanLine(point.x(), point.y());

    BitmapImage* image = static_cast<LayerBitmap*>(mTargetFillToLayer)->getLastBitmapImageAtFrame(frameIndex, 0);
    mFillToImageColor = image->constScanLine(point.x(), point.y());

    mPixelCache = new QHash<QRgb, bool>();
}

bool BitmapBucket::allowFill(const QPoint& checkPoint) const
{
    if (mProperties.fillMode == 0 && qAlpha(mBucketColor) == 0)
    {
        // Filling in overlay mode with a fully transparent color has no
        // effect, so we can skip it in this case
        return false;
    }
    Q_ASSERT(mTargetFillToLayer);

    BitmapImage targetImage = *static_cast<LayerBitmap*>(mTargetFillToLayer)->getLastBitmapImageAtFrame(mEditor->currentFrame(), 0);

    if (!targetImage.isLoaded()) { return false; }

    QRgb colorOfReferenceImage = mReferenceImage.constScanLine(checkPoint.x(), checkPoint.y());
    QRgb targetPixelColor = targetImage.constScanLine(checkPoint.x(), checkPoint.y());

    // The remainder applies to drag fill: Ensure that we're only filling on either transparent or same color
    // and avoid filling the same area repeatedly

    // Rule 1: allow filling if the reference pixel is transparent and the target pixel is transparent
    if (mStartReferenceColor == 0 && colorOfReferenceImage == 0 && targetPixelColor == 0)
    {
        return true;
    }

    if ((BitmapImage::compareColor(colorOfReferenceImage, targetPixelColor, mTolerance, mPixelCache) &&
         BitmapImage::compareColor(mStartReferenceColor, targetPixelColor, mTolerance, mPixelCache)) ||
         (BitmapImage::compareColor(colorOfReferenceImage, mStartReferenceColor, mTolerance, mPixelCache) && targetPixelColor == 0)) {
        return true;
    }

    return false;
}

void BitmapBucket::paint(const QPointF updatedPoint, std::function<void(BucketState, int, int)> state)
{
    const QPoint point = QPoint(qFloor(updatedPoint.x()), qFloor(updatedPoint.y()));
    const int currentFrameIndex = mEditor->currentFrame();

    if (!allowFill(point)) { return; }

    BitmapImage* targetImage = static_cast<BitmapImage*>(mTargetFillToLayer->getLastKeyFrameAtPosition(currentFrameIndex));

    if (targetImage == nullptr || !targetImage->isLoaded()) { return; } // Can happen if the first frame is deleted while drawing

    QRgb fillColor = mBucketColor;
    if (mProperties.fillMode == 1)
    {
        // Pass a fully opaque version of the new color to floodFill
        // This is required so we can fully mask out the existing data before
        // writing the new color.
        QColor tempColor;
        tempColor.setRgba(fillColor);
        tempColor.setAlphaF(1);
        fillColor = tempColor.rgba();
    }

    BitmapImage* replaceImage = nullptr;

    int expandValue = mProperties.bucketFillExpandEnabled ? mProperties.bucketFillExpand : 0;
    bool didFloodFill = BitmapImage::floodFill(&replaceImage,
                           &mReferenceImage,
                           mMaxFillRegion,
                           point,
                           fillColor,
                           mTolerance,
                           expandValue);

    if (!didFloodFill) {
        delete replaceImage;
        return;
    }
    Q_ASSERT(replaceImage != nullptr);

    state(BucketState::WillFillTarget, mTargetFillToLayerIndex, currentFrameIndex);

    if (mProperties.fillMode == 0)
    {
        targetImage->paste(replaceImage);
    }
    else if (mProperties.fillMode == 2)
    {
        targetImage->paste(replaceImage, QPainter::CompositionMode_DestinationOver);
    }
    else
    {
        // fill mode replace
        targetImage->paste(replaceImage, QPainter::CompositionMode_DestinationOut);
        // Reduce the opacity of the fill to match the new color
        BitmapImage properColor(replaceImage->bounds(), QColor::fromRgba(mBucketColor));
        properColor.paste(replaceImage, QPainter::CompositionMode_DestinationIn);
        // Write reduced-opacity fill image on top of target image
        targetImage->paste(&properColor);
    }

    targetImage->modification();
    delete replaceImage;

    state(BucketState::DidFillTarget, mTargetFillToLayerIndex, currentFrameIndex);
}

BitmapImage BitmapBucket::flattenBitmapLayersToImage()
{
    BitmapImage flattenImage = BitmapImage();
    int currentFrame = mEditor->currentFrame();
    auto layerMan = mEditor->layers();
    for (int i = 0; i < layerMan->count(); i++)
    {
        Layer* layer = layerMan->getLayer(i);
        Q_ASSERT(layer);
        if (layer->type() == Layer::BITMAP && layer->visible())
        {
            BitmapImage* image = static_cast<LayerBitmap*>(layer)->getLastBitmapImageAtFrame(currentFrame);
            if (image) {
                flattenImage.paste(image);
            }
        }
    }
    return flattenImage;
}

std::pair<Layer*, int> BitmapBucket::findBitmapLayerBelow(Layer* targetLayer, int layerIndex) const
{
    bool foundLayerBelow = false;
    int layerBelowIndex = layerIndex;
    for (int i = layerIndex - 1; i >= 0; i--)
    {
        Layer* searchlayer = mEditor->layers()->getLayer(i);
        Q_ASSERT(searchlayer);

        if (searchlayer->type() == Layer::BITMAP && searchlayer->visible())
        {
            targetLayer = searchlayer;
            foundLayerBelow = true;
            layerBelowIndex = i;
            break;
        }
    }

    if (foundLayerBelow && !targetLayer->keyExists(mEditor->currentFrame()))
    {
        targetLayer->addNewKeyFrameAt(mEditor->currentFrame());
        emit mEditor->updateTimeLine();
    }
    return std::make_pair(targetLayer, layerBelowIndex);
}
