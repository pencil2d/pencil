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


#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include <cstdint>
#include <ctime>
#include <deque>
#include <memory>

#include <QColor>
#include <QPoint>
#include <QWidget>
#include <QPixmapCache>

#include "movemode.h"
#include "log.h"
#include "pencildef.h"
#include "bitmapimage.h"
#include "canvaspainter.h"
#include "overlaypainter.h"
#include "preferencemanager.h"
#include "strokemanager.h"
#include "selectionpainter.h"

class Layer;
class Editor;
class BaseTool;
class PointerEvent;
class BitmapImage;
class VectorImage;


class ScribbleArea : public QWidget
{
    Q_OBJECT

    friend class MoveTool;
    friend class EditTool;
    friend class SmudgeTool;
    friend class BucketTool;

public:
    ScribbleArea(QWidget* parent);
    ~ScribbleArea() override;

    bool init();
    void setEditor(Editor* e) { mEditor = e; }
    StrokeManager* getStrokeManager() const { return mStrokeManager.get(); }
    Editor* editor() const { return mEditor; }

    void deleteSelection();
    void applySelectionChanges();
    void displaySelectionProperties();

    void paintTransformedSelection();
    void applyTransformedSelection();
    void cancelTransformedSelection();

    bool isLayerPaintable() const;

    QVector<QPoint> calcSelectionCenterPoints();

    void setEffect(SETTING e, bool isOn);

    LayerVisibility getLayerVisibility() const { return mLayerVisibility; }
    qreal getCurveSmoothing() const { return mCurveSmoothingLevel; }
    bool usePressure() const { return mUsePressure; }
    bool makeInvisible() const { return mMakeInvisible; }

    QRect getCameraRect();
    QPointF getCentralPoint();

    /** Update current frame.
     *  calls update() behind the scene and update cache if necessary */
    void updateCurrentFrame();
    /** Update frame.
     * calls update() behind the scene and update cache if necessary */
    void updateFrame(int frame);

    /** Frame scrubbed, invalidate relevant cache */
    void onScrubbed(int frameNumber);

    /** Multiple frames modified, invalidate cache for affected frames */
    void onFramesModified();

    /** Playstate changed, invalidate relevant cache */
    void onPlayStateChanged();

    /** View updated, invalidate relevant cache */
    void onViewChanged();

    /** Frame modified, invalidate cache for frame if any */
    void onFrameModified(int frameNumber);

    /** Current frame modified, invalidate current frame cache if any.
     * Convenient function that does the same as onFrameModified */
    void onCurrentFrameModified();

    /** Layer changed, invalidate relevant cache */
    void onLayerChanged();

    /** Selection was changed, keep cache */
    void onSelectionChanged();

    /** Onion skin type changed, all frames will be affected.
     * All cache will be invalidated */
    void onOnionSkinTypeChanged();

    /** Object updated, invalidate all cache */
    void onObjectLoaded();

    /** Set frame on layer to modified and invalidate current frame cache */
    void setModified(int layerNumber, int frameNumber);
    void setModified(const Layer* layer, int frameNumber);

    void flipSelection(bool flipVertical);
    void renderOverlays();
    void prepOverlays();

    BaseTool* currentTool() const;
    BaseTool* getTool(ToolType eToolMode);
    void setCurrentTool(ToolType eToolMode);

    void floodFillError(int errorType);

    bool isMouseInUse() const { return mMouseInUse; }
    bool isTabletInUse() const { return mTabletInUse; }
    bool isPointerInUse() const { return mMouseInUse || mTabletInUse; }

    void keyEvent(QKeyEvent* event);
    void keyEventForSelection(QKeyEvent* event);

signals:
    void modified(int, int);
    void multiLayerOnionSkinChanged(bool);
    void refreshPreview();
    void selectionUpdated();

public slots:
    void clearImage();
    void setCurveSmoothing(int);
    void toggleThinLines();
    void toggleOutlines();
    void increaseLayerVisibilityIndex();
    void decreaseLayerVisibilityIndex();
    void setLayerVisibility(LayerVisibility visibility);

    void updateToolCursor();
    void paletteColorChanged(QColor);

