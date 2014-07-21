#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <QTransform>
#include "basemanager.h"


class ViewManager : public BaseManager
{
    Q_OBJECT
public:
    explicit ViewManager(QObject *parent = 0);
    bool init() override;

    QPointF mapCanvasToScreen( QPointF p );
    QPointF mapScreenToCanvas( QPointF p );

    QRectF mapCanvasToScreen( const QRectF& rect );
    QRectF mapScreenToCanvas( const QRectF& rect );

    QPainterPath mapCanvasToScreen( const QPainterPath& path );
    QPainterPath mapScreenToCanvas( const QPainterPath& path );

    QTransform getView();

    QPointF translation() { return mTranslate; }
    void translate( float dx, float dy );
    void translate( QPointF offset );

    float rotation() { return mRotate; }
    void rotate( float degree );

    float scaling();
    void scale( float scaleValue );

private:
    QTransform createViewTransform();

    QTransform mView;

    QPointF mTranslate;
    float mRotate;
    float mScale;
};

#endif // VIEWMANAGER_H
