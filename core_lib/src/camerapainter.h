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

#ifndef CAMERAPAINTER_H
#define CAMERAPAINTER_H

#include <memory>
#include <QColor>
#include <QTransform>

#include "onionskinpainteroptions.h"
#include "onionskinsubpainter.h"

class LayerCamera;
class Object;
class QPalette;
class QPixmap;
class QRect;

class CameraPainter
{
public:
    explicit CameraPainter();

    void paint() const;
    void paintCached();

    void setOnionSkinPainterOptions(const OnionSkinPainterOptions& options) { mOnionSkinOptions = options; }
    void setCanvas(QPixmap* canvas);
    void preparePainter(const Object* object, int layerIndex, int frameIndex, const QTransform& transform, bool isPlaying, bool showHandles, const QPalette& palette);
    void resetCache();

private:
    void initializePainter(QPainter& painter, QPixmap& pixmap) const;
    void paintVisuals(QPainter& painter) const;
    void paintBorder(QPainter& painter, const QTransform& camTransform, const QRect& camRect) const;
    void paintInterpolations(QPainter& painter, const LayerCamera* cameraLayer) const;
    void paintHandles(QPainter& painter, const QTransform& camTransform, const QRect& cameraRect, const qreal scale, const qreal rotation, bool hollowHandles) const;
    void paintPath(QPainter& painter, const LayerCamera* cameraLayer, const int frameIndex, const QPointF& pathPoint) const;

    const Object* mObject = nullptr;
    QPixmap* mCanvas = nullptr;

    std::unique_ptr<QPixmap> mCachedPaint = nullptr;
    QTransform mViewTransform;

    OnionSkinSubPainter mOnionSkinPainter;
    OnionSkinPainterOptions mOnionSkinOptions;

    int mFrameIndex = 0;
    int mCurrentLayerIndex = 0;

    bool mIsPlaying = false;
    bool mShowHandles = false;

    QColor mHighlightColor;
    QColor mHighlightedTextColor;
};

#endif // CAMERAPAINTER_H
