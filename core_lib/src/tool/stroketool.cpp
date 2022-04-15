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

#include "stroketool.h"

#include <QKeyEvent>
#include "scribblearea.h"
#include "strokemanager.h"
#include "viewmanager.h"
#include "editor.h"
#include "toolmanager.h"

#ifdef Q_OS_MAC
extern "C" {
    void detectWhichOSX();
    void disableCoalescing();
    void enableCoalescing();
}
#else
extern "C" {
    void detectWhichOSX() {}
    void disableCoalescing() {}
    void enableCoalescing() {}
}
#endif

StrokeTool::StrokeTool(QObject* parent) : BaseTool(parent)
{
    detectWhichOSX();
}

void StrokeTool::startStroke(PointerEvent::InputType inputType)
{
    if (emptyFrameActionEnabled())
    {
        mScribbleArea->handleDrawingOnEmptyFrame();
    }

    mFirstDraw = true;
    mLastPixel = getCurrentPixel();

    mStrokePoints.clear();

    //Experimental
    QPointF startStrokes = strokeManager()->interpolateStart(mLastPixel);
    mStrokePoints << mEditor->view()->mapScreenToCanvas(startStrokes);

    mStrokePressures.clear();
    mStrokePressures << strokeManager()->getPressure();

    mCurrentInputType = inputType;

    disableCoalescing();
}

bool StrokeTool::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Alt:
        if (mEditor->tools()->setTemporaryTool(EYEDROPPER, {}, Qt::AltModifier))
        {
            return true;
        }
        break;
    case Qt::Key_Space:
        if (mEditor->tools()->setTemporaryTool(HAND, Qt::Key_Space, Qt::NoModifier))
        {
            return true;
        }
        break;
    }
    return BaseTool::keyPressEvent(event);
}

bool StrokeTool::emptyFrameActionEnabled()
{
    return true;
}

void StrokeTool::endStroke()
{
    strokeManager()->interpolateEnd();
    mStrokePressures << strokeManager()->getPressure();
    mStrokePoints.clear();
    mStrokePressures.clear();

    enableCoalescing();

    mScribbleArea->setModified(mEditor->currentLayerIndex(), mEditor->currentFrame());
}

void StrokeTool::drawStroke()
{
    QPointF pixel = getCurrentPixel();
    if (pixel != mLastPixel || !mFirstDraw)
    {
        // get last pixel before interpolation initializes
        QPointF startStrokes = strokeManager()->interpolateStart(getLastPixel());
        mStrokePoints << mEditor->view()->mapScreenToCanvas(startStrokes);
        mStrokePressures << strokeManager()->getPressure();
    }
    else
    {
        mFirstDraw = false;
    }
}
