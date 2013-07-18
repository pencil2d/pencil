#ifndef PENCILTOOL_H
#define PENCILTOOL_H

#include "basetool.h"

class PencilTool : public BaseTool
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

signals:
    
public slots:
    
};

#endif // PENCILTOOL_H
