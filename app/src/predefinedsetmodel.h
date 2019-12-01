#ifndef PREDEFINEDSETMODEL_H
#define PREDEFINEDSETMODEL_H

// predefinedsetmodel.h
#include <QAbstractTableModel>

struct PredefinedKeySet
{
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
            return "Files";
            case 1:
            return "KeyFrame Pos";
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
