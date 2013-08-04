
#include <QVBoxLayout>
#include "colorwheel.h"
#include "colorspinboxgroup.h"
#include "colorbox.h"

ColorBox::ColorBox(QWidget *parent) :
    QWidget(parent) 
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    m_colorWheel = new ColorWheel(this);
    m_colorInspector = new ColorSpinBoxGroup(this);

    layout->addWidget(m_colorWheel);
    layout->addWidget(m_colorInspector);
    
    setLayout(layout);

    connect(m_colorWheel, SIGNAL(colorChanged(QColor)),
            this, SLOT(onWheelChange(QColor)));
    connect(m_colorInspector, SIGNAL(colorChange(QColor)),
            this, SLOT(onSpinboxChange(QColor)));
}

ColorBox::~ColorBox()
{
}

QColor ColorBox::color()
{
    return m_colorWheel->color();
}

void ColorBox::setColor(const QColor& color)
{
    onSpinboxChange(color);
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
