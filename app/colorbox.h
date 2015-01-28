#ifndef COLORBOX_H
#define COLORBOX_H

#include <QWidget>

class ColorWheel;
class ColorInspector;

class ColorBox : public QWidget
{
    Q_OBJECT

public:
    explicit ColorBox(QWidget *parent = 0);
    ~ColorBox();

    QColor color();
    void setColor(const QColor &);

Q_SIGNALS:
    void colorChanged(const QColor &);

private:
    void onSpinboxChange(const QColor &);
    void onWheelChange(const QColor &);

    ColorWheel* m_colorWheel;
    ColorInspector* m_colorInspector;
};

#endif // COLORBOX_H
