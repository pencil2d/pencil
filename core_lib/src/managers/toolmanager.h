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

#ifndef TOOLMANAGER_H
#define TOOLMANAGER_H

#include <QObject>
#include <QHash>
#include "basetool.h"
#include "pencildef.h"
#include "basemanager.h"
#include "camerafieldoption.h"

class StrokeTool;

class ToolManager : public BaseManager
{
    Q_OBJECT
public:
    explicit ToolManager(Editor* editor);
    ~ToolManager() override;

    bool init() override;
    Status load(Object*) override;
    Status save(Object*) override;

    BaseTool* currentTool() const;
    BaseTool* getTool(ToolType eToolType);
    BaseTool* getTool(ToolCategory eToolCategory);
    void setDefaultTool();
    void setCurrentTool(ToolType eToolType);
    void tabletSwitchToEraser();
    void tabletRestorePrevTool();
    bool setTemporaryTool(ToolType eToolType, QFlags<Qt::Key> keys, Qt::KeyboardModifiers modifiers);
    bool setTemporaryTool(ToolType eToolType, Qt::MouseButtons buttons);
    bool tryClearTemporaryTool(Qt::Key key);
    bool tryClearTemporaryTool(Qt::MouseButton button);
    void clearTemporaryTool();
    void cleanupAllToolsData();
    bool leavingThisTool();

signals:
    void toolChanged(ToolType);

public slots:
    void resetAllTools();

private:
    void setTemporaryTool(ToolType eToolType);

    BaseTool* mCurrentTool = nullptr;
    BaseTool* mTabletEraserTool = nullptr;
    BaseTool* mTemporaryTool = nullptr;

    Qt::KeyboardModifiers mTemporaryTriggerModifiers = Qt::NoModifier;
    QFlags<Qt::Key> mTemporaryTriggerKeys;
    Qt::MouseButtons mTemporaryTriggerMouseButtons = Qt::NoButton;

    QHash<ToolType, BaseTool*> mToolSetHash;
};

#endif // TOOLMANAGER_H
