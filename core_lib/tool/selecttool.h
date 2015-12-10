#ifndef SELECTTOOL_H
#define SELECTTOOL_H

#include "basetool.h"


class SelectTool : public BaseTool
{
    Q_OBJECT
public:
    SelectTool();
    ToolType type() override;
    void loadSettings() override;
    QCursor cursor() override;

    void mousePressEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;

    bool keyPressEvent(QKeyEvent *event) override;
    bool keyReleaseEvent(QKeyEvent *event) override;
};

#endif
