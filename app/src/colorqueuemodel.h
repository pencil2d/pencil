#ifndef COLORQUEUEMODEL_H
#define COLORQUEUEMODEL_H

#include <QAbstractListModel>

#include <QPixmap>

struct Swatch {
    QString description;
    QColor color;
    bool selected;

    Swatch(QString description, QColor color, bool selected) :
        description(description), color(color), selected(selected)
    {}
};

class ColorQueueModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ColorQueueModel(QObject *parent, const QColor& backgroundColor = QColor());
    ~ColorQueueModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addColor(const Swatch& swatch);
    void setSwatchSelected(int index, bool state);

    QList<Swatch> swatches() { return mSwatches; }
    void pushSwatchesLeft();
    void pushSwatchesRight();

private:

    QList<Swatch> mSwatches;
    QColor mBaseBackgroundColor;

    constexpr static int MAXSWATCHES = 10;
    constexpr static int TRAVERSETHRESHOLD = 5;
};

#endif // COLORQUEUEMODEL_H
