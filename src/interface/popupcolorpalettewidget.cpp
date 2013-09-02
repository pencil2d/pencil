#include <QtGui>
#include <QBoxLayout>
#include "colorbox.h"
#include "popupcolorpalettewidget.h"

PopupColorPaletteWidget::PopupColorPaletteWidget( ScribbleArea *parent ) :
    QWidget ( parent, Qt::Window ),
    m_container ( parent )
{
    QVBoxLayout *mainLayout = new QVBoxLayout( this );
    setVisible( false );
    this->setFixedWidth(200);
    m_colorBox = new ColorBox(this );
    m_colorBox->adjustSize();
    mainLayout->addWidget(m_colorBox);
    adjustSize();
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
    effect->setXOffset(2);
    effect->setYOffset(2);
    effect->setBlurRadius(5);
    setGraphicsEffect(effect);
    setAutoFillBackground(true);
    setWindowTitle("Color palette");
    setWindowFlags( ( (windowFlags()
                       | Qt::CustomizeWindowHint)
                      & ~Qt::WindowMaximizeButtonHint
                      & ~Qt::WindowMinimizeButtonHint) );    
    // --- bottom buttons layout ---
    QHBoxLayout *buttonsLayout = new QHBoxLayout( this );
    mainLayout->addLayout(buttonsLayout);
    closeButton = new QPushButton(this);
    closeButton->setText("close/toggle");
    buttonsLayout->addWidget(closeButton);
    // --- connections ---
    connect( closeButton , SIGNAL( clicked() ) , m_container , SLOT( togglePopupPalette() ) );
    connect( m_colorBox, SIGNAL( colorChanged(QColor) ), this, SLOT( onColorChanged(QColor) ) );
}

bool PopupColorPaletteWidget::popup()
{
    if ( this->isVisible() )
    {
        color = m_colorBox->color();
        hide();
        return true;
    }
    m_colorBox->setFocus();

    QPoint cPos = QCursor::pos();
    int radius = width()/2;

    cPos.setX(cPos.x()-radius); // adjust cPos to center widget
    cPos.setY(cPos.y()-radius);

    move( cPos );
    show();
    return false;
}

void PopupColorPaletteWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key()==Qt::Key_Enter)
    {
        m_colorBox->setFocus();
        return;
    }
    else if (event->key()==Qt::Key_Escape)
    {
        close();
    }
    else
    {
        event->ignore();
        QCoreApplication::sendEvent(m_container, event);
    }
}

// --------------- slots ---------------

void PopupColorPaletteWidget::onColorChanged(const QColor& color)
{
    m_container->getEditor()->setColor( m_colorBox->color() );
}
