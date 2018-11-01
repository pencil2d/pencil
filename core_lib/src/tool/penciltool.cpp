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
#include "penciltool.h"

#include <QSettings>
#include <QPixmap>
#include <QMouseEvent>

#include "layermanager.h"
#include "colormanager.h"
#include "strokemanager.h"
#include "viewmanager.h"
#include "preferencemanager.h"

#include "editor.h"
#include "scribblearea.h"
#include "blitrect.h"
#include "layervector.h"
#include "vectorimage.h"


PencilTool::PencilTool(QObject* parent) : StrokeTool(parent)
{
}


void PencilTool::loadSettings()
{
    m_enabledProperties[WIDTH] = true;
    m_enabledProperties[PRESSURE] = true;
    m_enabledProperties[VECTORMERGE] = false;
    m_enabledProperties[STABILIZATION] = true;
    m_enabledProperties[FILLCONTOUR] = true;

    QSettings settings(PENCIL2D, PENCIL2D);
    properties.width = settings.value("pencilWidth").toDouble();
    properties.feather = 50;
    properties.pressure = settings.value("pencilPressure").toBool();
    properties.stabilizerLevel = settings.value("pencilLineStabilization").toInt();
    properties.useAA = DISABLED;
    properties.useFeather = true;
    properties.useFillContour = false;

    //    properties.invisibility = 1;
    //    properties.preserveAlpha = 0;

    if (properties.width <= 0)
    {
        // setting the default value to 4
        // seems to give great results with pressure on
        setWidth(4);
        setPressure(true);
    }
}

void PencilTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("pencilWidth", width);
    settings.sync();
}

void PencilTool::setFeather(const qreal feather)
{
    properties.feather = feather;
}

void PencilTool::setUseFeather(const bool usingFeather)
{
    // Set current property
    properties.useFeather = usingFeather;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("brushUseFeather", usingFeather);
    settings.sync();
}


void PencilTool::setInvisibility(const bool)
{
    // force value
    properties.invisibility = 1;
}

void PencilTool::setPressure(const bool pressure)
{
    // Set current property
    properties.pressure = pressure;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("pencilPressure", pressure);
    settings.sync();
}

void PencilTool::setPreserveAlpha(const bool preserveAlpha)
{
    // force value
    Q_UNUSED( preserveAlpha );
    properties.preserveAlpha = 0;
}

void PencilTool::setStabilizerLevel(const int level)
{
    properties.stabilizerLevel = level;

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("pencilLineStabilization", level);
    settings.sync();
}

void PencilTool::setUseFillContour(const bool useFillContour)
{
    properties.useFillContour = useFillContour;

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("FillContour", useFillContour);
    settings.sync();
}

QCursor PencilTool::cursor()
{
    if (mEditor->preference()->isOn(SETTING::TOOL_CURSOR))
    {
        return QCursor(QPixmap(":icons/pencil2.png"), 0, 16);
    }
    return Qt::CrossCursor;
}

void PencilTool::tabletPressEvent(QTabletEvent*)
{
    mScribbleArea->setAllDirty();

    mMouseDownPoint = getCurrentPoint();
    mLastBrushPoint = getCurrentPoint();

    startStroke();

    // note: why are we doing this on device press event?
    if ( !mEditor->preference()->isOn(SETTING::INVISIBLE_LINES) )
    {
        mScribbleArea->toggleThinLines();
    }
}

void PencilTool::tabletMoveEvent(QTabletEvent*)
{
    drawStroke();
    if (properties.stabilizerLevel != m_pStrokeManager->getStabilizerLevel())
        m_pStrokeManager->setStabilizerLevel(properties.stabilizerLevel);
}

void PencilTool::tabletReleaseEvent(QTabletEvent*)
{
    mEditor->backup(typeName());

    Layer* layer = mEditor->layers()->currentLayer();
    qreal distance = QLineF(getCurrentPoint(), mMouseDownPoint).length();
    if (distance < 1)
    {
        paintAt(mMouseDownPoint);
    }
    else
    {
        drawStroke();
    }

    if (layer->type() == Layer::BITMAP)
        paintBitmapStroke();
    else if (layer->type() == Layer::VECTOR)
        paintVectorStroke(layer);
    endStroke();
}

void PencilTool::mousePressEvent(QMouseEvent *)
{
    mScribbleArea->setAllDirty();

    mMouseDownPoint = getCurrentPoint();
    mLastBrushPoint = getCurrentPoint();

    startStroke();

    // note: why are we doing this on device press event?
    if ( !mEditor->preference()->isOn(SETTING::INVISIBLE_LINES) )
    {
        mScribbleArea->toggleThinLines();
    }
}

void PencilTool::mouseReleaseEvent(QMouseEvent *)
{
    mEditor->backup(typeName());

    Layer* layer = mEditor->layers()->currentLayer();
    qreal distance = QLineF(getCurrentPoint(), mMouseDownPoint).length();
    if (distance < 1)
    {
        paintAt(mMouseDownPoint);
    }
    else
    {
        drawStroke();
    }

    if (layer->type() == Layer::BITMAP)
        paintBitmapStroke();
    else if (layer->type() == Layer::VECTOR)
        paintVectorStroke(layer);
    endStroke();
}

