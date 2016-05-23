#include <QVBoxLayout>
#include "colorwheel.h"
#include "colorinspector.h"
#include "colorbox.h"

ColorBox::ColorBox( const QString& strTitle, QWidget* parent ) : BaseDockWidget( strTitle, parent )
{
    QVBoxLayout* layout = new QVBoxLayout();

    mColorWheel = new ColorWheel(this);
    mColorInspector = new ColorInspector(this);

    layout->setContentsMargins(5,5,5,5);
    layout->addWidget(mColorWheel);
    layout->addWidget(mColorInspector);

    QWidget* mainWidget = new QWidget;
    mainWidget->setLayout(layout);

    setWidget( mainWidget );

    connect( mColorWheel, &ColorWheel::colorChanged, this, &ColorBox::onWheelMove );
    connect( mColorInspector, &ColorInspector::colorChanged, this, &ColorBox::onSpinboxChange );
    connect( mColorWheel, &ColorWheel::colorSelected, this, &ColorBox::onWheelRelease );

    mColorWheel->setColor(Qt::black);
    mColorInspector->setColor(Qt::black);
    mColorWheel->setMinimumSize(100,100);


}

ColorBox::~ColorBox()
{
}

void ColorBox::initUI()
{

}

void ColorBox::updateUI()
{

}

QColor ColorBox::color()
{
    return mColorWheel->color();
}

void ColorBox::setColor(const QColor& newColor)
{
    if ( newColor != mColorWheel->color() )
    {
        mColorWheel->setColor(newColor);
        mColorInspector->setColor(newColor);

        emit colorChanged(newColor);
    }
}

void ColorBox::onSpinboxChange(const QColor& color)
{
    if ( mColorWheel->color() != color )
    {
        mColorWheel->setColor(color);
        emit colorChanged(color);
    }
}

void ColorBox::onWheelMove(const QColor& color)
{
    if ( mColorInspector->color() != color )
    {
        mColorInspector->setColor(color);
    }
}

void ColorBox::onWheelRelease(const QColor& color)
{
     mColorInspector->setColor(color);
     emit colorChanged(color);
}
