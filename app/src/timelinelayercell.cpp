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

#include "timelinelayercell.h"

#include "preferencemanager.h"
#include "layermanager.h"
#include "viewmanager.h"
#include "pencilsettings.h"

#include "layer.h"
#include "layercamera.h"
#include "camerapropertiesdialog.h"
#include "timelinelayercelleditorwidget.h"

#include <QPalette>
#include <QInputDialog>
#include <QRegularExpression>
#include <QMouseEvent>

#include <QDebug>

TimeLineLayerCell::TimeLineLayerCell(TimeLine* timeline,
                                     QWidget* parent,
                                     Editor* editor,
                                     Layer* layer,
                                     const QPoint& origin, int width, int height)
    : TimeLineBaseCell(timeline, parent, editor)
{
    mEditorWidget = new TimeLineLayerCellEditorWidget(parent, editor, layer);
    mEditorWidget->setGeometry(QRect(origin, QSize(width, height)));
    mEditorWidget->show();
}

TimeLineLayerCell::~TimeLineLayerCell()
{
    mEditorWidget->deleteLater();
}

void TimeLineLayerCell::setSize(const QSize& size)
{
    mEditorWidget->resize(size);
}

