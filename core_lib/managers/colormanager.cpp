/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "colormanager.h"

#include "object.h"
#include "editor.h"


ColorManager::ColorManager( QObject* parent ) : BaseManager( parent )
{
}

ColorManager::~ColorManager()
{
}

bool ColorManager::init()
{
    return true;
}

Status ColorManager::load( Object* o )
{
    return Status::OK;
}

Status ColorManager::save( Object* o )
{
	o->data()->setCurrentColor( mCurrentFrontColor );
	return Status::OK;
}

QColor ColorManager::frontColor()
{
    return editor()->object()->getColour( mCurrentColorIndex ).colour;
}

void ColorManager::setColorNumber( int n )
{
    Q_ASSERT( n >= 0 );

    mCurrentColorIndex = n;

    QColor currentColor = editor()->object()->getColour( mCurrentColorIndex ).colour;
    emit colorNumberChanged(mCurrentColorIndex);
    emit colorChanged(currentColor);
}

void ColorManager::setColor(const QColor& newColor)
{
    QColor currentColor = editor()->object()->getColour( mCurrentColorIndex ).colour;
    if (currentColor != newColor)
    {
        editor()->object()->setColour( mCurrentColorIndex, newColor );
        emit colorChanged(newColor);

		qDebug("Pick Color(R=%d, G=%d, B=%d, A=%d)", newColor.red(), newColor.green(), newColor.blue(), newColor.alpha());
    }
}

int ColorManager::frontColorNumber()
{
    return mCurrentColorIndex;
}
