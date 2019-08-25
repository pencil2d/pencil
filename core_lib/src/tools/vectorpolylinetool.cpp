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

#include "vectorpolylinetool.h"


#include "editor.h"
#include "scribblearea.h"

#include "strokemanager.h"
#include "layermanager.h"
#include "colormanager.h"
#include "viewmanager.h"
#include "pointerevent.h"
#include "layervector.h"
#include "layerbitmap.h"
#include "vectorimage.h"


VectorPolylineTool::VectorPolylineTool(QObject* parent) : BaseTool(parent)
{
}

void VectorPolylineTool::loadSettings()
{
    mPropertyEnabled[WIDTH] = true;
    mPropertyEnabled[BEZIER] = true;

    QSettings settings(PENCIL2D, PENCIL2D);

    properties.width = settings.value("polyLineWidth", 8.0).toDouble();
    properties.pressure = false;
    properties.invisibility = OFF;
    properties.stabilizerLevel = -1;
}

void VectorPolylineTool::resetToDefault()
{
    setWidth(8.0);
    setBezier(false);
}

void VectorPolylineTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("polyLineWidth", width);
    settings.sync();
}

QCursor VectorPolylineTool::cursor()
{
    return Qt::CrossCursor;
}

void VectorPolylineTool::clearToolData()
{
    mPoints.clear();
}

void VectorPolylineTool::pointerPressEvent(PointerEvent* event)
{
    Layer* layer = mEditor->layers()->currentLayer();
    Q_ASSERT(layer->type() == Layer::VECTOR);

    if (event->button() == Qt::LeftButton)
    {
        mScribbleArea->handleDrawingOnEmptyFrame();


        static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0)->deselectAll();
        if (mScribbleArea->makeInvisible() && !mEditor->preference()->isOn(SETTING::INVISIBLE_LINES))
        {
            mScribbleArea->toggleThinLines();
        }

        mPoints << getCurrentPoint();
        mScribbleArea->setAllDirty();
    }
}

void VectorPolylineTool::pointerMoveEvent(PointerEvent*)
{
    drawPolyline(mPoints, getCurrentPoint());
}

void VectorPolylineTool::pointerReleaseEvent(PointerEvent *)
{}

void VectorPolylineTool::pointerDoubleClickEvent(PointerEvent*)
{
    // include the current point before ending the line.
    mPoints << getCurrentPoint();

    mEditor->backup(typeName());

    endPolyline(mPoints);
    clearToolData();
}


bool VectorPolylineTool::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Return:
        if (mPoints.size() > 0)
        {
            endPolyline(mPoints);
            clearToolData();
            return true;
        }
        break;

    case Qt::Key_Escape:
        if (mPoints.size() > 0)
        {
            cancelPolyline();
            clearToolData();
            return true;
        }
        break;

    default:
        return false;
    }

    return false;
}

void VectorPolylineTool::drawPolyline(QList<QPointF> points, QPointF endPoint)
{
    if (points.size() > 0)
    {
        QPen pen(mEditor->color()->frontColor(),
                 properties.width,
                 Qt::SolidLine,
                 Qt::RoundCap,
                 Qt::RoundJoin);

        // Bitmap by default
        QPainterPath tempPath;
        if (properties.bezier_state)
        {
            tempPath = BezierCurve(points).getSimplePath();
        }
        else
        {
            tempPath = BezierCurve(points).getStraightPath();
        }
        tempPath.lineTo(endPoint);

        // Vector otherwise
        tempPath = mEditor->view()->mapCanvasToScreen(tempPath);
        if (mScribbleArea->makeInvisible() == true)
        {
            pen.setWidth(0);
            pen.setStyle(Qt::DotLine);
        }
        else
        {
            pen.setWidth(properties.width * mEditor->view()->scaling());
        }

        mScribbleArea->drawPolyline(tempPath, pen, properties.useAA);
    }
}


void VectorPolylineTool::cancelPolyline()
{
    // Clear the in-progress polyline from the bitmap buffer.
    mScribbleArea->clearBitmapBuffer();
    mScribbleArea->updateCurrentFrame();
}

void VectorPolylineTool::endPolyline(QList<QPointF> points)
{
    Layer* layer = mEditor->layers()->currentLayer();

    BezierCurve curve = BezierCurve(points);
    if (mScribbleArea->makeInvisible() == true)
    {
        curve.setWidth(0);
    }
    else
    {
        curve.setWidth(properties.width);
    }
    curve.setColourNumber(mEditor->color()->frontColorNumber());
    curve.setVariableWidth(false);
    curve.setInvisibility(mScribbleArea->makeInvisible());

    VectorImage* vecImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
    vecImage->addCurve(curve, mEditor->view()->scaling());

    mScribbleArea->mBufferImg->clear();
    mScribbleArea->setModified(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame());
}
