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
#ifndef PREDEFINEDSETMODEL_H
#define PREDEFINEDSETMODEL_H

// predefinedsetmodel.h
#include <QAbstractTableModel>
#include <QCoreApplication>

struct PredefinedKeySet
{
    Q_DECLARE_TR_FUNCTIONS(PredefinedKeySet)
public:
    int size() const { return mKeyframeIndexes.size(); }
    void insert(const int& keyFrameIndex, const QString& filePath)
    {
        mKeyframeIndexes.append(keyFrameIndex);
        mFilePaths.append(filePath);
    }

    int keyFrameIndexAt(const int& index) const
    {
        return mKeyframeIndexes.at(index);
    }

    QString filePathAt(const int& index) const
    {
        return mFilePaths.at(index);
    }

    bool contains(const QString& path) const {
        return mFilePaths.contains(path);
    }

    bool isEmpty() const {
        return mFilePaths.isEmpty();
    }

    enum ColumnType {
        FILES,
        KEYFRAMEPOS
    };

    const QString layerName() { return mLayerName; }
    void setLayerName(const QString& layerName) { mLayerName = layerName; }

    QString type(const int& index) const {
        switch(index)
        {
            case 0:
            return tr("Files");
            case 1:
            return tr("KeyFrame Pos");
            default:
            return "";
        }
    }

private:
    QList<QString> mFilePaths;
    QList<int> mKeyframeIndexes;
    QString mLayerName;

};

class PredefinedSetModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    PredefinedSetModel(QObject *parent, const PredefinedKeySet& keyFrameSet);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    const PredefinedKeySet mKeySet;
};

#endif // PREDEFINEDSETMODEL_H
