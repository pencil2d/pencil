#ifndef EDITTOOL_H
#define EDITTOOL_H

#include "basetool.h"


class EditTool : public BaseTool
{
    Q_OBJECT
public:
    EditTool();
    ToolType type();
    void loadSettings();

    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
};

#endif
