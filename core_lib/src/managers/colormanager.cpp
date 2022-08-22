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

#include "colormanager.h"

#include "object.h"
#include "editor.h"


ColorManager::ColorManager(Editor* editor) : BaseManager(editor, __FUNCTION__)
{
}

ColorManager::~ColorManager()
{
}

bool ColorManager::init()
{
    return true;
}

Status ColorManager::load(Object* o)
{
    mCurrentColorIndex = 0;
    mCurrentFrontColor = o->data()->getCurrentColor();

    return Status::OK;
}

Status ColorManager::save(Object* o)
{
    o->data()->setCurrentColor(mCurrentFrontColor);
    return Status::OK;
}

void ColorManager::workingLayerChanged(Layer* layer)
{
    mIsWorkingOnVectorLayer = (layer->type() == Layer::VECTOR);
    if (mIsWorkingOnVectorLayer)
    {
        mCurrentFrontColor = object()->getColor(mCurrentColorIndex).color;
        emit colorChanged(mCurrentFrontColor, mCurrentColorIndex);
    }
}

QColor ColorManager::frontColor(bool useIndexedColor)
{
    if (mIsWorkingOnVectorLayer && useIndexedColor)
        return object()->getColor(mCurrentColorIndex).color;
    else
        return mCurrentFrontColor;
}

void ColorManager::setColorNumber(int n)
{
    Q_ASSERT(n >= 0);

    mCurrentColorIndex = n;

    QColor currentColor = object()->getColor(mCurrentColorIndex).color;

    emit colorNumberChanged(mCurrentColorIndex);
    setFrontColor(currentColor);
}

void ColorManager::setFrontColor(const QColor& newFrontColor)
{
    mCurrentFrontColor = newFrontColor;
    emit colorChanged(newFrontColor, mCurrentColorIndex);
}

void ColorManager::setIndexedColor(const QColor& newColor)
{
    object()->setColor(mCurrentColorIndex, newColor);
    emit colorChanged(newColor, mCurrentColorIndex);
}

int ColorManager::frontColorNumber() const
{
    return mCurrentColorIndex;
}
