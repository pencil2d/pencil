#include <QSettings>
#include <QPixmap>
#include <QPainter>

#include "layer.h"
#include "scribblearea.h"

#include "pencilsettings.h"
#include "editor.h"
#include "strokemanager.h"

#include "erasertool.h"
#include "blitrect.h"
#include "layervector.h"

EraserTool::EraserTool(QObject *parent) :
    StrokeTool(parent)
{
}

ToolType EraserTool::type()
{
    return ERASER;
}

void EraserTool::loadSettings()
{
    QSettings settings("Pencil", "Pencil");

    properties.width = settings.value("eraserWidth").toDouble();
    properties.feather = settings.value("eraserFeather").toDouble();
    properties.opacity = 0.5;

    properties.pressure = ON;
    properties.invisibility = DISABLED;
    properties.preserveAlpha = OFF;

    if (properties.feather == 0)
    {
        properties.feather = 70;
        settings.setValue("eraserFeather", properties.feather);
    }
    if (properties.width == 0)
    {
        properties.width = 48;
        settings.setValue("eraserWidth", properties.width);
    }
}

QCursor EraserTool::cursor()
{
    if (isAdjusting) // being dynamically resized
    {
        return circleCursors(); // two circles cursor
    }
    if ( pencilSettings()->value( SETTING_TOOL_CURSOR ).toBool() )
    {
        return circleCursors();
    }
    return Qt::CrossCursor;
}

void EraserTool::adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice)
{
    currentWidth = properties.width;
    if (m_pScribbleArea->usePressure() && !mouseDevice)
    {
        currentPressure = pressure;
    }
    else
    {
        currentPressure = 1.0;
    }
}

void EraserTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_pEditor->backup(typeName());
        m_pScribbleArea->setAllDirty();
    }

    startStroke();
    lastBrushPoint = getCurrentPoint();
}

void EraserTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();

    if (event->button() == Qt::LeftButton)
    {
        if (m_pScribbleArea->isLayerPaintable())
        {
            drawStroke();
        }

        if (layer->type == Layer::BITMAP)
        {
            m_pScribbleArea->paintBitmapBuffer();
            m_pScribbleArea->setAllDirty();
        }
        else if (layer->type == Layer::VECTOR)
        {
            VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);
            // Clear the area containing the last point
            //vectorImage->removeArea(lastPoint);
            // Clear the temporary pixel path
            m_pScribbleArea->clearBitmapBuffer();
            vectorImage->deleteSelectedPoints();
            //update();
            m_pScribbleArea->setModified(m_pEditor->m_nCurrentLayerIndex, m_pEditor->m_nCurrentFrameIndex);
            m_pScribbleArea->setAllDirty();
        }
    }

    endStroke();
}

void EraserTool::mouseMoveEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();

    if (event->buttons() & Qt::LeftButton)
    {
        if (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)
        {
            drawStroke();
        }
        if (layer->type == Layer::VECTOR)
        {
            qreal radius = (properties.width / 2) / m_pScribbleArea->getTempViewScaleX();
            QList<VertexRef> nearbyVertices = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0)
                ->getVerticesCloseTo(getCurrentPoint(), radius);
            for (int i = 0; i < nearbyVertices.size(); i++)
            {
                ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0)->setSelected(nearbyVertices.at(i), true);
            }
            //update();
            m_pScribbleArea->setAllDirty();
        }
    }
}

// draw a single paint dab at the given location
void EraserTool::paintAt(QPointF point)
{
}

void EraserTool::drawStroke()
{
    StrokeTool::drawStroke();
    QList<QPointF> p = m_pStrokeManager->interpolateStroke(currentWidth);

    Layer *layer = m_pEditor->getCurrentLayer();

    if (layer->type == Layer::BITMAP)
    {
        for (int i = 0; i < p.size(); i++) {
            p[i] = m_pScribbleArea->pixelToPoint(p[i]);
        }

        qreal opacity = 1.0;
        qreal brushWidth = currentWidth +  0.5 * properties.feather;
        qreal offset = qMax(0.0, currentWidth - 0.5 * properties.feather) / brushWidth;
        opacity = currentPressure;
        brushWidth = brushWidth * currentPressure;

        //        if (tabletInUse) { opacity = tabletPressure; }
        //        if (usePressure) { brushWidth = brushWidth * tabletPressure; }

        qreal brushStep = 0.5 * currentWidth + 0.5 * properties.feather;
        brushStep = brushStep * currentPressure;

        //        if (usePressure) { brushStep = brushStep * tabletPressure; }
        brushStep = qMax(1.0, brushStep);

        currentWidth = properties.width;
        BlitRect rect;

        QRadialGradient radialGrad(QPointF(0,0), 0.5 * brushWidth);
        m_pScribbleArea->setGaussianGradient(radialGrad, QColor(255,255,255), opacity, offset);

        QPointF a = lastBrushPoint;
        QPointF b = getCurrentPoint();

        //        foreach (QSegment segment, calculateStroke(brushWidth))
        //        {
        //            QPointF a = lastBrushPoint;
        //            QPointF b = m_pScribbleArea->pixelToPoint(segment.second);

        qreal distance = 4 * QLineF(b, a).length();
        int steps = qRound(distance) / brushStep;

        for (int i = 0; i < steps; i++)
        {
            QPointF point = lastBrushPoint + (i + 1) * (brushStep) * (b - lastBrushPoint) / distance;
            rect.extend(point.toPoint());
            m_pScribbleArea->drawBrush(point, brushWidth, offset, QColor(255,255,255), opacity);

            if (i == (steps - 1))
            {
                lastBrushPoint = point;
            }
        }
        //        }

        int rad = qRound(brushWidth) / 2 + 2;
        m_pScribbleArea->refreshBitmap(rect, rad);
    }
    else if (layer->type == Layer::VECTOR)
    {
        QPen pen(Qt::white, currentWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        int rad = qRound((currentWidth / 2 + 2) * (qAbs(m_pScribbleArea->getTempViewScaleX()) + qAbs(m_pScribbleArea->getTempViewScaleY())));

        if (p.size() == 4) {
            QSizeF size(2,2);
            QPainterPath path(p[0]);
            path.cubicTo(p[1],
                p[2],
                p[3]);
            m_pScribbleArea->drawPath(path, pen, Qt::NoBrush, QPainter::CompositionMode_Source);
            m_pScribbleArea->refreshVector(path.boundingRect().toRect(), rad);
        }
    }
}