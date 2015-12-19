#ifndef MOVETOOL_H
#define MOVETOOL_H

#include "basetool.h"


class MoveTool : public BaseTool
{
    Q_OBJECT
public:
    MoveTool();
    ToolType type() override;
    void loadSettings() override;
    QCursor cursor() override;

    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;

    bool keyPressEvent(QKeyEvent *event) override;

private:
    void cancelChanges();
    void applyChanges();
};

#endif
