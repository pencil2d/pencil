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

#ifndef DISPLAYOPTIONDOCKWIDGET_H
#define DISPLAYOPTIONDOCKWIDGET_H

#include "basedockwidget.h"

namespace Ui
{
    class DisplayOption;
}
class Editor;
class QToolButton;
class ViewManager;

class DisplayOptionWidget : public BaseDockWidget
{
    Q_OBJECT
public:
    explicit DisplayOptionWidget(QWidget* parent);
    virtual ~DisplayOptionWidget() override;

    void initUI() override;
    void updateUI() override;

private slots:
    void onionPrevButtonClicked(bool);
    void onionNextButtonClicked(bool);
    void onionBlueButtonClicked(bool);
    void onionRedButtonClicked(bool);
    void toggleMirror(bool);
    void toggleMirrorV(bool);
    void toggleOverlayCenter(bool isOn);
    void toggleOverlayThirds(bool isOn);
    void toggleOverlayGoldenRatio(bool isOn);
    void toggleOverlaySafeAreas(bool isOn);
    void toggleOverlayPerspective1(bool isOn);
    void toggleOverlayPerspective2(bool isOn);
    void toggleOverlayPerspective3(bool isOn);
    void anglePreferences();
    void angle5degrees(bool);
    void angle10degrees(bool);
    void angle15degrees(bool);
    void angle20degrees(bool);
    void angle30degrees(bool);
    void angle45degrees(bool);

private:
    void makeConnections();
    void clearPreviousAngle(int angle);
    void changeAngle(int angle);
    Ui::DisplayOption* ui = nullptr;

    QString buttonStylesheet;
    int mOverlayAngle = 15;

};

#endif // DISPLAYOPTIONDOCKWIDGET_H
