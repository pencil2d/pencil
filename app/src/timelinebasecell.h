/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2008-2009 Mj Mendoza IV
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef TIMELINEBASECELL_H
#define TIMELINEBASECELL_H


#include <QPainter>
#include <QPalette>
#include <QObject>

#include "pencildef.h"

class TimeLine;
class Editor;
class Layer;
class PreferenceManager;
class QMouseEvent;

class TimeLineBaseCell: public QObject {
    Q_OBJECT
public:
    TimeLineBaseCell(TimeLine* timeline,
                     QWidget* parent,
                    Editor* editor);
    virtual ~TimeLineBaseCell();

    virtual void setSize(const QSize& size) = 0;

    Editor* mEditor = nullptr;
    TimeLine* mTimeLine = nullptr;
    PreferenceManager* mPrefs = nullptr;

private:
};

#endif // TIMELINEBASECELL_H
