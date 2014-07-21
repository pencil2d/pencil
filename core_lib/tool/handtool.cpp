
#include "handtool.h"
#include <cmath>
#include <QPixmap>
#include <Qvector2D>
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
}

void HandTool::mouseReleaseEvent( QMouseEvent* event )
{
    //---- stop the hand tool if this was mid button
    if ( event->button() == Qt::MidButton )
    {
        //qDebug("Stop Hand Tool");
        mScribbleArea->setPrevTool();
    }
}

void HandTool::mouseMoveEvent( QMouseEvent* event )
{
    if ( event->buttons() == Qt::NoButton )
    {
        return;
    }

    bool isTranslate = event->modifiers() == Qt::NoModifier;
    bool isRotate = event->modifiers() & Qt::AltModifier;
    bool isScale = event->modifiers() & Qt::ControlModifier || event->buttons() & Qt::RightButton;

    if ( isTranslate )
    {
        QPointF d = getCurrentPixel() - getLastPressPixel();
        editor()->view()->translate( d );
    }
    else if ( isRotate )
    {
        QPoint centralPixel( mScribbleArea->width() / 2, mScribbleArea->height() / 2 );
        QVector2D v1( getLastPressPixel() - centralPixel );
        QVector2D v2( getCurrentPixel() - centralPixel );

        float angle = acos( QVector2D::dotProduct( v1, v2 ) / v1.length() * v2.length() );
        angle = angle * 180.0 / M_PI;
        
        mEditor->view()->rotate( angle );
    }
    else if ( isScale )
    {
        float scaleValue = exp( 0.01 * ( getCurrentPixel().y() - getLastPressPixel().y() ) );
        mEditor->view()->scale( scaleValue );
    }
}

void HandTool::mouseDoubleClickEvent( QMouseEvent *event )
{
    if ( event->button() == Qt::RightButton )
    {
        mScribbleArea->resetView();
    }
}
