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

    BaseTool* currentTool() { return m_pCurrentTool; }
    BaseTool* getTool( ToolType eToolType );
    void      setCurrentTool( ToolType eToolType );
    void      cleanupAllToolsData();

    void      tabletSwitchToEraser();
    void      tabletRestorePrevTool();

signals:
    void penWidthValueChange( float );
    void penFeatherValueChange( float );
    void penInvisiblityValueChange( int );
    void penPreserveAlphaValueChange( int );
    void penPressureValueChange( int );

    void toolChanged( ToolType );
    void toolPropertyChanged();

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
