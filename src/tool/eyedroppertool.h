#ifndef EYEDROPPERTOOL_H
#define EYEDROPPERTOOL_H

#include "basetool.h"


class EyedropperTool : public BaseTool
{
    Q_OBJECT
public:
    explicit EyedropperTool(QObject *parent = 0);
    ToolType type();
    void loadSettings();
signals:
    
public slots:
    
};

#endif // EYEDROPPERTOOL_H
