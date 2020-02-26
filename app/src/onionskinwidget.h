/*

Pencil - Traditional Animation Software
Copyright (C) 2013-2018 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef ONIONSKINWIDGET_H
#define ONIONSKINWIDGET_H

#include "basedockwidget.h"

namespace Ui
{
    class OnionSkin;
}

class Editor;
class QToolButton;
class ViewManager;

class OnionSkinWidget : public BaseDockWidget
{
    Q_OBJECT

public:
    explicit OnionSkinWidget(QWidget* parent);
    virtual ~OnionSkinWidget() override;

    void initUI() override;
    void updateUI() override;

private slots:
    void playbackStateChanged(int);
    void onionPrevButtonClicked(bool);
    void onionNextButtonClicked(bool);
    void onionBlueButtonClicked(bool);
    void onionRedButtonClicked(bool);
    void onionMaxOpacityChange(int);
    void onionMinOpacityChange(int);
    void onionPrevFramesNumChange(int);
    void onionNextFramesNumChange(int);
    void onionSkinModeChange(int);

private:
    void makeConnections();
    Ui::OnionSkin* ui = nullptr;
};

#endif // ONIONSKINWIDGET_H