    void showLayerNotVisibleWarning();
    void updateOriginalPolygonF();
    void setOriginalPolygonF(QPolygonF polygon) { mOriginalPolygonF = polygon; }
    QPolygonF getOriginalPolygonF() { return mOriginalPolygonF; }


protected:
    bool event(QEvent *event) override;
    void tabletEvent(QTabletEvent*) override;
    void wheelEvent(QWheelEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseDoubleClickEvent(QMouseEvent*) override;
    void keyPressEvent(QKeyEvent*) override;
    void keyReleaseEvent(QKeyEvent*) override;
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;

public:
    void drawPolyline(QPainterPath path, QPen pen, bool useAA);
    void drawLine(QPointF P1, QPointF P2, QPen pen, QPainter::CompositionMode cm);
    void drawPath(QPainterPath path, QPen pen, QBrush brush, QPainter::CompositionMode cm);
    void drawPen(QPointF thePoint, qreal brushWidth, QColor fillColor, bool useAA = true);
    void drawPencil(QPointF thePoint, qreal brushWidth, qreal fixedBrushFeather, QColor fillColor, qreal opacity);
    void drawBrush(QPointF thePoint, qreal brushWidth, qreal offset, QColor fillColor, qreal opacity, bool usingFeather = true, bool useAA = false);
    void blurBrush(BitmapImage *bmiSource_, QPointF srcPoint_, QPointF thePoint_, qreal brushWidth_, qreal offset_, qreal opacity_);
    void liquifyBrush(BitmapImage *bmiSource_, QPointF srcPoint_, QPointF thePoint_, qreal brushWidth_, qreal offset_, qreal opacity_);

    void paintBitmapBuffer();
    void paintBitmapBufferRect(const QRect& rect);
    void paintCanvasCursor(QPainter& painter);
    void clearBitmapBuffer();
    void refreshBitmap(const QRectF& rect, int rad);
    void refreshVector(const QRectF& rect, int rad);
    void setGaussianGradient(QGradient &gradient, QColor color, qreal opacity, qreal offset);

    void pointerPressEvent(PointerEvent*);
    void pointerMoveEvent(PointerEvent*);
    void pointerReleaseEvent(PointerEvent*);

    void updateCanvasCursor();

    /// Call this when starting to use a paint tool. Checks whether we are drawing
    /// on an empty frame, and if so, takes action according to use preference.
    void handleDrawingOnEmptyFrame();

    BitmapImage* mBufferImg = nullptr; // used to pre-draw vector modifications

    QPixmap mCursorImg;
    QPixmap mTransCursImg;

private:

    /** Invalidate the layer pixmap cache.
     * Call this in most situations where the layer rendering order is affected.
     * Peviously known as setAllDirty.
    */
    void invalidateLayerPixmapCache();

    /** Invalidate cache for the given frame */
    void invalidateCacheForFrame(int frameNumber);

    /** Invalidate all cache.
     * call this if you're certain that the change you've made affects all frames */
    void invalidateAllCache();

    /** invalidate cache for dirty keyframes. */
    void invalidateCacheForDirtyFrames();

    /** invalidate onion skin cache around frame */
    void invalidateOnionSkinsCacheAround(int frame);

    void prepCanvas(int frame, QRect rect);
    void drawCanvas(int frame, QRect rect);
    void settingUpdated(SETTING setting);
    void paintSelectionVisuals(QPainter &painter);

    BitmapImage* currentBitmapImage(Layer* layer) const;
    VectorImage* currentVectorImage(Layer* layer) const;

    MoveMode mMoveMode = MoveMode::NONE;

    BitmapImage mBitmapSelection; // used to temporary store a transformed portion of a bitmap image

    std::unique_ptr<StrokeManager> mStrokeManager;

    Editor* mEditor = nullptr;


    bool mIsSimplified = false;
    bool mShowThinLines = false;
    bool mQuickSizing = true;
    LayerVisibility mLayerVisibility = LayerVisibility::ALL;
    bool mUsePressure   = true;
    bool mMakeInvisible = false;
    bool mToolCursors = true;
    qreal mCurveSmoothingLevel = 0.0;
    bool mMultiLayerOnionSkin = false; // future use. If required, just add a checkbox to updated it.
    QColor mOnionColor;

private:
    bool mKeyboardInUse = false;
    bool mMouseInUse = false;
    bool mMouseRightButtonInUse = false;
    bool mTabletInUse = false;
    qreal mDevicePixelRatio = 1.;

    // Double click handling for tablet input
    void handleDoubleClick();
    bool mIsFirstClick = true;
    int mDoubleClickMillis = 0;
    // Microsoft suggests that a double click action should be no more than 500 ms
    const int DOUBLE_CLICK_THRESHOLD = 500;
    QTimer* mDoubleClickTimer = nullptr;

    QPoint mCursorCenterPos;
    QPointF mTransformedCursorPos;

    PreferenceManager* mPrefs = nullptr;

    QPixmap mCanvas;
    CanvasPainter mCanvasPainter;
    OverlayPainter mOverlayPainter;
    SelectionPainter mSelectionPainter;

    QPolygonF mOriginalPolygonF = QPolygonF();

    // Pixmap Cache keys
    QMap<unsigned int, QPixmapCache::Key> mPixmapCacheKeys;

    // debug
    QLoggingCategory mLog{ "ScribbleArea" };
};

#endif
