#include "colorgrid.h"

#include <QDataStream>
#include <QScrollArea>
#include <QList>
#include "colorgriditem.h"
#include "colorwheel.h"
#include "flowlayout.h"

ColorGrid::ColorGrid(QWidget *parent) :
    QScrollArea(parent),
    layout_(0)
{
    initItems();
}

void ColorGrid::initItems()
{
    QWidget *widget = new QWidget(this);
    widget->setBackgroundRole(QPalette::Light);
    layout_ = new FlowLayout(widget, 3, 1, 1);
    for(int i=0;i<gridCount;++i){
        ColorGridItem * item = new ColorGridItem(i,this);
        connect(item, SIGNAL(colorDroped(int)),
                this, SIGNAL(colorDroped(int)));
        connect(item, SIGNAL(colorPicked(int,QColor)),
                this, SIGNAL(colorPicked(int,QColor)));
        items.append(item);
        layout_->addWidget(item);
    }
    widget->setLayout(layout_);
    this->setWidget(widget);
    setWidgetResizable(true);
}

void ColorGrid::setColor(const int &id, const QColor &c)
{
    if(items.count() <= id){
        return;
    }
    items[id]->setColor(c);
}

QByteArray ColorGrid::dataExport()
{
    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);
    for(int i=0;i<layout_->count();++i){
        ColorGridItem * item = items[i];
        if(item){
            stream << item->color();
        }
    }
    return array;
}

bool ColorGrid::dataImport(const QByteArray &array)
{
    QDataStream stream(array);
    for(int i=0;i<layout_->count();++i){
        ColorGridItem * item = items[i];
        if(item){
            QColor color;
            stream >> color;
            item->setColor(color);
        }
    }
    return true;
}

QSize ColorGrid::sizeHint () const
{
    return QSize(width(), height());
}
QSize ColorGrid::minimumSizeHint () const
{
    return QSize(100, 100);
}
