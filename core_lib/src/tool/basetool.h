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
#include <QPainter>
#include <QHash>
#include <QEvent>
#include "pencildef.h"

class QPixmap;
class Editor;
class ScribbleArea;
class QEnterEvent;
class QKeyEvent;
class QMouseEvent;
class QTabletEvent;
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
    bool  closedPolylinePath = false;
    bool  useFeather = true;
    int   useAA = 0;
    int   fillMode = 0;
    int   stabilizerLevel = 0;
    qreal tolerance = 0;
    bool toleranceEnabled = false;
    int bucketFillExpand = 0;
    bool bucketFillExpandEnabled = 0;
    int bucketFillReferenceMode = 0;
    bool  useFillContour = false;
    bool  showSelectionInfo = true;
    bool  cameraShowPath = true;
    DotColorType cameraPathDotColorType = DotColorType::RED;
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

    virtual bool enterEvent(QEnterEvent*) { return false; }
    virtual bool leaveEvent(QEvent*) { return false; }

    virtual void clearToolData() {}
    virtual void resetToDefault() {}

    /** Check if the tool is active.
     *
     *  An active tool is definied as one which is actively modifying the buffer.
     *  This is used to check if an full frame cache can be used instead of redrawing with CanvasPainter.
     *
     * @return Returns true if the tool is currently active, else returns false.
     */
    virtual bool isActive() const;

    virtual void setWidth(const qreal width);
    virtual void setFeather(const qreal feather);
    virtual void setInvisibility(const bool invisibility);
    virtual void setBezier(const bool bezier_state);
    virtual void setClosedPath(const bool closed);
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
    virtual void setFillReferenceMode(int referenceMode);
    virtual void setUseFillContour(const bool useFillContour);
    virtual void setShowSelectionInfo(const bool b);
    virtual void setShowCameraPath(const bool showCameraPath);
    virtual void setPathDotColorType(const DotColorType dotColorType);
    virtual void resetCameraPath();

    virtual void paint(QPainter& painter, const QRect& blitRect) { Q_UNUSED(painter) Q_UNUSED(blitRect) }

    /// Will clean up `active` connections
    virtual bool leavingThisTool();

    /// Setup `active` connections here that should only emit while tool is active
    /// `leavingThisTool` will handle the cleanup of `active` connections
    virtual bool enteringThisTool() { return true; }

    Properties properties;

    bool isPropertyEnabled(ToolPropertyType t) { return mPropertyEnabled[t]; }
    bool isDrawingTool();

signals:
    bool isActiveChanged(ToolType, bool);

protected:
    Editor* editor() { return mEditor; }

    QHash<ToolPropertyType, bool> mPropertyEnabled;

    Editor* mEditor = nullptr;
    ScribbleArea* mScribbleArea = nullptr;
    QList<QMetaObject::Connection> mActiveConnections;
};

#endif // BASETOOL_H
