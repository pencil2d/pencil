#ifndef EYEDROPPERTOOL_H
#define EYEDROPPERTOOL_H

#include "basetool.h"


class EyedropperTool : public BaseTool
{
    Q_OBJECT
public:
    explicit EyedropperTool( QObject* parent = 0 );
    ToolType type() override { return EYEDROPPER; }
    void loadSettings() override;
    QCursor cursor();
    QCursor cursor( const QColor colour );

    void mousePressEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;
};

#endif // EYEDROPPERTOOL_H
