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

#include "basemanager.h"
#include "editor.h"

BaseManager::BaseManager(Editor* editor) : QObject(editor)
{
    Q_ASSERT(editor != nullptr);
    mEditor = editor;
}

BaseManager::~BaseManager()
{
    mEditor = nullptr;
}

Object* BaseManager::object() const
{
    return mEditor->object();
}
