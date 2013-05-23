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

}

QCursor SmudgeTool::cursor()
{
    return QCursor(QPixmap(":icons/smudge.png"),3 ,16);
}