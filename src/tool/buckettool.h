#ifndef BUCKETTOOL_H
#define BUCKETTOOL_H

#include "basetool.h"


class BucketTool : public BaseTool
{
    Q_OBJECT
public:
    explicit BucketTool(QObject *parent = 0);
    ToolType type();
    void loadSettings();
    QCursor cursor();

    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

signals:
    
public slots:
    
};

#endif // BUCKETTOOL_H
