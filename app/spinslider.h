/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

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


namespace Ui {
    class SpinSlider;
}

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
    SpinSlider(QWidget* parent = nullptr);
    ~SpinSlider();
    void init(QString text, GROWTH_TYPE, VALUE_TYPE, qreal min, qreal max);
    void setValue(qreal);
    void setPixelPos(qreal min, qreal max, int val, int space, bool upsideDown);
    void setExponent(const qreal);
    void setLabel(QString newText);

signals:
    void valueChanged(qreal);

private slots:
    void onSliderValueChanged(int);

private:
    void changeValue(qreal);

private:
    Ui::SpinSlider* ui = nullptr;

    qreal mValue = 50.0;
    qreal mMin = 0.1;
    qreal mMax = 100.0;
    qreal mExp = 2.0;
    QString mText = "";

    GROWTH_TYPE mGrowthType = LINEAR;
    VALUE_TYPE  mValueType = INTEGER;
};

#endif // SPINSLIDER_H
