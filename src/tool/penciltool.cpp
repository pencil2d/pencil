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
    BaseTool(parent)
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

        if (!m_pScribbleArea->showThinLines)
        {
            m_pScribbleArea->toggleThinLines();
        }
        m_pScribbleArea->mousePath.append(m_pScribbleArea->lastPoint);
        m_pScribbleArea->updateAll = true;

        m_firstDraw = true;
        lastPixel = event->pos();
    }
}

void PencilTool::drawStrokes()
{
    Layer *layer = m_pEditor->getCurrentLayer();
    QPen pen;
    float width = 1;
    int rad;

    if (layer->type == Layer::BITMAP)
    {
        pen = QPen(QBrush(m_pEditor->currentColor), properties.width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        width = properties.width;
        rad = qRound(properties.width / 2) + 3;
    }
    else if (layer->type == Layer::VECTOR)
    {
        pen = QPen(m_pEditor->currentColor, 1, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
        rad = qRound((properties.width / 2 + 2) * qAbs(m_pScribbleArea->myTempView.m11()));
    }

    QList<QPoint> pixels = m_pScribbleArea->getStrokeManager()->applyStroke(width);
    QPointF lastPoint = m_pScribbleArea->pixelToPoint(lastPixel);

    foreach (QPoint pixel, pixels) {
        if (pixel != lastPixel || !m_firstDraw)
        {
            m_firstDraw = false;

            if (layer->type == Layer::BITMAP)
            {
                QPointF currentPoint = m_pScribbleArea->pixelToPoint(pixel);
                m_pScribbleArea->bufferImg->drawLine(lastPoint, currentPoint, pen,
                                                     QPainter::CompositionMode_Source, m_pScribbleArea->antialiasing);
                m_pScribbleArea->update(m_pScribbleArea->myTempView.
                                        mapRect(QRect(lastPoint.toPoint(),
                                                      currentPoint.toPoint())
                                                .normalized().adjusted(-rad, -rad, +rad, +rad)));
                lastPoint = currentPoint;
            }
            else if (layer->type == Layer::VECTOR)
            {
                m_pScribbleArea->bufferImg->drawLine(lastPixel, pixel,
                                                     pen,
                                                     QPainter::CompositionMode_SourceOver,
                                                     m_pScribbleArea->antialiasing);
                m_pScribbleArea->update(QRect(lastPixel.toPoint(), pixel).normalized().adjusted(-rad, -rad, +rad, +rad));

            }

            lastPixel = pixel;
        }
    }

}

void PencilTool::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        drawStrokes();
    }
}

void PencilTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();

    if (event->button() == Qt::LeftButton)
    {
        drawStrokes();

        if (layer->type == Layer::BITMAP)
        {
            m_pScribbleArea->paintBitmapBuffer();
            m_pScribbleArea->updateAll = true;
        }
        else if (layer->type == Layer::VECTOR &&  m_pScribbleArea->mousePath.size() > -1)
        {
            // Clear the temporary pixel path
            m_pScribbleArea->bufferImg->clear();
            qreal tol = m_pScribbleArea->curveSmoothing / qAbs(m_pScribbleArea->myView.m11());
            BezierCurve curve(m_pScribbleArea->mousePath, m_pScribbleArea->mousePressure, tol);
            curve.setWidth(0);
            curve.setFeather(0);
            curve.setInvisibility(true);
            curve.setVariableWidth(false);
            curve.setColourNumber(properties.colourNumber);
            VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);

            //curve.setSelected(true);
            //qDebug() << "this curve has " << curve.getVertexSize() << "vertices";

            vectorImage->addCurve(curve, qAbs(m_pScribbleArea->myView.m11()));
            m_pScribbleArea->setModified(m_pEditor->m_nCurrentLayerIndex, m_pEditor->m_nCurrentFrameIndex);
            m_pScribbleArea->updateAll = true;
        }
    }

}

void PencilTool::adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice)
{
    if (m_pScribbleArea->usePressure && !mouseDevice)
    {
        m_pScribbleArea->currentPressuredColor.setAlphaF(m_pEditor->currentColor.alphaF() * pressure);
    }
    else
    {
        m_pScribbleArea->currentPressuredColor.setAlphaF(m_pEditor->currentColor.alphaF());
    }
    m_pScribbleArea->currentWidth = properties.width;
}
