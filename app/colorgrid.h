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
#ifndef COLORGRID_H
#define COLORGRID_H

#include <QScrollArea>

class FlowLayout;
class ColorGridItem;

class ColorGrid : public QScrollArea
{
    Q_OBJECT
public:
    explicit ColorGrid(QWidget *parent = 0);
    virtual QSize sizeHint () const;
    virtual QSize minimumSizeHint () const;
    QByteArray dataExport();
    bool dataImport(const QByteArray &array);
signals:
    void colorDroped(const int &, const QColor &);
    void colorDroped(const int &);
    void colorPicked(const int &, const QColor &);
    
public slots:
    void setColor(const int &, const QColor &);
protected:
    void initItems();
    FlowLayout* mLayout;
    QList<ColorGridItem*> items;
    static const int gridCount = 100;
};

#endif // COLORGRID_H
