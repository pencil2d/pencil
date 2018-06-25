/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

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
#include "editor.h"


ToolManager::ToolManager(Editor* editor) : BaseManager(editor)
{
}

bool ToolManager::init()
{
    mIsSwitchedToEraser = false;

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

    foreach(BaseTool* pTool, mToolSetHash.values())
    {
        pTool->initialize(editor());
    }

    setDefaultTool();

    return true;
}

Status ToolManager::load(Object*)
{
    return Status::OK;
}

Status ToolManager::save(Object*)
{
    return Status::OK;
}

BaseTool* ToolManager::getTool(ToolType eToolType)
{
    return mToolSetHash[eToolType];
}

void ToolManager::setDefaultTool()
{
    // Set default tool
    // (called by the main window init)
    ToolType defaultToolType = PENCIL;

    setCurrentTool(defaultToolType);
    meTabletBackupTool = defaultToolType;
}

void ToolManager::setCurrentTool(ToolType eToolType)
{
    if (mCurrentTool != NULL)
    {
       leavingThisTool();
    }

    mCurrentTool = getTool(eToolType);
    Q_EMIT toolChanged(eToolType);
}

bool ToolManager::leavingThisTool()
{
    return mCurrentTool->leavingThisTool();
}

void ToolManager::cleanupAllToolsData()
{
    foreach(BaseTool* pTool, mToolSetHash.values())
    {
        pTool->clear();
    }
}

void ToolManager::resetAllTools()
{
    // Reset can be useful to solve some pencil settings problems.
    // Beta-testers should be recommended to reset before sending tool related issues.
    // This can prevent from users to stop working on their project.
    getTool(PEN)->properties.width = 1.5; // not supposed to use feather
    getTool(PEN)->properties.stabilizerLevel = -1;
    getTool(POLYLINE)->properties.width = 1.5; // PEN dependent
    getTool(PENCIL)->properties.width = 1.0;
    getTool(PENCIL)->properties.feather = -1.0; // locks feather usage (can be changed)
    getTool(PENCIL)->properties.stabilizerLevel = -1;
    getTool(ERASER)->properties.width = 25.0;
    getTool(ERASER)->properties.feather = 50.0;
    getTool(BRUSH)->properties.width = 15.0;
    getTool(BRUSH)->properties.feather = 200.0;
    getTool(BRUSH)->properties.stabilizerLevel = -1;
    getTool(BRUSH)->properties.useFeather = false;
    getTool(SMUDGE)->properties.width = 25.0;
    getTool(SMUDGE)->properties.feather = 200.0;
    getTool(BUCKET)->properties.tolerance = 10.0;

    // todo: add all the default settings

    qDebug("tools restored to default settings");
}

void ToolManager::setWidth(float newWidth)
{
    if (std::isnan(newWidth) || newWidth < 0)
    {
        newWidth = 1.f;
    }

    currentTool()->setWidth(newWidth);
    Q_EMIT penWidthValueChanged(newWidth);
    Q_EMIT toolPropertyChanged(currentTool()->type(), WIDTH);
}

void ToolManager::setFeather(float newFeather)
{
    if (std::isnan(newFeather) || newFeather < 0)
    {
        newFeather = 0.f;
    }

    currentTool()->setFeather(newFeather);
    Q_EMIT penFeatherValueChanged(newFeather);
    Q_EMIT toolPropertyChanged(currentTool()->type(), FEATHER);
}

void ToolManager::setUseFeather(bool usingFeather)
{
    int usingAA = currentTool()->properties.useAA;
    int value = propertySwitch(usingFeather, usingAA);

    currentTool()->setAA(value);
    currentTool()->setUseFeather(usingFeather);
    Q_EMIT toolPropertyChanged(currentTool()->type(), USEFEATHER);
    Q_EMIT toolPropertyChanged(currentTool()->type(), ANTI_ALIASING);
}

void ToolManager::setInvisibility(bool isInvisible)
{
    currentTool()->setInvisibility(isInvisible);
    Q_EMIT toolPropertyChanged(currentTool()->type(), INVISIBILITY);
}

void ToolManager::setPreserveAlpha(bool isPreserveAlpha)
{
    currentTool()->setPreserveAlpha(isPreserveAlpha);
    Q_EMIT toolPropertyChanged(currentTool()->type(), PRESERVEALPHA);
}

void ToolManager::setVectorMergeEnabled(bool isVectorMergeEnabled)
{
    currentTool()->setVectorMergeEnabled(isVectorMergeEnabled);
    Q_EMIT toolPropertyChanged(currentTool()->type(), VECTORMERGE);
}

void ToolManager::setBezier(bool isBezierOn)
{
    currentTool()->setBezier(isBezierOn);
    Q_EMIT toolPropertyChanged(currentTool()->type(), BEZIER);
}

void ToolManager::setPressure(bool isPressureOn)
{
    currentTool()->setPressure(isPressureOn);
    Q_EMIT toolPropertyChanged(currentTool()->type(), PRESSURE);
}

void ToolManager::setAA(int usingAA)
{
    currentTool()->setAA(usingAA);
    Q_EMIT toolPropertyChanged(currentTool()->type(), ANTI_ALIASING);
}

void ToolManager::setStabilizerLevel(int level)
{
    currentTool()->setStabilizerLevel(level);
    Q_EMIT toolPropertyChanged(currentTool()->type(), STABILIZATION);
}

void ToolManager::setTolerance(int newTolerance)
{
    if (newTolerance < 0)
    {
        newTolerance = 1;
    }

    currentTool()->setTolerance(newTolerance);
    Q_EMIT toleranceValueChanged(newTolerance);
    Q_EMIT toolPropertyChanged(currentTool()->type(), TOLERANCE);
}

void ToolManager::setUseFillContour(bool useFillContour)
{
    currentTool()->setUseFillContour(useFillContour);
    Q_EMIT toolPropertyChanged(currentTool()->type(), FILLCONTOUR);
}


// Switches on/off two actions
// eg. if x = true, then y = false
int ToolManager::propertySwitch(bool condition, int tool)
{
    int value = 0;
    int newValue = 0;

    if (condition == true) {
        value = -1;
        newValue = mOldValue;
        mOldValue = tool;
    }

    if (condition == false) {
        if (newValue == 1) {
            value = 1;
        }
        else {
            value = mOldValue;
        }
    }
    return value;
}

void ToolManager::tabletSwitchToEraser()
{
    if (!mIsSwitchedToEraser)
    {
        mIsSwitchedToEraser = true;

        meTabletBackupTool = mCurrentTool->type();
        setCurrentTool(ERASER);
    }
}

void ToolManager::tabletRestorePrevTool()
{
    if (mIsSwitchedToEraser)
    {
        mIsSwitchedToEraser = false;
        if (meTabletBackupTool == INVALID_TOOL)
        {
            meTabletBackupTool = PENCIL;
        }
        setCurrentTool(meTabletBackupTool);
    }
}
