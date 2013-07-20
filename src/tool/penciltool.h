#ifndef PENCILTOOL_H
#define PENCILTOOL_H

#include "stroketool.h"

class PencilTool : public StrokeTool
{
    Q_OBJECT
public:
    explicit PencilTool(QObject *parent = 0);
    ToolType type();
    void loadSettings();
    QCursor cursor();

    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

    void drawStroke();

    void adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice);

signals:
    
public slots:

    
};

#endif // PENCILTOOL_H
