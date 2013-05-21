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
signals:
    
public slots:
    
};

#endif // BUCKETTOOL_H
