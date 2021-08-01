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

#include "onionskinsubpainter.h"

class Object;
class BitmapImage;
class ViewManager;
class LayerCamera;

struct CanvasPainterOptions
{
    bool  bAntiAlias = false;
    bool  bGrid = false;
    int   nGridSizeW = 50; /* This is the grid Width IN PIXELS. The grid will scale with the image, though */
    int   nGridSizeH = 50; /* This is the grid Height IN PIXELS. The grid will scale with the image, though */
    bool  bAxis = false;
    bool  bThinLines = false;
    bool  bOutlines = false;
    LayerVisibility eLayerVisibility = LayerVisibility::RELATED;
    float fLayerVisibilityThreshold = 0.f;
    float scaling = 1.0f;
    QPainter::CompositionMode cmBufferBlendMode = QPainter::CompositionMode_SourceOver;
    OnionSkinPainterOptions mOptionSkinOptions;
};

class CanvasPainter
{
    Q_DECLARE_TR_FUNCTIONS(CanvasPainter)
public:
    explicit CanvasPainter();
    virtual ~CanvasPainter();

    void setCanvas(QPixmap* canvas);
    void setViewTransform(const QTransform view, const QTransform viewInverse);

    void setOnionSkinOptions(const OnionSkinPainterOptions& onionSkinOptions) { mOnionSkinPaintOptions = onionSkinOptions;}
    void setOptions(const CanvasPainterOptions& p) { mOptions = p; }
    void setTransformedSelection(QRect selection, QTransform transform);
    void ignoreTransformedSelection();

    void setPaintSettings(const Object* object, int currentLayer, int frame, QRect rect, BitmapImage* buffer);
    void paint();
    void paintCached();
    void renderGrid(QPainter& painter) const;
    void renderOverlays(QPainter& painter) const;
    void resetLayerCache();

private:

    /**
     * CanvasPainter::initializePainter
     * Enriches the painter with a context and sets it's initial matrix.
     * @param painter The in/out painter
     * @param pixmap The paint device ie. a pixmap
     */
    void initializePainter(QPainter& painter, QPixmap& pixmap);

    void renderPreLayers(QPainter& painter);
    void renderCurLayer(QPainter& painter);
    void renderPostLayers(QPainter& painter);

    void paintBackground();
    void paintOnionSkin(QPainter& painter);

    void renderPostLayers(QPixmap* pixmap);
    void renderCurLayer(QPixmap* pixmap);
    void renderPreLayers(QPixmap* pixmap);

    void paintCurrentFrame(QPainter& painter, int startLayer, int endLayer);

    void paintBitmapFrame(QPainter&, Layer* layer, int nFrame, bool colorize, bool useLastKeyFrame, bool isCurrentFrame);
    void paintVectorFrame(QPainter&, Layer* layer, int nFrame, bool colorize, bool useLastKeyFrame, bool isCurrentFrame);

    void paintTransformedSelection(QPainter& painter) const;
    void paintGrid(QPainter& painter) const;
    void paintAxis(QPainter& painter) const;
    void prescale(BitmapImage* bitmapImage);

    /** Calculate layer opacity based on current layer offset */
    qreal calculateRelativeOpacityForLayer(int layerIndex) const;

private:

    CanvasPainterOptions mOptions;

    const Object* mObject = nullptr;
    QPixmap* mCanvas = nullptr;
    QTransform mViewTransform;
    QTransform mViewInverse;

    int mCurrentLayerIndex = 0;
    int mFrameNumber = 0;
    BitmapImage* mBuffer = nullptr;

    QImage mScaledBitmap;

    bool bMultiLayerOnionSkin = false;

    // Handle selection transformation
    bool mRenderTransform = false;
    QRect mSelection;
    QTransform mSelectionTransform;

    // Caches specifically for when drawing on the canvas
    std::unique_ptr<QPixmap> mPreLayersCache, mPostLayersCache;

    OnionSkinSubPainter mOnionSkinSubPainter;
    OnionSkinPainterOptions mOnionSkinPaintOptions;

    const static int OVERLAY_SAFE_CENTER_CROSS_SIZE = 25;
};

#endif // CANVASRENDERER_H
