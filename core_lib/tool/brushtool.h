#ifndef BRUSHTOOL_H
#define BRUSHTOOL_H

#include "stroketool.h"

class BrushTool : public StrokeTool
{
    Q_OBJECT
public:
    explicit BrushTool( QObject *parent = 0 );
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

#endif // BRUSHTOOL_H
