/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "buckettool.h"

#include <QPixmap>
#include <QPainter>
#include <QPointer>
#include <QtMath>
#include <QSettings>
#include "pointerevent.h"

#include "layer.h"
#include "layervector.h"
#include "layerbitmap.h"
#include "layermanager.h"
#include "colormanager.h"
#include "strokemanager.h"
#include "viewmanager.h"
#include "vectorimage.h"
#include "editor.h"
#include "scribblearea.h"


BucketTool::BucketTool(QObject* parent) : StrokeTool(parent)
{
}

ToolType BucketTool::type()
{
    return BUCKET;
}

void BucketTool::loadSettings()
{
    mPropertyEnabled[TOLERANCE] = true;
    mPropertyEnabled[WIDTH] = true;
    mPropertyEnabled[FILL_MODE] = true;

    QSettings settings(PENCIL2D, PENCIL2D);

    properties.width = settings.value("fillThickness", 4.0).toDouble();
    properties.feather = 10;
    properties.stabilizerLevel = StabilizationLevel::NONE;
    properties.useAA = DISABLED;
    properties.fillMode = settings.value("fillMode", 0).toInt();
    properties.tolerance = settings.value("tolerance", 32.0).toDouble();
}

void BucketTool::resetToDefault()
{
    setWidth(4.0);
    setTolerance(32.0);
    setFillMode(0);
}

QCursor BucketTool::cursor()
{
    if (mEditor->preference()->isOn(SETTING::TOOL_CURSOR))
    {
        QPixmap pixmap(":icons/bucketTool.png");
        QPainter painter(&pixmap);
        painter.end();

        return QCursor(pixmap, 4, 20);
    }
    else
    {
        return QCursor(QPixmap(":icons/cross.png"), 10, 10);
    }
}

void BucketTool::setTolerance(const int tolerance)
{
    // Set current property
    properties.tolerance = tolerance;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("tolerance", tolerance);
    settings.sync();
}

/**
 * @brief BrushTool::setWidth
 * @param width
 * set fill thickness
 */
void BucketTool::setWidth(const qreal width)
{
    // Set current property
    properties.width = width;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("fillThickness", width);
    settings.sync();
}

void BucketTool::setFillMode(int mode)
{
    // Set current property
    properties.fillMode = mode;

    // Update settings
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue("fillMode", mode);
    settings.sync();
}

void BucketTool::pointerPressEvent(PointerEvent* event)
{
    startStroke(event->inputType());
}

void BucketTool::pointerMoveEvent(PointerEvent* event)
{
    if (event->buttons() & Qt::LeftButton && event->inputType() == mCurrentInputType)
    {
        Layer* layer = mEditor->layers()->currentLayer();
        if (layer->type() == Layer::VECTOR)
        {
            drawStroke();
        }
    }
}

void BucketTool::pointerReleaseEvent(PointerEvent* event)
{
    if (event->inputType() != mCurrentInputType) return;

    Layer* layer = editor()->layers()->currentLayer();
    if (layer == nullptr) { return; }

    if (event->button() == Qt::LeftButton)
    {
        mEditor->backup(typeName());

        switch (layer->type())
        {
        case Layer::BITMAP: paintBitmap(layer); break;
        case Layer::VECTOR: paintVector(layer); break;
        default:
            break;
        }
    }
    endStroke();
}

bool BucketTool::startAdjusting(Qt::KeyboardModifiers modifiers, qreal argStep)
{
    mQuickSizingProperties.clear();
    if (mEditor->layers()->currentLayer()->type() == Layer::VECTOR)
    {
        mQuickSizingProperties.insert(Qt::ShiftModifier, WIDTH);
    }
    else
    {
        mQuickSizingProperties.insert(Qt::ControlModifier, TOLERANCE);
    }
    return BaseTool::startAdjusting(modifiers, argStep);
}

