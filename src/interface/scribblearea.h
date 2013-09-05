/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon

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

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QFrame>
#include <QHash>
#include "vectorimage.h"
#include "bitmapimage.h"
#include "colourref.h"
#include "vectorselection.h"
#include "basetool.h"

class Editor;
class Layer;
class StrokeManager;
class BaseTool;
class PopupColorPaletteWidget;

class ScribbleArea : public QWidget
{
    Q_OBJECT

    friend class MoveTool;
    friend class EditTool;
    friend class SmudgeTool;

public:
    ScribbleArea(QWidget *parent = 0, Editor *m_pEditor = 0);

    void next(const int &i);

    void resetTools();

    void deleteSelection();
    void setSelection(QRectF rect, bool);
    void displaySelectionProperties();
    QRectF getSelection() const { return mySelection; }
    bool somethingSelected;
    bool readCanvasFromCache;
    QRectF mySelection, myTransformedSelection, myTempTransformedSelection;
    qreal myRotatedAngle;

    bool isModified() const { return modified; }
    bool areLayersSane() const;
    bool isLayerPaintable() const;

    static QBrush getBackgroundBrush(QString);

    bool showThinLines() const { return m_showThinLines; }
    int showAllLayers() const { return m_showAllLayers; }
    qreal getCurveSmoothing() const { return curveSmoothing; }
    bool useAntialiasing() const { return m_antialiasing; }
    bool usePressure() const { return m_usePressure; }
    bool makeInvisible() const { return m_makeInvisible; }

    enum MoveMode { MIDDLE, TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT, ROTATION};
    MoveMode getMoveMode() const { return m_moveMode; }
    void setMoveMode(MoveMode moveMode) { m_moveMode = moveMode; }

    QMatrix getView();
    QRectF getViewRect();
    QPointF getCentralPoint();

    qreal getViewScaleX() const { return myView.m11(); }
    qreal getTempViewScaleX() const { return myTempView.m11(); }
    qreal getViewScaleY() const { return myView.m22(); }
    qreal getTempViewScaleY() const { return myTempView.m22(); }

    QMatrix getTransformationMatrix() const { return transMatrix; }
    void setTransformationMatrix(QMatrix matrix);
    void applyTransformationMatrix();

    void updateFrame();
    void updateFrame(int frame);
    void updateAllFrames();
    void updateAllVectorLayersAtCurrentFrame();
    void updateAllVectorLayersAt(int frame);
    void updateAllVectorLayers();

    bool shouldUpdateAll() const {  return updateAll; }
    void setAllDirty() { updateAll = true; }

    BaseTool *currentTool();
    BaseTool *getTool(ToolType eToolMode);
    void setCurrentTool(ToolType eToolMode);
    void setTemporaryTool(ToolType eToolMode);
    void switchTool(ToolType type);
    QList<BaseTool *> getTools();

    void setPrevTool();

    QPointF pixelToPoint(QPointF pixel);

    StrokeManager *getStrokeManager() const { return m_strokeManager; }

    PopupColorPaletteWidget *getPopupPalette() const { return m_popupPaletteWidget; }

    void keyPressed(QKeyEvent *event);

    Editor *getEditor() { return m_pEditor; }

signals:
    void modification();
    void modification(int);
    void thinLinesChanged(bool);
    void outlinesChanged(bool);
    //void showAllLayersChanged(bool);

    void onionPrevChanged(bool);
    void onionNextChanged(bool);

    void pencilOn();
    void eraserOn();
    void selectOn();
    void moveOn();
    void penOn();
    void handOn();
    void polylineOn();
    void bucketOn();
    void eyedropperOn();
    void brushOn();
    void smudgeOn();

public slots:
    void clearImage();
    void calculateSelectionRect();
    void calculateSelectionTransformation();
    void paintTransformedSelection();
    void setModified(int layerNumber, int frameNumber);

    void selectAll();
    void deselectAll();

