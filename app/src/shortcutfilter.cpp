/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2018 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "shortcutfilter.h"

ShortcutFilter::ShortcutFilter(ScribbleArea* scribbleArea , QObject* parent) :
    QObject(parent)
{
    mScribbleArea = scribbleArea;
}

bool ShortcutFilter::eventFilter(QObject* obj, QEvent* event)
{
    if (mScribbleArea->isMouseInUse())
    {
        return true;
    }
    return QObject::eventFilter(obj, event);
}
