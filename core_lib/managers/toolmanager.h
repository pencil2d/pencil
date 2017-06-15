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
    explicit ToolManager( QObject* parent );
    
    bool init() override;
    Status load( Object* ) override;
	Status save( Object* ) override;

    BaseTool* currentTool() { return mCurrentTool; }
    BaseTool* getTool( ToolType eToolType );
    void      setDefaultTool();
    void      setCurrentTool( ToolType eToolType );
    void      cleanupAllToolsData();

    void      tabletSwitchToEraser();
    void      tabletRestorePrevTool();
    int propertySwitch( bool condition, int property );

Q_SIGNALS:
    void penWidthValueChanged( float );
    void penFeatherValueChanged( float );
    void toleranceValueChanged( qreal );

    void toolChanged( ToolType );
    void toolPropertyChanged( ToolType, ToolPropertyType );

public slots:
    void resetAllTools();

    void noInpolSelected() { setInpolLevel( 0 ); }
    void SimplepolSelected() { setInpolLevel( 1 ); }
    void StrongpolSelected() { setInpolLevel( 2 ); }
    void ExtremepolSelected() { setInpolLevel( 3 ); }

    void setWidth( float );
    void setFeather( float );
    void setUseFeather( bool );
    void setInvisibility( bool );
    void setPreserveAlpha( bool );
    void setVectorMergeEnabled( bool );
    void setBezier( bool );
    void setPressure( bool );
    void setAA( int );
    void setInpolLevel ( int );
    void setTolerance ( int );

private:
    BaseTool* mCurrentTool       = nullptr;
    ToolType  meTabletBackupTool = PENCIL;
    bool mIsSwitchedToEraser     = false;
    QHash<ToolType, BaseTool*> mToolSetHash;

    int oldValue = 0;

};

#endif // TOOLMANAGER_H
