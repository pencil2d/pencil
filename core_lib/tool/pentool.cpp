#include <QPixmap>

#include "layervector.h"
#include "colormanager.h"
#include "strokemanager.h"
#include "layermanager.h"
#include "pencilsettings.h"
#include "editor.h"
#include "scribblearea.h"

#include "pentool.h"


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
    properties.feather = -1;
    properties.pressure = ON;
    properties.invisibility = OFF;
    properties.preserveAlpha = OFF;

    if ( properties.width <= 0 )
    {
        properties.width = 1.5;
        settings.setValue("penWidth", properties.width);
    }

    currentWidth = properties.width;
}

QCursor PenTool::cursor()
{
    if (isAdjusting) // being dynamically resized
    {
         return QCursor(circleCursors()); // two circles cursor
    }
    if ( pencilSettings()->value( SETTING_TOOL_CURSOR ).toBool() )
    {
        return QCursor(QPixmap(":icons/pen.png"), 7, 0);
    }
    return Qt::CrossCursor;
}

void PenTool::adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice)
{
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
    if (event->button() == Qt::LeftButton)
    {
        m_pEditor->backup(typeName());
        m_pScribbleArea->setAllDirty();
    }

    startStroke();
}

void PenTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer* layer = m_pEditor->getCurrentLayer();

    if (event->button() == Qt::LeftButton)
    {
        if (m_pScribbleArea->isLayerPaintable())
        {
            drawStroke();
        }

        if (layer->type() == Layer::BITMAP)
        {
            m_pScribbleArea->paintBitmapBuffer();
            m_pScribbleArea->setAllDirty();
        }
        else if (layer->type() == Layer::VECTOR && strokePoints.size() > -1)
        {
            // Clear the temporary pixel path
            m_pScribbleArea->clearBitmapBuffer();
            qreal tol = m_pScribbleArea->getCurveSmoothing() / qAbs(m_pScribbleArea->getViewScaleX());
            BezierCurve curve(strokePoints, strokePressures, tol);
            curve.setWidth( properties.width );
            curve.setFeather( properties.feather );
            curve.setInvisibility(false);
            curve.setVariableWidth(m_pScribbleArea->usePressure());
            curve.setColourNumber( m_pEditor->color()->frontColorNumber() );

			auto pLayerVector = static_cast< LayerVector* >( layer );
            VectorImage* vectorImage = pLayerVector->getLastVectorImageAtFrame( m_pEditor->layerManager()->currentFramePosition(), 0 );
            vectorImage->addCurve(curve, qAbs(m_pScribbleArea->getViewScaleX()));

            m_pScribbleArea->setModified(m_pEditor->layerManager()->currentLayerIndex(), m_pEditor->layerManager()->currentFramePosition());
            m_pScribbleArea->setAllDirty();
        }
    }

    endStroke();
}

void PenTool::mouseMoveEvent(QMouseEvent *event)
{
	Layer* layer = m_pEditor->layerManager()->currentLayer();
    if (layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR)
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
    QList<QPointF> p = m_pStrokeManager->interpolateStroke();

    Layer *layer = m_pEditor->getCurrentLayer();

    if (layer->type() == Layer::BITMAP)
    {
        QPen pen = QPen(m_pEditor->color()->frontColor(),
                        currentWidth,
                        Qt::SolidLine,
                        Qt::RoundCap,
                        Qt::RoundJoin);

        int rad = qRound(currentWidth / 2) + 3;

        for (int i = 0; i < p.size(); i++) 
		{
            p[i] = m_pScribbleArea->pixelToPoint(p[i]);
        }

        if ( p.size() == 4 )
        {
            QPainterPath path( p[0] );
			//path.lineTo( p[ 1 ] );
			//path.lineTo( p[ 2 ] );
			path.lineTo( p[ 3 ] );
			//qDebug() << p[ 0 ] << p[ 1 ] << p[ 2 ] << p[ 3 ];
			//path.cubicTo( p[1], p[2], p[3] );
            m_pScribbleArea->drawPath(path, pen, Qt::NoBrush, QPainter::CompositionMode_Source);
            m_pScribbleArea->refreshBitmap(path.boundingRect().toRect(), rad);
        }
    }
    else if (layer->type() == Layer::VECTOR)
    {
        int rad = qRound((currentWidth / 2 + 2) * (qAbs(m_pScribbleArea->getTempViewScaleX()) + qAbs(m_pScribbleArea->getTempViewScaleY())));

        QPen pen(m_pEditor->color()->frontColor(),
                 currentWidth * m_pScribbleArea->getTempViewScaleX(),
                 Qt::SolidLine,
                 Qt::RoundCap,
                 Qt::RoundJoin);

        if (p.size() == 4)
        {
            QPainterPath path( p[0] );
            path.cubicTo( p[1],
                          p[2],
                          p[3] );
            m_pScribbleArea->drawPath(path, pen, Qt::NoBrush, QPainter::CompositionMode_Source);
            m_pScribbleArea->refreshVector(path.boundingRect().toRect(), rad);
        }
    }
}
