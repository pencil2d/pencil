#ifndef BRUSHTOOL_H
#define BRUSHTOOL_H

#include "stroketool.h"

class BrushTool : public StrokeTool
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

    void adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice);

    void drawStroke();

signals:
    
public slots:

protected:
    QPointF lastBrushPoint;
    
};

#endif // BRUSHTOOL_H
