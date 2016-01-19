#include "handtool.h"
#include <cmath>
#include <QPixmap>
#include <QVector2D>
#include "layer.h"
#include "layercamera.h"
#include "editor.h"
#include "viewmanager.h"
#include "scribblearea.h"


HandTool::HandTool()
{
}

void HandTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
}

QCursor HandTool::cursor()
{
    return QPixmap( ":icons/hand.png" );
}

void HandTool::mousePressEvent( QMouseEvent* )
{
    mLastPixel = getLastPressPixel();
}

void HandTool::mouseReleaseEvent( QMouseEvent* event )
{
    //---- stop the hand tool if this was mid button
    if ( event->button() == Qt::MidButton )
    {
        qDebug( "[HandTool] Stop Hand Tool" );
        mScribbleArea->setPrevTool();
    }
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
        QPointF d = getCurrentPoint() - getLastPoint();
        QPointF offset = editor()->view()->translation() + d;
        //qDebug() << "d=" << d << ", offset=" << offset;
        editor()->view()->translate( offset );
    }
    else if ( isRotate )
    {
        QPoint centralPixel( mScribbleArea->width() / 2, mScribbleArea->height() / 2 );
        QVector2D v1( getLastPressPixel() - centralPixel );
        QVector2D v2( getCurrentPixel() - centralPixel );

        float angle = acos( QVector2D::dotProduct( v1, v2 ) / ( v1.length() * v2.length() ) );
        //angle = angle * 180.0 / M_PI;

        mEditor->view()->rotate( angle );
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
