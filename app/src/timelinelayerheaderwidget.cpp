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

#include "timelinelayerheaderwidget.h"

#include <QPalette>
#include <QApplication>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QDebug>
#include <QCheckBox>
#include <QSpacerItem>

#include "editor.h"
#include "timeline.h"
#include "layervisibilitybutton.h"

TimeLineLayerHeaderWidget::TimeLineLayerHeaderWidget(TimeLine* timeLine,
                                                Editor* editor)
    : QWidget(timeLine)
{
    mHLayout = new QHBoxLayout(this);
    mVisibilityButton = new LayerVisibilityButton(this, LayerVisibilityContext::GLOBAL, nullptr, editor);

    mHLayout->setContentsMargins(0,0,0,0);
    mHLayout->addWidget(mVisibilityButton);
    mHLayout->addSpacerItem(new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Expanding));
}

TimeLineLayerHeaderWidget::~TimeLineLayerHeaderWidget()
{
}
