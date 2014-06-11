#ifndef EYEDROPPERTOOL_H
#define EYEDROPPERTOOL_H

#include "basetool.h"


class EyedropperTool : public BaseTool
{
    Q_OBJECT
public:
    explicit EyedropperTool(QObject *parent = 0);
    ToolType type() { return EYEDROPPER; }
    void loadSettings();
    QCursor cursor();
    QCursor cursor(const QColor colour);

    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);    
};

#endif // EYEDROPPERTOOL_H
