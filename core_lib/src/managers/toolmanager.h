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

#ifndef TOOLMANAGER_H
#define TOOLMANAGER_H

#include <QObject>
#include <QHash>
#include "basetool.h"
#include "basemanager.h"

class ScribbleArea;

class ToolManager : public BaseManager
{
    Q_OBJECT
public:
    explicit ToolManager(Editor* editor);

    bool init() override;
    Status load(Object*) override;
    Status save(Object*) override;

    BaseTool* currentTool() { return mCurrentTool; }
    BaseTool* getTool(ToolType eToolType);
    void setDefaultTool();
    void setCurrentTool(ToolType eToolType);
    void cleanupAllToolsData();
    bool leavingThisTool();

    void tabletSwitchToEraser();
    void tabletRestorePrevTool();
    int propertySwitch(bool condition, int property);

Q_SIGNALS:
    void penWidthValueChanged(float);
    void penFeatherValueChanged(float);
    void toleranceValueChanged(qreal);

    void toolChanged(ToolType);
    void toolPropertyChanged(ToolType, ToolPropertyType);

public slots:
    void resetAllTools();

    void setWidth(float);
    void setFeather(float);
    void setUseFeather(bool);
    void setInvisibility(bool);
    void setPreserveAlpha(bool);
    void setVectorMergeEnabled(bool);
    void setBezier(bool);
    void setPressure(bool);
    void setAA(int);
    void setStabilizerLevel(int);
    void setTolerance(int);
    void setUseFillContour(bool);

private:
    BaseTool * mCurrentTool = nullptr;
    ToolType  meTabletBackupTool = PENCIL;
    bool mIsSwitchedToEraser = false;
    QHash<ToolType, BaseTool*> mToolSetHash;

    int mOldValue = 0;

};

#endif // TOOLMANAGER_H
