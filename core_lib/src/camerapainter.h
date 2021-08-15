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

#include <QRect>
#include <QTransform>
#include <QColor>
#include <QPalette>

#include "onionskinsubpainter.h"

class LayerCamera;
class Object;
class QPixmap;

class CameraPainter
{
public:
    explicit CameraPainter();

    void paint() const;
    void paintCached();

    void setOnionSkinPaintOptions(const OnionSkinPainterOptions& options) { mOnionSkinOptions = options; }
    void setCanvas(QPixmap* canvas);
    void preparePainter(const Object* object, int layerIndex, int frameIndex, QTransform transform, bool isPlaying, const QPalette palette);
    void resetCache();

private:
    void initializePainter(QPainter& painter, QPixmap& pixmap) const;
    void paintVisuals(QPainter& painter) const;
    void paintBorder(QPainter& painter, const QTransform& camTransform, const QRect& camRect) const;
    void paintInterpolations(QPainter& painter, LayerCamera* cameraLayer) const;
    void paintHandles(QPainter& painter, const QTransform& camTransform, const QRect& cameraRect, const qreal scale, const qreal rotation) const;
    void paintPath(QPainter& painter, const LayerCamera* cameraLayer, const int frameIndex, const QPointF& midPoint) const;

    const Object* mObject = nullptr;
    QPixmap* mCanvas = nullptr;

    std::unique_ptr<QPixmap> mCachedPaint = nullptr;
    QTransform mViewTransform;
    qreal mViewScaling = 1.0;

    OnionSkinSubPainter mOnionSkinPainter;
    OnionSkinPainterOptions mOnionSkinOptions;

    const int DOT_WIDTH = 6;
    const int HANDLE_WIDTH = 12;

    int mFrameIndex = 0;
    int mCurrentLayerIndex = 0;

    bool mIsPlaying = false;

    QColor mHighlightColor;
    QColor mHighlightedTextColor;
    QColor mTextColor;
};

#endif // CAMERAPAINTER_H
