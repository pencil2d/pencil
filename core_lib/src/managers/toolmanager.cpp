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
#include "bitmappentool.h"
#include "bitmappenciltool.h"
#include "bitmapbrushtool.h"
#include "bitmapbuckettool.h"
#include "bitmaperasertool.h"
#include "bitmapeyedroppertool.h"
#include "handtool.h"
#include "bitmapmovetool.h"
#include "bitmappolylinetool.h"
#include "bitmapselecttool.h"
#include "bitmapsmudgetool.h"
#include "vectorpentool.h"
#include "vectorpenciltool.h"
#include "vectorbuckettool.h"
#include "vectorselecttool.h"
#include "vectorerasertool.h"
#include "vectorbrushtool.h"
#include "vectorpolylinetool.h"
#include "vectormovetool.h"
#include "vectorsmudgetool.h"
#include "vectoreyedroppertool.h"
#include "notool.h"
#include "editor.h"

#include <QSettings>

#include <QDebug>

ToolManager::ToolManager(Editor* editor) : BaseManager(editor)
{
}

bool ToolManager::init()
{
    mIsSwitchedToEraser = false;

    mVectorToolSetHash.insert(PEN, new VectorPenTool(this));
    mVectorToolSetHash.insert(PENCIL, new VectorPencilTool(this));
    mVectorToolSetHash.insert(BRUSH, new VectorBrushTool(this));
    mVectorToolSetHash.insert(ERASER, new VectorEraserTool(this));
    mVectorToolSetHash.insert(BUCKET, new VectorBucketTool(this));
    mVectorToolSetHash.insert(EYEDROPPER, new VectorEyedropperTool(this));
    mVectorToolSetHash.insert(MOVE, new VectorMoveTool(this));
    mVectorToolSetHash.insert(POLYLINE, new VectorPolylineTool(this));
    mVectorToolSetHash.insert(SELECT, new VectorSelectTool(this));
    mVectorToolSetHash.insert(SMUDGE, new VectorSmudgeTool(this));
    mVectorToolSetHash.insert(HAND, new HandTool(this));

    mBitmapToolSetHash.insert(SMUDGE, new BitmapSmudgeTool(this));
    mBitmapToolSetHash.insert(SELECT, new BitmapSelectTool(this));
    mBitmapToolSetHash.insert(POLYLINE, new BitmapPolylineTool(this));
    mBitmapToolSetHash.insert(MOVE, new BitmapMoveTool(this));
    mBitmapToolSetHash.insert(EYEDROPPER, new BitmapEyedropperTool(this));
    mBitmapToolSetHash.insert(BUCKET, new BitmapBucketTool(this));
    mBitmapToolSetHash.insert(ERASER, new BitmapEraserTool(this));
    mBitmapToolSetHash.insert(BRUSH, new BitmapBrushTool(this));
    mBitmapToolSetHash.insert(PENCIL, new BitmapPencilTool(this));
    mBitmapToolSetHash.insert(PEN, new BitmapPenTool(this));
    mBitmapToolSetHash.insert(HAND, new HandTool(this));

    foreach(BaseTool* pTool, mVectorToolSetHash.values())
    {
        pTool->initialize(editor());
    }

    foreach(BaseTool* pTool, mBitmapToolSetHash.values())
    {
        pTool->initialize(editor());
    }

    setDefaultTool(mLayerType);

    return true;
}

void ToolManager::workingLayerChanged(Layer* layer)
{
    Layer::LAYER_TYPE layerType = layer->type();
    if (layerType == Layer::CAMERA || layerType == Layer::SOUND) {
        setCurrentTool(HAND, layerType);
    } else {

        QSettings settings (PENCIL2D, PENCIL2D);
        ToolType toolType = INVALID_TOOL;
        toolType = static_cast<ToolType>(settings.value(SETTING_TOOL_LASTUSED).toInt());

        setCurrentTool(toolType, layerType);
    }
    mLayerType = layerType;

}

Status ToolManager::load(Object*)
{
    return Status::OK;
}

Status ToolManager::save(Object*)
{
    return Status::OK;
}

ToolType ToolManager::safeToolType(const ToolType& type)
{
    if (type == INVALID_TOOL) {
        return mFallbackToolType;
    }
    return type;
}

BaseTool* ToolManager::getTool(const ToolType eToolType, const Layer::LAYER_TYPE layerType)
{
    BaseTool* tool = nullptr;
    ToolType toolType = safeToolType(eToolType);
    if (layerType == Layer::BITMAP) {
        tool = mBitmapToolSetHash[toolType];
    } else if (layerType == Layer::VECTOR) {
        tool = mVectorToolSetHash[toolType];
    } else {
        // hack: one of the toolsets should be selected, doesn't matter which...
        tool = mBitmapToolSetHash[toolType];
    }
    return tool;
}

void ToolManager::setDefaultTool(const Layer::LAYER_TYPE layerType)
{
    // Set default tool
    // (called by the main window init)
    ToolType defaultToolType = PENCIL;

    setCurrentTool(defaultToolType, layerType);
    mFallbackToolType = defaultToolType;
}

void ToolManager::setCurrentTool(const ToolType eToolType, const Layer::LAYER_TYPE layerType)
{
    if (mCurrentTool != nullptr)
    {
       leavingThisTool();
    }

    mCurrentTool = getTool(eToolType, layerType);
    Q_EMIT toolChanged(eToolType);

}

bool ToolManager::leavingThisTool()
{
    return mCurrentTool->leavingThisTool();
}

void ToolManager::cleanupAllToolsData()
{
    foreach(BaseTool* tool, mBitmapToolSetHash)
    {
        tool->clearToolData();
    }

    foreach(BaseTool* tool, mVectorToolSetHash)
    {
        tool->clearToolData();
    }
}

void ToolManager::updateCurrentTool()
{
    setCurrentTool(mCurrentTool->type(), mLayerType);
}

void ToolManager::resetAllTools()
{
    // Reset can be useful to solve some pencil settings problems.
    // Beta-testers should be recommended to reset before sending tool related issues.
    // This can prevent from users to stop working on their project.

    foreach(BaseTool* tool, mBitmapToolSetHash)
    {
        tool->resetToDefault();
    }

    foreach(BaseTool* tool, mVectorToolSetHash)
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
    Q_EMIT penWidthValueChanged(newWidth);
    Q_EMIT toolPropertyChanged(currentTool()->type(), WIDTH);
}

void ToolManager::setFeather(float newFeather)
{
    if (std::isnan(newFeather) || newFeather < 0)
    {
        newFeather = 0.f;
    }

    currentTool()->setFeather(static_cast<qreal>(newFeather));
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
    newTolerance = qMax(0, newTolerance);

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

        mFallbackToolType = mCurrentTool->type();
        setCurrentTool(ERASER, mLayerType);
    }
}

void ToolManager::tabletRestorePrevTool()
{
    if (mIsSwitchedToEraser)
    {
        mIsSwitchedToEraser = false;
        if (mFallbackToolType == INVALID_TOOL)
        {
            mFallbackToolType = PENCIL;
        }
        setCurrentTool(mFallbackToolType, mLayerType);
    }
}
