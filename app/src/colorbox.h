/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef COLORBOX_H
#define COLORBOX_H

#include "basedockwidget.h"

class ColorWheel;

class ColorBox : public BaseDockWidget
{
    Q_OBJECT

public:
    explicit ColorBox( QWidget* parent );
    virtual ~ColorBox() override;

    void initUI() override;
    void updateUI() override;

    QColor color();
    void setColor(QColor);

Q_SIGNALS:
    void colorChanged(const QColor&);

private:
    void onWheelMove(const QColor&);
    void onWheelRelease(const QColor&);

    ColorWheel* mColorWheel = nullptr;

//    ColorInspector* mColorInspector = nullptr;
};

#endif // COLORBOX_H
