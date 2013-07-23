#ifndef COLORBOX_H
#define COLORBOX_H

#include <QWidget>

namespace Ui {
class ColorBox;
}

class ColorBox : public QWidget
{
    Q_OBJECT
    
public:
    explicit ColorBox(QWidget *parent = 0);
    ~ColorBox();
    QColor color();
signals:
    void colorChanged(const QColor &);
public slots:
    void setColor(const QColor &);
private slots:
    void onSpinboxChange(const QColor &);
    void onWheelChange(const QColor &);
private:
    Ui::ColorBox *ui;
};

#endif // COLORBOX_H
