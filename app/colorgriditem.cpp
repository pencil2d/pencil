/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "colorgriditem.h"

#include <QMouseEvent>
#include <QMenu>
#include <QCursor>
#include <QPainter>
#include <QStyleOptionViewItem>
#include "colorwheel.h"

static const int gridWidth = 16;


ColorGridItem::ColorGridItem(int id, QWidget *parent) :
    QWidget(parent),
    mGridId(id),
    menu(this),
    mColor(Qt::transparent),
    mHovered(false),
    picker(QPixmap("iconset/ui/picker-cursor.png"))
{
    setContentsMargins(0,0,0,0);
    drawTransparent();
    setMouseTracking(true);
    initMenu();
}

void ColorGridItem::initMenu()
{
    menu.addAction(tr("Pick"), this,SLOT(onColorPicked()));
    menu.addAction(tr("Tint"),this,SLOT(onColorDroped()));
    menu.addAction(tr("Clear"),this,SLOT(onColorCleared()));
    menu.addAction(tr("Cancel"));
}

void ColorGridItem::drawTransparent()
{
    mBackgroundImg = QPixmap(gridWidth,gridWidth);
    mBackgroundImg.fill(Qt::white);
    QPainter painter(&mBackgroundImg);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(224, 224, 224));
    int k = gridWidth/2;
    painter.drawRect(k,0,k,k);
    painter.drawRect(0,k,k,k);
    painter.setBrush(QColor(240, 240, 240));
    painter.drawRect(0,0,k,k);
    painter.drawRect(k,k,k,k);
}

void ColorGridItem::paintEvent(QPaintEvent *)
{
    QPalette pal = this->palette();
    QPen borderPen(pal.color(QPalette::WindowText));

    QPainter painter(this);
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.fillRect(0,0,gridWidth, gridWidth, QBrush(Qt::white));
    painter.restore();
    if(mColor == QColor(Qt::transparent)){
        painter.drawPixmap(0,0,mBackgroundImg);
    }else{
        painter.save();
        painter.setBrush(mColor);
        painter.setPen(Qt::NoPen);
        painter.drawRect(0,0,gridWidth, gridWidth);
        painter.restore();
    }
    if(mHovered){
        painter.setPen(borderPen);
        painter.drawRect(0, 0, gridWidth-1, gridWidth-1);
    }
}

void ColorGridItem::enterEvent(QEvent* )
{
    mHovered = true;
    setCursor(picker);
    update();
}

void ColorGridItem::leaveEvent(QEvent* )
{
    mHovered = false;
    unsetCursor();
    update();
}

void ColorGridItem::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        menu.popup(mapToGlobal(event->pos()));
    }else{
        if(color() != Qt::transparent) onColorPicked();
    }
    update();
}

QColor ColorGridItem::color()
{
    return mColor;
}

void ColorGridItem::setColor(const QColor& color)
{
    mColor = color;
    update();
}

void ColorGridItem::onColorPicked()
{
    emit colorPicked(this->mGridId, this->mColor);
}

void ColorGridItem::onColorDroped()
{
    emit colorDroped(this->mGridId);
}

void ColorGridItem::onColorCleared()
{
    this->setColor(Qt::transparent);
    update();
}

QSize ColorGridItem::sizeHint() const
{
    return QSize(gridWidth, gridWidth);
}

QSize ColorGridItem::minimumSizeHint() const
{
    return QSize(gridWidth, gridWidth);
}