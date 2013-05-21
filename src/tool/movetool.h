#ifndef MOVETOOL_H
#define MOVETOOL_H

#include "basetool.h"


class MoveTool : public BaseTool
{
    Q_OBJECT
public:
    MoveTool();
    ToolType type();
    void loadSettings();
};

#endif