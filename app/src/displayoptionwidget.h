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
class QTimer;

class DisplayOptionWidget : public BaseDockWidget
{
    Q_OBJECT
public:
    explicit DisplayOptionWidget(QWidget* parent);
    virtual ~DisplayOptionWidget() override;

    void initUI() override;
    void updateUI() override;

public slots:
    void prepareOverlayManager();

protected:
    int getMinHeightForWidth(int width) override;

private slots:
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
    void angle2degrees(bool);
    void angle3degrees(bool);
    void angle5degrees(bool);
    void angle7_5degrees(bool);
    void angle10degrees(bool);
    void angle15degrees(bool);
    void angle20degrees(bool);
    void angle30degrees(bool);

private:
    void makeConnections();
    void clearPreviousAngle(int angle);
    void changeAngle(int angle);
    Ui::DisplayOption* ui = nullptr;

    QString buttonStylesheet;

    // vars for perspective overlays
    int mOverlayAngle = 15;
    int mPerspectiveX = -1;
    int mPerspectiveY = 1;
};

#endif // DISPLAYOPTIONDOCKWIDGET_H
