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
#ifndef TIMECODECONTROLWIDGET_H
#define TIMECODECONTROLWIDGET_H

#include <QWidget>
#include "pencildef.h"

struct TimeCodeControls {
    bool enabled = false;
    bool showFrames = false;

    TimecodeKind kind = TimecodeKind::NONE;

    TimecodeKind timecodeKindFromInt(int value) const
    {
        switch (value)
        {
            case 0:
                return TimecodeKind::NONE;
            case 1:
                return TimecodeKind::SMPTE;
            case 2:
                return TimecodeKind::SFF;
            default:
                return TimecodeKind::NONE;
        }
    }
};

class TimeCodeControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TimeCodeControlWidget(TimeCodeControls* controls, QWidget* parent = nullptr);

signals:
    void timecodeUpdated();

private:

    int timecodeKindToInt(TimecodeKind kind) const;

    void setTimecodeTimerKind(TimecodeKind kind);
    void showFrames(bool shown);
    void showTimecode(bool shown);

    TimeCodeControls* mControls = nullptr;
};

#endif // TIMECODECONTROLWIDGET_H
