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
    Status onObjectLoaded( Object* ) override;

    BaseTool* currentTool() { return m_pCurrentTool; }
    BaseTool* getTool( ToolType eToolType );
    void      setDefaultTool();
    void      setCurrentTool( ToolType eToolType );
    void      cleanupAllToolsData();

    void      tabletSwitchToEraser();
    void      tabletRestorePrevTool();

Q_SIGNALS:
    void penWidthValueChanged( float );
    void penFeatherValueChanged( float );

    void toolChanged( ToolType );
    void toolPropertyChanged( ToolType, ToolPropertyType );

public slots:
    void resetAllTools();

    void setWidth( float );
    void setFeather( float );
    void setInvisibility( int );
    void setPreserveAlpha( int );
    void setPressure( int );

private:
    BaseTool* m_pCurrentTool;
    ToolType  m_eTabletBackupTool;
    bool isSwitchedToEraser;
    QHash<ToolType, BaseTool*> m_toolSetHash;

};

#endif // TOOLMANAGER_H
