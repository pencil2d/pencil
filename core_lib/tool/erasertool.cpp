#include <QSettings>
#include <QPixmap>
#include <QPainter>

#include "scribblearea.h"

#include "pencilsettings.h"
#include "strokemanager.h"
#include "layermanager.h"
#include "editor.h"
#include "blitrect.h"
#include "layervector.h"
#include "erasertool.h"


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

    properties.pressure = ON;
    properties.invisibility = DISABLED;
    properties.preserveAlpha = OFF;

    if (properties.width <= 0)
    {
        properties.width = 25;
        settings.setValue("eraserWidth", properties.width);
    }
    if (properties.feather <= 0)
    {
        properties.feather = 50;
        settings.setValue("eraserFeather", properties.feather);
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
    mCurrentWidth = properties.width;
    if (mScribbleArea->usePressure() && !mouseDevice)
    {
        mCurrentPressure = pressure;
    }
    else
    {
        mCurrentPressure = 1.0;
    }
}

void EraserTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mEditor->backup(typeName());
        mScribbleArea->setAllDirty();
    }

    startStroke();
    lastBrushPoint = getCurrentPoint();
}

void EraserTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer *layer = mEditor->getCurrentLayer();

    if (event->button() == Qt::LeftButton)
    {
        if (mScribbleArea->isLayerPaintable())
        {
            drawStroke();
        }

        if (layer->type() == Layer::BITMAP)
        {
            mScribbleArea->paintBitmapBuffer();
            mScribbleArea->setAllDirty();
        }
        else if (layer->type() == Layer::VECTOR)
        {
            VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(mEditor->layers()->currentFramePosition(), 0);
            // Clear the area containing the last point
            //vectorImage->removeArea(lastPoint);
            // Clear the temporary pixel path
            mScribbleArea->clearBitmapBuffer();
            vectorImage->deleteSelectedPoints();
            //update();
            mScribbleArea->setModified(mEditor->layers()->currentLayerIndex(), mEditor->layers()->currentFramePosition());
            mScribbleArea->setAllDirty();
        }
    }

    endStroke();
}

void EraserTool::mouseMoveEvent(QMouseEvent *event)
{
    Layer *layer = mEditor->getCurrentLayer();

    if (event->buttons() & Qt::LeftButton)
    {
        if (layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR)
        {
            drawStroke();
        }
        if (layer->type() == Layer::VECTOR)
        {
            qreal radius = (properties.width / 2) / mScribbleArea->getTempViewScaleX();
            QList<VertexRef> nearbyVertices = ((LayerVector *)layer)->getLastVectorImageAtFrame(mEditor->layers()->currentFramePosition(), 0)
                ->getVerticesCloseTo(getCurrentPoint(), radius);
            for (int i = 0; i < nearbyVertices.size(); i++)
            {
                ((LayerVector *)layer)->getLastVectorImageAtFrame(mEditor->layers()->currentFramePosition(), 0)->setSelected(nearbyVertices.at(i), true);
            }
            //update();
            mScribbleArea->setAllDirty();
        }
    }
}

// draw a single paint dab at the given location
void EraserTool::paintAt(QPointF)
{
}

void EraserTool::drawStroke()
{
    StrokeTool::drawStroke();
    QList<QPointF> p = m_pStrokeManager->interpolateStroke();

    Layer *layer = mEditor->getCurrentLayer();

    if (layer->type() == Layer::BITMAP)
    {
        for (int i = 0; i < p.size(); i++) 
		{
            p[i] = mScribbleArea->pixelToPoint( p[i] );
        }

        qreal opacity = 1.0;
        qreal brushWidth = mCurrentWidth +  0.5 * properties.feather;
        qreal offset = qMax(0.0, mCurrentWidth - 0.5 * properties.feather) / brushWidth;
        opacity = mCurrentPressure;
        brushWidth = brushWidth * mCurrentPressure;

        //        if (tabletInUse) { opacity = tabletPressure; }
        //        if (usePressure) { brushWidth = brushWidth * tabletPressure; }

        qreal brushStep = 0.5 * mCurrentWidth + 0.5 * properties.feather;
        brushStep = brushStep * mCurrentPressure;

        //        if (usePressure) { brushStep = brushStep * tabletPressure; }
        brushStep = qMax(1.0, brushStep);

        mCurrentWidth = properties.width;
        BlitRect rect;

        QRadialGradient radialGrad(QPointF(0,0), 0.5 * brushWidth);
        mScribbleArea->setGaussianGradient(radialGrad, QColor(255,255,255), opacity, offset);

        QPointF a = lastBrushPoint;
        QPointF b = getCurrentPoint();

        qreal distance = 4 * QLineF(b, a).length();
        int steps = qRound(distance) / brushStep;

        for (int i = 0; i < steps; i++)
        {
            QPointF point = lastBrushPoint + (i + 1) * (brushStep) * (b - lastBrushPoint) / distance;
            rect.extend(point.toPoint());
            mScribbleArea->drawBrush(point, brushWidth, offset, QColor(255,255,255), opacity);

            if (i == (steps - 1))
            {
                lastBrushPoint = point;
            }
        }

        int rad = qRound(brushWidth) / 2 + 2;
        mScribbleArea->refreshBitmap(rect, rad);
    }
    else if (layer->type() == Layer::VECTOR)
    {
        QPen pen(Qt::white, mCurrentWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        int rad = qRound((mCurrentWidth / 2 + 2) * (qAbs(mScribbleArea->getTempViewScaleX()) + qAbs(mScribbleArea->getTempViewScaleY())));

        if (p.size() == 4) {
            QSizeF size(2,2);
            QPainterPath path(p[0]);
            path.cubicTo(p[1],
                p[2],
                p[3]);
            mScribbleArea->drawPath(path, pen, Qt::NoBrush, QPainter::CompositionMode_Source);
            mScribbleArea->refreshVector(path.boundingRect().toRect(), rad);
        }
    }
}