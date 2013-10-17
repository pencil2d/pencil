#ifndef TOOLMANAGER_H
#define TOOLMANAGER_H

#include <QObject>

class BaseTool;

class ToolManager : public QObject
{
    Q_OBJECT
public:
    explicit ToolManager(QObject *parent = 0);
    
    BaseTool* currentTool();
signals:
    
public slots:
    
private:
    BaseTool* m_pCurrentTool;
};

#endif // TOOLMANAGER_H
