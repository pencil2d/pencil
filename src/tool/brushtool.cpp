#include <QSettings>
#include <QPixmap>
#include <QPainter>

#include "pencilsettings.h"
#include "layer.h"
#include "colormanager.h"
#include "strokemanager.h"
#include "editor.h"
#include "scribblearea.h"
#include "blitrect.h"

#include "brushtool.h"

BrushTool::BrushTool(QObject *parent) :
    StrokeTool(parent)
{
}

ToolType BrushTool::type()
{
    return BRUSH;
}

void BrushTool::loadSettings()
{
    QSettings settings("Pencil", "Pencil");

    properties.width = settings.value("brushWidth").toDouble();
    properties.feather = settings.value("brushFeather").toDouble();
    properties.opacity = 0.5;

    properties.pressure = ON;
    properties.invisibility = DISABLED;
    properties.preserveAlpha = OFF;

    if (properties.feather == 0)
    {
        properties.feather = 70;
        settings.setValue("brushFeather", properties.feather);
    }
    if (properties.width == 0)
    {
        properties.width = 48;
        settings.setValue("brushWidth", properties.width);
    }
}

QCursor BrushTool::cursor()
{
    if (isAdjusting) // being dynamically resized
    {
        return circleCursors(); // two circles cursor
    }
    if ( pencilSettings()->value( SETTING_TOOL_CURSOR ).toBool() ) // doesn't need else
    {
        return QCursor(QPixmap(":icons/brush.png"), 0, 13);
    }
    return Qt::CrossCursor;
}

void BrushTool::adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice)
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

void BrushTool::mousePressEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();

    if (event->button() == Qt::LeftButton)
    {
        m_pEditor->backup(typeName());
        m_pScribbleArea->setAllDirty();
    }

    startStroke();
    lastBrushPoint = getCurrentPoint();
}

void BrushTool::mouseReleaseEvent(QMouseEvent *event)
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
        }
    }

    endStroke();
}

void BrushTool::mouseMoveEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();

    if (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)
    {
        if (event->buttons() & Qt::LeftButton)
        {
            drawStroke();
        }
    }
}

// draw a single paint dab at the given location
void BrushTool::paintAt(QPointF point)
{
}

void BrushTool::drawStroke()
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
        m_pScribbleArea->setGaussianGradient(radialGrad,
            m_pEditor->colorManager()->frontColor(),
            opacity,
            offset);

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
            m_pScribbleArea->drawBrush( point,
                brushWidth,
                offset,
                m_pEditor->colorManager()->frontColor(),
                opacity);

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
        QPen pen(Qt::gray, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
        int rad = qRound((currentWidth / 2 + 2) * qAbs(m_pScribbleArea->getTempViewScaleX()));

        //        foreach (QSegment segment, calculateStroke(currentWidth))
        //        {
        //            QPointF a = segment.first;
        //            QPointF b = segment.second;
        //            m_pScribbleArea->drawLine(a, b, pen, QPainter::CompositionMode_SourceOver);
        //            m_pScribbleArea->refreshVector(QRect(a.toPoint(), b.toPoint()), rad);
        //        }
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