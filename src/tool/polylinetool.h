#ifndef POLYLINETOOL_H
#define POLYLINETOOL_H

#include "basetool.h"


class PolylineTool : public BaseTool
{
    Q_OBJECT
public:
    explicit PolylineTool(QObject *parent = 0);
    ToolType type();
    void loadSettings();
    QCursor cursor();

    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

signals:
    
public slots:
    
};

#endif // POLYLINETOOL_H
