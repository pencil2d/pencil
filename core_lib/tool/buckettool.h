#ifndef BUCKETTOOL_H
#define BUCKETTOOL_H

#include "basetool.h"


class BucketTool : public BaseTool
{
    Q_OBJECT
public:
    explicit BucketTool( QObject *parent = 0 );
    ToolType type() override;
    void loadSettings() override;
    QCursor cursor() override;

    void mousePressEvent( QMouseEvent * ) override;
    void mouseMoveEvent( QMouseEvent * ) override;
    void mouseReleaseEvent( QMouseEvent * ) override;

signals:

    public slots :
};

#endif // BUCKETTOOL_H
