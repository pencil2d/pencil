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

#include "preview.h"
#include <QPainter>


PreviewCanvas::PreviewCanvas( QWidget* parent ) : QWidget( parent )
{
	setFixedSize( 200, 200 );
}

void PreviewCanvas::paintEvent( QPaintEvent* )
{
	QPainter painter( this );
	if ( mBitmapImage )
	{
		painter.drawImage( rect( ), *( mBitmapImage->image() ) );
	}
	painter.end( );
}

PreviewWidget::PreviewWidget( QWidget* parent ) : QDockWidget( parent )
{
	mCanvas = new PreviewCanvas( this );
	setWidget( mCanvas );
}

void PreviewWidget::updateImage()
{
	mCanvas->update();
}