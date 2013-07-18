#ifndef HANDTOOL_H
#define HANDTOOL_H

#include "basetool.h"


class HandTool : public BaseTool
{
    Q_OBJECT
public:
    HandTool();
    ToolType type();
    void loadSettings();
    QCursor cursor();

    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
};

#endif
