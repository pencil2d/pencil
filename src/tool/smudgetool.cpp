#include <QPixmap>
#include "editor.h"
#include "layer.h"
#include "scribblearea.h"

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
    qDebug() << "smudge tool";
    return QCursor(QPixmap(":icons/smudge.png"),3 ,16);
}

void SmudgeTool::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    qDebug() << "smudge event";
}

void SmudgeTool::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void SmudgeTool::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}
