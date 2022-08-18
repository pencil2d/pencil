/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "popupcolorpalettewidget.h"

#include <QBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QPushButton>
#include <QKeyEvent>
#include "colorbox.h"
#include "editor.h"
#include "scribblearea.h"


PopupColorPaletteWidget::PopupColorPaletteWidget( ScribbleArea *parent ) :
    QWidget ( parent, Qt::Window ),
    mContainer ( parent )
{
    QVBoxLayout* mainLayout = new QVBoxLayout( this );

    setVisible( false );
    setFixedWidth(200);

    mColorBox = new ColorBox();
    mainLayout->addWidget(mColorBox);
    mColorBox->adjustSize();

    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
    effect->setXOffset(2);
    effect->setYOffset(2);
    effect->setBlurRadius(5);
    setGraphicsEffect(effect);

    setAutoFillBackground(true);
    setWindowTitle(tr("Color palette"));
    setWindowFlags( ( (windowFlags()
                       | Qt::CustomizeWindowHint)
                      & ~Qt::WindowMaximizeButtonHint
                      & ~Qt::WindowMinimizeButtonHint) );

    // --- bottom buttons layout ---
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    mainLayout->addLayout(buttonsLayout);
    closeButton = new QPushButton(this);
    closeButton->setText(tr("close/toggle"));
    buttonsLayout->addWidget(closeButton);

    // --- connections ---
    connect( closeButton , &QPushButton::clicked , mContainer , &ScribbleArea::togglePopupPalette );
    connect( mColorBox, &ColorBox::colorChanged, this, &PopupColorPaletteWidget::onColorChanged );
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
    mColorBox->setColor( mContainer->editor()->color()->frontColor() );
    mColorBox->setFocus();

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
        mColorBox->setFocus();
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
        QCoreApplication::sendEvent(mContainer, event);
    }
}

// --------------- slots ---------------

void PopupColorPaletteWidget::onColorChanged(const QColor& color)
{
    mContainer->editor()->color()->setColor( color );
}
