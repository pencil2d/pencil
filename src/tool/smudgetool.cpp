#include <QPixmap>
#include "editor.h"
#include "scribblearea.h"

#include "layer.h"
#include "colormanager.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "strokemanager.h"
#include "blitrect.h"

#include "smudgetool.h"

SmudgeTool::SmudgeTool(QObject *parent) :
    StrokeTool(parent)
{
    toolMode = 0; // tool mode
}

ToolType SmudgeTool::type()
{
    return SMUDGE;
}

void SmudgeTool::loadSettings()
{
    QSettings settings("Pencil", "Pencil");
    properties.width = settings.value("smudgeWidth").toDouble();
    properties.feather = settings.value("smudgeFeather").toDouble();

    if (properties.width <= 0)
    {
        properties.width = 25;
        settings.setValue("smudgeWidth", properties.width);
    }
    if (properties.feather <= 0)
    {
        properties.feather = 200;
        settings.setValue("smudgeFeather", properties.feather);
    }

}

QCursor SmudgeTool::cursor()
{
    qDebug() << "smudge tool";
    if (isAdjusting) // being dynamically resized
    {
        return circleCursors(); // two circles cursor
    }
    return QCursor(QPixmap(":icons/smudge.png"),3 ,16);
}

void SmudgeTool::adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice)
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

void SmudgeTool::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "smudge event";

    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer == NULL) { return; }

    if (event->button() == Qt::LeftButton)
    {
        if (layer->type == Layer::BITMAP)
        {
            if (event->modifiers())
            {
                toolMode = 1; // not a temporary tool but toolMode
            }
            else
            {
                toolMode = 0;
            }
            m_pEditor->backup(typeName());
            m_pScribbleArea->setAllDirty();
            startStroke();
            lastBrushPoint = getCurrentPoint();
        }
        else if (layer->type == Layer::VECTOR)
        {
            m_pScribbleArea->closestCurves = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0)
                ->getCurvesCloseTo(getCurrentPoint(), m_pScribbleArea->tol / m_pScribbleArea->getTempViewScaleX());
            m_pScribbleArea->closestVertices = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0)
                ->getVerticesCloseTo(getCurrentPoint(), m_pScribbleArea->tol / m_pScribbleArea->getTempViewScaleX());

            if (m_pScribbleArea->closestVertices.size() > 0 || m_pScribbleArea->closestCurves.size() > 0)      // the user clicks near a vertex or a curve
            {
                //qDebug() << "closestCurves:" << closestCurves << " | closestVertices" << closestVertices;
                m_pEditor->backup(typeName());
                VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);

                if (event->modifiers() != Qt::ShiftModifier && !vectorImage->isSelected(m_pScribbleArea->closestVertices))
                {
                    m_pScribbleArea->paintTransformedSelection();
                    m_pScribbleArea->deselectAll();
                }

                vectorImage->setSelected(m_pScribbleArea->closestVertices, true);
                m_pScribbleArea->vectorSelection.add(m_pScribbleArea->closestCurves);
                m_pScribbleArea->vectorSelection.add(m_pScribbleArea->closestVertices);

                m_pScribbleArea->update();
            }
            else
            {
                m_pScribbleArea->deselectAll();
            }
        }
    }
}

void SmudgeTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer == NULL) { return; }

    if (event->button() == Qt::LeftButton)
    {
        if (layer->type == Layer::BITMAP)
        {
            drawStroke();
            m_pScribbleArea->paintBitmapBuffer();
            m_pScribbleArea->setAllDirty();
            endStroke();
        }
        else if (layer->type == Layer::VECTOR)
        {
            VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);
            vectorImage->applySelectionTransformation();
            m_pScribbleArea->selectionTransformation.reset();
            for (int k = 0; k < m_pScribbleArea->vectorSelection.curve.size(); k++)
            {
                int curveNumber = m_pScribbleArea->vectorSelection.curve.at(k);
                vectorImage->curve[curveNumber].smoothCurve();
            }
            m_pScribbleArea->setModified(m_pEditor->m_nCurrentLayerIndex, m_pEditor->m_nCurrentFrameIndex);
        }
    }
}

