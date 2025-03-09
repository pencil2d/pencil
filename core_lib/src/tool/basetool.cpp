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

#include "basetool.h"

#include <array>
#include "editor.h"
#include "viewmanager.h"
#include "scribblearea.h"
#include "strokeinterpolator.h"
#include "pointerevent.h"

QString BaseTool::TypeName(ToolType type)
{
    static std::array<QString, TOOL_TYPE_COUNT> map;

    if (map[0].isEmpty())
    {
        map[PENCIL] = tr("Pencil");
        map[ERASER] = tr("Eraser");
        map[SELECT] = tr("Select");
        map[MOVE] = tr("Move");
        map[HAND] = tr("Hand");
        map[SMUDGE] = tr("Smudge");
        map[PEN] = tr("Pen");
        map[POLYLINE] = tr("Polyline");
        map[BUCKET] = tr("Bucket");
        map[EYEDROPPER] = tr("Eyedropper");
        map[BRUSH] = tr("Brush");
        map[CAMERA] = tr("Camera");
    }
    return map.at(type);
}

BaseTool::BaseTool(QObject* parent) : QObject(parent)
{
    mPropertyEnabled.insert(WIDTH, false);
    mPropertyEnabled.insert(FEATHER, false);
    mPropertyEnabled.insert(USEFEATHER, false);
    mPropertyEnabled.insert(PRESSURE, false);
    mPropertyEnabled.insert(INVISIBILITY, false);
    mPropertyEnabled.insert(PRESERVEALPHA, false);
    mPropertyEnabled.insert(BEZIER, false);
    mPropertyEnabled.insert(CLOSEDPATH, false);
    mPropertyEnabled.insert(ANTI_ALIASING, false);
    mPropertyEnabled.insert(FILL_MODE, false);
    mPropertyEnabled.insert(STABILIZATION, false);
    mPropertyEnabled.insert(CAMERA_SHOWPATH_CHECKED, false);
}

void BaseTool::saveSettings()
{
    if (getProperties()) {
        QSettings settings(PENCIL2D, PENCIL2D);
        getProperties()->save(settings);
    }
}

void BaseTool::resetSettings()
{
    if (getProperties()) {
        QSettings settings(PENCIL2D, PENCIL2D);
        getProperties()->setDefaults();
    }
}

QCursor BaseTool::cursor()
{
    return Qt::ArrowCursor;
}

bool BaseTool::leavingThisTool()
{
   for (auto& connection : mActiveConnections) {
       disconnect(connection);
       mActiveConnections.removeOne(connection);
   }

   saveSettings();

   return true;
}

void BaseTool::initialize(Editor* editor)
{
    Q_ASSERT(editor);
    mEditor = editor;
    mScribbleArea = editor->getScribbleArea();
    Q_ASSERT(mScribbleArea);

    loadSettings();
}

void BaseTool::pointerPressEvent(PointerEvent* event)
{
    event->accept();
}

void BaseTool::pointerMoveEvent(PointerEvent* event)
{
    event->accept();
}

void BaseTool::pointerReleaseEvent(PointerEvent* event)
{
    event->accept();
}

void BaseTool::pointerDoubleClickEvent(PointerEvent* event)
{
    pointerPressEvent(event);
}

/**
 * @brief BaseTool::isDrawingTool - A drawing tool is anything that applies something to the canvas.
 * SELECT and MOVE does not count here because they modify already applied content.
 * @return true if not a drawing tool and false otherwise
 */
bool BaseTool::isDrawingTool()
{
    if (type() == ToolType::HAND || type() == ToolType::MOVE || type() == ToolType::CAMERA || type() == ToolType::SELECT )
    {
        return false;
    }
    return true;
}

bool BaseTool::isActive() const
{
    return false;
}
