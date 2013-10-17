#include "toolmanager.h"
#include "basetool.h"


ToolManager::ToolManager(QObject *parent) :
    QObject(parent)
{
}

BaseTool *ToolManager::currentTool()
{
    return m_pCurrentTool;
}
