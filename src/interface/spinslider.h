#ifndef SPINSLIDER_H
#define SPINSLIDER_H

#include <QWidget>

class QLabel;
class QSlider;


class SpinSlider : public QWidget
{
    Q_OBJECT
public:
    SpinSlider(QString text, QString type, QString dataType, qreal min, qreal max, QWidget* parent = 0);
public slots:
    void changeValue(qreal);
    void changeValue(int);
    void setValue(qreal);
    void sliderReleased();
    void sliderMoved(int);
signals:
    void valueChanged(qreal);
private:
    QLabel* valueLabel;
    QSlider* slider;
    qreal value, min, max;
    QString type, dataType;
};

#endif // SPINSLIDER_H
