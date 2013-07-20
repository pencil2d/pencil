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

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void drawStroke();

    void adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice);

signals:
    
public slots:
    
};

#endif // ERASERTOOL_H
