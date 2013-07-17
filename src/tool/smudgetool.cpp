#include <QPixmap>
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
    properties.width = -1;
    properties.feather = -1;
}

QCursor SmudgeTool::cursor()
{
    return QCursor(QPixmap(":icons/smudge.png"),3 ,16);
}
