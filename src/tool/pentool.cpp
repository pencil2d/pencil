#include <QPixmap>

#include "pentool.h"

#include "strokemanager.h"
#include "pencilsettings.h"
#include "editor.h"
#include "scribblearea.h"

#include "layervector.h"

PenTool::PenTool(QObject *parent) : StrokeTool(parent)
{

}

ToolType PenTool::type()
{
    return PEN;
}

void PenTool::loadSettings()
{
    QSettings settings("Pencil","Pencil");
    properties.width = settings.value("penWidth").toDouble();    
    properties.colourNumber = 0;
    properties.feather = 0;    
    properties.pressure = ON;
    properties.invisibility = OFF;
    properties.preserveAlpha = OFF;

    if ( properties.width <= 0 )
    {
        properties.width = 1.5;
        settings.setValue("penWidth", properties.width);
    }

    if ( properties.feather < 0 )
    {
        properties.feather = 0;
    }

    currentWidth = properties.width;
}

QCursor PenTool::cursor()
{
    if (isAdjusting) // being dynamically resized
    {
         return QCursor(circleCursors()); // width and feather cursors
    }
    if ( pencilSettings()->value( kSettingToolCursor ).toBool() )
    {
        return QCursor(QPixmap(":icons/pen.png"), 7, 0);
    }
    return Qt::CrossCursor;

}

void PenTool::adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice)
{
    //editor->currentColor = getTool( PEN )->properties.colour;
    //editor->currentColor.setAlphaF(pen.colour.alphaF());
    if (m_pScribbleArea->usePressure() && !mouseDevice)
    {
        currentWidth = 2.0 * properties.width * pressure;
    }
    else
    {
        currentWidth = properties.width;
    }
    // we choose the "normal" width to correspond to a pressure 0.5
}

void PenTool::mousePressEvent(QMouseEvent *event)
{
    // sanity checks
    Layer *layer = m_pEditor->getCurrentLayer();

    if (layer->type == Layer::VECTOR)
    {
        m_pEditor->selectVectorColourNumber(properties.colourNumber);
    }

    if (event->button() == Qt::LeftButton)
    {
        m_pEditor->backup(typeName());
        m_pScribbleArea->setAllDirty();
    }

    startStroke();
}

void PenTool::mouseReleaseEvent(QMouseEvent *event)
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
        else if (layer->type == Layer::VECTOR && strokePoints.size() > -1)
        {
            // Clear the temporary pixel path
            m_pScribbleArea->clearBitmapBuffer();
            qreal tol = m_pScribbleArea->getCurveSmoothing() / qAbs(m_pScribbleArea->getViewScaleX());
            BezierCurve curve(strokePoints, strokePressures, tol);
            curve.setWidth(properties.width);
            curve.setFeather(0);
            curve.setInvisibility(false);
            curve.setVariableWidth(m_pScribbleArea->usePressure());
            curve.setColourNumber(properties.colourNumber);

            VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(m_pEditor->m_nCurrentFrameIndex, 0);

            vectorImage->addCurve(curve, qAbs(m_pScribbleArea->getViewScaleX()));
            m_pScribbleArea->setModified(m_pEditor->m_nCurrentLayerIndex, m_pEditor->m_nCurrentFrameIndex);
            m_pScribbleArea->setAllDirty();
        }
    }

    endStroke();
}

void PenTool::mouseMoveEvent(QMouseEvent *event)
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

void PenTool::drawStroke()
{
    StrokeTool::drawStroke();
    QList<QPointF> p = m_pStrokeManager->interpolateStroke(currentWidth);

    Layer *layer = m_pEditor->getCurrentLayer();

    if (layer->type == Layer::BITMAP)
    {
        QPen pen = QPen(m_pEditor->currentColor, currentWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        int rad = qRound(currentWidth / 2) + 3;

        for (int i = 0; i < p.size(); i++) {
            p[i] = m_pScribbleArea->pixelToPoint(p[i]);
        }

        if (p.size() == 4) {
            QSizeF size(2,2);
            QPainterPath path(p[0]);
            path.cubicTo(p[1],
                    p[2],
                    p[3]);
            m_pScribbleArea->drawPath(path, pen, Qt::NoBrush, QPainter::CompositionMode_Source);
            m_pScribbleArea->refreshBitmap(path.boundingRect().toRect(), rad);
        }
    }
    else if (layer->type == Layer::VECTOR)
    {
        int rad = qRound((currentWidth / 2 + 2) * (qAbs(m_pScribbleArea->getTempViewScaleX()) + qAbs(m_pScribbleArea->getTempViewScaleY())));
        QPen pen(m_pEditor->currentColor, currentWidth * m_pScribbleArea->getTempViewScaleX(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

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

