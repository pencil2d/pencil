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

#include "eyedroppertool.h"

#include <QPainter>
#include <QPixmap>
#include <QBitmap>
#include "pointerevent.h"

#include "vectorimage.h"
#include "layervector.h"
#include "layerbitmap.h"
#include "colormanager.h"
#include "object.h"
#include "editor.h"
#include "layermanager.h"
#include "scribblearea.h"


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
        return Qt::CrossCursor;
    }
}

QCursor EyedropperTool::cursor(const QColor color)
{
    QPixmap icon(":icons/eyedropper.png");

    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.drawPixmap(0, 0, icon);
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
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
    if (layer == NULL) { return; }

    if (layer->type() == Layer::BITMAP)
    {
        auto targetImage = static_cast<BitmapImage*>(layer->getLastKeyFrameAtPosition(mEditor->currentFrame()));
        if (targetImage->contains(getCurrentPoint()))
        {
            QColor pickedColor;
            //pickedColor.setRgba(targetImage->pixel(getCurrentPoint().x(), getCurrentPoint().y()));
            pickedColor.setRgba(targetImage->pixel(getCurrentPoint().x(), getCurrentPoint().y()));
            int transp = 255 - pickedColor.alpha();
            pickedColor.setRed(pickedColor.red() + transp);
            pickedColor.setGreen(pickedColor.green() + transp);
            pickedColor.setBlue(pickedColor.blue() + transp);

            if (pickedColor.alpha() != 0)
            {
                mScribbleArea->setCursor(cursor(pickedColor));
            }
            else
            {
                mScribbleArea->setCursor(cursor());
            }
        }
        else
        {
            mScribbleArea->setCursor(cursor());
        }
    }
    if (layer->type() == Layer::VECTOR)
    {
        auto vectorImage = static_cast<VectorImage*>(layer->getLastKeyFrameAtPosition(mEditor->currentFrame()));
        int colorNumber = vectorImage->getColorNumber(getCurrentPoint());
        const qreal toleranceDistance = 10.0;
        QList<int> closestCurve = vectorImage->getCurvesCloseTo(getCurrentPoint(), toleranceDistance);
        if (colorNumber != -1)

        {
            mScribbleArea->setCursor(cursor(mEditor->object()->getColor(colorNumber).color));
        }
        else if(!closestCurve.isEmpty())
        {
            int curveColor = vectorImage->getCurvesColor(closestCurve.first());
            mScribbleArea->setCursor(cursor(mEditor->object()->getColor(curveColor).color));
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
        //qDebug() << "was left button or tablet button";
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
        auto targetImage = static_cast<BitmapImage*>(layer->getLastKeyFrameAtPosition(mEditor->currentFrame()));
        QColor pickedColor;
        pickedColor.setRgba(targetImage->pixel(getLastPoint().x(), getLastPoint().y()));
        int transp = 255 - pickedColor.alpha();
        pickedColor.setRed(pickedColor.red() + transp);
        pickedColor.setGreen(pickedColor.green() + transp);
        pickedColor.setBlue(pickedColor.blue() + transp);
        if (pickedColor.alpha() != 0)

        {
            mEditor->color()->setColor(pickedColor);
        }
    }
    else if (layer->type() == Layer::VECTOR)
    {
        auto vectorImage = static_cast<VectorImage*>(layer->getLastKeyFrameAtPosition(mEditor->currentFrame()));
        int colorNumber = vectorImage->getColorNumber(getLastPoint());
        const qreal toleranceDistance = 10.0;
        QList<int> closestCurve = vectorImage->getCurvesCloseTo(getCurrentPoint(), toleranceDistance);
        if (colorNumber != -1)

        {
            mEditor->color()->setColorNumber(colorNumber);
        }
        else if(!closestCurve.isEmpty())
        {
            int curveColor = vectorImage->getCurvesColor(closestCurve.first());
            mEditor->color()->setColorNumber(curveColor);
        }
    }
}
