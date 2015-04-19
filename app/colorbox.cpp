#include <QVBoxLayout>
#include "colorwheel.h"
#include "colorinspector.h"
#include "colorbox.h"

ColorBox::ColorBox(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    m_colorWheel = new ColorWheel(this);
    m_colorInspector = new ColorInspector(this);

    layout->addWidget(m_colorWheel);
    layout->addWidget(m_colorInspector);

    setLayout(layout);

    connect(m_colorWheel, &ColorWheel::colorChanged, this, &ColorBox::onWheelChange);
    connect(m_colorInspector, &ColorInspector::colorChanged, this, &ColorBox::onSpinboxChange);

    m_colorWheel->setColor(Qt::black);
    m_colorInspector->setColor(Qt::black);
}

ColorBox::~ColorBox()
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

void ColorBox::onWheelChange(const QColor& color)
{
    if ( m_colorInspector->color() != color )
    {
        m_colorInspector->setColor(color);
        emit colorChanged(color);
    }
}
