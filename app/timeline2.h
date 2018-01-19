/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2018 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef TIMELINE2_H
#define TIMELINE2_H

#include <QDockWidget>
#include "basedockwidget.h"

namespace Ui {
class Timeline2;
}


class Timeline2 : public BaseDockWidget
{
    Q_OBJECT

public:
    explicit Timeline2(QWidget* parent = 0);
    ~Timeline2();

    void initUI() override;
    void updateUI() override;

private:
    Ui::Timeline2* ui;
};

#endif // TIMELINE2_H
