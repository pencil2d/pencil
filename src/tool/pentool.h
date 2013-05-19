#ifndef PENTOOL_H
#define PENTOOL_H

#include "basetool.h"



class PenTool : public BaseTool
{
public:
    PenTool();
    ToolType type();
    void loadSettings();
};

#endif // PENTOOL_H
