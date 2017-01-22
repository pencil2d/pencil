
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


ToolManager::ToolManager(QObject* parent ) : BaseManager( parent )
{
}

bool ToolManager::init()
{
    mIsSwitchedToEraser = false;

    mToolSetHash.insert( PEN, new PenTool( parent() ) );
    mToolSetHash.insert( PENCIL, new PencilTool( parent() ) );
    mToolSetHash.insert( BRUSH, new BrushTool( parent() ) );
    mToolSetHash.insert( ERASER, new EraserTool( parent() ) );
    mToolSetHash.insert( BUCKET, new BucketTool( parent() ) );
    mToolSetHash.insert( EYEDROPPER, new EyedropperTool( parent() ) );
    mToolSetHash.insert( HAND, new HandTool( parent() ) );
    mToolSetHash.insert( MOVE, new MoveTool( parent() ) );
    mToolSetHash.insert( POLYLINE, new PolylineTool( parent() ) );
    mToolSetHash.insert( SELECT, new SelectTool( parent() ) );
    mToolSetHash.insert( SMUDGE, new SmudgeTool( parent() ) );

    foreach( BaseTool* pTool, mToolSetHash.values() )
    {
        pTool->initialize( editor() );
    }

    setDefaultTool();

    return true;
}

Status ToolManager::load( Object* )
{
    return Status::OK;
}

Status ToolManager::save( Object* )
{
	return Status::OK;
}

BaseTool* ToolManager::getTool(ToolType eToolType)
{
    return mToolSetHash[ eToolType ];
}

void ToolManager::setDefaultTool()
{
    // Set default tool
    // (called by the main window init)
    ToolType defaultToolType = PENCIL;

    setCurrentTool(defaultToolType);
    meTabletBackupTool = defaultToolType;
}

void ToolManager::setCurrentTool( ToolType eToolType )
{
    if (mCurrentTool != NULL)
    {
        mCurrentTool->leavingThisTool();
    }
    mCurrentTool = getTool( eToolType );
    Q_EMIT toolChanged( eToolType );
}

void ToolManager::cleanupAllToolsData()
{
    foreach ( BaseTool* pTool, mToolSetHash.values() )
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

    // todo: add all the default settings

    qDebug( "tools restored to default settings" );
}

void ToolManager::setWidth( float newWidth )
{
    if ( std::isnan( newWidth ) || newWidth < 0 )
    {
        newWidth = 1.f;
    }

    currentTool()->setWidth(newWidth);
    Q_EMIT penWidthValueChanged( newWidth );
    Q_EMIT toolPropertyChanged( currentTool()->type(), WIDTH );
}

void ToolManager::setFeather( float newFeather )
{
    if ( std::isnan( newFeather ) || newFeather < 0 )
    {
        newFeather = 0.f;
    }

    currentTool()->setFeather(newFeather);
    Q_EMIT penFeatherValueChanged( newFeather );
    Q_EMIT toolPropertyChanged( currentTool()->type(), FEATHER );
}

void ToolManager::setUseFeather( bool usingFeather )
{
    currentTool()->setUseFeather( usingFeather );
    Q_EMIT toolPropertyChanged( currentTool()->type(), USEFEATHER );
}

void ToolManager::setInvisibility( bool isInvisible )
{
    currentTool()->setInvisibility(isInvisible);
    Q_EMIT toolPropertyChanged( currentTool()->type(), INVISIBILITY );
}

void ToolManager::setPreserveAlpha( bool isPreserveAlpha )
{
    currentTool()->setPreserveAlpha(isPreserveAlpha);
    Q_EMIT toolPropertyChanged( currentTool()->type(), PRESERVEALPHA );
}

void ToolManager::setBezier( bool isBezierOn )
{
    currentTool()->setBezier( isBezierOn );
    Q_EMIT toolPropertyChanged( currentTool()->type(), BEZIER );
}

void ToolManager::setPressure( bool isPressureOn )
{
    currentTool()->setPressure( isPressureOn );
    Q_EMIT toolPropertyChanged( currentTool()->type(), PRESSURE );
}

void ToolManager::tabletSwitchToEraser()
{
    if (!mIsSwitchedToEraser)
    {
        mIsSwitchedToEraser = true;

        meTabletBackupTool = mCurrentTool->type();
        setCurrentTool( ERASER );
    }
}

void ToolManager::tabletRestorePrevTool()
{
    if ( mIsSwitchedToEraser )
    {
        mIsSwitchedToEraser = false;
        if ( meTabletBackupTool == INVALID_TOOL )
        {
            meTabletBackupTool = PENCIL;
        }
        setCurrentTool( meTabletBackupTool );
    }
}
