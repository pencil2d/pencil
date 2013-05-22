#ifndef ERASERTOOL_H
#define ERASERTOOL_H

#include "basetool.h"


class EraserTool : public BaseTool
{
    Q_OBJECT
public:
    explicit EraserTool(QObject *parent = 0);
    ToolType type();
    void loadSettings();
    QCursor cursor();
signals:
    
public slots:
    
};

#endif // ERASERTOOL_H
