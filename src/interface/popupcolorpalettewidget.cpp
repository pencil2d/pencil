#include <QtGui>
#include <QBoxLayout>
#include "colorbox.h"
#include "popupcolorpalettewidget.h"

PopupColorPaletteWidget::PopupColorPaletteWidget( ScribbleArea *parent ) :
    QWidget ( parent, Qt::Window ),
    m_container ( parent )
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    setVisible(false);
    m_colorBox = new ColorBox(this);
    m_colorBox->adjustSize();
    layout->addWidget(m_colorBox);
    adjustSize();
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
    effect->setXOffset(2);
    effect->setYOffset(2);
    effect->setBlurRadius(5);
    setGraphicsEffect(effect);
    setAutoFillBackground(true);
    setWindowTitle("Color palette");
}

bool PopupColorPaletteWidget::popup()
{
    if ( this->isVisible() )
    {
        this->color = m_colorBox->color();
        hide();
        return true;
    }
    QPoint cPos = QCursor::pos();
    int w = width();
    int h = height();
    int radius = w/2;

    cPos.setX(cPos.x()-radius); // adjust cPos to center widget
    cPos.setY(cPos.y()-radius);

    if ( cPos.x()<2 )
    {
        cPos.setX(2);
    }
    else if ( cPos.x()+ w > m_container->width()-7)
    {
        cPos.setX( m_container->width()-w-7 );
    }
    if ( cPos.y()<2 )
    {
        cPos.setY( 2 );
    }
    else if ( cPos.y()+ h > m_container->height()-7)
    {
        cPos.setY( m_container->height()-h-7 );
    }
    move( cPos );
    show();
    return false;
}

void PopupColorPaletteWidget::keyPressEvent(QKeyEvent *event)
{
    m_container->keyPressed( event );
}

