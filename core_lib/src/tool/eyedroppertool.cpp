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

#include "eyedroppertool.h"

#include <QPainter>
#include <QPixmap>
#include <QBitmap>
#include <QtMath>
#include "pointerevent.h"

#include "vectorimage.h"
#include "layervector.h"
#include "layerbitmap.h"
#include "colormanager.h"
#include "object.h"
#include "editor.h"
#include "layermanager.h"
#include "scribblearea.h"
#include "util.h"

EyedropperTool::EyedropperTool(QObject* parent) : BaseTool(parent)
{
}

void EyedropperTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
    properties.useFeather = false;
    properties.useAA = -1;
}

QCursor EyedropperTool::cursor()
{
    if (mEditor->preference()->isOn(SETTING::TOOL_CURSOR))
    {
        return QCursor(QPixmap(":icons/eyedropper.png"), 0, 15);
    }
    else
    {
        return QCursor(QPixmap(":icons/cross.png"), 10, 10);
    }
}

QCursor EyedropperTool::cursor(const QColor color)
{
    QPixmap icon(":icons/eyedropper.png");

    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.drawPixmap(0, 0, icon);

    painter.setBrush(Qt::white);
    painter.drawRect(17, 17, 13, 13);
    painter.setPen(QPen(Qt::white, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(color);
    painter.drawRect(16, 16, 15, 15);
    painter.end();

    return QCursor(pixmap, 0, 15);
}

void EyedropperTool::pointerPressEvent(PointerEvent*)
{}

void EyedropperTool::pointerMoveEvent(PointerEvent*)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr) { return; }

    if (layer->type() == Layer::BITMAP)
    {
        QColor pickedColor = getBitmapColor(static_cast<LayerBitmap*>(layer));
        if (pickedColor.isValid())
        {
            mScribbleArea->setCursor(cursor(pickedColor));
        }
        else
        {
            mScribbleArea->setCursor(cursor());
        }
    }
    if (layer->type() == Layer::VECTOR)
    {
        int pickedColor = getVectorColor(static_cast<LayerVector*>(layer));
        if (pickedColor >= 0)
        {
            mScribbleArea->setCursor(cursor(mEditor->object()->getColor(pickedColor).color));
        }
        else
        {
            mScribbleArea->setCursor(cursor());
        }
    }
}

void EyedropperTool::pointerReleaseEvent(PointerEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        updateFrontColor();

        // reset cursor
        mScribbleArea->setCursor(cursor());
    }
}

void EyedropperTool::updateFrontColor()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr) { return; }
    if (layer->type() == Layer::BITMAP)
    {
        QColor pickedColor = getBitmapColor(static_cast<LayerBitmap*>(layer));
        if (pickedColor.isValid())
        {
            mEditor->color()->setFrontColor(pickedColor);
        }
    }
    else if (layer->type() == Layer::VECTOR)
    {
        int pickedColor = getVectorColor(static_cast<LayerVector*>(layer));
        if (pickedColor >= 0)
        {
            mEditor->color()->setColorNumber(pickedColor);
        }
    }
}

QColor EyedropperTool::getBitmapColor(LayerBitmap* layer)
{
    BitmapImage* targetImage = layer->getLastBitmapImageAtFrame(mEditor->currentFrame(), 0);
    if (targetImage == nullptr || !targetImage->contains(getCurrentPoint())) return QColor();

    QColor pickedColour;
    const QRgb pixelColor = targetImage->constScanLine(qFloor(getCurrentPoint().x()),
                                                       qFloor(getCurrentPoint().y()));
    pickedColour.setRgba(pixelColor);

    if (pickedColour.alpha() <= 0) pickedColour = QColor();
    return pickedColour;
}

int EyedropperTool::getVectorColor(LayerVector* layer)
{
    auto vectorImage = static_cast<VectorImage*>(layer->getLastKeyFrameAtPosition(mEditor->currentFrame()));
    if (vectorImage == nullptr) return -1;

    // Check curves
    const qreal toleranceDistance = 10.0;
    const QList<int> closestCurves = vectorImage->getCurvesCloseTo(getCurrentPoint(), toleranceDistance);
    const QList<int> visibleClosestCurves = filter(closestCurves, [vectorImage](int i) { return vectorImage->isCurveVisible(i); });

    if (!visibleClosestCurves.isEmpty())
    {
        return vectorImage->getCurvesColor(visibleClosestCurves.last());
    }

    // Check fills
    int colorNumber = vectorImage->getColorNumber(getCurrentPoint());
    return colorNumber;
}
