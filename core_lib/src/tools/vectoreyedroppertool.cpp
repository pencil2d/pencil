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

#include "vectoreyedroppertool.h"

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


VectorEyedropperTool::VectorEyedropperTool(QObject* parent) : BaseTool(parent)
{
}

void VectorEyedropperTool::loadSettings()
{
    properties.width = -1;
    properties.feather = -1;
    properties.useFeather = false;
    properties.useAA = -1;
}

QCursor VectorEyedropperTool::cursor()
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

QCursor VectorEyedropperTool::cursor(const QColor colour)
{
    QPixmap icon(":icons/eyedropper.png");

    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.drawPixmap(0, 0, icon);
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(colour);
    painter.drawRect(16, 16, 15, 15);
    painter.end();

    return QCursor(pixmap, 0, 15);
}

void VectorEyedropperTool::pointerPressEvent(PointerEvent*)
{
    Q_ASSERT(mEditor->layers()->currentLayer()->type() == Layer::VECTOR);
}

void VectorEyedropperTool::pointerMoveEvent(PointerEvent*)
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == NULL) { return; }

    VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
    int colourNumber = vectorImage->getColourNumber(getCurrentPoint());
    if (colourNumber != -1)
    {
        mScribbleArea->setCursor(cursor(mEditor->object()->getColour(colourNumber).colour));
    }
    else
    {
        mScribbleArea->setCursor(cursor());
    }
}

void VectorEyedropperTool::pointerReleaseEvent(PointerEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        qDebug() << "was left button or tablet button";
        updateFrontColor();

        // reset cursor
        mScribbleArea->setCursor(cursor());
    }
}

void VectorEyedropperTool::updateFrontColor()
{
    Layer* layer = mEditor->layers()->currentLayer();
    if (layer == nullptr) { return; }

    VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
    int colourNumber = vectorImage->getColourNumber(getLastPoint());
    if (colourNumber != -1)
    {
        mEditor->color()->setColorNumber(colourNumber);
    }
}
