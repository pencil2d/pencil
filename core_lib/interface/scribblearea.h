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
#include "vectorimage.h"
#include "bitmapimage.h"
#include "colourref.h"
#include "vectorselection.h"
#include "basetool.h"
#include "editor.h"
#include "colormanager.h"
#include "viewmanager.h"

class Editor;
class Layer;
class StrokeManager;
class BaseTool;
class ColorManager;
class PopupColorPaletteWidget;

enum DisplayEffect : uint32_t
{
    EFFECT_ANTIALIAS = 0,
    EFFECT_SHADOW,
    EFFECT_PREV_ONION,
    EFFECT_NEXT_ONION,
    EFFECT_AXIS,
	EFFECT_GRID_A,
    EFFECT_COUNT,
};


class ScribbleArea : public QWidget
{
    Q_OBJECT

    friend class MoveTool;
    friend class EditTool;
    friend class SmudgeTool;

public:
    ScribbleArea( QWidget *parent );
    ~ScribbleArea();

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

    bool isEffectOn( DisplayEffect e ) { return mEffects[ e ]; }
	void setEffect( DisplayEffect e, bool isOn ) { mEffects[ e ] = isOn; updateAllFrames(); }

    bool showThinLines() const { return mShowThinLines; }
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

    qreal getViewScaleX() const { return mEditor->view()->scaling(); }
    qreal getTempViewScaleX() const { return mEditor->view()->scaling(); }
    qreal getViewScaleY() const { return mEditor->view()->scaling(); }
    qreal getTempViewScaleY() const { return mEditor->view()->scaling(); }

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

    QPointF pixelToPoint( QPointF pixel );

    StrokeManager *getStrokeManager() const { return mStrokeManager; }

    PopupColorPaletteWidget *getPopupPalette() const { return m_popupPaletteWidget; }

    Editor* editor() { return mEditor; }

signals:
    void modification();
    void modification( int );
    void thinLinesChanged( bool );
    void outlinesChanged( bool );

    void onionPrevChanged( bool );
    void onionNextChanged( bool );
    void multiLayerOnionSkinChanged( bool );

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
    void toggleGridA( bool );

    void resetView();

    void zoomIn();
    void zoomOut();
    void rotatecw();
    void rotateacw();

    void setCurveSmoothing( int );
    void setBackground( int );
    void setBackgroundBrush( QString );
    void toggleThinLines();
    void toggleOutlines();
    void toggleMirror();
    void toggleMirrorV();
    void toggleShowAllLayers();
    void escape();

    void toggleMultiLayerOnionSkin( bool );
    void togglePopupPalette();

    void updateToolCursor();

protected:
    void tabletEvent( QTabletEvent *event ) override;
    void wheelEvent( QWheelEvent *event ) override;
    void mousePressEvent( QMouseEvent *event ) override;
    void mouseMoveEvent( QMouseEvent *event ) override;
    void mouseReleaseEvent( QMouseEvent *event ) override;
    void mouseDoubleClickEvent( QMouseEvent *event ) override;
    void keyPressEvent( QKeyEvent *event ) override;
    void keyReleaseEvent( QKeyEvent *event ) override;
    void paintEvent( QPaintEvent *event ) override;
    void resizeEvent( QResizeEvent *event ) override;

public:
    void drawPolyline( QList<QPointF> points, QPointF lastPoint );
    void endPolyline( QList<QPointF> points );

    void drawLine( QPointF P1, QPointF P2, QPen pen, QPainter::CompositionMode cm );
    void drawPath( QPainterPath path, QPen pen, QBrush brush, QPainter::CompositionMode cm );
    void drawBrush( QPointF thePoint, qreal brushWidth, qreal offset, QColor fillColour, qreal opacity );
    void blurBrush( BitmapImage *bmiSource_, QPointF srcPoint_, QPointF thePoint_, qreal brushWidth_, qreal offset_, qreal opacity_ );
    void liquifyBrush( BitmapImage *bmiSource_, QPointF srcPoint_, QPointF thePoint_, qreal brushWidth_, qreal offset_, qreal opacity_ );
    void floodFill( VectorImage *vectorImage, QPoint point, QRgb targetColour, QRgb replacementColour, int tolerance );

    void paintBitmapBuffer();
    void clearBitmapBuffer();
    void refreshBitmap( QRect rect, int rad );
    void refreshVector( QRect rect, int rad );
    void setGaussianGradient( QGradient &gradient, QColor colour, qreal opacity, qreal offset );

private:
    void drawCanvas( int frame, QRect rect );
    void drawShadow( QPainter& );
	void drawAxis( QPainter& );
	void drawGrid( QPainter& );

    void toggledOnionColor();
    void recentre();
    void setView( const QTransform& );

	void floodFillError( int errorType );

    MoveMode mMoveMode;
    ToolType mPrevTemporalToolType;
    ToolType mPrevToolType; // previous tool (except temporal)

    StrokeManager* mStrokeManager;

    Editor* mEditor;

    PopupColorPaletteWidget* m_popupPaletteWidget; // color palette popup (may be enhanced with tools)

    bool mIsSimplified = false;
    bool mShowThinLines;
    int  mShowAllLayers;
    bool mUsePressure = true;
    bool mMakeInvisible;
    bool mToolCursors;
    qreal mCurveSmoothingLevel;
    bool onionBlue;
    bool onionRed;
    bool mMultiLayerOnionSkin; // future use. If required, just add a checkbox to updated it.
    QColor onionColor;

    bool mNeedUpdateAll;

    QBrush backgroundBrush;
public:
    BitmapImage* mBufferImg; // used to pre-draw vector modifications

private:
    void initDisplayEffect( std::vector< uint32_t >& );
    std::vector< uint32_t > mEffects;

    bool mKeyboardInUse;
    bool mMouseInUse;
    QPointF lastPixel, currentPixel;
    QPointF lastPoint, currentPoint;

    qreal tol;
    QList<int> closestCurves;
    QList<VertexRef> closestVertices;
    QPointF offset;

    //instant tool (temporal eg. eraser)
    bool instantTool = false; //whether or not using temporal tool

    VectorSelection vectorSelection;
    QTransform selectionTransformation;

    // View Matrix
    QTransform mView;

    QPixmap mCanvas;

    // debug
    QRectF debugRect;

    QLoggingCategory mLog;
};

#endif
