#include "viewmanager.h"
#include <utility>
#include "object.h"


ViewManager::ViewManager(QObject *parent) : BaseManager(parent)
{
}

bool ViewManager::init()
{
    return true;
}

QPointF ViewManager::mapCanvasToScreen( QPointF p )
{
    return mView.map( p );
}

QPointF ViewManager::mapScreenToCanvas(QPointF p)
{
    return mView.inverted().map( p );
}

QPainterPath ViewManager::mapCanvasToScreen( const QPainterPath& path )
{
    return mView.map( path );
}

QRectF ViewManager::mapCanvasToScreen( const QRectF& rect )
{
    return std::move( mView.mapRect( rect ) );
}

QRectF ViewManager::mapScreenToCanvas( const QRectF& rect )
{
    return std::move( mView.inverted().mapRect( rect ) );
}

QPainterPath ViewManager::mapScreenToCanvas( const QPainterPath& path )
{
    return mView.inverted().map( path );
}


QTransform ViewManager::getView()
{
    return mView;
}

QTransform ViewManager::createViewTransform()
{
    QTransform t;
    t.translate( mTranslate.x(), mTranslate.y() );
    t.scale( mScale, mScale );
    t.rotate( mRotate );

    return std::move( t );
}

void ViewManager::translate(float dx, float dy)
{
    mTranslate += QPointF( dx, dy );
    mView = createViewTransform();
}

void ViewManager::translate(QPointF offset)
{
    translate( offset.x(), offset.y() );
}

void ViewManager::rotate(float degree)
{
    mRotate += degree;
    mView = createViewTransform();
}

void ViewManager::scale(float scaleValue)
{
    mScale *= scaleValue;
    mView = createViewTransform();
}

void ViewManager::resetView()
{
    mView = QTransform();
}
