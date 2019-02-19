/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "handtool.h"

#include <cmath>
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
    //---- stop the hand tool if this was mid button
    if (event->button() == Qt::MidButton)
    {
        qDebug("[HandTool] Stop Hand Tool");
        mScribbleArea->setPrevTool();
    }
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

        float angleOffset = (atan2(curV.y(), curV.x()) - atan2(startV.y(), startV.x())) * 180.0 / M_PI;
        float newAngle = viewMgr->rotation() + angleOffset;
        viewMgr->rotate(newAngle);
    }
    else if (isScale)
    {
        float delta = (getCurrentPixel().y() - mLastPixel.y()) / 100.f;
        float scaleValue = viewMgr->scaling() * (1.f + delta);
        viewMgr->scale(scaleValue);
    }
}
