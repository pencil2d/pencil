/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2014 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include <cstdint>
#include <QColor>
#include <QTransform>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QFrame>
#include <QHash>
#include "log.h"
#include "pencildef.h"
#include "vectorimage.h"
#include "bitmapimage.h"
#include "colourref.h"
#include "vectorselection.h"
#include "colormanager.h"
#include "viewmanager.h"
#include "canvasrenderer.h"
#include "preferencemanager.h"


class Layer;
class Editor;
class BaseTool;
class StrokeManager;
class ColorManager;
//class PopupColorPaletteWidget;


class ScribbleArea : public QWidget
{
    Q_OBJECT

    friend class MoveTool;
    friend class EditTool;
    friend class SmudgeTool;

public:
    ScribbleArea( QWidget *parent );
    ~ScribbleArea();

    bool init();
    void setCore( Editor* pCore ) { mEditor = pCore; }

    void deleteSelection();
    void setSelection( QRectF rect, bool );
    void displaySelectionProperties();
    QRectF getSelection() const { return mySelection; }
    bool somethingSelected;
    QRectF mySelection, myTransformedSelection, myTempTransformedSelection;
    qreal myRotatedAngle;
    qreal myFlipX, myFlipY; // scale -1.0 or +1.0

    bool areLayersSane() const;
    bool isLayerPaintable() const;

    static QBrush getBackgroundBrush( QString );

    void setEffect(EFFECT e, bool isOn );

    int showAllLayers() const { return mShowAllLayers; }
    qreal getCurveSmoothing() const { return mCurveSmoothingLevel; }
    bool usePressure() const { return mUsePressure; }
    bool makeInvisible() const { return mMakeInvisible; }

    enum MoveMode { MIDDLE, TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT, ROTATION, SYMMETRY };
    MoveMode getMoveMode() const { return mMoveMode; }
    void setMoveMode( MoveMode moveMode ) { mMoveMode = moveMode; }

    QTransform getView();
    QRectF getViewRect();
    QPointF getCentralPoint();

    void updateCurrentFrame();
    void updateFrame( int frame );
    void updateAllFrames();
    void updateAllVectorLayersAtCurrentFrame();
    void updateAllVectorLayersAt( int frame );
    void updateAllVectorLayers();

    bool shouldUpdateAll() const { return mNeedUpdateAll; }
    void setAllDirty() { mNeedUpdateAll = true; }

    BaseTool* currentTool();
    BaseTool* getTool( ToolType eToolMode );
    void setCurrentTool( ToolType eToolMode );
    void setTemporaryTool( ToolType eToolMode );
    void setPrevTool();

    StrokeManager *getStrokeManager() const { return mStrokeManager; }

    //PopupColorPaletteWidget *getPopupPalette() const { return m_popupPaletteWidget; }

    Editor* editor() { return mEditor; }

    void floodFillError( int errorType );

signals:
    void modification();
    void modification( int );
    void multiLayerOnionSkinChanged( bool );
    void refreshPreview();

public slots:
    void clearImage();
    void calculateSelectionRect();
    void calculateSelectionTransformation();
    void paintTransformedSelection();
    void setModified( int layerNumber, int frameNumber );

    void selectAll();
    void deselectAll();

    void toggleOnionPrev( bool );
    void toggleOnionNext( bool );
    void toggleOnionBlue( bool );
    void toggleOnionRed( bool );
    void toggleGrid( bool );

    void setCurveSmoothing( int );
    void setBackground( int );
    void setBackgroundBrush( QString );
    void toggleThinLines();
    void toggleOutlines();
    void toggleShowAllLayers();
    void toggleCameraBorder( bool );
    void escape();

    void toggleMultiLayerOnionSkin( bool );
    void togglePopupPalette();

    void updateToolCursor();
    void paletteColorChanged(QColor);

protected:
    void tabletEvent( QTabletEvent* ) override;
    void wheelEvent( QWheelEvent* ) override;
    void mousePressEvent( QMouseEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;
    void mouseDoubleClickEvent( QMouseEvent* ) override;
    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;
    void paintEvent( QPaintEvent* ) override;
    void resizeEvent( QResizeEvent* ) override;

public:
    void drawPolyline( QList<QPointF> points, QPointF lastPoint );
    void endPolyline( QList<QPointF> points );

    void drawLine( QPointF P1, QPointF P2, QPen pen, QPainter::CompositionMode cm );
    void drawPath( QPainterPath path, QPen pen, QBrush brush, QPainter::CompositionMode cm );
    void drawPencil( QPointF thePoint, qreal brushWidth, QColor fillColour, qreal opacity );
    void drawBrush( QPointF thePoint, qreal brushWidth, qreal offset, QColor fillColour, qreal opacity );
    void drawEraser( QPointF thePoint, qreal brushWidth, qreal offset, QColor fillColour, qreal opacity );
    void blurBrush( BitmapImage *bmiSource_, QPointF srcPoint_, QPointF thePoint_, qreal brushWidth_, qreal offset_, qreal opacity_ );
    void liquifyBrush( BitmapImage *bmiSource_, QPointF srcPoint_, QPointF thePoint_, qreal brushWidth_, qreal offset_, qreal opacity_ );

    void paintBitmapBuffer();
    void clearBitmapBuffer();
    void refreshBitmap( const QRectF& rect, int rad );
    void refreshVector( const QRectF& rect, int rad );
    void setGaussianGradient( QGradient &gradient, QColor colour, qreal opacity, qreal offset );

    BitmapImage* mBufferImg; // used to pre-draw vector modifications
    BitmapImage* mStrokeImg; // used for brush strokes before they are finalized

private:
    void drawCanvas( int frame, QRect rect );
    void drawShadow( QPainter& );
    void drawAxis( QPainter& );
    void drawGrid( QPainter& );

    void toggledOnionColor();

    MoveMode mMoveMode = MIDDLE;
    ToolType mPrevTemporalToolType;
    ToolType mPrevToolType = PEN; // previous tool (except temporal)

    StrokeManager* mStrokeManager = nullptr;

    Editor* mEditor = nullptr;

    //PopupColorPaletteWidget* m_popupPaletteWidget; // color palette popup (may be enhanced with tools)

    bool mIsSimplified  = false;
    bool mShowThinLines = false;
    int  mShowAllLayers;
    bool mUsePressure   = true;
    bool mMakeInvisible = false;
    bool mToolCursors   = true;
    qreal mCurveSmoothingLevel;
    bool onionBlue = false;
    bool onionRed  = false;
    bool mMultiLayerOnionSkin; // future use. If required, just add a checkbox to updated it.
    QColor onionColor;

    bool mNeedUpdateAll = false;

    QBrush mBackgroundBrush;
  

private: 

    bool mKeyboardInUse = false;
    bool mMouseInUse    = false;
    QPointF mLastPixel;
    QPointF mCurrentPixel;
    QPointF mLastPoint;
    QPointF mCurrentPoint;

    qreal tol;
    QList<int> mClosestCurves;
    QList<VertexRef> mClosestVertices;
    QPointF mOffset;

    //instant tool (temporal eg. eraser)
    bool instantTool = false; //whether or not using temporal tool

    VectorSelection vectorSelection;
    QTransform selectionTransformation;

    PreferenceManager *mPrefs = nullptr;

    QPixmap mCanvas;
    CanvasRenderer mCanvasRenderer;

    // debug
    QRectF debugRect;
    QLoggingCategory mLog;
};

#endif
