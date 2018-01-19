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

#ifndef PREVIEW_H
#define PREVIEW_H

#include <QDockWidget>
#include "bitmapimage.h"

class PreviewCanvas : public QWidget
{
	Q_OBJECT

public:
	PreviewCanvas( QWidget* );
	void setImage( BitmapImage* img ) { mBitmapImage = img; }

protected:
	void paintEvent( QPaintEvent* ) override;

private:
	BitmapImage* mBitmapImage = nullptr;
};



class PreviewWidget : public QDockWidget
{
    Q_OBJECT
public:
    PreviewWidget(QWidget *parent = 0);
	void setImage( BitmapImage* img ) { mCanvas->setImage( img ); }
	void updateImage();

private:
	PreviewCanvas* mCanvas = nullptr;
	
};

#endif // PREVIEW_H
