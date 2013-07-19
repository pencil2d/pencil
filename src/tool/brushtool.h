#ifndef BRUSHTOOL_H
#define BRUSHTOOL_H

#include "basetool.h"


class BrushTool : public BaseTool
{
    Q_OBJECT
public:
    explicit BrushTool(QObject *parent = 0);
    ToolType type();
    void loadSettings();
    QCursor cursor();

    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

signals:
    
public slots:
    
};

#endif // BRUSHTOOL_H
