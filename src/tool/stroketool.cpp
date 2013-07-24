#include "stroketool.h"

#include "scribblearea.h"
#include "strokemanager.h"
#include "editor.h"

StrokeTool::StrokeTool(QObject *parent) :
    BaseTool(parent)
{
}

void StrokeTool::startStroke()
{
    m_firstDraw = true;
    lastPixel = getCurrentPixel();
    strokePoints.clear();
    strokePoints << m_pScribbleArea->pixelToPoint(lastPixel);
    strokePressures.clear();
    strokePressures << m_pStrokeManager->getPressure();
}

void StrokeTool::endStroke()
{
    strokePoints.clear();
    strokePressures.clear();
}

void StrokeTool::drawStroke()
{

    QPointF pixel = getCurrentPixel();
    if (pixel != lastPixel || !m_firstDraw)
    {
        lastPixel = pixel;
        strokePoints << m_pScribbleArea->pixelToPoint(pixel);
        strokePressures << m_pStrokeManager->getPressure();
    }
    else
    {
        m_firstDraw = false;
    }
}
