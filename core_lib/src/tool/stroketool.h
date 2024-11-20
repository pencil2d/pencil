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

#ifndef STROKETOOL_H
#define STROKETOOL_H

#include "basetool.h"
#include "pointerevent.h"
#include "preferencesdef.h"
#include "strokeinterpolator.h"

#include "canvascursorpainter.h"

#include <QList>
#include <QPointF>


class StrokeTool : public BaseTool
{
    Q_OBJECT

public:
    explicit StrokeTool(QObject* parent);

    void startStroke(PointerEvent::InputType inputType);
    void drawStroke();
    void endStroke();

    bool leavingThisTool() override;
    bool enteringThisTool() override;

    void updateCanvasCursor();

    static const qreal FEATHER_MIN;
    static const qreal FEATHER_MAX;
    static const qreal WIDTH_MIN;
    static const qreal WIDTH_MAX;

    void loadSettings() override;
    bool isActive() const override { return mInterpolator.isActive(); };

    bool keyPressEvent(QKeyEvent* event) override;
    void pointerPressEvent(PointerEvent* event) override;
    void pointerMoveEvent(PointerEvent* event) override;
    void pointerReleaseEvent(PointerEvent* event) override;
    bool enterEvent(QEnterEvent*) override;
    bool leaveEvent(QEvent*) override;

    bool handleQuickSizing(PointerEvent* event);

    void paint(QPainter& painter, const QRect& blitRect) override;

public slots:
    void onPreferenceChanged(SETTING setting);
    void onViewUpdated();

protected:
    QPointF getCurrentPressPixel() const;
    QPointF getCurrentPressPoint() const;
    QPointF getCurrentPixel() const;
    QPointF getCurrentPoint() const;
    QPointF getLastPixel() const;
    QPointF getLastPoint() const;

    // dynamic cursor adjustment
    virtual bool startAdjusting(Qt::KeyboardModifiers modifiers);
    virtual void stopAdjusting();
    virtual void adjustCursor(Qt::KeyboardModifiers modifiers);

    static bool mQuickSizingEnabled;
    static bool msIsAdjusting;

    QHash<Qt::KeyboardModifiers, ToolPropertyType> mQuickSizingProperties;
    bool mFirstDraw = false;

    QList<QPointF> mStrokePoints;
    QList<qreal> mStrokePressures;

    qreal mCurrentWidth    = 0.0;
    qreal mCurrentPressure = 0.5;

    PointerEvent::InputType mCurrentInputType = PointerEvent::Unknown;

    /// Whether to enable the "drawing on empty frame" preference.
    /// If true, then the user preference is honored.
    /// If false, then the stroke is drawn on the previous key-frame (i.e. the
    /// "old" Pencil behaviour).
    /// Returns true by default.
    virtual bool emptyFrameActionEnabled();

    bool mCanvasCursorEnabled = false;
    QPointF mLastPixel { 0, 0 };

    QPointF mAdjustPosition;

    CanvasCursorPainter mCanvasCursorPainter;

    StrokeInterpolator mInterpolator;
};

#endif // STROKETOOL_H
