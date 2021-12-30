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
#include <QPixmap>
#include <QVector2D>
#include <pointerevent.h>

#include "layer.h"
#include "layercamera.h"
#include "editor.h"
#include "strokemanager.h"
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
}

QCursor HandTool::cursor()
{
    return mIsHeld ? Qt::ClosedHandCursor : Qt::OpenHandCursor;
}

void HandTool::pointerPressEvent(PointerEvent*)
{
    mLastPixel = getCurrentPixel();
    mStartPoint = mEditor->view()->mapScreenToCanvas(mLastPixel);
    mIsHeld = true;

    mScribbleArea->updateToolCursor();
}

void HandTool::pointerMoveEvent(PointerEvent* event)
{
    if (event->buttons() == Qt::NoButton)
    {
        return;
    }

    transformView(event->modifiers(), event->buttons());
    mLastPixel = getCurrentPixel();
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

void HandTool::transformView(Qt::KeyboardModifiers keyMod, Qt::MouseButtons buttons)
{
    bool isTranslate = keyMod == Qt::NoModifier;
    bool isRotate = keyMod & Qt::AltModifier;
    bool isScale = (keyMod & Qt::ControlModifier) || (buttons & Qt::RightButton);

    ViewManager* viewMgr = mEditor->view();

    if (isTranslate)
    {
        QPointF d = getCurrentPoint() - getLastPoint();
        QPointF offset = viewMgr->translation() + d;
        viewMgr->translate(offset);
    }
    else if (isRotate)
    {
        QPoint centralPixel(mScribbleArea->width() / 2, mScribbleArea->height() / 2);
        QVector2D startV(getLastPixel() - centralPixel);
        QVector2D curV(getCurrentPixel() - centralPixel);

        qreal angleOffset = static_cast<qreal>(std::atan2(curV.y(), curV.x()) - std::atan2(startV.y(), startV.x()));
        angleOffset = qRadiansToDegrees(angleOffset);
        float newAngle = viewMgr->rotation() + static_cast<float>(angleOffset);
        viewMgr->rotate(newAngle);
    }
    else if (isScale)
    {
        float delta = (static_cast<float>(getCurrentPixel().y() - mLastPixel.y())) / 100.f;
        qreal scaleValue = viewMgr->scaling() * (1 + delta);
        viewMgr->scaleWithOffset(scaleValue, mStartPoint);
    }
}
