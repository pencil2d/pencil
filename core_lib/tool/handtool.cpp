/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

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
    properties.inpolLevel = -1;
    properties.useAA = -1;
}

QCursor HandTool::cursor()
{
    return buttonsDown > 0 ? Qt::ClosedHandCursor : Qt::OpenHandCursor;
}

void HandTool::mousePressEvent( QMouseEvent* )
{
    mLastPixel = getLastPressPixel();
    mCurrentRotation = 0;
    ++buttonsDown;
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
    --buttonsDown;
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

    if ( isTranslate )
    {
        QPointF offset = getCurrentPixel() - getLastPixel();
        editor()->view()->translate( offset );
    }
    else if ( isRotate )
    {
        QPoint centralPixel( mScribbleArea->width() / 2, mScribbleArea->height() / 2 );
        QVector2D startV( getLastPressPixel() - centralPixel );
        QVector2D curV( getCurrentPixel() - centralPixel );

        float angle = ( atan2( curV.y(), curV.x() ) - atan2( startV.y(), startV.x() ) ) * 180.0 / M_PI;
        if ( angle != mCurrentRotation )
        {
            mEditor->view()->rotate( angle - mCurrentRotation );
            mCurrentRotation = angle;
        }
    }
    else if ( isScale )
    {
        float delta = ( getCurrentPixel().y() - mLastPixel.y() ) / 100.f ;
        float scaleValue = mEditor->view()->scaling() * ( 1.f + delta );

        mEditor->view()->scale( scaleValue );
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
