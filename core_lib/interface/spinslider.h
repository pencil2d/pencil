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


#ifndef SPINSLIDER_H
#define SPINSLIDER_H

#include <QWidget>
#include <QLabel>

class QLabel;
class QSlider;
class QStyle;


class SpinSlider : public QWidget
{
    Q_OBJECT
public:
    enum GROWTH_TYPE
    {
        LINEAR,
        LOG,
        EXPONENT,
    };

    enum VALUE_TYPE
    {
        INTEGER,
        FLOAT,
    };

    SpinSlider( QString text, GROWTH_TYPE, VALUE_TYPE, qreal min, qreal max, QWidget* parent = 0 );
    void setValue( qreal );
    void setPixelPos(qreal min, qreal max, int val, int space, bool upsideDown);
    void setExponent( const qreal );
    void setLabel( QString newText );
    
signals:
    void valueChanged(qreal);

private:
    void onSliderValueChanged( int );
    void changeValue( qreal );

private:
    QSlider* mSlider     = nullptr;
    qreal mValue = 50.0;
    qreal mMin   = 0.1;
    qreal mMax   = 100.0;
    qreal mExp   = 2.0;
    QString mText = "";

    QLabel* mLabel;

    GROWTH_TYPE mGrowthType = LINEAR;
    VALUE_TYPE  mValueType  = INTEGER;
};

#endif // SPINSLIDER_H
