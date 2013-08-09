#include <QtGui>
#include <QBoxLayout>
#include "colorbox.h"
#include "popupcolorpalettewidget.h"

PopupColorPaletteWidget::PopupColorPaletteWidget( QWidget *parent ) :
    QWidget ( parent )
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    setVisible(false);
    m_colorBox = new ColorBox(this);
    m_colorBox->adjustSize();
    layout->addWidget(m_colorBox);
    adjustSize();
    setWindowOpacity(0.5);
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
    effect->setXOffset(2);
    effect->setYOffset(2);
    effect->setBlurRadius(5);
    setGraphicsEffect(effect);
    setAutoFillBackground(true);

}