void SmudgeTool::mouseMoveEvent(QMouseEvent *event)
{    
    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer == NULL) { return; }

    if (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)
    {
        if (event->buttons() & Qt::LeftButton)   // the user is also pressing the mouse (dragging) {
        {
            if (layer->type == Layer::BITMAP)
            {
                drawStroke();
            }
            else if (layer->type == Layer::VECTOR)
            {
                if (event->modifiers() != Qt::ShiftModifier)    // (and the user doesn't press shift)
                {
                    // transforms the selection
                    m_pScribbleArea->selectionTransformation = QMatrix().translate(m_pScribbleArea->offset.x(), m_pScribbleArea->offset.y());
                    ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0)->setSelectionTransformation(m_pScribbleArea->selectionTransformation);
                }
            }
        }
        else     // the user is moving the mouse without pressing it
        {
            if (layer->type == Layer::VECTOR)
            {
                m_pScribbleArea->closestVertices = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0)
                    ->getVerticesCloseTo(getCurrentPoint(), m_pScribbleArea->tol / m_pScribbleArea->getTempViewScaleX());
            }
        }
        m_pScribbleArea->update();
        m_pScribbleArea->setAllDirty();
    }

}

void SmudgeTool::drawStroke()
{
    if ( !m_pScribbleArea->isLayerPaintable() ) return;

    Layer *layer = m_pEditor->getCurrentLayer();
    if (layer == NULL) { return; }

    BitmapImage *targetImage = ((LayerBitmap *)layer)->getLastBitmapImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);
    StrokeTool::drawStroke();
    QList<QPointF> p = m_pStrokeManager->interpolateStroke(currentWidth);

    for (int i = 0; i < p.size(); i++) {
        p[i] = m_pScribbleArea->pixelToPoint(p[i]);
    }

    qreal opacity = 1.0;
    qreal brushWidth = currentWidth +  0.0 * properties.feather;
    qreal offset = qMax(0.0, currentWidth - 0.5 * properties.feather) / brushWidth;
    //opacity = currentPressure; // todo: Probably not interesting?!
    //brushWidth = brushWidth * opacity;

    BlitRect rect;
    QPointF a = lastBrushPoint;
    QPointF b = getCurrentPoint();


    if (toolMode == 0) { // normal (smoothed)
        qreal brushStep = 0.5 * ( currentWidth + properties.feather ) / 80;
        qreal distance = QLineF(b, a).length();
        brushStep = qMax( 1.0, brushStep * opacity );
        //currentWidth = properties.width; // here ?
        int steps = qRound(distance) / brushStep;
        int rad = qRound(brushWidth) / 2 + 2;

        for (int i = 0; i < steps; i++)
        {
            QPointF targetPoint = lastBrushPoint + (i + 1) * (brushStep) * (b - lastBrushPoint) / distance;
            rect.extend(targetPoint.toPoint());
            m_pScribbleArea->drawTexturedBrush( targetImage,
                                                lastBrushPoint,
                                                targetPoint,
                                                brushWidth,
                                                offset,
                                                opacity);

            if (i == (steps - 1))
            {
                lastBrushPoint = targetPoint;
            }
        }
        m_pScribbleArea->refreshBitmap(rect, rad);
        m_pScribbleArea->paintBitmapBuffer();
    }
    else // smudge (liquified)
    {
        qreal brushStep = 0.5 * ( currentWidth + properties.feather ) / 80;
        qreal distance = QLineF(b, a).length()/4;
        brushStep = qMax( 1.0, brushStep * opacity );
        //currentWidth = properties.width; // here ?
        int steps = qRound(distance) / brushStep;
        int rad = qRound(brushWidth) / 2 + 2;

        QPointF sourcePoint = lastBrushPoint;
        for (int i = 0; i < steps; i++)
        {
            QPointF targetPoint = lastBrushPoint + (i + 1) * (brushStep) * (b - lastBrushPoint) / distance;
            rect.extend(targetPoint.toPoint());
            m_pScribbleArea->slideTexturedBrush( targetImage,
                                                sourcePoint,
                                                targetPoint,
                                                brushWidth,
                                                offset,
                                                opacity);

            if (i == (steps - 1))
            {
                lastBrushPoint = targetPoint;
            }
            sourcePoint = targetPoint;
            m_pScribbleArea->refreshBitmap(rect, rad);
            m_pScribbleArea->paintBitmapBuffer();
        }
    }
}
