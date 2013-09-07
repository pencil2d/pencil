#ifndef SMUDGETOOL_H
#define SMUDGETOOL_H

#include "stroketool.h"

class SmudgeTool : public StrokeTool
{
    Q_OBJECT
public:
    explicit SmudgeTool(QObject *parent = 0);
    ToolType type();
    void loadSettings();
    QCursor cursor();

    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

    void adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice);
    void drawStroke();

signals:
    
public slots:

protected:
    QPointF lastBrushPoint;
};

#endif // SMUDGETOOL_H
