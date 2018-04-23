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
#include <QPixmap>
#include <QVector2D>
#include <QMouseEvent>

#include "layer.h"
#include "layercamera.h"
#include "editor.h"
#include "viewmanager.h"
#include "scribblearea.h"


HandTool::HandTool(QObject *parent) :
    BaseTool(parent)
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
    return mButtonsDown > 0 ? Qt::ClosedHandCursor : Qt::OpenHandCursor;
}

void HandTool::mousePressEvent( QMouseEvent* )
{
    mLastPixel = getLastPressPixel();
    ++mButtonsDown;
    mScribbleArea->updateToolCursor();
}

void HandTool::mouseReleaseEvent( QMouseEvent* event )
{
    //---- stop the hand tool if this was mid button
    if ( event->button() == Qt::MidButton )
    {
        qDebug( "[HandTool] Stop Hand Tool" );
        mScribbleArea->setPrevTool();
    }
    --mButtonsDown;
    mScribbleArea->updateToolCursor();
}

void HandTool::mouseMoveEvent( QMouseEvent* evt )
{
    if ( evt->buttons() == Qt::NoButton )
    {
        return;
    }

    bool isTranslate = evt->modifiers() == Qt::NoModifier;
    bool isRotate = evt->modifiers() & Qt::AltModifier;
    bool isScale = ( evt->modifiers() & Qt::ControlModifier ) || ( evt->buttons() & Qt::RightButton );


    ViewManager* viewMgr = mEditor->view();

    if ( isTranslate )
    {
        QPointF d = getCurrentPoint() - getLastPoint();
        QPointF offset = viewMgr->translation() + d;
        viewMgr->translate( offset );
    }
    else if ( isRotate )
    {
        QPoint centralPixel( mScribbleArea->width() / 2, mScribbleArea->height() / 2 );
        QVector2D startV( getLastPixel() - centralPixel );
        QVector2D curV( getCurrentPixel() - centralPixel );

        float angleOffset = ( atan2( curV.y(), curV.x() ) - atan2( startV.y(), startV.x() ) ) * 180.0 / M_PI;
        float newAngle = viewMgr->rotation() + angleOffset;
        viewMgr->rotate(newAngle);
    }
    else if ( isScale )
    {
        float delta = ( getCurrentPixel().y() - mLastPixel.y() ) / 100.f;
        float scaleValue = viewMgr->scaling() * (1.f + delta);
        viewMgr->scale(scaleValue);
    }

    mLastPixel = getCurrentPixel();
}

void HandTool::mouseDoubleClickEvent( QMouseEvent *event )
{
    if ( event->button() == Qt::RightButton )
    {
        mEditor->view()->resetView();
    }
}
