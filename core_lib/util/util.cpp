
#include "util.h"

void safeDelete( QObject*& p )
{
    if ( p )
    {
        p->deleteLater();
    }
    p = nullptr;
}


QTransform RectMapTransform( QRectF source, QRectF target )
{
    qreal x1 = source.left();
    qreal y1 = source.top();
    qreal x2 = source.right();
    qreal y2 = source.bottom();
    qreal x1P = target.left();
    qreal y1P = target.top();
    qreal x2P = target.right();
    qreal y2P = target.bottom();

    QTransform matrix;
    if ( ( x1 != x2 ) && ( y1 != y2 ) )
    {
        matrix = QTransform( ( x2P - x1P ) / ( x2 - x1 ), // scale x
                             0,
                             0,
                             ( y2P - y1P ) / ( y2 - y1 ), // scale y
                             ( x1P * x2 - x2P * x1 ) / ( x2 - x1 ),    // dx
                             ( y1P * y2 - y2P * y1 ) / ( y2 - y1 ) );  // dy
    }
    else
    {
        matrix.reset();
    }
    return matrix;
}
