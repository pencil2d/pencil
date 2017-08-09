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

#include "viewmanager.h"
#include "object.h"


ViewManager::ViewManager(QObject *parent) : BaseManager(parent)
{
}

bool ViewManager::init()
{
    return true;
}

Status ViewManager::load( Object* o )
{
    mView = o->data()->getCurrentView();

    if ( mView.isIdentity() )
    {
        translate( 0, 0 );
    }
	updateViewTransforms();

    return Status::OK;
}

Status ViewManager::save( Object* o )
{
	o->data()->setCurrentView( mView );
	return Status();
}

QPointF ViewManager::mapCanvasToScreen( QPointF p )
{
    return mViewCanvas.map( p );
}

QPointF ViewManager::mapScreenToCanvas(QPointF p)
{
    return mViewCanvasInverse.map( p );
}

QPainterPath ViewManager::mapCanvasToScreen( const QPainterPath& path )
{
    return mViewCanvas.map( path );
}

QRectF ViewManager::mapCanvasToScreen( const QRectF& rect )
{
    return mViewCanvas.mapRect( rect ) ;
}

QRectF ViewManager::mapScreenToCanvas( const QRectF& rect )
{
    return mViewCanvasInverse.mapRect( rect ) ;
}

QPainterPath ViewManager::mapScreenToCanvas( const QPainterPath& path )
{
    return mViewCanvasInverse.map( path );
}

QTransform ViewManager::getView()
{
    return mViewCanvas;
}

void ViewManager::updateViewTransforms()
{
    mViewInverse = mView.inverted();
    mViewCanvas = mView * mCentre;
    mViewCanvasInverse = mViewCanvas.inverted();
}

void ViewManager::translate(float dx, float dy)
{
    QTransform t;
    t.translate(dx, dy);

    mView = mView * t;
    updateViewTransforms();
    
    Q_EMIT viewChanged();
}

void ViewManager::translate(QPointF offset)
{
    translate( offset.x(), offset.y() );
}

void ViewManager::rotate(float degree)
{
    QTransform r;
    r.rotate(degree);
    mView = mView * r;

    updateViewTransforms();

    Q_EMIT viewChanged();
}

void ViewManager::scaleUp()
{
    scale(1.18f);
}

void ViewManager::scaleDown()
{
    scale(0.8333f);
}

void ViewManager::scale(float scaleValue)
{
    float newScale = mScale * scaleValue;

    if (newScale < mMinScale)
    {
        newScale = mMinScale;
    }
    else if (newScale > mMaxScale)
    {
        newScale = mMaxScale;
    }
    else if (newScale == mMinScale || newScale == mMaxScale)
    {
        return;
    }

    mScale = newScale;

    QTransform s = QTransform::fromScale( scaleValue, scaleValue );
    mView = mView * s;
  
    updateViewTransforms();

    Q_EMIT viewChanged();
}

void ViewManager::flipHorizontal( bool b )
{
    if ( b != mIsFlipHorizontal )
    {
        mIsFlipHorizontal = b;
        
        QTransform s = QTransform::fromScale(-1.0, 1.0);
        mView = mView * s;
        
        updateViewTransforms();
        Q_EMIT viewChanged();
    }
}

void ViewManager::flipVertical( bool b )
{
    if ( b != mIsFlipVertical )
    {
        mIsFlipVertical = b;

        QTransform s = QTransform::fromScale(1.0, -1.0);
        mView = mView * s;

        updateViewTransforms();
        Q_EMIT viewChanged();
    }
}

void ViewManager::setCanvasSize( QSize size )
{
    mCanvasSize = size;
    
    QTransform c;

    mCentre = QTransform::fromTranslate(mCanvasSize.width() / 2.f, mCanvasSize.height() / 2.f);

    updateViewTransforms();
    Q_EMIT viewChanged();
}

void ViewManager::resetView()
{
    mScale = 1.f;
    mView = QTransform();
    
    mViewInverse = mView.inverted();
    mViewCanvas = mView * mCentre;
    mViewCanvasInverse = mViewCanvas.inverted();

    Q_EMIT viewChanged();
}
