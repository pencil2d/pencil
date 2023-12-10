/*

Pencil2D - Traditional Animation Software
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef CANVASPAINTER_H
#define CANVASPAINTER_H

#include <memory>
#include <QCoreApplication>
#include <QObject>
#include <QTransform>
#include <QPainter>
#include "log.h"
#include "pencildef.h"

#include "layer.h"

#include "onionskinpainteroptions.h"
#include "onionskinsubpainter.h"


class TiledBuffer;
class Object;
class BitmapImage;
class ViewManager;

struct CanvasPainterOptions
{
    bool  bAntiAlias = false;
    bool  bThinLines = false;
    bool  bOutlines = false;

    LayerVisibility eLayerVisibility = LayerVisibility::RELATED;
    float fLayerVisibilityThreshold = 0.f;
    float scaling = 1.0f;
    QPainter::CompositionMode cmBufferBlendMode = QPainter::CompositionMode_SourceOver;
    OnionSkinPainterOptions mOnionSkinOptions;
};

class CanvasPainter
{
    Q_DECLARE_TR_FUNCTIONS(CanvasPainter)
public:
    explicit CanvasPainter(QPixmap& canvas);
    virtual ~CanvasPainter();

    void reset();
    void setViewTransform(const QTransform view, const QTransform viewInverse);

    void setOnionSkinOptions(const OnionSkinPainterOptions& onionSkinOptions) { mOnionSkinPainterOptions = onionSkinOptions;}
    void setOptions(const CanvasPainterOptions& p) { mOptions = p; }
    void setTransformedSelection(QRect selection, QTransform transform);
    void ignoreTransformedSelection();

    void setPaintSettings(const Object* object, int currentLayer, int frame, TiledBuffer* tilledBuffer);
    void paint(const QRect& blitRect);
    void paintCached(const QRect& blitRect);
    void resetLayerCache();

private:

    /**
     * CanvasPainter::initializePainter
     * Enriches the painter with a context and sets it's initial matrix.
     * @param painter The in/out painter
     * @param pixmap The paint device ie. a pixmap
     * @param blitRect The rect where the blitting will occur
     */
    void initializePainter(QPainter& painter, QPaintDevice& device, const QRect& blitRect);

    void paintOnionSkin(QPainter& painter, const QRect& blitRect);

    void renderPostLayers(QPainter& painter, const QRect& blitRect);
    void renderPreLayers(QPainter& painter, const QRect& blitRect);

    void paintCurrentFrame(QPainter& painter, const QRect& blitRect, int startLayer, int endLayer);

    void paintTransformedSelection(QPainter& painter, BitmapImage* bitmapImage, const QRect& selection) const;

    void paintBitmapOnionSkinFrame(QPainter& painter, const QRect& blitRect, Layer* layer, int nFrame, bool colorize);
    void paintVectorOnionSkinFrame(QPainter& painter, const QRect& blitRect, Layer* layer, int nFrame, bool colorize);
    void paintOnionSkinFrame(QPainter& painter, QPainter& onionSkinPainter, int nFrame, bool colorize, qreal frameOpacity);

    void paintCurrentBitmapFrame(QPainter& painter, const QRect& blitRect, Layer* layer, bool isCurrentLayer);
    void paintCurrentVectorFrame(QPainter& painter, const QRect& blitRect, Layer* layer, bool isCurrentLayer);

    CanvasPainterOptions mOptions;

    const Object* mObject = nullptr;
    QPixmap& mCanvas;
    QTransform mViewTransform;
    QTransform mViewInverse;

    int mCurrentLayerIndex = 0;
    int mFrameNumber = 0;
    TiledBuffer* mTiledBuffer = nullptr;

    QImage mScaledBitmap;

    // Handle selection transformation
    bool mRenderTransform = false;
    QRect mSelection;
    QTransform mSelectionTransform;

    // Caches specifically for when drawing on the canvas
    QPixmap mPostLayersPixmap;
    QPixmap mPreLayersPixmap;
    QPixmap mCurrentLayerPixmap;
    QPixmap mOnionSkinPixmap;
    bool mPreLayersPixmapCacheValid = false;
    bool mPostLayersPixmapCacheValid = false;

    // There's a considerable amount of overhead in simply allocating a QPointF on the fly.
    // Since we just need to draw it at 0,0, we might as well make a const value for that purpose
    const QPointF mPointZero;


    OnionSkinSubPainter mOnionSkinSubPainter;
    OnionSkinPainterOptions mOnionSkinPainterOptions;

    const static int OVERLAY_SAFE_CENTER_CROSS_SIZE = 25;
};

#endif // CANVASRENDERER_H
