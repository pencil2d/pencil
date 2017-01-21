#ifndef BUCKETTOOL_H
#define BUCKETTOOL_H

#include "stroketool.h"


class BucketTool : public StrokeTool
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

    void drawStroke();
};

#endif // BUCKETTOOL_H
