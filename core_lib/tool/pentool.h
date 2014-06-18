#ifndef PENTOOL_H
#define PENTOOL_H

#include "stroketool.h"

class PenTool : public StrokeTool
{
    Q_OBJECT
public:
    PenTool( QObject *parent = 0 );
    ToolType type() override;
    void loadSettings() override;
    QCursor cursor() override;

    void mousePressEvent( QMouseEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;

    void drawStroke();

    void adjustPressureSensitiveProperties( qreal pressure, bool mouseDevice );
};

#endif // PENTOOL_H
