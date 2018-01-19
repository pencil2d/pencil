/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "util.h"


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

SignalBlocker::SignalBlocker( QObject* o )
    : mObject( o ),
    mBlocked( o && o->blockSignals( true ) )
{}

SignalBlocker::~SignalBlocker()
{
    if ( mObject )
        mObject->blockSignals( mBlocked );
}
