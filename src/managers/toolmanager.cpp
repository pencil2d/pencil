
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
#include "editor.h"
#include "pencilsettings.h"


ToolManager::ToolManager(QObject* parent ) 
    : BaseManager( parent )
    , m_pCurrentTool( nullptr )
    , m_eTabletBackupTool( INVALID_TOOL )
{
}


bool ToolManager::initialize()
{
    m_toolSetHash.insert( PEN, new PenTool );
    m_toolSetHash.insert( PENCIL, new PencilTool );
    m_toolSetHash.insert( BRUSH, new BrushTool );
    m_toolSetHash.insert( ERASER, new EraserTool );
    m_toolSetHash.insert( BUCKET, new BucketTool );
    m_toolSetHash.insert( EYEDROPPER, new EyedropperTool );
    m_toolSetHash.insert( HAND, new HandTool );
    m_toolSetHash.insert( MOVE, new MoveTool );
    m_toolSetHash.insert( POLYLINE, new PolylineTool );
    m_toolSetHash.insert( SELECT, new SelectTool );
    m_toolSetHash.insert( SMUDGE, new SmudgeTool );

    foreach( BaseTool* pTool, m_toolSetHash.values() )
    {
        pTool->initialize( editor(), editor()->getScribbleArea() );
    }

    m_pCurrentTool = getTool( PENCIL );

    return true;
}


BaseTool* ToolManager::getTool(ToolType eToolType)
{
    return m_toolSetHash[ eToolType ];
}

void ToolManager::setCurrentTool( ToolType eToolType )
{
    if ( m_pCurrentTool->type() != eToolType )
    {
        m_pCurrentTool = getTool( eToolType );

        setWidth( m_pCurrentTool->properties.width );
        setFeather( m_pCurrentTool->properties.feather );
        setPressure( m_pCurrentTool->properties.pressure );
        setPreserveAlpha( m_pCurrentTool->properties.preserveAlpha );
        setInvisibility( m_pCurrentTool->properties.invisibility ); // by definition the pencil is invisible in vector mode

        emit toolChanged( eToolType );
    }
}

void ToolManager::cleanupAllToolsData()
{
    foreach ( BaseTool* pTool, m_toolSetHash.values() )
    {
        pTool->clear();
    }
}

void ToolManager::resetAllTools()
{
    // Reset can be useful to solve some pencil settings problems.
    // Betatesters should be recommended to reset before sending tool related issues.
    // This can prevent from users to stop working on their project.
    getTool( PEN )->properties.width = 1.5; // not supposed to use feather
    getTool( POLYLINE )->properties.width = 1.5; // PEN dependent
    getTool( PENCIL )->properties.width = 1.0;
    getTool( PENCIL )->properties.feather = -1.0; // locks feather usage (can be changed)
    getTool( ERASER )->properties.width = 25.0;
    getTool( ERASER )->properties.feather = 50.0;
    getTool( BRUSH )->properties.width = 15.0;
    getTool( BRUSH )->properties.feather = 200.0;
    getTool( SMUDGE )->properties.width = 25.0;
    getTool( SMUDGE )->properties.feather = 200.0;

    pencilSettings()->setValue( SETTING_TOOL_CURSOR, true );
    // todo: add all the default settings

    qDebug( "tools restored to default settings" );
}

void ToolManager::setWidth( float newWidth )
{
    if ( currentTool()->properties.width != newWidth )
    {
        currentTool()->properties.width = newWidth;
        emit penWidthValueChange( newWidth );
        emit toolPropertyChanged();
    }
}

void ToolManager::setFeather( float newFeather )
{
    if ( currentTool()->properties.feather != newFeather )
    {
        currentTool()->properties.feather = newFeather;
        emit penFeatherValueChange( newFeather );
        emit toolPropertyChanged();
    }
}

void ToolManager::setInvisibility( int isInvisible  )
{
    if ( currentTool()->properties.invisibility != isInvisible )
    {
        currentTool()->properties.invisibility = isInvisible;
        emit penInvisiblityValueChange( isInvisible );
        emit toolPropertyChanged();
    }
}

void ToolManager::setPreserveAlpha( int isPreserveAlpha )
{
    if ( currentTool()->properties.preserveAlpha != isPreserveAlpha )
    {
        currentTool()->properties.preserveAlpha = isPreserveAlpha;
        emit penPreserveAlphaValueChange( isPreserveAlpha );
        emit toolPropertyChanged();
    }
}

void ToolManager::setPressure( int isPressureOn )
{
    if ( currentTool()->properties.pressure != isPressureOn )
    {
        currentTool()->properties.pressure = isPressureOn;
        emit penPressureValueChange( isPressureOn );
        emit toolPropertyChanged();
    }
}

void ToolManager::tabletSwitchToEraser()
{
    if ( currentTool()->type() != ERASER )
    {
        m_eTabletBackupTool = currentTool()->type();
        setCurrentTool( ERASER );
    }
}

void ToolManager::tabletRestorePrevTool()
{
    if ( currentTool()->type() != ERASER )
    {
        if ( m_eTabletBackupTool == INVALID_TOOL )
        {
            m_eTabletBackupTool = PEN;
        }
        setCurrentTool( m_eTabletBackupTool );
    }
}
