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

#include "handtool.h"

#include <QtMath>
#include <QVector2D>
#include <pointerevent.h>

#include "layer.h"
#include "layercamera.h"
#include "editor.h"
#include "strokeinterpolator.h"
#include "viewmanager.h"
#include "scribblearea.h"


HandTool::HandTool(QObject* parent) : BaseTool(parent)
{
}

void HandTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
    properties.useFeather = false;
    properties.stabilizerLevel = -1;
    properties.useAA = -1;

    mDeltaFactor = mEditor->preference()->isOn(SETTING::INVERT_DRAG_ZOOM_DIRECTION) ? -1 : 1;
    connect(mEditor->preference(), &PreferenceManager::optionChanged, this, &HandTool::updateSettings);
}

void HandTool::updateSettings(const SETTING setting)
{
    switch (setting)
    {
    case SETTING::INVERT_DRAG_ZOOM_DIRECTION:
    {
        mDeltaFactor = mEditor->preference()->isOn(SETTING::INVERT_DRAG_ZOOM_DIRECTION) ? -1 : 1;
        break;
    }
    default:
        break;
    }
}

QCursor HandTool::cursor()
{
    return mIsHeld ? QCursor(Qt::ClosedHandCursor) : QCursor(Qt::OpenHandCursor);
}

void HandTool::pointerPressEvent(PointerEvent* event)
{
    mLastPixel = event->viewportPos();
    mStartPoint = event->canvasPos();
    mIsHeld = true;

    mScribbleArea->updateToolCursor();
}

void HandTool::pointerMoveEvent(PointerEvent* event)
{
    if (event->buttons() == Qt::NoButton)
    {
        return;
    }

    transformView(event->modifiers(), event->viewportPos(), event->buttons());
    mLastPixel = event->viewportPos();
}

void HandTool::pointerReleaseEvent(PointerEvent* event)
{
    Q_UNUSED(event)
    mIsHeld = false;
    mScribbleArea->updateToolCursor();
}

void HandTool::pointerDoubleClickEvent(PointerEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        mEditor->view()->resetView();
    }
}

void HandTool::transformView(Qt::KeyboardModifiers keyMod, const QPointF& pos, Qt::MouseButtons buttons)
{
    bool isTranslate = keyMod == Qt::NoModifier;
    bool isRotate = keyMod & Qt::AltModifier;
    bool isScale = (keyMod & Qt::ControlModifier) || (buttons & Qt::RightButton);

    ViewManager* viewMgr = mEditor->view();

    if (isTranslate)
    {
        QPointF d = viewMgr->mapScreenToCanvas(pos) - viewMgr->mapScreenToCanvas(mLastPixel);
        QPointF offset = viewMgr->translation() + d;
        viewMgr->translate(offset);
    }
    else if (isRotate)
    {
        QPoint centralPixel(mScribbleArea->width() / 2, mScribbleArea->height() / 2);
        QVector2D startV(mLastPixel - centralPixel);
        QVector2D curV(pos - centralPixel);

        qreal angleOffset = static_cast<qreal>(std::atan2(curV.y(), curV.x()) - std::atan2(startV.y(), startV.x()));
        angleOffset = qRadiansToDegrees(angleOffset);
        // Invert rotation direction if view is flipped either vertically or horizontally
        const float delta = viewMgr->isFlipHorizontal() == !viewMgr->isFlipVertical()
            ? static_cast<float>(angleOffset * -1) : static_cast<float>(angleOffset);
        viewMgr->rotateRelative(delta);
    }
    else if (isScale)
    {
        const float delta = (static_cast<float>(pos.y() - mLastPixel.y())) / 100.f;
        const qreal scaleValue = viewMgr->scaling() * (1 + (delta * mDeltaFactor));
        viewMgr->scaleAtOffset(scaleValue, mStartPoint);
    }
}
