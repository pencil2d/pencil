/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

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


ColorManager::ColorManager(Editor* editor) : BaseManager(editor)
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
        mCurrentFrontColor = object()->getColour(mCurrentColorIndex).colour;
        emit colorChanged(mCurrentFrontColor, mCurrentColorIndex);
    }
}

QColor ColorManager::frontColor()
{
    if (mIsWorkingOnVectorLayer)
        return object()->getColour(mCurrentColorIndex).colour;
    else
        return mCurrentFrontColor;
}

void ColorManager::setColorNumber(int n)
{
    Q_ASSERT(n >= 0);

    mCurrentColorIndex = n;

    QColor currentColor = object()->getColour(mCurrentColorIndex).colour;
    emit colorNumberChanged(mCurrentColorIndex);
    emit colorChanged(currentColor, mCurrentColorIndex);
}

void ColorManager::setColor(const QColor& newColor)
{
    if (mCurrentFrontColor != newColor)
    {
        mCurrentFrontColor = newColor;
        
        if ( mIsWorkingOnVectorLayer )
            object()->setColour(mCurrentColorIndex, newColor);

        emit colorChanged(newColor, (mIsWorkingOnVectorLayer) ? mCurrentColorIndex : -1);
    }
}

int ColorManager::frontColorNumber()
{
    return mCurrentColorIndex;
}
