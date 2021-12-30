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

#ifndef BASETOOL_H
#define BASETOOL_H

#include <QObject>
#include <QString>
#include <QCursor>
#include <QPointF>
#include <QHash>
#include "movemode.h"
#include "pencildef.h"

class QPixmap;
class Editor;
class ScribbleArea;
class QKeyEvent;
class QMouseEvent;
class QTabletEvent;
class StrokeManager;
class PointerEvent;

class Properties
{
public:
    qreal width = 1.0;
    qreal feather = 1.0;
    bool  pressure = true;
    int   invisibility = 0;
    int   preserveAlpha = 0;
    bool  vectorMergeEnabled = false;
    bool  bezier_state = false;
    bool  useFeather = true;
    int   useAA = 0;
    int   fillMode = 0;
    int   stabilizerLevel = 0;
    qreal tolerance = 0;
    bool toleranceEnabled = false;
    int bucketFillExpand = 0;
    bool bucketFillExpandEnabled = 0;
    int bucketFillToLayerMode = 0;
    int bucketFillReferenceMode = 0;
    bool  useFillContour = false;
    bool  showSelectionInfo = true;
};

const int ON = 1;
const int OFF = 0;
const int DISABLED = -1;


class BaseTool : public QObject
{
    Q_OBJECT
protected:
    explicit BaseTool(QObject* parent);

public:
    static QString TypeName(ToolType);
    QString typeName() { return TypeName(type()); }

    void initialize(Editor* editor);

    virtual ToolType type() = 0;
    virtual void loadSettings() = 0;
    virtual QCursor cursor();

    virtual void pointerPressEvent(PointerEvent*) = 0;
    virtual void pointerMoveEvent(PointerEvent*) = 0;
    virtual void pointerReleaseEvent(PointerEvent*) = 0;
    virtual void pointerDoubleClickEvent(PointerEvent*);

    // return true if handled
    virtual bool keyPressEvent(QKeyEvent*) { return false; }
    virtual bool keyReleaseEvent(QKeyEvent*) { return false; }

    // dynamic cursor adjustment
    virtual bool startAdjusting(Qt::KeyboardModifiers modifiers, qreal argStep);
    virtual void stopAdjusting();
    virtual void adjustCursor(Qt::KeyboardModifiers modifiers);

    virtual void clearToolData() {}
    virtual void resetToDefault() {}

    static QPixmap canvasCursor(float brushWidth, float brushFeather, bool useFeather, float scalingFac, int windowWidth);
    QPixmap quickSizeCursor(qreal scalingFac);
    static QCursor selectMoveCursor(MoveMode mode, ToolType type);
    static bool isAdjusting() { return msIsAdjusting; }

    /** Check if the tool is active.
     *
     *  An active tool is definied as one which is actively modifying the buffer.
     *  This is used to check if an full frame cache can be used instead of redrawing with CanvasPainter.
     *
     * @return Returns true if the tool is currently active, else returns false.
     */
    virtual bool isActive();

    virtual void setWidth(const qreal width);
    virtual void setFeather(const qreal feather);
    virtual void setInvisibility(const bool invisibility);
    virtual void setBezier(const bool bezier_state);
    virtual void setPressure(const bool pressure);
    virtual void setUseFeather(const bool usingFeather);
    virtual void setPreserveAlpha(const bool preserveAlpha);
    virtual void setVectorMergeEnabled(const bool vectorMergeEnabled);
    virtual void setAA(const int useAA);
    virtual void setFillMode(const int mode);
    virtual void setStabilizerLevel(const int level);
    virtual void setTolerance(const int tolerance);
    virtual void setToleranceEnabled(const bool enabled);
    virtual void setFillExpand(const int fillExpandValue);
    virtual void setFillExpandEnabled(const bool enabled);
    virtual void setFillToLayer(int layerMode);
    virtual void setFillReferenceMode(int referenceMode);
    virtual void setUseFillContour(const bool useFillContour);
    virtual void setShowSelectionInfo(const bool b);

    virtual bool leavingThisTool() { return true; }
    virtual bool switchingLayer() { return true; } // default state should be true

    Properties properties;

    QPointF getCurrentPressPixel();
    QPointF getCurrentPressPoint();
    QPointF getCurrentPixel();
    QPointF getCurrentPoint();
    QPointF getLastPixel();
    QPointF getLastPoint();
    QPointF getLastPressPixel();
    QPointF getLastPressPoint();

    bool isPropertyEnabled(ToolPropertyType t) { return mPropertyEnabled[t]; }
    bool isDrawingTool();

signals:
    bool isActiveChanged(ToolType, bool);

protected:
    StrokeManager* strokeManager() { return mStrokeManager; }
    Editor* editor() { return mEditor; }

    QHash<ToolPropertyType, bool> mPropertyEnabled;

    Editor* mEditor = nullptr;
    ScribbleArea* mScribbleArea = nullptr;

    QHash<Qt::KeyboardModifiers, ToolPropertyType> mQuickSizingProperties;

private:
    StrokeManager* mStrokeManager = nullptr;
    qreal mAdjustmentStep = 0.0f;

    static bool msIsAdjusting;
    static qreal msOriginalPropertyValue;  // start from previous value (width, or feather ...)
};

#endif // BASETOOL_H
