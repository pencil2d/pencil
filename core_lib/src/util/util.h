/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

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

class QAbstractSpinBox;

QTransform RectMapTransform( QRectF source, QRectF target );

void clearFocusOnFinished(QAbstractSpinBox *spinBox);

// NOTE: Replace this implementation with QScopeGuard once we drop support for Qt < 5.12
class ScopeGuard
{
public:
    explicit ScopeGuard(std::function< void() > onScopeExit) { m_onScopeExit = onScopeExit; }
    ScopeGuard(const ScopeGuard&) = delete;
    ~ScopeGuard() { if(m_invoke) { m_onScopeExit(); } }

    void dismiss() { m_invoke = false; };
private:
    std::function<void()> m_onScopeExit;
    bool m_invoke = true;
};

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)

#define OnScopeExit( callback ) ScopeGuard SCOPEGUARD_LINENAME( myScopeGuard, __LINE__ ) ( [&] { callback; } );

template <typename Container, typename Pred>
Container filter(const Container& container, Pred predicate) {
    Container result;
    std::copy_if(container.begin(), container.end(), std::back_inserter(result), predicate);
    return result;
}

QString ffprobeLocation();
QString ffmpegLocation();

quint64 imageSize(const QImage&);
QString uniqueString(int len);

#endif // UTIL_H
