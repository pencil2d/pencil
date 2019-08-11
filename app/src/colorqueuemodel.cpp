#include "colorqueuemodel.h"

#include <QPainter>
#include <QDebug>

ColorQueueModel::ColorQueueModel(QObject *parent, const QColor& backgroundColor)
    : QAbstractListModel(parent), mBaseBackgroundColor(backgroundColor)
{
}

ColorQueueModel::~ColorQueueModel()
{
}

void ColorQueueModel::addColor(const Swatch& swatch)
{
    if (mSwatches.count() == MAXSWATCHES) {
        mSwatches.removeLast();
    }

    beginInsertRows(QModelIndex(), 0, mSwatches.count());
    mSwatches.prepend(swatch);
    endInsertRows();
}

int ColorQueueModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mSwatches.count();
}

QVariant ColorQueueModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DecorationRole)
    {
       const int row = index.row();

       const Swatch& swatch = mSwatches.at(row);
       const QColor& color = swatch.color;
       QPixmap pixmap = QPixmap(QSize(16,16));
       pixmap.fill(Qt::transparent);
       QPainter painter(&pixmap);
       painter.drawTiledPixmap(QRect(QPoint(0,0),pixmap.size()), QPixmap("://icons/new/checkerboard_smaller"));
       painter.setPen(mBaseBackgroundColor);
       painter.fillRect(pixmap.rect(), color);
       painter.setCompositionMode(QPainter::CompositionMode_Overlay);
       painter.drawRect(pixmap.rect().adjusted(0,0,-1,-1));

       if (swatch.selected) {
           painter.setCompositionMode(QPainter::CompositionMode_Source);
           painter.setRenderHint(QPainter::Antialiasing);
           QPen pen;
           pen.setWidth(2);
           pen.setColor(Qt::white);
           painter.setPen(pen);
           painter.drawRect(pixmap.rect().adjusted(1,1,-1,-1));
       }
       return pixmap;
    } else if (role == Qt::ToolTipRole) {
        const int row = index.row();
        return mSwatches.at(row).description;
    }
    return QVariant();
}

void ColorQueueModel::setSwatchSelected(int index, bool state)
{
    Swatch& swatch = mSwatches[index];
    swatch.selected = state;
}

void ColorQueueModel::pushSwatchesLeft()
{
    if (mSwatches.count() < 2) { return; }
    mSwatches.push_front(mSwatches.last());
    mSwatches.pop_back();
}

void ColorQueueModel::pushSwatchesRight()
{
    if (mSwatches.count() < 2) { return; }
    mSwatches.push_back(mSwatches.first());
    mSwatches.pop_front();
}
