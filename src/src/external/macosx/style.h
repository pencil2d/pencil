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
#ifndef STYLE_H
#define STYLE_H

#include <QtGui>
#include <QMacStyle>

class AquaStyle : public QMacStyle
{
    Q_OBJECT

 public:
     AquaStyle() {}

     //void polish(QPalette &palette);
     //void polish(QWidget *widget);
     //void unpolish(QWidget *widget);
     int pixelMetric(PixelMetric metric, const QStyleOption *option,
                     const QWidget *widget) const;
     //int styleHint(StyleHint hint, const QStyleOption *option,
     //              const QWidget *widget, QStyleHintReturn *returnData) const;
     void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                        QPainter *painter, const QWidget *widget) const;
     void drawControl(ControlElement control, const QStyleOption *option,
                      QPainter *painter, const QWidget *widget) const;
		 void drawComplexControl ( ComplexControl control, const QStyleOptionComplex * option, 
											QPainter * painter, const QWidget * widget = 0 ) const;
		 QRect subControlRect ( ComplexControl control, const QStyleOptionComplex * option,
											SubControl subControl, const QWidget * widget = 0 ) const;
		 QPixmap standardPixmap ( StandardPixmap standardPixmap, const QStyleOption * option = 0,
											const QWidget * widget = 0 ) const;
 private:
     //static void setTexture(QPalette &palette, QPalette::ColorRole role,
     //                       const QPixmap &pixmap);
     //static QPainterPath roundRectPath(const QRect &rect);
 };
 
 #endif