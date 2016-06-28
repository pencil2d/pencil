#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QTransform>
#include "basemanager.h"


class ViewManager : public BaseManager
{
    Q_OBJECT

public:
    explicit ViewManager( QObject* parent = 0 );

    bool init() override;
    Status onObjectLoaded( Object* ) override;
    
    QTransform getView();
    void resetView();

    QPointF mapCanvasToScreen( QPointF p );
    QPointF mapScreenToCanvas( QPointF p );

    QRectF mapCanvasToScreen( const QRectF& rect );
    QRectF mapScreenToCanvas( const QRectF& rect );

    QPainterPath mapCanvasToScreen( const QPainterPath& path );
    QPainterPath mapScreenToCanvas( const QPainterPath& path );

    QPointF translation() { return mTranslate; }
    void translate( float dx, float dy );
    void translate( QPointF offset );

    float rotation() { return mRotate; }
    void rotate( float degree );

    float scaling() { return mScale; }
    void scale( float scaleValue );
    void scaleUp();
    void scaleDown();

    void flipHorizontal( bool b );
    void flipVertical( bool b );

    bool isFlipHorizontal() { return mIsFlipHorizontal; }
    bool isFlipVertical() { return mIsFlipVertical; }

    void setCanvasSize( QSize size );

    Q_SIGNAL void viewChanged();

private:
    const float mMinScale = 0.01f;
    const float mMaxScale = 100.f;

    QTransform createViewTransform();

    QTransform mView;
    QTransform mCentre;

    QPointF mTranslate;
    float mRotate = 0.f;
    float mScale = 1.f;

    QSize mCanvasSize = { 1, 1 };

    bool mIsFlipHorizontal = false;
    bool mIsFlipVertical = false;
};

#endif // VIEWMANAGER_H
