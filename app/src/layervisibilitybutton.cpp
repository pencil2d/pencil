/*
Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2008-2009 Mj Mendoza IV
Copyright (C) 2012-2020 Matthew Chiawen Chang
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/
#include "layervisibilitybutton.h"

#include "layer.h"
#include "editor.h"
#include "layermanager.h"

#include <QIcon>
#include <QPainter>
#include <QApplication>
#include <QDebug>
#include <QMouseEvent>

LayerVisibilityButton::LayerVisibilityButton(QWidget* parent,
                                             const LayerVisibilityContext& visibilityContext,
                                             Layer* layer,
                                             Editor* editor)
    : QWidget(parent),
      mLayer(layer),
      mEditor(editor),
      mVisibilityContext(visibilityContext)
{
}

QSize LayerVisibilityButton::sizeHint() const
{
    return QSize(22,22);
}

void LayerVisibilityButton::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    const QPalette& palette = QApplication::palette();

    switch (mVisibilityContext) {
    case LayerVisibilityContext::LOCAL:
        paintLocalVisibilityState(painter, palette);
        break;
    case LayerVisibilityContext::GLOBAL:
        paintGlobalVisibilityState(painter, palette);
        break;
    }

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawEllipse(rect().center().x() - (mVisibilityCircleSize.width() * 0.5) + painter.pen().width(),
                        rect().center().y() - (mVisibilityCircleSize.height() * 0.5) + painter.pen().width(),
                        mVisibilityCircleSize.width(), mVisibilityCircleSize.height());
    painter.setRenderHint(QPainter::Antialiasing, false);

    mIconValid = true;
}

void LayerVisibilityButton::paintLocalVisibilityState(QPainter& painter, const QPalette& palette)
{
    int isSelected = mLayer->id() == mEditor->layers()->currentLayer()->id();

    const LayerVisibility& visibility = mEditor->layerVisibility();
    QColor penColor = painter.pen().color();

    if (mEditor->layers()->currentLayer()->id() == mLayer->id())
    {
        penColor = palette.color(QPalette::HighlightedText);
    }
    else
    {
        penColor = palette.color(QPalette::Text);
    }

    if (!mLayer->visible())
    {
        if (visibility == LayerVisibility::CURRENTONLY && !isSelected) {
            penColor.setAlphaF(0.5);
        }
        painter.setPen(penColor);
        painter.setBrush(palette.color(QPalette::Base));
    }
    else
    {
        if ((visibility == LayerVisibility::ALL) || isSelected)
        {
            painter.setBrush(palette.color(QPalette::Text));
        }
        else if (visibility == LayerVisibility::CURRENTONLY)
        {
            painter.setBrush(palette.color(QPalette::Base));
        }
        else if (visibility == LayerVisibility::RELATED)
        {
            QColor color = palette.color(QPalette::Text);
            color.setAlpha(128);
            painter.setBrush(color);
        }
    }

    painter.setPen(penColor);
}

void LayerVisibilityButton::paintGlobalVisibilityState(QPainter &painter, const QPalette& palette)
{
    // --- draw circle
    painter.setPen(palette.color(QPalette::Text));
    if (mEditor->layerVisibility() == LayerVisibility::CURRENTONLY)
    {
        painter.setBrush(palette.color(QPalette::Base));
    }
    else if (mEditor->layerVisibility() == LayerVisibility::RELATED)
    {
        QColor color = palette.color(QPalette::Text);
        color.setAlpha(128);
        painter.setBrush(color);
    }
    else if (mEditor->layerVisibility() == LayerVisibility::ALL)
    {
        painter.setBrush(palette.brush(QPalette::Text));
    }
}

void LayerVisibilityButton::mousePressEvent(QMouseEvent* event)
{
    switch (mVisibilityContext) {
        case LayerVisibilityContext::GLOBAL: {
            if (event->buttons() & Qt::LeftButton) {
                mEditor->increaseLayerVisibilityIndex();
            } else if (event->buttons() & Qt::RightButton) {
                mEditor->decreaseLayerVisibilityIndex();
            }
            break;
        }
        case LayerVisibilityContext::LOCAL:
        {
            mLayer->switchVisibility();
        }
    }

    emit visibilityChanged();
}
