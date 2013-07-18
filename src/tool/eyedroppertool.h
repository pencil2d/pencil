#ifndef EYEDROPPERTOOL_H
#define EYEDROPPERTOOL_H

#include "basetool.h"


class EyedropperTool : public BaseTool
{
    Q_OBJECT
public:
    explicit EyedropperTool(QObject *parent = 0);
    ToolType type();
    void loadSettings();
    QCursor cursor();

    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

signals:
    
public slots:
    
};

#endif // EYEDROPPERTOOL_H
