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
#include "toolmanager.h"

#include <cmath>
#include "stroketool.h"
#include "pentool.h"
#include "penciltool.h"
#include "brushtool.h"
#include "buckettool.h"
#include "erasertool.h"
#include "eyedroppertool.h"
#include "handtool.h"
#include "movetool.h"
#include "polylinetool.h"
#include "selecttool.h"
#include "smudgetool.h"
#include "cameratool.h"
#include "editor.h"


ToolManager::ToolManager(Editor* editor) : BaseManager(editor, __FUNCTION__)
{
}

ToolManager::~ToolManager()
{
    foreach(BaseTool* tool, mToolSetHash)
    {
        tool->saveSettings();
    }
}

bool ToolManager::init()
{
    mToolSetHash.insert(PEN, new PenTool(this));
    mToolSetHash.insert(PENCIL, new PencilTool(this));
    mToolSetHash.insert(BRUSH, new BrushTool(this));
    mToolSetHash.insert(ERASER, new EraserTool(this));
    mToolSetHash.insert(BUCKET, new BucketTool(this));
    mToolSetHash.insert(EYEDROPPER, new EyedropperTool(this));
    mToolSetHash.insert(HAND, new HandTool(this));
    mToolSetHash.insert(MOVE, new MoveTool(this));
    mToolSetHash.insert(POLYLINE, new PolylineTool(this));
    mToolSetHash.insert(SELECT, new SelectTool(this));
    mToolSetHash.insert(SMUDGE, new SmudgeTool(this));
    mToolSetHash.insert(CAMERA, new CameraTool(this));

    foreach(BaseTool* pTool, mToolSetHash.values())
    {
        pTool->initialize(editor());
    }

    setDefaultTool();

    return true;
}

Status ToolManager::load(Object*)
{
    setDefaultTool();
    return Status::OK;
}

Status ToolManager::save(Object*)
{
    return Status::OK;
}

BaseTool* ToolManager::currentTool() const
{
    if (mTemporaryTool != nullptr)
    {
        return mTemporaryTool;
    }
    else if (mTabletEraserTool != nullptr)
    {
        return mTabletEraserTool;
    }
    return mCurrentTool;
}

BaseTool* ToolManager::getTool(ToolType eToolType)
{
    return mToolSetHash[eToolType];
}

BaseTool* ToolManager::getTool(ToolCategory eToolCategory)
{
    switch (eToolCategory) {
        case ToolCategory::TRANSFORMTOOL:
        case ToolCategory::STROKETOOL: {
            if (currentTool()->category() == eToolCategory) {
                return currentTool();
            } else {
                return nullptr;
            }
        }
        case ToolCategory::BASETOOL: {
            return currentTool();
        }
    }
}

void ToolManager::setDefaultTool()
{
    // Set default tool
    // (called by the main window init)
    ToolType defaultToolType = PENCIL;

    setCurrentTool(defaultToolType);
    mTabletEraserTool = nullptr;
    mTemporaryTool = nullptr;
}

void ToolManager::setCurrentTool(ToolType eToolType)
{
    // We're already using this tool
    if (mCurrentTool == getTool(eToolType)) { return; }

    if (mCurrentTool != nullptr)
    {
       mCurrentTool->leavingThisTool();
    }

    mCurrentTool = getTool(eToolType);
    mCurrentTool->enteringThisTool();
    if (mTemporaryTool == nullptr && mTabletEraserTool == nullptr)
    {
        emit toolChanged(eToolType);
    }
}

bool ToolManager::leavingThisTool()
{
    return currentTool()->leavingThisTool();
}

void ToolManager::cleanupAllToolsData()
{
    foreach(BaseTool* tool, mToolSetHash)
    {
        tool->clearToolData();
    }
}

void ToolManager::resetAllTools()
{
    // Reset can be useful to solve some pencil settings problems.
    // Beta-testers should be recommended to reset before sending tool related issues.
    // This can prevent from users to stop working on their project.

    foreach(BaseTool* tool, mToolSetHash)
    {
        tool->resetSettings();
    }
    qDebug("tools restored to default settings");
}

void ToolManager::tabletSwitchToEraser()
{
    mTabletEraserTool = getTool(ERASER);

    // We should only notify a tool change if we're positive that the state has changed and it should only happen once
    // if the user for some reason is using another temporary tool at the same time, that takes first priority
    if (mTemporaryTool == nullptr)
    {
        emit toolChanged(ERASER);
    }
}

void ToolManager::tabletRestorePrevTool()
{
    if (mTemporaryTool == nullptr && mTabletEraserTool != nullptr)
    {
        mTabletEraserTool = nullptr;
        emit toolChanged(currentTool()->type());
    }
}

bool ToolManager::setTemporaryTool(ToolType eToolType, QFlags<Qt::Key> keys, Qt::KeyboardModifiers modifiers)
{
    if (mTemporaryTool != nullptr) return false;
    mTemporaryTriggerKeys = keys;
    mTemporaryTriggerModifiers = modifiers;
    mTemporaryTriggerMouseButtons = Qt::NoButton;
    setTemporaryTool(eToolType);
    return true;
}

bool ToolManager::setTemporaryTool(ToolType eToolType, Qt::MouseButtons buttons)
{
    if (mTemporaryTool != nullptr) return false;
    mTemporaryTriggerKeys = {};
    mTemporaryTriggerModifiers = Qt::NoModifier;
    mTemporaryTriggerMouseButtons = buttons;
    setTemporaryTool(eToolType);
    return true;
}

bool ToolManager::tryClearTemporaryTool(Qt::Key key)
{
    Qt::KeyboardModifier modifier = Qt::NoModifier;
    switch(key)
    {
    case Qt::Key_Control:
        modifier = Qt::ControlModifier;
        break;
    case Qt::Key_Shift:
        modifier = Qt::ShiftModifier;
        break;
    case Qt::Key_Alt:
        modifier = Qt::AltModifier;
        break;
    case Qt::Key_Meta:
        modifier = Qt::MetaModifier;
        break;
    default:
        break;
    }

    if (mTemporaryTriggerKeys.testFlag(key) ||
        mTemporaryTriggerModifiers.testFlag(modifier))
    {
        clearTemporaryTool();
        return true;
    }
    return false;
}

bool ToolManager::tryClearTemporaryTool(Qt::MouseButton button)
{
    if (mTemporaryTriggerMouseButtons != Qt::NoButton && mTemporaryTriggerMouseButtons.testFlag(button))
    {
        clearTemporaryTool();
        return true;
    }
    return false;
}

void ToolManager::setTemporaryTool(ToolType eToolType)
{
    mTemporaryTool = getTool(eToolType);
    emit toolChanged(eToolType);
}

void ToolManager::clearTemporaryTool()
{
    if (mTemporaryTool) {
        mTemporaryTool->leavingThisTool();
        mTemporaryTool = nullptr;
    }
    mTemporaryTriggerKeys = {};
    mTemporaryTriggerModifiers = Qt::NoModifier;
    mTemporaryTriggerMouseButtons = Qt::NoButton;
    emit toolChanged(currentTool()->type());
}
