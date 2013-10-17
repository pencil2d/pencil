
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
#include "toolmanager.h"


ToolManager::ToolManager(QObject* parent, Editor* pEditor, ScribbleArea* pScribbleArea) :
    QObject( parent )
{
    m_toolSetHash.insert(PEN, new PenTool);
    m_toolSetHash.insert(PENCIL, new PencilTool);
    m_toolSetHash.insert(BRUSH, new BrushTool);
    m_toolSetHash.insert(ERASER, new EraserTool);
    m_toolSetHash.insert(BUCKET, new BucketTool);
    m_toolSetHash.insert(EYEDROPPER, new EyedropperTool);
    m_toolSetHash.insert(HAND, new HandTool);
    m_toolSetHash.insert(MOVE, new MoveTool);
    m_toolSetHash.insert(POLYLINE, new PolylineTool);
    m_toolSetHash.insert(SELECT, new SelectTool);
    m_toolSetHash.insert(SMUDGE, new SmudgeTool);

    foreach ( BaseTool* pTool, m_toolSetHash.values() )
    {
        pTool->initialize(pEditor, pScribbleArea);
    }

    m_pCurrentTool = getTool( PENCIL );
}

BaseTool* ToolManager::currentTool()
{
    return m_pCurrentTool;
}

BaseTool* ToolManager::getTool(ToolType eToolType)
{
    return m_toolSetHash[ eToolType ];
}

void ToolManager::setCurrentTool(ToolType eToolType)
{
    m_pCurrentTool = getTool( eToolType );
}

void ToolManager::cleanupAllToolsData()
{
    foreach ( BaseTool* pTool, m_toolSetHash.values() )
    {
        pTool->clear();
    }
}
