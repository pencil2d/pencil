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
#ifndef UTIL_H
#define UTIL_H

#include <cstddef>
#include <functional>
#include <QTransform>


QTransform RectMapTransform( QRectF source, QRectF target );

class ScopeGuard
{
public:
    explicit ScopeGuard(std::function< void() > onScopeExit) { m_onScopeExit = onScopeExit; }
    ~ScopeGuard() { m_onScopeExit(); }
private:
    std::function<void()> m_onScopeExit;
};

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)

#define OnScopeExit( callback ) ScopeGuard SCOPEGUARD_LINENAME( myScopeGuard, __LINE__ ) ( [&] { callback; } );


#define NULLReturnVoid( p ) if ( p == nullptr ) { return; }
#define NULLReturn( p, ret ) if ( p == nullptr ) { return ret; }
#define NULLReturnAssert( p ) if ( p == nullptr ) { Q_ASSERT(false); return; }


class SignalBlocker
{
public:
    explicit SignalBlocker(QObject* o);
    ~SignalBlocker();
private:
    QObject* mObject = nullptr;
    bool mBlocked = false;
};


#endif // UTIL_H
