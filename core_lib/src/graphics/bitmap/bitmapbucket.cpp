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
    const QPoint& point = QPoint(qFloor(fillPoint.x()), qFloor(fillPoint.y()));

    Q_ASSERT(mTargetFillToLayer);

    BitmapImage singleLayerImage = *static_cast<BitmapImage*>(initialLayer->getLastKeyFrameAtPosition(frameIndex));
    if (properties.bucketFillReferenceMode == 1) // All layers
    {
        mReferenceImage = flattenBitmapLayersToImage();
    } else {
        mReferenceImage = singleLayerImage;
    }
    mStartReferenceColor = mReferenceImage.constScanLine(point.x(), point.y());
    mUseDragToFill = canUseDragToFill(point, color, singleLayerImage);

    mPixelCache = new QHash<QRgb, bool>();
}

bool BitmapBucket::canUseDragToFill(const QPoint& fillPoint, const QColor& bucketColor, const BitmapImage& referenceImage)
{
    QRgb pressReferenceColorSingleLayer = referenceImage.constScanLine(fillPoint.x(), fillPoint.y());
    QRgb startRef = qUnpremultiply(pressReferenceColorSingleLayer);

    if (mProperties.fillMode == 0 && ((QColor(qRed(startRef), qGreen(startRef), qBlue(startRef)) == bucketColor.rgb() && qAlpha(startRef) == 255) || bucketColor.alpha() == 0)) {
        // In overlay mode: When the reference pixel matches the bucket color and the reference is fully opaque
        // Otherwise when the bucket alpha is zero.
        return false;
    } else if (mProperties.fillMode == 2 && qAlpha(startRef) == 255) {
        // In behind mode: When the reference pixel is already fully opaque, the output will be invisible.
        return false;
    }

    return true;
}

bool BitmapBucket::allowFill(const QPoint& checkPoint, const QRgb& checkColor) const
{
    // A normal click to fill should happen unconditionally, because the alternative is utterly confusing.
    if (!mFilledOnce) {
        return true;
    }

    return allowContinuousFill(checkPoint, checkColor);
}

bool BitmapBucket::allowContinuousFill(const QPoint& checkPoint, const QRgb& checkColor) const
{
    if (!mUseDragToFill) {
        return false;
    }

    const QRgb& colorOfReferenceImage = mReferenceImage.constScanLine(checkPoint.x(), checkPoint.y());

    if (checkColor == mBucketColor && (mProperties.fillMode == 1 || qAlpha(checkColor) == 255))
    {
        // Avoid filling if target pixel color matches fill color
        // to avoid creating numerous seemingly useless undo operations
        return false;
    }

    return BitmapImage::compareColor(colorOfReferenceImage, mStartReferenceColor, mTolerance, mPixelCache) &&
           (checkColor == 0 || BitmapImage::compareColor(checkColor, mStartReferenceColor, mTolerance, mPixelCache));
}

void BitmapBucket::paint(const QPointF& updatedPoint, std::function<void(BucketState, int, int)> state)
{
    const QPoint& point = QPoint(qFloor(updatedPoint.x()), qFloor(updatedPoint.y()));
    const int currentFrameIndex = mEditor->currentFrame();

    BitmapImage* targetImage = static_cast<LayerBitmap*>(mTargetFillToLayer)->getLastBitmapImageAtFrame(mEditor->currentFrame(), 0);
    if (targetImage == nullptr || !targetImage->isLoaded()) { return; } // Can happen if the first frame is deleted while drawing

    const QRgb& targetPixelColor = targetImage->constScanLine(point.x(), point.y());

    if (!allowFill(point, targetPixelColor)) {
        return;
    }

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
    mFilledOnce = true;
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
