#ifndef ERASERTOOL_H
#define ERASERTOOL_H

#include "stroketool.h"

class EraserTool : public StrokeTool
{
    Q_OBJECT
public:
    explicit EraserTool(QObject *parent = 0);
    ToolType type();
    void loadSettings();
    QCursor cursor();

    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

    void adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice);

    void drawStroke();

    void paintAt(QPointF point);

protected:
    QPointF lastBrushPoint;

};

#endif // ERASERTOOL_H

