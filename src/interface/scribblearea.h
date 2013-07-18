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
#include "basetool.h"
#include "vectorselection.h"

class Editor;
class Layer;


class ScribbleArea : public QWidget
{
    Q_OBJECT

    // we declare them all friends for now until we move out all the tool relevant code to the tool classes
    // we'll then try to find some sensible interfaces between the tools and the scribble area
    // more specifically, i'm thinking of a stroke handler that will contain all the information about the current mouse stroke
    // and a drawing facade responsible for updating the scribblearea
    friend class PencilTool;
    friend class EraserTool;
    friend class PenTool;
    friend class BucketTool;

public:
    ScribbleArea(QWidget *parent = 0, Editor *m_pEditor = 0);

    void next(const int &i);

    void setColour(const int);
    void setColour(const QColor);
    void resetColours();

    void deleteSelection();
    void setSelection(QRectF rect, bool);
    void displaySelectionProperties();
    QRectF getSelection() { return mySelection; }
    bool somethingSelected;
    bool readCanvasFromCache;

    bool isModified() const { return modified; }

    static QBrush getBackgroundBrush(QString);

    bool thinLines() const { return showThinLines; }
    int allLayers() const { return showAllLayers; }

    QMatrix getView();
    QRectF getViewRect();
    QPointF getCentralPoint();

    void updateFrame();
    void updateFrame(int frame);
    void updateAllFrames();
    void updateAllVectorLayersAtCurrentFrame();
    void updateAllVectorLayersAt(int frame);
    void updateAllVectorLayers();
    bool getUpdateAll() {return updateAll;}

    QRectF mySelection, myTransformedSelection, myTempTransformedSelection;

    BaseTool *currentTool();
    BaseTool *getTool(ToolType eToolMode);
    void setCurrentTool(ToolType eToolMode);
    QList<BaseTool *> getTools();

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

protected:
    void setPrevMode();
    void paintBitmapBuffer();
    void updateCanvas(int frame, QRect rect);
    void setGaussianGradient(QGradient &gradient, QColor colour, qreal opacity, qreal offset);
    void drawBrush(QPointF thePoint, qreal brushWidth, qreal offset, QColor fillColour, qreal opacity);
    void drawLineTo(const QPointF &endPixel, const QPointF &endPoint);
    void drawEyedropperPreview(const QColor colour);
    void drawPolyline();
    void endPolyline();

    void floodFill(VectorImage *vectorImage, QPoint point, QRgb targetColour, QRgb replacementColour, int tolerance);
    void floodFillError(int errorType);

    enum myMoveModes { MIDDLE, TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT };

    myMoveModes moveMode;
    ToolType prevMode;

    BaseTool *m_currentTool;
    QHash<ToolType, BaseTool *> m_toolSetHash;

    Editor *m_pEditor;

    int tabletEraserBackupToolMode;
    bool modified;
    bool simplified;

    bool showThinLines;
    int showAllLayers;
    bool usePressure, makeInvisible;
    bool highResPosition;
    bool antialiasing;
    bool shadows;
    bool toolCursors;
    int gradients;
    qreal curveOpacity;
    qreal curveSmoothing;
    bool onionPrev, onionNext;
    bool updateAll;

    Properties brush;
    Properties eraser;

    qreal currentWidth;
    QColor currentPressuredColor;
    bool followContour;

    QBrush backgroundBrush;
    BitmapImage *bufferImg; // used to pre-draw vector modifications
    //Buffer buffer; // used to pre-draw bitmap modifications, such as lines, brushes, etc.

    bool mouseInUse;
    QList<QPointF> mousePoints; // copy of points clicked using polyline tool
    QList<QPointF> mousePath; // copy of points drawn using pencil, pen, eraser, etc, tools
    QList<qreal> mousePressure;
    QPointF lastPixel, currentPixel;
    QPointF lastPoint, currentPoint;
    QPointF lastBrushPoint;
    //QBrush brush; // the current brush

    qreal tol;
    QList<int> closestCurves;
    QList<VertexRef> closestVertices;
    QPointF offset;

    bool resizingTool; //whether or not resizing
    enum myResizingToolMode {rtmWIDTH, rtmFEATHER}; //interactive brush resizing modes
    myResizingToolMode resizingToolMode ;
    qreal brushOrgSize; //start resizing from previous width or feather

    VectorSelection vectorSelection;
    //bool selectionChanged;
    QMatrix selectionTransformation;


    bool tabletInUse;
    qreal tabletPressure;
    QPointF tabletPosition;

    QMatrix myView, myTempView, centralView, transMatrix;

    QPixmap canvas;

    // debug
    QRectF debugRect;
};

#endif


