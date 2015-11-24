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

    void adjustPressureSensitiveProperties( qreal pressure, bool mouseDevice );

    void setWidth( const qreal width );
    void setFeather( const qreal feather );
    void setPressure( const bool pressure );
};

#endif // PENTOOL_H