    void toggleOnionPrev(bool);
    void toggleOnionNext(bool);
    void grid();
    //void pressureSlot(int);
    //void invisibleSlot(int);

    void resetView();
    void setMyView(QMatrix view);
    QMatrix getMyView();

    void zoom();
    void zoom1();
    void rotatecw();
    void rotateacw();
    void setWidth(const qreal);
    void setFeather(const qreal);
    void setOpacity(const qreal);
    void setPressure(const bool);
    void setInvisibility(const bool);
    void setPreserveAlpha(const bool);
    void setFollowContour(const bool);

    void setCurveOpacity(int);
    void setCurveSmoothing(int);
    void setHighResPosition(int);
    void setAntialiasing(int);
    void setGradients(int);
    void setBackground(int);
    void setBackgroundBrush(QString);
    void setShadows(int);
    void setToolCursors(int);
    void setStyle(int);
    void toggleThinLines();
    void toggleOutlines();
    void toggleMirror();
    void toggleMirrorV();
    void toggleShowAllLayers();
    void escape();

    void togglePopupPalette();

protected:
    void tabletEvent(QTabletEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void recentre();
    void setView();
    void setView(QMatrix);

public:
    void drawPolyline(QList<QPointF> points, QPointF lastPoint);
    void endPolyline(QList<QPointF> points);

    void drawLine( QPointF P1, QPointF P2, QPen pen, QPainter::CompositionMode cm);
    void drawPath(QPainterPath path, QPen pen, QBrush brush, QPainter::CompositionMode cm);
    void drawBrush(QPointF thePoint, qreal brushWidth, qreal offset, QColor fillColour, qreal opacity);
    void drawTexturedBrush(BitmapImage *argImg, QPointF srcPoint, QPointF thePoint, qreal brushWidth, qreal offset, qreal opacity);
    void floodFill(VectorImage *vectorImage, QPoint point, QRgb targetColour, QRgb replacementColour, int tolerance);

    void paintBitmapBuffer();
    void clearBitmapBuffer();
    void refreshBitmap(QRect rect, int rad);
    void refreshVector(QRect rect, int rad);
    void setGaussianGradient(QGradient &gradient, QColor colour, qreal opacity, qreal offset);

protected:
    void updateCanvas(int frame, QRect rect);

    void floodFillError(int errorType);

    MoveMode m_moveMode;
    ToolType prevMode;

    StrokeManager *m_strokeManager;
    BaseTool *m_currentTool;
    QHash<ToolType, BaseTool *> m_toolSetHash;

    Editor *m_pEditor;

    PopupColorPaletteWidget* m_popupPaletteWidget; // color palette popup (may be enhanced with tools)

    int tabletEraserBackupToolMode;
    bool modified;
    bool simplified;

    bool m_showThinLines;
    int  m_showAllLayers;
    bool m_usePressure;
    bool m_makeInvisible;
    bool m_antialiasing;
    bool shadows;
    bool toolCursors;
    int  gradients;
    qreal curveOpacity;
    qreal curveSmoothing;
    bool onionPrev, onionNext;
    bool updateAll;

    bool followContour;

    QBrush backgroundBrush;
public:
    BitmapImage *bufferImg; // used to pre-draw vector modifications
protected:
    //Buffer buffer; // used to pre-draw bitmap modifications, such as lines, brushes, etc.

    bool keyboardInUse;
    bool mouseInUse;
    QPointF lastPixel, currentPixel;
    QPointF lastPoint, currentPoint;
    //QBrush brush; // the current brush

    qreal tol;
    QList<int> closestCurves;
    QList<VertexRef> closestVertices;
    QPointF offset;

    //instant tool (temporal eg. eraser)
    bool instantTool; //whether or not using temporal tool
    ToolType prevToolType; //to recover current tool

    VectorSelection vectorSelection;
    //bool selectionChanged;
    QMatrix selectionTransformation;

    QMatrix myView, myTempView, centralView, transMatrix;

    QPixmap canvas;

    // debug
    QRectF debugRect;
};

#endif


