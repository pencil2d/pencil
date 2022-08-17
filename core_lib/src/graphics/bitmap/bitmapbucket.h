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
#ifndef BITMAPBUCKET_H
#define BITMAPBUCKET_H

#include "bitmapimage.h"
#include "basetool.h"

#include <functional>

class Layer;
class Editor;

enum class BucketState
{
    WillFillTarget, // Before applying to target image
    DidFillTarget, // After calling floodfill and applied to target
};

class BitmapBucket
{
public:
    explicit BitmapBucket();
    explicit BitmapBucket(Editor* editor, QColor color, QRect maxFillRegion, QPointF fillPoint, Properties properties);

    /** Will paint at the given point, given that it makes sense.. canUse is always called prior to painting
     *
     * @param updatedPoint - the point where to point
     * @param progress - a function that returns the progress of the paint operation,
     * the layer and frame that was affected at the given point.
     */
    void paint(const QPointF updatedPoint, std::function<void(BucketState, int, int)> progress);

private:


    /** Based on the various factors dependant on which tool properties are set,
     *  the result will:
     *
     *  BucketProgress: BeforeFill
     *  to allow filling
     *
     * @param checkPoint
     * @return True if you are allowed to fill, otherwise false
     */
    bool shouldFill(QPointF checkPoint) const;

    std::pair<Layer*, int> findBitmapLayerBelow(Layer* targetLayer, int layerIndex) const;
    BitmapImage flattenBitmapLayersToImage();

    Editor* mEditor = nullptr;
    Layer* mTargetFillToLayer = nullptr;

    BitmapImage mReferenceImage;
    QRgb mBucketColor = 0;
    QRgb mReferenceColor = 0;
    QRgb mAppliedColor = 0;

    QPointF mBucketStartPoint;
    QRect mMaxFillRegion;

    int mTargetFillToLayerIndex = -1;

    Properties mProperties;

    bool mFirstPaint = true;
};

#endif // BITMAPBUCKET_H
