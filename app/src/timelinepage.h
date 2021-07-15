/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef TIMELINEPAGE_H
#define TIMELINEPAGE_H

class PreferenceManager;

namespace Ui {
class TimelinePage;
}

class TimelinePage : public QWidget
{
Q_OBJECT
public:
    TimelinePage();
    ~TimelinePage() override;

    void setManager(PreferenceManager* p) { mManager = p; }

public slots:
    void updateValues();

    void timelineLengthChanged(int);
    void fontSizeChanged(int);
    void scrubChanged(int);
    void drawEmptyKeyRadioButtonToggled(bool);
    void flipRollMsecSliderChanged(int value);
    void flipRollMsecSpinboxChanged(int value);
    void flipRollNumDrawingsSliderChanged(int value);
    void flipRollNumDrawingsSpinboxChanged(int value);
    void flipInbetweenMsecSliderChanged(int value);
    void flipInbetweenMsecSpinboxChanged(int value);
    void soundScrubActiveChanged(int i);
    void soundScrubMsecSliderChanged(int value);
    void soundScrubMsecSpinboxChanged(int value);
    void layerVisibilityChanged(int);
    void layerVisibilityThresholdChanged(int);

signals:
    void soundScrubChanged(bool b);
    void soundScrubMsecChanged(int mSec);

private:
    Ui::TimelinePage* ui = nullptr;
    PreferenceManager* mManager = nullptr;
};

#endif // TIMELINEPAGE_H
