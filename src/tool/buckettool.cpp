#include "buckettool.h"

BucketTool::BucketTool(QObject *parent) :
    BaseTool(parent)
{
}


ToolType BucketTool::type()
{
    return BUCKET;
}
