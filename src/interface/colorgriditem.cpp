#include "colorgriditem.h"

#include <QMouseEvent>
#include <QMenu>
#include <QCursor>
#include <QPainter>
#include <QStyleOptionViewItem>
#include "colorwheel.h"

ColorGridItem::ColorGridItem(int id, QWidget *parent) :
    QWidget(parent),
    gridId_(id),
    menu(this),
    color_(Qt::transparent),
    hovered_(false),
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
    backgroundImg = QPixmap(gridWidth,gridWidth);
    backgroundImg.fill(Qt::white);
    QPainter painter(&backgroundImg);
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
    if(color_ == QColor(Qt::transparent)){
        painter.drawPixmap(0,0,backgroundImg);
    }else{
        painter.save();
        painter.setBrush(color_);
        painter.setPen(Qt::NoPen);
        painter.drawRect(0,0,gridWidth, gridWidth);
        painter.restore();
    }
    if(hovered_){
        painter.setPen(borderPen);
        painter.drawRect(0, 0, gridWidth-1, gridWidth-1);
    }
}

void ColorGridItem::enterEvent(QEvent *)
{
    hovered_ = true;
    setCursor(picker);
    update();
}

void ColorGridItem::leaveEvent(QEvent *)
{
    hovered_ = false;
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
    return color_;
}

void ColorGridItem::setColor(const QColor& color)
{
    color_ = color;
    update();
}

void ColorGridItem::onColorPicked()
{
    emit colorPicked(this->gridId_, this->color_);
}

void ColorGridItem::onColorDroped()
{
    emit colorDroped(this->gridId_);
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