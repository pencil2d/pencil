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
#ifndef GRADIENT_H
#define GRADIENT_H

//#include "vectorimage.h"

class QPainter;
class VectorImage;  // forward declaration

class Gradient
{
public:
    Gradient();

    static void paint1(QPainter& painter, VectorImage* vectorImage, int areaNumber, int gradients);
    static void paint2(QPainter& painter, VectorImage* vectorImage, int areaNumber, int gradients);
    static void paint3(QPainter& painter, VectorImage* vectorImage, int areaNumber, int gradients);
    static void paint4(QPainter& painter, VectorImage* vectorImage, int areaNumber, int gradients);
    static void paint5(QPainter& painter, VectorImage* vectorImage, int areaNumber, int gradients);


};

#endif

