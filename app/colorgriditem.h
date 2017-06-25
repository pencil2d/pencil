#ifndef COLORGRIDITEM_H
#define COLORGRIDITEM_H

#include <QWidget>
#include <QMenu>

class ColorGridItem : public QWidget
{
    Q_OBJECT
public:
    explicit ColorGridItem(int id, QWidget *parent = 0);
    QColor color();
    virtual QSize sizeHint () const;
    virtual QSize minimumSizeHint () const;

signals:
    void colorDroped(int);
    void colorPicked(int, const QColor &);

public slots:
//    void setSource(ColorWheel * wheel);
    void setColor(const QColor& color);
    void onColorPicked();
    void onColorDroped();
    void onColorCleared();
protected:
    void drawTransparent();
    void paintEvent(QPaintEvent *);
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);
    void mousePressEvent(QMouseEvent *event);
    void initMenu();

private:
    int mGridId;
    QMenu menu;
    QColor mColor;
    QPixmap mBackgroundImg;
    bool mHovered;
    QCursor picker;
};

#endif // COLORGRIDITEM_H
