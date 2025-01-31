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

#ifndef TIMELINELAYERHEADERWIDGET_H
#define TIMELINELAYERHEADERWIDGET_H

#include "timelinebasecell.h"

#include <QWidget>
#include <QPainter>


class TimeLine;
class Editor;
class LayerVisibilityButton;
class QHBoxLayout;

class TimeLineLayerHeaderWidget : public QWidget
{
public:
    TimeLineLayerHeaderWidget(TimeLine* timeLine,
                            Editor* editor);
                            
    ~TimeLineLayerHeaderWidget() override;

private:
    LayerVisibilityButton* mVisibilityButton = nullptr;
    QHBoxLayout* mHLayout = nullptr;
};

#endif // TIMELINELAYERHEADERWIDGET_H
