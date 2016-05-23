
#include <cmath>
#include <QLabel>
#include <QSlider>
#include <QGridLayout>
#include <QLocale>
#include <QDebug>
#include "spinslider.h"


SpinSlider::SpinSlider( QString text, GROWTH_TYPE type, VALUE_TYPE dataType, qreal min, qreal max, QWidget* parent ) : QWidget( parent )
{
    if ( type == LOG )
    {
        // important! dividing by zero is not acceptable.
        Q_ASSERT_X( min > 0.f , "SpinSlider" , "Real type value must larger than 0!!" );
    }

    mValue = 1.0;
    mGrowthType = type;
    mValueType = dataType;
    mMin = min;
    mMax = max;

    QLabel* label = new QLabel(text+": ");
    label->setFont( QFont( "Helvetica", 10 ) );

    mSlider = new QSlider(Qt::Horizontal, this);
    mSlider->setMinimum( 0 );
    mSlider->setMaximum( 100 );
    mSlider->setMaximumWidth( 70 );

    QGridLayout* layout = new QGridLayout();
    layout->setMargin( 2 );
    layout->setSpacing( 2 );

    layout->addWidget( label, 0, 0, 1, 1 );
    layout->addWidget( mSlider, 1, 0, 1, 2 );

    setLayout( layout );
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );

    connect( mSlider, &QSlider::valueChanged,   this, &SpinSlider::onSliderValueChanged );
    connect( mSlider, &QSlider::sliderReleased, this, &SpinSlider::sliderReleased );
    connect( mSlider, &QSlider::sliderMoved,    this, &SpinSlider::sliderMoved );
}

void SpinSlider::changeValue(qreal value)
{
    mValue = value;
    emit valueChanged( value );
}

void SpinSlider::onSliderValueChanged( int v )
{
    //qDebug() << "Value changed!! " << v;
    qreal value2 = 0.0;
    if ( mGrowthType == LINEAR )
    {
        value2 = mMin + v * ( mMax - mMin ) / mSlider->maximum();
    }
    else if ( mGrowthType == LOG )
    {
        value2 = mMin * std::exp( v * std::log( mMax / mMin ) / mSlider->maximum() );
    }
    else if ( mGrowthType == EXPONENT ) {
        value2 = mMin + std::pow( v, mExp ) * ( mMax - mMin ) / std::pow( mSlider->maximum(), mExp );
    }
    changeValue( value2 );
}

void SpinSlider::setValue( qreal v )
{
    //qDebug() << "setValue!!" << v;
    int value2 = 0;
    if ( mGrowthType == LINEAR )
    {
        value2 = std::round( mSlider->maximum() * ( v - mMin ) / ( mMax - mMin ) );
    }
    else if ( mGrowthType == LOG )
    {
        value2 = std::round( std::log( v / mMin ) * mSlider->maximum() / std::log( mMax / mMin ) );
    }
    else if ( mGrowthType == EXPONENT )
    {
        value2 = std::round( std::pow( ( v - mMin ) * std::pow( mSlider->maximum(), mExp ) / ( mMax - mMin ), 1 / mExp ));
    }
    //qDebug() << "Position! " << value2;

    changeValue( v );
    mSlider->setSliderPosition( value2 );
}

void SpinSlider::setExponent( const qreal exp )
{
    mExp = exp;
}

void SpinSlider::sliderReleased()
{
}

void SpinSlider::sliderMoved(int value)
{
}
