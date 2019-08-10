#include "colorqueuemodel.h"

#include <QPainter>

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

       const QColor color = mSwatches.at(row).color;
       QPixmap pixmap = QPixmap(QSize(16,16));
       pixmap.fill(Qt::transparent);
       QPainter painter(&pixmap);
       painter.drawTiledPixmap(QRect(QPoint(0,0),pixmap.size()), QPixmap("://icons/new/checkerboard_smaller"));
       painter.setPen(mBaseBackgroundColor);
       painter.fillRect(pixmap.rect(), color);
       painter.setCompositionMode(QPainter::CompositionMode_Overlay);
       painter.drawRect(QRect(0,0,15,13));
       return pixmap;
    } else if (role == Qt::ToolTipRole) {
        const int row = index.row();
        return mSwatches.at(row).description;
    }
    return QVariant();
}
