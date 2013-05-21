#include "smudgetool.h"

SmudgeTool::SmudgeTool(QObject *parent) :
    BaseTool(parent)
{
}

ToolType SmudgeTool::type()
{
    return SMUDGE;
}

void SmudgeTool::loadSettings()
{

}
