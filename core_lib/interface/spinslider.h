#ifndef SPINSLIDER_H
#define SPINSLIDER_H

#include <QWidget>

class QLabel;
class QSlider;


class SpinSlider : public QWidget
{
    Q_OBJECT
public:
    enum GROWTH_TYPE
    {
        LINEAR,
        LOG,
    };

    enum VALUE_TYPE
    {
        INTEGER,
        FLOAT,
    };

    SpinSlider( QString text, GROWTH_TYPE, VALUE_TYPE, qreal min, qreal max, QWidget* parent = 0 );
    void setValue( qreal );
    
signals:
    void valueChanged(qreal);

private:
    void onSliderValueChanged( int );
    void sliderReleased();
    void sliderMoved( int );
    void changeValue( qreal );

private:
    QLabel*  mValueLabel = nullptr;
    QSlider* mSlider     = nullptr;
    qreal mValue = 50.0;
    qreal mMin   = 0.1;
    qreal mMax   = 100.0;

    GROWTH_TYPE mGrowthType = LINEAR;
    VALUE_TYPE  mValueType  = INTEGER;
};

#endif // SPINSLIDER_H
