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


ToolManager::ToolManager(Editor* editor) : BaseManager(editor, __FUNCTION__)
{
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

BaseTool* ToolManager::currentTool()
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
    if (mCurrentTool != nullptr)
    {
       mCurrentTool->leavingThisTool();
    }

    mCurrentTool = getTool(eToolType);
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
        tool->resetToDefault();
    }
    qDebug("tools restored to default settings");
}

void ToolManager::setWidth(float newWidth)
{
    if (std::isnan(newWidth) || newWidth < 0)
    {
        newWidth = 1.f;
    }

    currentTool()->setWidth(static_cast<qreal>(newWidth));
    emit penWidthValueChanged(newWidth);
    emit toolPropertyChanged(currentTool()->type(), WIDTH);
}

void ToolManager::setFeather(float newFeather)
{
    if (std::isnan(newFeather) || newFeather < 0)
    {
        newFeather = 0.f;
    }

    currentTool()->setFeather(static_cast<qreal>(newFeather));
    emit penFeatherValueChanged(newFeather);
    emit toolPropertyChanged(currentTool()->type(), FEATHER);
}

void ToolManager::setUseFeather(bool usingFeather)
{
    int usingAA = currentTool()->properties.useAA;
    int value = propertySwitch(usingFeather, usingAA);

    currentTool()->setAA(value);
    currentTool()->setUseFeather(usingFeather);
    emit toolPropertyChanged(currentTool()->type(), USEFEATHER);
    emit toolPropertyChanged(currentTool()->type(), ANTI_ALIASING);
}

void ToolManager::setInvisibility(bool isInvisible)
{
    currentTool()->setInvisibility(isInvisible);
    emit toolPropertyChanged(currentTool()->type(), INVISIBILITY);
}

void ToolManager::setPreserveAlpha(bool isPreserveAlpha)
{
    currentTool()->setPreserveAlpha(isPreserveAlpha);
    emit toolPropertyChanged(currentTool()->type(), PRESERVEALPHA);
}

void ToolManager::setVectorMergeEnabled(bool isVectorMergeEnabled)
{
    currentTool()->setVectorMergeEnabled(isVectorMergeEnabled);
    emit toolPropertyChanged(currentTool()->type(), VECTORMERGE);
}

void ToolManager::setBezier(bool isBezierOn)
{
    currentTool()->setBezier(isBezierOn);
    emit toolPropertyChanged(currentTool()->type(), BEZIER);
}

void ToolManager::setPressure(bool isPressureOn)
{
    currentTool()->setPressure(isPressureOn);
    emit toolPropertyChanged(currentTool()->type(), PRESSURE);
}

void ToolManager::setAA(int usingAA)
{
    currentTool()->setAA(usingAA);
    emit toolPropertyChanged(currentTool()->type(), ANTI_ALIASING);
}

void ToolManager::setFillMode(int mode)
{
    currentTool()->setFillMode(mode);
    emit toolPropertyChanged(currentTool()->type(), FILL_MODE);
}

void ToolManager::setStabilizerLevel(int level)
{
    currentTool()->setStabilizerLevel(level);
    emit toolPropertyChanged(currentTool()->type(), STABILIZATION);
}

void ToolManager::setTolerance(int newTolerance)
{
    newTolerance = qMax(0, newTolerance);

    currentTool()->setTolerance(newTolerance);
    emit toleranceValueChanged(newTolerance);
    emit toolPropertyChanged(currentTool()->type(), TOLERANCE);
}

void ToolManager::setBucketColorToleranceEnabled(bool enabled)
{
    currentTool()->setToleranceEnabled(enabled);
    emit toolPropertyChanged(currentTool()->type(), USETOLERANCE);
}

void ToolManager::setBucketFillExpandEnabled(bool expandValue)
{
    currentTool()->setFillExpandEnabled(expandValue);
    emit toolPropertyChanged(currentTool()->type(), USEBUCKETFILLEXPAND);
}

void ToolManager::setBucketFillExpand(int expandValue)
{
    currentTool()->setFillExpand(expandValue);
    emit toolPropertyChanged(currentTool()->type(), BUCKETFILLEXPAND);
}

void ToolManager::setBucketFillToLayer(int layerIndex)
{
    currentTool()->setFillToLayer(layerIndex);
    emit toolPropertyChanged(currentTool()->type(), BUCKETFILLLAYERMODE);
}

void ToolManager::setBucketFillReferenceMode(int referenceMode)
{
    currentTool()->setFillReferenceMode(referenceMode);
    emit toolPropertyChanged(currentTool()->type(), BUCKETFILLLAYERREFERENCEMODE);
}

void ToolManager::setUseFillContour(bool useFillContour)
{
    currentTool()->setUseFillContour(useFillContour);
    emit toolPropertyChanged(currentTool()->type(), FILLCONTOUR);
}

void ToolManager::setShowSelectionInfo(bool b)
{
    currentTool()->setShowSelectionInfo(b);
}

// Switches on/off two actions
// eg. if x = true, then y = false
int ToolManager::propertySwitch(bool condition, int tool)
{
    int value = 0;
    int newValue = 0;

    if (condition == true)
    {
        value = -1;
        newValue = mOldValue;
        mOldValue = tool;
    }
    else if (condition == false)
    {
        value = (newValue == 1) ? 1 : mOldValue;
    }
    return value;
}

void ToolManager::tabletSwitchToEraser()
{
    mTabletEraserTool = getTool(ERASER);
    if (mTemporaryTool == nullptr)
    {
        emit toolChanged(ERASER);
    }
}

void ToolManager::tabletRestorePrevTool()
{
    mTabletEraserTool = nullptr;
    if (mTemporaryTool == nullptr)
    {
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
    mTemporaryTool = nullptr;
    mTemporaryTriggerKeys = {};
    mTemporaryTriggerModifiers = Qt::NoModifier;
    mTemporaryTriggerMouseButtons = Qt::NoButton;
    emit toolChanged(currentTool()->type());
}
