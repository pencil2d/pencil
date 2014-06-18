#ifndef POLYLINETOOL_H
#define POLYLINETOOL_H

#include <QPointF>

#include "basetool.h"

class PolylineTool : public BaseTool
{
    Q_OBJECT
public:
    explicit PolylineTool( QObject *parent = 0 );
    ToolType type();
    void loadSettings();
    QCursor cursor();

    void mousePressEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;
    void mouseDoubleClickEvent( QMouseEvent* ) override;
    bool keyPressEvent( QKeyEvent* event );

    void clear();

private:
    QList<QPointF> points;
};

#endif // POLYLINETOOL_H
