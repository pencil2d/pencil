/*

Pencil2D - Traditional Animation Software
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef COLORSPINBOXGROUP_H
#define COLORSPINBOXGROUP_H

#include <QPaintEvent>
#include "basedockwidget.h"

namespace Ui {
class ColorInspector;
}

class ColorInspector : public BaseDockWidget
{
    Q_OBJECT

    friend class ColorSliders;

public:
    explicit ColorInspector(QWidget *parent = nullptr);
    ~ColorInspector() override;
    QColor color();

    void initUI() override;
    void updateUI() override;

signals:
    void colorChanged(const QColor& c);
    void colorSpecChanged(const bool& isRgb);

public slots:
    void setColor(QColor newColor);

private slots:
    void onColorSpecChanged();
    void onColorChanged();
    void onColorChanged(const QColor& color);

private:
    void updateControls();

    Ui::ColorInspector* ui = nullptr;
    bool isRgbColors = true;
    QColor mCurrentColor;
};

#endif // COLORSPINBOXGROUP_H
