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

#ifndef BACKGROUNDWIDGET_H
#define BACKGROUNDWIDGET_H

#include <QWidget>
#include "preferencemanager.h"

class BackgroundWidget : public QWidget
{
    Q_OBJECT
public:
    BackgroundWidget( QWidget *parent );
    ~BackgroundWidget();

    void init(PreferenceManager* prefs);

public slots:

protected:

    void paintEvent( QPaintEvent* ) override;


private slots:

    void settingUpdated(SETTING setting);

private:

    void drawShadow(QPainter &painter);
    void loadBackgroundStyle();

    PreferenceManager* mPrefs = nullptr;

    QString mStyle;
    bool mHasShadow = false;

};

#endif // BACKGROUNDWIDGET_H
