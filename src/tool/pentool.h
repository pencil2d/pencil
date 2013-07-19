#ifndef PENTOOL_H
#define PENTOOL_H

#include "basetool.h"



class PenTool : public BaseTool
{
    Q_OBJECT
public:
    PenTool();
    ToolType type();
    void loadSettings();
    QCursor cursor();

    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

    void adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice);
};

#endif // PENTOOL_H
