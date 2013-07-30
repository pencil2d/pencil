
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

void ColorBox::setColor(const QColor &c)
{
    onSpinboxChange(c);
}

void ColorBox::onSpinboxChange(const QColor &c)
{
    if ( m_colorWheel->color() != c )
    {
        m_colorWheel->setColor(c);
        emit colorChanged(c);
    }
}

void ColorBox::onWheelChange(const QColor &c)
{
    if ( m_colorInspector->color() != c )
    {
        m_colorInspector->setColor(c);
        emit colorChanged(c);
    }
}
