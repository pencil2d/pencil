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

#ifndef BASEMANAGER_H
#define BASEMANAGER_H

#include <QObject>
#include "pencilerror.h"

class Object;
class Layer;
class Editor;

class BaseManager : public QObject
{
    Q_OBJECT
protected:
    explicit BaseManager(Editor* editor = nullptr);
    virtual ~BaseManager();

public:
    Editor* editor() const { return mEditor; }
    Object* object() const;

    virtual bool init() = 0;
    virtual Status load(Object* o) = 0;
    virtual Status save(Object* o) = 0;
    virtual void workingLayerChanged(Layer*) {}

private:
    Editor* mEditor = nullptr;
};

#endif // BASEMANAGER_H
