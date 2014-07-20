#ifndef HANDTOOL_H
#define HANDTOOL_H

#include "basetool.h"


class HandTool : public BaseTool
{
    Q_OBJECT
public:
    HandTool();
    ToolType type() override { return HAND; }
    void loadSettings() override;
    QCursor cursor() override;

    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseDoubleClickEvent(QMouseEvent *) override;
};

#endif
