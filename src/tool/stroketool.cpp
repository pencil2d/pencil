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

QList<QPair<QPointF, QPointF> > StrokeTool::calculateStroke(float width)
{
    QList<QPointF> pixels = m_pStrokeManager->interpolateStroke(width);
    QList<QPair<QPointF, QPointF> > segments;

    foreach (QPointF pixel, pixels) {
        if (pixel != lastPixel || !m_firstDraw)
        {
            m_firstDraw = false;

            segments << QPair<QPointF, QPointF>(lastPixel, pixel);

            lastPixel = pixel;
            strokePoints << m_pScribbleArea->pixelToPoint(pixel);
            strokePressures << m_pStrokeManager->getPressure();
        }
    }

    return segments;

}
