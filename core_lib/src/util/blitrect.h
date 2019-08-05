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

#ifndef BLITRECT_H
#define BLITRECT_H

#include <QRect>
#include <QPoint>

class BlitRect : public QRect
{
public:
    explicit BlitRect();
    explicit BlitRect(const QPoint p);
    void extend(const QPoint p);

private:
    bool mInitialized = false;
};

#endif // BLITRECT_H
