#ifndef PENTOOL_H
#define PENTOOL_H

#include "basetool.h"



class PenTool : public BaseTool
{
    Q_OBJECT
public:
    PenTool();
    ToolType type();
    void loadSettings();
    QCursor cursor();
};

#endif // PENTOOL_H
