#include <QSettings>
#include <QPixmap>
#include <QMouseEvent>

#include "editor.h"
#include "scribblearea.h"

#include "pencilsettings.h"
#include "penciltool.h"

#include "strokemanager.h"

#include "layer.h"

PencilTool::PencilTool(QObject *parent) :
    StrokeTool(parent)
{
}

ToolType PencilTool::type()
{
    return PENCIL;
}

void PencilTool::loadSettings()
{
    QSettings settings("pencil", "pencil");

    properties.width = settings.value("pencilWidth").toDouble();    
    properties.colourNumber = 0;
    properties.feather = -1;
    properties.opacity = 0.8;
    properties.pressure = 1;
    properties.invisibility = 1;
    properties.preserveAlpha = 0;

    if (properties.width == 0)
    {
        properties.width = 1;
        settings.setValue("pencilWidth", properties.width);
    }
}

QCursor PencilTool::cursor()
{
    if ( pencilSettings()->value( kSettingToolCursor ).toBool() )
    {
        return QCursor(QPixmap(":icons/pencil2.png"), 0, 16);
    }
    else
    {
        return Qt::CrossCursor;
    }
}

void PencilTool::mousePressEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();

    if (layer->type == Layer::VECTOR)
    {
        m_pEditor->selectVectorColourNumber(properties.colourNumber);
    }

    if (event->button() == Qt::LeftButton)
    {
        m_pEditor->backup(typeName());

        if (!m_pScribbleArea->showThinLines())
        {
            m_pScribbleArea->toggleThinLines();
        }
        m_pScribbleArea->setAllDirty();
    }

    startStroke();
}

void PencilTool::mouseMoveEvent(QMouseEvent *event)
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

void PencilTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();

    if (event->button() == Qt::LeftButton)
    {
        if (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)
        {
            drawStroke();
        }

        if (layer->type == Layer::BITMAP)
        {
            m_pScribbleArea->paintBitmapBuffer();
            m_pScribbleArea->setAllDirty();
        }
        else if (layer->type == Layer::VECTOR &&  strokePoints.size() > -1)
        {
            // Clear the temporary pixel path
            m_pScribbleArea->clearBitmapBuffer();
            qreal tol = m_pScribbleArea->getCurveSmoothing() / qAbs(m_pScribbleArea->getViewScaleX());
            BezierCurve curve(strokePoints, strokePressures, tol);
            curve.setWidth(0);
            curve.setFeather(0);
            curve.setInvisibility(true);
            curve.setVariableWidth(false);
            curve.setColourNumber(properties.colourNumber);
            VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);

            vectorImage->addCurve(curve, qAbs(m_pScribbleArea->getViewScaleX()));
            m_pScribbleArea->setModified(m_pEditor->m_nCurrentLayerIndex, m_pEditor->m_nCurrentFrameIndex);
            m_pScribbleArea->setAllDirty();
        }
    }

    endStroke();
}

void PencilTool::adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice)
{
    if (m_pScribbleArea->usePressure() && !mouseDevice)
    {
        currentPressuredColor.setAlphaF(m_pEditor->currentColor.alphaF() * pressure);
    }
    else
    {
        currentPressuredColor.setAlphaF(m_pEditor->currentColor.alphaF());
    }

    currentWidth = properties.width;
}

void PencilTool::drawStroke()
{
    Layer *layer = m_pEditor->getCurrentLayer();
    float width = 1;
    int rad;

    if (layer->type == Layer::BITMAP)
    {
        QPen pen(QBrush(m_pEditor->currentColor), properties.width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        width = properties.width;
        rad = qRound(properties.width / 2) + 3;

        QList<QPointF> p = m_pStrokeManager->interpolateStroke(width);
        for (int i = 0; i < p.size(); i++) {
            p[i] = m_pScribbleArea->pixelToPoint(p[i]);
        }

        if (p.size() == 2) {
            qDebug() << "draw line";
            m_pScribbleArea->drawLine(p[0], p[1], pen, QPainter::CompositionMode_Source);
            m_pScribbleArea->refreshBitmap(QRectF(p[0], p[1]).toRect(), rad);
        }

        if (p.size() == 4) {

            qDebug() << "curve" << p[0] << p[3] << p[1] << p[2];
            QSizeF size(2,2);
            QPainterPath path(p[0]);
            path.cubicTo(p[1],
                    p[2],
                    p[3]);
            m_pScribbleArea->drawPath(path, pen, Qt::NoBrush, QPainter::CompositionMode_Source);
            m_pScribbleArea->refreshBitmap(path.boundingRect().toRect(), rad);

            if (false) {
                QRectF rect(p[0], size);

                QPen penBlue(Qt::blue);

                m_pScribbleArea->bufferImg->drawRect(rect, Qt::NoPen, QBrush(Qt::red), QPainter::CompositionMode_Source, false);
                m_pScribbleArea->bufferImg->drawRect(QRectF(p[3], size), Qt::NoPen, QBrush(Qt::red), QPainter::CompositionMode_Source, false);
                m_pScribbleArea->bufferImg->drawRect(QRectF(p[1], size), Qt::NoPen, QBrush(Qt::green), QPainter::CompositionMode_Source, false);
                m_pScribbleArea->bufferImg->drawRect(QRectF(p[2], size), Qt::NoPen, QBrush(Qt::green), QPainter::CompositionMode_Source, false);
                m_pScribbleArea->bufferImg->drawLine(p[0], p[1], penBlue, QPainter::CompositionMode_Source, true);
                m_pScribbleArea->bufferImg->drawLine(p[2], p[3], penBlue, QPainter::CompositionMode_Source, true);
                m_pScribbleArea->refreshBitmap(QRectF(p[0], p[3]).toRect(), 20);
                m_pScribbleArea->refreshBitmap(rect.toRect(), rad);
            }

        }
//        foreach (QSegment segment, calculateStroke(width))
//        {
//            QPointF a = m_pScribbleArea->pixelToPoint(segment.first);
//            QPointF b = m_pScribbleArea->pixelToPoint(segment.second);
//            m_pScribbleArea->drawLine(a, b, pen, QPainter::CompositionMode_Source);
//            m_pScribbleArea->refreshBitmap(QRect(a.toPoint(), b.toPoint()), rad);
//        }
    }
    else if (layer->type == Layer::VECTOR)
    {
        QPen pen(m_pEditor->currentColor, 1, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
        rad = qRound((properties.width / 2 + 2) * qAbs(m_pScribbleArea->getTempViewScaleX()));
        foreach (QSegment segment, calculateStroke(width))
        {
            QPointF a = segment.first;
            QPointF b = segment.second;
            m_pScribbleArea->drawLine(a, b,
                                      pen,
                                      QPainter::CompositionMode_SourceOver);
            m_pScribbleArea->refreshVector(QRect(a.toPoint(), b.toPoint()), rad);
        }
    }
}

