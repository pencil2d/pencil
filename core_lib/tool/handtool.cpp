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
    properties.useFeather = -1;
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
        QPointF d = getCurrentPoint() - getLastPoint();
        QPointF offset = editor()->view()->translation() + d;
        //qDebug() << "d=" << d << ", offset=" << offset;
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
