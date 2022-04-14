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

        if (column == PredefinedKeySet::ColumnType::FILES)
        {
            return mKeySet.filePathAt(row);
        }
        else if (column == PredefinedKeySet::ColumnType::KEYFRAMEPOS)
        {
            return QString::number(mKeySet.keyFrameIndexAt(row));
        }
    }
    return QVariant();
}
