#ifndef PENTOOL_H
#define PENTOOL_H

#include "stroketool.h"

class PenTool : public StrokeTool
{
    Q_OBJECT
public:
    PenTool(QObject *parent = 0);
    ToolType type();
    void loadSettings();
    QCursor cursor();

    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

    void drawStroke();

    void adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice);

};

#endif // PENTOOL_H
