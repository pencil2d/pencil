#ifndef SELECTTOOL_H
#define SELECTTOOL_H

#include "basetool.h"


class SelectTool : public BaseTool
{
    Q_OBJECT
public:
    SelectTool();
    ToolType type();
    void loadSettings();
};

#endif