/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2014 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef BACKGROUNDWIDGET_H
#define BACKGROUNDWIDGET_H

#include <QWidget>
#include "preferencemanager.h"

class BackgroundWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BackgroundWidget(QWidget *parent = 0);

    void init(PreferenceManager* prefs);

signals:

public slots:

protected:

    void paintEvent( QPaintEvent* ) override;


private slots:

    void settingUpdated(SETTING setting);

private:

    void drawShadow(QPainter &painter);
    QString getBackgroundStyle();

    PreferenceManager* mPrefs;

};

#endif // BACKGROUNDWIDGET_H
