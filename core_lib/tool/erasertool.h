#ifndef ERASERTOOL_H
#define ERASERTOOL_H

#include "stroketool.h"

class EraserTool : public StrokeTool
{
    Q_OBJECT
public:
    explicit EraserTool( QObject *parent = 0 );
    ToolType type() override;
    void loadSettings() override;
    QCursor cursor() override;

    void mouseMoveEvent( QMouseEvent* ) override;
    void mousePressEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;

    void adjustPressureSensitiveProperties( qreal pressure, bool mouseDevice );

    void drawStroke();

    void paintAt( QPointF point );

protected:
    QPointF lastBrushPoint;
};

#endif // ERASERTOOL_H
