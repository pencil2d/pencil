#include <QPixmap>
#include "editor.h"
#include "scribblearea.h"

#include "layermanager.h"
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
    if ( toolMode == 0 ) { //normal mode
        return QCursor(QPixmap(":icons/smudge.png"),3 ,16);
    } else { // blured mode
        return QCursor(QPixmap(":icons/liquify.png"),3,16);
    }
}

void SmudgeTool::adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice)
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

bool SmudgeTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Alt)
    {
        toolMode = 1; // alternative mode
        mScribbleArea->setCursor( cursor() ); // update cursor
        return true;
    }
    return false;
}

bool SmudgeTool::keyReleaseEvent(QKeyEvent*)
{
    
    toolMode = 0; // default mode
    mScribbleArea->setCursor( cursor() ); // update cursor

    return true;
}

void SmudgeTool::mousePressEvent(QMouseEvent *event)
{
    //qDebug() << "smudgetool: mousePressEvent";

    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == NULL) { return; }

    if (event->button() == Qt::LeftButton)
    {
        if (layer->type() == Layer::BITMAP)
        {
            mEditor->backup(typeName());
            mScribbleArea->setAllDirty();
            startStroke();
            lastBrushPoint = getCurrentPoint();
        }
        else if (layer->type() == Layer::VECTOR)
        {
            mScribbleArea->closestCurves = ((LayerVector *)layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0)
                ->getCurvesCloseTo(getCurrentPoint(), mScribbleArea->tol / mScribbleArea->getTempViewScaleX());
            mScribbleArea->closestVertices = ((LayerVector *)layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0)
                ->getVerticesCloseTo(getCurrentPoint(), mScribbleArea->tol / mScribbleArea->getTempViewScaleX());

            if (mScribbleArea->closestVertices.size() > 0 || mScribbleArea->closestCurves.size() > 0)      // the user clicks near a vertex or a curve
            {
                //qDebug() << "closestCurves:" << closestCurves << " | closestVertices" << closestVertices;
                mEditor->backup(typeName());
                VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);

                if (event->modifiers() != Qt::ShiftModifier && !vectorImage->isSelected(mScribbleArea->closestVertices))
                {
                    mScribbleArea->paintTransformedSelection();
                    mScribbleArea->deselectAll();
                }

                vectorImage->setSelected(mScribbleArea->closestVertices, true);
                mScribbleArea->vectorSelection.add(mScribbleArea->closestCurves);
                mScribbleArea->vectorSelection.add(mScribbleArea->closestVertices);

                mScribbleArea->update();
            }
            else
            {
                mScribbleArea->deselectAll();
            }
        }
    }
}

void SmudgeTool::mouseReleaseEvent(QMouseEvent *event)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == NULL) { return; }

    if (event->button() == Qt::LeftButton)
    {
        if (layer->type() == Layer::BITMAP)
        {
            drawStroke();
            mScribbleArea->paintBitmapBuffer();
            mScribbleArea->setAllDirty();
            endStroke();
        }
        else if (layer->type() == Layer::VECTOR)
        {
            VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
            vectorImage->applySelectionTransformation();
            mScribbleArea->selectionTransformation.reset();
            for (int k = 0; k < mScribbleArea->vectorSelection.curve.size(); k++)
            {
                int curveNumber = mScribbleArea->vectorSelection.curve.at(k);
                vectorImage->m_curves[curveNumber].smoothCurve();
            }
            mScribbleArea->setModified(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame());
        }
    }
}

void SmudgeTool::mouseMoveEvent(QMouseEvent *event)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == NULL) { return; }

    if (layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR)
    {
        if (event->buttons() & Qt::LeftButton)   // the user is also pressing the mouse (dragging) {
        {
            if (layer->type() == Layer::BITMAP)
            {
                drawStroke();
            }
            else if (layer->type() == Layer::VECTOR)
            {
                if (event->modifiers() != Qt::ShiftModifier)    // (and the user doesn't press shift)
                {
                    // transforms the selection
                    mScribbleArea->selectionTransformation = QMatrix().translate(mScribbleArea->offset.x(), mScribbleArea->offset.y());
                    ((LayerVector *)layer)->getLastVectorImageAtFrame( mEditor->currentFrame(), 0)->setSelectionTransformation(mScribbleArea->selectionTransformation);
                }
            }
        }
        else     // the user is moving the mouse without pressing it
        {
            if (layer->type() == Layer::VECTOR)
            {
                mScribbleArea->closestVertices = ((LayerVector *)layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0)
                    ->getVerticesCloseTo(getCurrentPoint(), mScribbleArea->tol / mScribbleArea->getTempViewScaleX());
            }
        }
        mScribbleArea->update();
        mScribbleArea->setAllDirty();
    }
}

void SmudgeTool::drawStroke()
{
    if ( !mScribbleArea->isLayerPaintable() ) return;

    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == NULL) { return; }

    BitmapImage *targetImage = ((LayerBitmap *)layer)->getLastBitmapImageAtFrame(mEditor->currentFrame(), 0);
    StrokeTool::drawStroke();
    QList<QPointF> p = m_pStrokeManager->interpolateStroke();

    for (int i = 0; i < p.size(); i++) {
        p[i] = mScribbleArea->pixelToPoint(p[i]);
    }

    qreal opacity = 1.0;
    qreal brushWidth = mCurrentWidth +  0.0 * properties.feather;
    qreal offset = qMax(0.0, mCurrentWidth - 0.5 * properties.feather) / brushWidth;
    //opacity = currentPressure; // todo: Probably not interesting?!
    //brushWidth = brushWidth * opacity;

    BlitRect rect;
    QPointF a = lastBrushPoint;
    QPointF b = getCurrentPoint();


    if (toolMode == 0) // liquify hard (default)
    {
        qreal brushStep = 2;
        qreal distance = QLineF(b, a).length()/2.0;
        int steps = qRound(distance / brushStep);
        int rad = qRound(brushWidth / 2.0) + 2;

        QPointF sourcePoint = lastBrushPoint;
        for (int i = 0; i < steps; i++)
        {
            QPointF targetPoint = lastBrushPoint + (i + 1) * (brushStep) * (b - lastBrushPoint) / distance;
            rect.extend(targetPoint.toPoint());
            mScribbleArea->liquifyBrush( targetImage,
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
            mScribbleArea->refreshBitmap(rect, rad);
            mScribbleArea->paintBitmapBuffer();
        }
    }
    else // liquify smooth
    {
        qreal brushStep = 2.0;
        qreal distance = QLineF(b, a).length();
        int steps = qRound(distance / brushStep);
        int rad = qRound(brushWidth / 2.0) + 2;

        QPointF sourcePoint = lastBrushPoint;
        for (int i = 0; i < steps; i++)
        {
            QPointF targetPoint = lastBrushPoint + (i + 1) * (brushStep) * (b - lastBrushPoint) / distance;
            rect.extend(targetPoint.toPoint());
            mScribbleArea->blurBrush( targetImage,
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
            mScribbleArea->refreshBitmap(rect, rad);
            mScribbleArea->paintBitmapBuffer();
        }
    }
}