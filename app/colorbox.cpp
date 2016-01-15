#include <QVBoxLayout>
#include "colorwheel.h"
#include "colorinspector.h"
#include "colorbox.h"

ColorBox::ColorBox( const QString& strTitle, QWidget* parent ) : BaseDockWidget( strTitle, parent )
{
    QVBoxLayout* layout = new QVBoxLayout();

    m_colorWheel = new ColorWheel(this);
    m_colorInspector = new ColorInspector(this);

    layout->setContentsMargins(5,5,5,5);
    layout->addWidget(m_colorWheel);
    layout->addWidget(m_colorInspector);

    QWidget* mainWidget = new QWidget;
    mainWidget->setLayout(layout);

    setWidget( mainWidget );

    connect( m_colorWheel, &ColorWheel::colorChanged, this, &ColorBox::onWheelMove );
    connect( m_colorInspector, &ColorInspector::colorChanged, this, &ColorBox::onSpinboxChange );
    connect( m_colorWheel, &ColorWheel::colorSelected, this, &ColorBox::onWheelRelease );

    m_colorWheel->setColor(Qt::black);
    m_colorInspector->setColor(Qt::black);
    m_colorWheel->setMinimumSize(100,100);


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
    return m_colorWheel->color();
}

void ColorBox::setColor(const QColor& newColor)
{
    if ( newColor != m_colorWheel->color() )
    {
        m_colorWheel->setColor(newColor);
        m_colorInspector->setColor(newColor);

        emit colorChanged(newColor);
    }
}

void ColorBox::onSpinboxChange(const QColor& color)
{
    if ( m_colorWheel->color() != color )
    {
        m_colorWheel->setColor(color);
        emit colorChanged(color);
    }
}

void ColorBox::onWheelMove(const QColor& color)
{
    if ( m_colorInspector->color() != color )
    {
        m_colorInspector->setColor(color);
    }
}

void ColorBox::onWheelRelease(const QColor& color)
{
     m_colorInspector->setColor(color);
     emit colorChanged(color);
}
