#ifndef COLORWHEEL_H
#define COLORWHEEL_H

#include <QWidget>


class ColorWheel : public QWidget
{
    Q_OBJECT
public:
    explicit ColorWheel(QWidget *parent = 0);

    virtual QSize sizeHint () const;
    virtual QSize minimumSizeHint () const;
    QColor color();
    
signals:
    void colorChanged(const QColor &color);
    
public slots:
    void setColor(const QColor &color);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *);

private:
    void hueChanged(const int &hue);
    void svChanged(const QColor &newcolor);

    QColor pickColor(const QPoint &point);
    
    void drawHueIndicator(const int &hue);
    void drawPicker(const QColor &color);

    void drawWheelImage(const QSize &newSize);
    void drawSquareImage(const int &hue);
    void composeWheel(QPixmap& pixmap);

    QSize initSize;
    QImage wheelImage;
    QImage squareImage;
    QPixmap wheel;
   
    int wheelWidth;
    QRegion wheelRegion;
    QRegion squareRegion;
    QColor currentColor;
    bool inWheel;
    bool inSquare;
    
};

#endif // COLORWHEEL_H
