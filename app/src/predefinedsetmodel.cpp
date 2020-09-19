// PredefinedSetModel.cpp
#include "predefinedsetmodel.h"

PredefinedSetModel::PredefinedSetModel(QObject *parent, const PredefinedKeySet& mKeySet)
    :QAbstractTableModel(parent), mKeySet(mKeySet)
{
}

int PredefinedSetModel::rowCount(const QModelIndex & /*parent*/) const
{
   return mKeySet.size();
}

int PredefinedSetModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 2;
}

QVariant PredefinedSetModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            return mKeySet.type(section);
        }
    }
    return QVariant();
}

QVariant PredefinedSetModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        const int row = index.row();
        const int column = index.column();

        const int& keyFrameIndex = mKeySet.keyFrameIndexAt(row);
        const QString& filePath = mKeySet.filePathAt(row);
        if (column == PredefinedKeySet::ColumnType::FILES) {
            return QString("%1").arg(filePath);
        }
        if (column == PredefinedKeySet::ColumnType::KEYFRAMEPOS) {
            return QString("%1").arg(keyFrameIndex);
        }
    }
    return QVariant();
}
