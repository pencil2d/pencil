#ifndef POLYLINETOOL_H
#define POLYLINETOOL_H

#include "basetool.h"


class PolylineTool : public BaseTool
{
    Q_OBJECT
public:
    explicit PolylineTool(QObject *parent = 0);
    ToolType type();
    void loadSettings();
signals:
    
public slots:
    
};

#endif // POLYLINETOOL_H
