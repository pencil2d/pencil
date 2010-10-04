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
#ifndef BEZIERAREA_H
#define BEZIERAREA_H

#include <QtGui>
#include <QtXml>

#include "vertexref.h"

//class BezierArea : public QObject
class BezierArea
{
    //Q_OBJECT

public:
    BezierArea();
	//BezierArea(QList<QList<int> > pointList, VectorImage* vectorImage);
	BezierArea(QList<VertexRef> vertexList, int colour);
	
	QDomElement createDomElement(QDomDocument &doc);
	void loadDomElement(QDomElement element);
	
	VertexRef getVertexRef(int i);
	int getColourNumber() { return colourNumber; }
	void decreaseColourNumber() { colourNumber--; }
	void setSelected(bool YesOrNo);
	bool isSelected() const { return selected; }
	void setColourNumber(int cn) { colourNumber = cn; }
	
	QList<VertexRef> vertex;
	QPainterPath path;
	int colourNumber;
	
private:
	//VectorImage* picture;
	bool selected;
};

#endif
