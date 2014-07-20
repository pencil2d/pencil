
#include "handtool.h"
#include <QPixmap>
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

void HandTool::mousePressEvent( QMouseEvent* event )
{
    Q_UNUSED( event );
}

void HandTool::mouseReleaseEvent( QMouseEvent* event )
{
    mScribbleArea->applyTransformationMatrix();

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
    bool isScale = event->modifiers() & Qt::ControlModifier;

    if ( isTranslate )
    {
        QPointF d = getCurrentPixel() - getLastPressPixel();
        editor()->view()->translate( d );
    }
    else if ( isRotate )
    {

    }

    if ( event->modifiers() & Qt::ControlModifier || event->modifiers() & Qt::AltModifier || event->buttons() & Qt::RightButton )
    {
        QPoint centralPixel( mScribbleArea->width() / 2, mScribbleArea->height() / 2 );
        if ( getLastPressPixel().x() != centralPixel.x() )
        {
            qreal scale = 1.0;
            qreal cosine = 1.0;
            qreal sine = 0.0;
            if ( event->modifiers() & Qt::AltModifier )    // rotation
            {
                QPointF V1 = getLastPressPixel() - centralPixel;
                QPointF V2 = getCurrentPixel() - centralPixel;
                cosine = ( V1.x() * V2.x() + V1.y() * V2.y() ) / ( BezierCurve::eLength( V1 ) * BezierCurve::eLength( V2 ) );
                sine = ( -V1.x() * V2.y() + V1.y() * V2.x() ) / ( BezierCurve::eLength( V1 ) * BezierCurve::eLength( V2 ) );
            }
            if ( event->modifiers() & Qt::ControlModifier || event->buttons() & Qt::RightButton )    // scale
            {
                scale = exp( 0.01 * ( getCurrentPixel().y() - getLastPressPixel().y() ) );
            }
            mScribbleArea->setTransformationMatrix( QTransform(
                                                        scale * cosine, -scale * sine,
                                                        scale * sine, scale * cosine,
                                                        0.0,
                                                        0.0
                                                        ) );
        }
    }
}

void HandTool::mouseDoubleClickEvent( QMouseEvent *event )
{
    if ( event->button() == Qt::RightButton )
    {
        mScribbleArea->resetView();
    }
}
