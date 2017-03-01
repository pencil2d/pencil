#ifndef PENTOOL_H
#define PENTOOL_H

#include "stroketool.h"

class PenTool : public StrokeTool
{
    Q_OBJECT
public:
    PenTool( QObject *parent = 0 );
    ToolType type() override { return PEN; }
    void loadSettings() override;
    QCursor cursor() override;

    void mousePressEvent( QMouseEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;

    void drawStroke();
    void paintAt( QPointF point );
    void paintVectorStroke();

    void adjustPressureSensitiveProperties( qreal pressure, bool mouseDevice ) override;

    void setWidth( const qreal width ) override;
    void setPressure( const bool pressure ) override;
    void setAA( const int AA ) override;
    void setInpolLevel(const int level) override;

private:
    QPointF mLastBrushPoint;
    QPointF mMouseDownPoint;
};

#endif // PENTOOL_H