void BucketTool::paintBitmap(Layer* layer)
{
    Layer* targetLayer = layer; // by default
    int layerNumber = editor()->layers()->currentLayerIndex(); // by default

    BitmapImage* targetImage = static_cast<LayerBitmap*>(targetLayer)->getLastBitmapImageAtFrame(editor()->currentFrame(), 0);
    if (targetImage == nullptr) { return; } // Can happen if the first frame is deleted while drawing

    QPoint point = QPoint(qFloor(getLastPoint().x()), qFloor(getLastPoint().y()));
    QRect cameraRect = mScribbleArea->getCameraRect().toRect();

    QRgb fillColor = qPremultiply(mEditor->color()->frontColor().rgba());
    QRgb origColor = fillColor;
    if (properties.fillMode == 0)
    {
        if (qAlpha(fillColor) == 0)
        {
            // Filling in overlay mode with a fully transparent color has no
            // effect, so we can skip it in this case
            return;
        }
    }
    else if (properties.fillMode == 1)
    {
        // Pass a fully opaque version of the new color to floodFill
        // This is required so we can fully mask out the existing data before
        // writing the new color.
        QColor tempColor;
        tempColor.setRgba(fillColor);
        tempColor.setAlphaF(1);
        fillColor = tempColor.rgba();
    }

    std::unique_ptr<BitmapImage> fillImage(
                BitmapImage::floodFill(targetImage,
                                       cameraRect,
                                       point,
                                       fillColor,
                                       properties.tolerance));

    if (fillImage == nullptr)
    {
        // Nothing was filled for whatever reason
        return;
    }

    switch(properties.fillMode)
    {
    default:
    case 0: // Overlay mode
        // Write fill image on top of target image
        targetImage->paste(fillImage.get());
        break;
    case 1: // Replace mode
        if (qAlpha(origColor) == 0xFF)
        {
            // When the new color is fully opaque, replace mode
            // behaves exactly like overlay mode, and origColor == fillColor
            targetImage->paste(fillImage.get());
        }
        else
        {
            // Clearly all pixels in the to-be-filled region from the target image
            targetImage->paste(fillImage.get(), QPainter::CompositionMode_DestinationOut);
            // Reduce the opacity of the fill to match the new color
            BitmapImage properColor(targetImage->bounds(), QColor::fromRgba(origColor));
            properColor.paste(fillImage.get(), QPainter::CompositionMode_DestinationIn);
            // Write reduced-opacity fill image on top of target image
            targetImage->paste(&properColor);
        }
        break;
    }

    mScribbleArea->setModified(layerNumber, mEditor->currentFrame());
}

void BucketTool::paintVector(Layer* layer)
{
    mScribbleArea->clearBitmapBuffer();

    VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
    if (vectorImage == nullptr) { return; } // Can happen if the first frame is deleted while drawing

    if (!vectorImage->isPathFilled())
    {
        vectorImage->fillSelectedPath(mEditor->color()->frontColorNumber());
    }

    vectorImage->applyWidthToSelection(properties.width);
    vectorImage->applyColorToSelectedCurve(mEditor->color()->frontColorNumber());
    vectorImage->applyColorToSelectedArea(mEditor->color()->frontColorNumber());

    applyChanges();

    mScribbleArea->setModified(mEditor->layers()->currentLayerIndex(), mEditor->currentFrame());
}

void BucketTool::applyChanges()
{
    mScribbleArea->applyTransformedSelection();
}

void BucketTool::drawStroke()
{
    StrokeTool::drawStroke();

    if (properties.stabilizerLevel != strokeManager()->getStabilizerLevel())
    {
        strokeManager()->setStabilizerLevel(properties.stabilizerLevel);
    }

    QList<QPointF> p = strokeManager()->interpolateStroke();

    Layer* layer = mEditor->layers()->currentLayer();

    if (layer->type() == Layer::VECTOR)
    {
        mCurrentWidth = 30;
        int rad = qRound((mCurrentWidth / 2 + 2) * mEditor->view()->scaling());

        QColor pathColor = qPremultiply(mEditor->color()->frontColor().rgba());
        //pathColor.setAlpha(255);

        QPen pen(pathColor,
                 mCurrentWidth * mEditor->view()->scaling(),
                 Qt::NoPen,
                 Qt::RoundCap,
                 Qt::RoundJoin);

        if (p.size() == 4)
        {
            QPainterPath path(p[0]);
            path.cubicTo(p[1], p[2], p[3]);
            mScribbleArea->drawPath(path, pen, Qt::NoBrush, QPainter::CompositionMode_Source);
            mScribbleArea->refreshVector(path.boundingRect().toRect(), rad);
        }
    }
}
