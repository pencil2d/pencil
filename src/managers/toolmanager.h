#ifndef TOOLMANAGER_H
#define TOOLMANAGER_H

#include <QObject>
#include <QHash>
#include "basetool.h"

class ScribbleArea;
class Editor;

class ToolManager : public QObject
{
    Q_OBJECT
public:
    explicit ToolManager(QObject* parent, Editor* pEditor, ScribbleArea* pScribbleArea);
    
    BaseTool* currentTool();
    BaseTool* getTool( ToolType eToolType );
    void      setCurrentTool( ToolType eToolType );
    void      cleanupAllToolsData();

    void      resetAllToolsData();
signals:
    
public slots:
    
private:
    BaseTool* m_pCurrentTool;
    QHash<ToolType, BaseTool*> m_toolSetHash;

};

#endif // TOOLMANAGER_H
