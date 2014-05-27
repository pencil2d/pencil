
#include "popupcolorpalettewidget.h"

#include <QBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QPushButton>
#include "colorbox.h"
#include "editor.h"
#include "scribblearea.h"


PopupColorPaletteWidget::PopupColorPaletteWidget( ScribbleArea *parent ) :
    QWidget ( parent, Qt::Window ),
    m_pContainer ( parent )
{
    QVBoxLayout* mainLayout = new QVBoxLayout( this );

    setVisible( false );
    setFixedWidth(200);

    m_pColorBox = new ColorBox();
    mainLayout->addWidget(m_pColorBox);
    m_pColorBox->adjustSize();

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
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    mainLayout->addLayout(buttonsLayout);
    closeButton = new QPushButton(this);
    closeButton->setText("close/toggle");
    buttonsLayout->addWidget(closeButton);

    // --- connections ---
    connect( closeButton , SIGNAL( clicked() ) , m_pContainer , SLOT( togglePopupPalette() ) );
    connect( m_pColorBox, SIGNAL( colorChanged(QColor) ), this, SLOT( onColorChanged(QColor) ) );
}

void PopupColorPaletteWidget::popup()
{
    if ( this->isVisible() )
    {
        //color = m_colorBox->color();
        hide();
        return;
    }
    // opening palette
    m_pColorBox->setColor( m_pContainer->editor()->colorManager()->frontColor() );
    m_pColorBox->setFocus();

    QPoint cPos = QCursor::pos();
    int radius = width() / 2;

    cPos.setX(cPos.x()-radius); // adjust cPos to center widget
    cPos.setY(cPos.y()-radius);

    move( cPos );
    show();
    return;
}

void PopupColorPaletteWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter)
    {
        m_pColorBox->setFocus();
        qDebug() << "sent key_enter";
        return;
    }
    else if (event->key() == Qt::Key_Escape)
    {
        close();
    }
    else
    {
        event->ignore();
        qDebug() << "sent event.ignore()";
        QCoreApplication::sendEvent(m_pContainer, event);
    }
}

// --------------- slots ---------------

void PopupColorPaletteWidget::onColorChanged(const QColor& color)
{
    m_pContainer->editor()->colorManager()->setColor( color );
}
