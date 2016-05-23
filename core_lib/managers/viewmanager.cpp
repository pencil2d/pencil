#include "viewmanager.h"

#include <utility>
#include "object.h"
#include "editorstate.h"


ViewManager::ViewManager(QObject *parent) : BaseManager(parent)
{
}

bool ViewManager::init()
{
    return true;
}

Status ViewManager::onObjectLoaded( Object* o )
{
    mView = o->editorState()->mCurrentView;
    return Status::OK;
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
    QTransform c;
    c.translate( mCanvasSize.width() / 2.f , mCanvasSize.height() / 2.f );
    
    QTransform t;
    t.translate( mTranslate.x(), mTranslate.y() );

    QTransform r;
    r.rotate( mRotate );
    
    float flipX = mIsFlipHorizontal ? -1.f : 1.f;
    float flipY = mIsFlipVertical ? -1.f : 1.f;

    QTransform s;
    s.scale( mScale * flipX, mScale * flipY );

    return std::move( t * s * r * c );
}

void ViewManager::translate(float dx, float dy)
{
    mTranslate = QPointF( dx, dy );
    mView = createViewTransform();
    Q_EMIT viewChanged();
}

void ViewManager::translate(QPointF offset)
{
    translate( offset.x(), offset.y() );
}

void ViewManager::rotate(float degree)
{
    mRotate += degree;
    mView = createViewTransform();
    Q_EMIT viewChanged();
}

void ViewManager::scale(float scaleValue)
{
    if( scaleValue < mMinScale )
    {
        scaleValue = mMinScale;
    }
    else if( scaleValue > mMaxScale)
    {
        scaleValue = mMaxScale;
    }
    else if( scaleValue == mMinScale || scaleValue == mMaxScale  )
    {
        return;
    }
    mScale = scaleValue;
    mView = createViewTransform();
    Q_EMIT viewChanged();
}

void ViewManager::flipHorizontal( bool b )
{
    if ( b != mIsFlipHorizontal )
    {
        mIsFlipHorizontal = b;
        mView = createViewTransform();
        Q_EMIT viewChanged();
    }
}

void ViewManager::flipVertical( bool b )
{
    if ( b != mIsFlipVertical )
    {
        mIsFlipVertical = b;
        mView = createViewTransform();
        Q_EMIT viewChanged();
    }
}

void ViewManager::setCanvasSize( QSize size )
{
    mCanvasSize = size;
    mView = createViewTransform();
    Q_EMIT viewChanged();
}

void ViewManager::resetView()
{
    mRotate = 0.f;
    mScale = 1.f;
    translate(0, 0);
    Q_EMIT viewChanged();
}
