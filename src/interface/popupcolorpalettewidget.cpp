#include "colorbox.h"
#include "popupcolorpalettewidget.h"

PopupColorPaletteWidget::PopupColorPaletteWidget( QWidget *parent ) :
    QWidget ( parent )
{
    QVBoxLayout *clayout = new QVBoxLayout(this);

    setVisible(false);
    //m_popupColorWidget->setPalette( QPalette( Qt::color0) );
    m_colorBox = new ColorBox(this);
    m_colorBox->adjustSize();
    clayout->addWidget(m_colorBox);
    adjustSize();
    this->setWindowOpacity(0.5);
}

