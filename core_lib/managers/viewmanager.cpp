#include "viewmanager.h"
#include <utility>

ViewManager::ViewManager(QObject *parent) : BaseManager(parent)
{
}

bool ViewManager::init()
{
    return true;
}

QPointF ViewManager::mapScreenToCanvas(QPointF p)
{
    return mView.map( p );
}

QPointF ViewManager::mapCanvasToScreen(QPointF p)
{
    mView.inverted().map( p );
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
    mTranslate.setX( dx );
    mTranslate.setY( dy );
    mView = createViewTransform();
}

void ViewManager::translate(QPointF offset)
{
    translate( offset.x(), offset.y() );
}

void ViewManager::rotate(float degree)
{
    mRotate = degree;
    mView = createViewTransform();
}

void ViewManager::scale(float scaleValue)
{
    mScale = scaleValue;
    mView = createViewTransform();
}
