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

#include "stroketool.h"

#include "scribblearea.h"
#include "strokemanager.h"
#include "viewmanager.h"
#include "editor.h"

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

StrokeTool::StrokeTool( QObject *parent ) :
BaseTool( parent )
{
    detectWhichOSX();
}

void StrokeTool::startStroke()
{
    if(emptyFrameActionEnabled())
    {
        mScribbleArea->handleDrawingOnEmptyFrame();
    }

    mFirstDraw = true;
    mLastPixel = getCurrentPixel();
    
    mStrokePoints.clear();

    //Experimental
    QPointF startStrokes =  m_pStrokeManager->interpolateStart(mLastPixel);
    mStrokePoints << mEditor->view()->mapScreenToCanvas( startStrokes );

    mStrokePressures.clear();
    mStrokePressures << m_pStrokeManager->getPressure();

    disableCoalescing();
}

bool StrokeTool::keyPressEvent(QKeyEvent *event)
{
    switch ( event->key() ) {
    case Qt::Key_Alt:
        mScribbleArea->setTemporaryTool( EYEDROPPER );
        return true;
    case Qt::Key_Space:
        mScribbleArea->setTemporaryTool( HAND ); // just call "setTemporaryTool()" to activate temporarily any tool
        return true;
    }
    return false;
}

bool StrokeTool::keyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
    return true;
}

bool StrokeTool::emptyFrameActionEnabled()
{
    return true;
}

void StrokeTool::endStroke()
{
    m_pStrokeManager->interpolateEnd();
    mStrokePressures << m_pStrokeManager->getPressure();
    mStrokePoints.clear();
    mStrokePressures.clear();

    enableCoalescing();
}

void StrokeTool::drawStroke()
{
    QPointF pixel = getCurrentPixel();
    if ( pixel != mLastPixel || !mFirstDraw )
    {

        // get last pixel before interpolation initializes
        QPointF startStrokes =  m_pStrokeManager->interpolateStart(getLastPixel());
        mStrokePoints << mEditor->view()->mapScreenToCanvas( startStrokes );
        mStrokePressures << m_pStrokeManager->getPressure();

    }
    else
    {
        mFirstDraw = false;
    }
}
