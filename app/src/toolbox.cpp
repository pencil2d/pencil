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

#include "toolbox.h"

#include <cmath>

#include <QToolButton>
#include <QGridLayout>
#include <QKeySequence>
#include <QResizeEvent>
#include <QDebug>
#include <QScrollBar>
#include <QBoxLayout>

#include "flowlayout.h"
#include "spinslider.h"
#include "editor.h"
#include "toolmanager.h"
#include "layermanager.h"
#include "pencilsettings.h"

ToolBoxDockWidget::ToolBoxDockWidget(QWidget* parent) :
    BaseDockWidget(parent)
{
    mWidget = new ToolBoxWidget(this);

    setWindowTitle(tr("Tools", "Window title of Tools"));
}

ToolBoxDockWidget::~ToolBoxDockWidget()
{
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("ToolBoxGeom", this->saveGeometry());
}

void ToolBoxDockWidget::initUI()
{
    mWidget->setEditor(editor());
    mWidget->initUI();

    setWidget(mWidget);
    setContentsMargins(0,0,0,0);

    connect(editor()->layers(), &LayerManager::currentLayerChanged, this, &ToolBoxDockWidget::onLayerDidChange);

    QSettings settings(PENCIL2D, PENCIL2D);
    restoreGeometry(settings.value("ToolBoxGeom").toByteArray());

    // Important to set the proper minimumSize;
    setMinimumSize(mWidget->minimumSize());
}

void ToolBoxDockWidget::setActiveTool(ToolType type)
{
    mWidget->setToolChecked(type);
}

void ToolBoxDockWidget::updateUI()
{
    mWidget->updateUI();
}

void ToolBoxDockWidget::onLayerDidChange(int)
{
    BaseTool* currentTool = editor()->tools()->currentTool();
    if (currentTool->type() == MOVE || currentTool->type() == CAMERA)
    {
        mWidget->moveOn();
    }
}
