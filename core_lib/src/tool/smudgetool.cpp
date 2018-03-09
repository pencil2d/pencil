/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "smudgetool.h"

#include <QPixmap>
#include "vectorimage.h"
#include "editor.h"
#include "scribblearea.h"
#include "layermanager.h"
#include "colormanager.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "strokemanager.h"
#include "blitrect.h"

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
    m_enabledProperties[WIDTH] = true;
    m_enabledProperties[FEATHER] = true;


    QSettings settings( PENCIL2D, PENCIL2D );
    properties.width = settings.value("smudgeWidth").toDouble();
    properties.feather = settings.value("smudgeFeather").toDouble();
    properties.pressure = 0;
    properties.inpolLevel = -1;

    // First run
    if (properties.width <= 0)
    {
        setWidth(25);
        setFeather(200);
        setPressure(0);
    }
}

void SmudgeTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("smudgeWidth", width);
    settings.sync();
}

void SmudgeTool::setFeather( const qreal feather )
{
    // Set current property
    properties.feather = feather;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("smudgeFeather", feather);
    settings.sync();
}

void SmudgeTool::setPressure( const bool pressure )
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue("smudgePressure", pressure);
    settings.sync();
}

QCursor SmudgeTool::cursor()
{
    qDebug() << "smudge tool";
    if ( toolMode == 0 ) { //normal mode
        return QCursor(QPixmap(":icons/smudge.png"),0 ,16);
    } else { // blured mode
        return QCursor(QPixmap(":icons/liquify.png"),-4,16);
    }
}

void SmudgeTool::adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice)
{
    mCurrentWidth = properties.width;
    if (properties.pressure && !mouseDevice)
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
            mScribbleArea->setAllDirty();
            startStroke();
            mLastBrushPoint = getCurrentPoint();
        }
        else if (layer->type() == Layer::VECTOR)
        {
            mScribbleArea->mClosestCurves = ((LayerVector *)layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0)
                ->getCurvesCloseTo( getCurrentPoint(),
                                    mScribbleArea->selectionTolerance / mEditor->view()->scaling() );
            mScribbleArea->mClosestVertices = ((LayerVector *)layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0)
                ->getVerticesCloseTo( getCurrentPoint(),
                                      mScribbleArea->selectionTolerance / mEditor->view()->scaling() );

            if (mScribbleArea->mClosestVertices.size() > 0 || mScribbleArea->mClosestCurves.size() > 0)      // the user clicks near a vertex or a curve
            {
                //qDebug() << "closestCurves:" << closestCurves << " | closestVertices" << closestVertices;
                VectorImage *vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);

                if (event->modifiers() != Qt::ShiftModifier && !vectorImage->isSelected(mScribbleArea->mClosestVertices))
                {
                    mScribbleArea->paintTransformedSelection();
                    mScribbleArea->deselectAll();
                }

                vectorImage->setSelected(mScribbleArea->mClosestVertices, true);
                mScribbleArea->vectorSelection.add(mScribbleArea->mClosestCurves);
                mScribbleArea->vectorSelection.add(mScribbleArea->mClosestVertices);

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
        mEditor->backup(typeName());

        if (layer->type() == Layer::BITMAP)
        {
            drawStroke();
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
                vectorImage->curve(curveNumber).smoothCurve();
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
                    mScribbleArea->selectionTransformation = QTransform().translate(mScribbleArea->mOffset.x(), mScribbleArea->mOffset.y());
                    ((LayerVector *)layer)->getLastVectorImageAtFrame( mEditor->currentFrame(), 0)->setSelectionTransformation(mScribbleArea->selectionTransformation);
                }
            }
        }
        else     // the user is moving the mouse without pressing it
        {
            if (layer->type() == Layer::VECTOR)
            {
                mScribbleArea->mClosestVertices = ((LayerVector *)layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0)
                    ->getVerticesCloseTo( getCurrentPoint(),
                                          mScribbleArea->selectionTolerance / mEditor->view()->scaling() );
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

    for (int i = 0; i < p.size(); i++)
    {
        p[ i ] = mEditor->view()->mapScreenToCanvas( p[ i ] );
    }

    qreal opacity = 1.0;
    qreal brushWidth = mCurrentWidth +  0.0 * properties.feather;
    qreal offset = qMax(0.0, mCurrentWidth - 0.5 * properties.feather) / brushWidth;
    //opacity = currentPressure; // todo: Probably not interesting?!
    //brushWidth = brushWidth * opacity;

    BlitRect rect;
    QPointF a = mLastBrushPoint;
    QPointF b = getCurrentPoint();


    if (toolMode == 1) // liquify hard
    {
        qreal brushStep = 2;
        qreal distance = QLineF(b, a).length()/2.0;
        int steps = qRound(distance / brushStep);
        int rad = qRound(brushWidth / 2.0) + 2;

        QPointF sourcePoint = mLastBrushPoint;
        for (int i = 0; i < steps; i++)
        {
            QPointF targetPoint = mLastBrushPoint + (i + 1) * (brushStep) * (b - mLastBrushPoint) / distance;
            rect.extend(targetPoint.toPoint());
            mScribbleArea->liquifyBrush(targetImage,
                                        sourcePoint,
                                        targetPoint,
                                        brushWidth,
                                        offset,
                                        opacity);

            if (i == (steps - 1))
            {
                mLastBrushPoint = targetPoint;
            }
            sourcePoint = targetPoint;
            mScribbleArea->paintBitmapBufferRect( rect );
            mScribbleArea->refreshBitmap(rect, rad);
        }
    }
    else // liquify smooth
    {
        qreal brushStep = 2.0;
        qreal distance = QLineF(b, a).length();
        int steps = qRound(distance / brushStep);
        int rad = qRound(brushWidth / 2.0) + 2;

        QPointF sourcePoint = mLastBrushPoint;
        for (int i = 0; i < steps; i++)
        {
            QPointF targetPoint = mLastBrushPoint + (i + 1) * (brushStep) * (b - mLastBrushPoint) / distance;
            rect.extend(targetPoint.toPoint());
            mScribbleArea->blurBrush(targetImage,
                                     sourcePoint,
                                     targetPoint,
                                     brushWidth,
                                     offset,
                                     opacity);

            if (i == (steps - 1))
            {
                mLastBrushPoint = targetPoint;
            }
            sourcePoint = targetPoint;
            mScribbleArea->paintBitmapBufferRect( rect );
            mScribbleArea->refreshBitmap(rect, rad);
        }
    }
}
