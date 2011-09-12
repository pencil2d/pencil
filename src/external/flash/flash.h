/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef FLASH_H
#define FLASH_H

#include <QtGui>
#include <QSize>
#include <QString>

#include <mingpp.h> // requires the Ming Library

#include "layer.h"
#include "layervector.h"
#include "layerbitmap.h"
#include "vectorimage.h"

#ifdef Q_OS_LINUX
#define  SWFSprite    SWFMovieClip
#endif

class Object;

class Flash
{	
	public:
		static void exportFlash(Object* object, int startFrame, int endFrame, QMatrix view, QSize exportSize, QString filePath, int fps, int compression);
                static void convertToSWFSprite( BitmapImage* image, Object* object, QMatrix view, SWFSprite* sprite );
                static void convertToSWFSprite( VectorImage* vectorImage, Object* object, QMatrix view, SWFSprite* sprite );
                static void addShape( SWFSprite* sprite, QPainterPath path, QColor fillColour, QColor borderColour, qreal width, bool fill );
};

#endif

