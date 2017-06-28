/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef COLORSPINBOXGROUP_H
#define COLORSPINBOXGROUP_H

#include <QWidget>

namespace Ui {
class ColorInspector;
}

class ColorInspector : public QWidget
{
    Q_OBJECT
    
public:
    explicit ColorInspector(QWidget *parent = 0);
    ~ColorInspector();
    QColor color();
signals:
    void colorChanged(const QColor& c);
    void modeChange(const bool& isRgb);

public slots:
    void setColor(const QColor &c);

private slots:
    void onModeChanged();
    void onColorChanged();
    
private:
    Ui::ColorInspector *ui;
    bool isRgbColors;
    bool noColorUpdate;
    QColor m_color;
};

#endif // COLORSPINBOXGROUP_H
