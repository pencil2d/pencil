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
#ifndef BLUR_H
#define BLUR_H

#include <QtGui>

class Blur
{
  public:
	Blur();

	static void expblur(QImage &img, int radius, int aprec, int zprec);
	
	static void fastbluralpha(QImage &img, int radius);
	
};

#endif

