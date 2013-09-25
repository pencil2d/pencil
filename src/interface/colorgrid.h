#ifndef COLORGRID_H
#define COLORGRID_H

#include <QScrollArea>

class FlowLayout;
class ColorGridItem;

class ColorGrid : public QScrollArea
{
    Q_OBJECT
public:
    explicit ColorGrid(QWidget *parent = 0);
    virtual QSize sizeHint () const;
    virtual QSize minimumSizeHint () const;
    QByteArray dataExport();
    bool dataImport(const QByteArray &array);
signals:
    void colorDroped(const int &, const QColor &);
    void colorDroped(const int &);
    void colorPicked(const int &, const QColor &);
    
public slots:
    void setColor(const int &, const QColor &);
protected:
    void initItems();
    FlowLayout *layout_;
    QList<ColorGridItem *> items;
    static const int gridCount = 100;
};

#endif // COLORGRID_H