void PencilTool::mouseMoveEvent( QMouseEvent *)
{
    drawStroke();
    if (properties.stabilizerLevel != m_pStrokeManager->getStabilizerLevel())
        m_pStrokeManager->setStabilizerLevel(properties.stabilizerLevel);
}

void PencilTool::adjustPressureSensitiveProperties(qreal pressure, bool mouseDevice)
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

// draw a single paint dab at the given location
void PencilTool::paintAt(QPointF point)
{
    qDebug() << "Made a single dab at " << point;
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer->type() == Layer::BITMAP)
    {
        qreal opacity = 1.0;
        mCurrentWidth = properties.width;
        if (properties.pressure)
        {
            opacity = mCurrentPressure / 2;
            mCurrentWidth *= mCurrentPressure;
        }
        qreal brushWidth = mCurrentWidth;
        qreal fixedBrushFeather = properties.feather;

        BlitRect rect;

        rect.extend(point.toPoint());
        mScribbleArea->drawPencil(point,
                                  brushWidth,
                                  fixedBrushFeather,
                                  mEditor->color()->frontColor(),
                                  opacity);

        int rad = qRound(brushWidth) / 2 + 2;
        mScribbleArea->refreshBitmap(rect, rad);
    }
}


void PencilTool::drawStroke()
{
    StrokeTool::drawStroke();
    QList<QPointF> p = m_pStrokeManager->interpolateStroke();

    Layer* layer = mEditor->layers()->currentLayer();

    if (layer->type() == Layer::BITMAP)
    {
        qreal opacity = 1.0;
        mCurrentWidth = properties.width;
        if (properties.pressure)
        {
            opacity = mCurrentPressure / 2;
            mCurrentWidth = properties.width * mCurrentPressure;
        }
        qreal brushWidth = mCurrentWidth;
        qreal fixedBrushFeather = properties.feather;

        qreal brushStep = (0.5 * brushWidth);
        brushStep = qMax(1.0, brushStep);

        BlitRect rect;

        QPointF a = mLastBrushPoint;
        QPointF b = getCurrentPoint();

        qreal distance = 4 * QLineF(b, a).length();
        int steps = qRound(distance / brushStep);

        for (int i = 0; i < steps; i++)
        {
            QPointF point = mLastBrushPoint + (i + 1) * brushStep * (getCurrentPoint() - mLastBrushPoint) / distance;
            rect.extend(point.toPoint());
            mScribbleArea->drawPencil(point,
                                      brushWidth,
                                      fixedBrushFeather,
                                      mEditor->color()->frontColor(),
                                      opacity);

            if (i == (steps - 1))
            {
                mLastBrushPoint = getCurrentPoint();
            }
        }

        int rad = qRound(brushWidth) / 2 + 2;

        mScribbleArea->paintBitmapBufferRect(rect);
        mScribbleArea->refreshBitmap(rect, rad);
    }
    else if (layer->type() == Layer::VECTOR)
    {
        properties.useFeather = false;
        mCurrentWidth = 0;
        QPen pen(mEditor->color()->frontColor(),
                 1,
                 Qt::DotLine,
                 Qt::RoundCap,
                 Qt::RoundJoin);

        int rad = qRound((mCurrentWidth / 2 + 2) * mEditor->view()->scaling());

        if (p.size() == 4)
        {
            QPainterPath path(p[0]);
            path.cubicTo(p[1],
                         p[2],
                         p[3]);
            mScribbleArea->drawPath(path, pen, Qt::NoBrush, QPainter::CompositionMode_Source);
            mScribbleArea->refreshVector(path.boundingRect().toRect(), rad);
        }
    }
}


void PencilTool::paintBitmapStroke()
{
    mScribbleArea->paintBitmapBuffer();
    mScribbleArea->setAllDirty();
    mScribbleArea->clearBitmapBuffer();
}

void PencilTool::paintVectorStroke(Layer* layer)
{
    if (mStrokePoints.empty())
        return;

    // Clear the temporary pixel path
    mScribbleArea->clearBitmapBuffer();
    qreal tol = mScribbleArea->getCurveSmoothing() / mEditor->view()->scaling();

    BezierCurve curve(mStrokePoints, mStrokePressures, tol);
    curve.setWidth(0);
    curve.setFeather(0);
    curve.setFilled(false);
    curve.setInvisibility(true);
    curve.setVariableWidth(false);
    curve.setColourNumber(mEditor->color()->frontColorNumber());
    VectorImage* vectorImage = ((LayerVector *)layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);

    vectorImage->addCurve(curve, qAbs(mEditor->view()->scaling()), properties.vectorMergeEnabled);

    if (properties.useFillContour)
    {
        vectorImage->fillContour(mStrokePoints,
                                 mEditor->color()->frontColorNumber());
    }

    if (vectorImage->isAnyCurveSelected() || mScribbleArea->isSomethingSelected())
    {
        mScribbleArea->deselectAll();
    }

    // select last/newest curve
    vectorImage->setSelected(vectorImage->getLastCurveNumber(), true);

    // TODO: selection doesn't apply on enter

    mScribbleArea->setModified(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame());
    mScribbleArea->setAllDirty();
}
