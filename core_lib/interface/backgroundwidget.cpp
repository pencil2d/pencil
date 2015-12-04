/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2014 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "QStyleOption"
#include "QPainter"
#include "backgroundwidget.h"

BackgroundWidget::BackgroundWidget(QWidget *parent) : QWidget(parent)
{

}

void BackgroundWidget::init(PreferenceManager *prefs)
{
    mPrefs = prefs;
    connect(mPrefs, &PreferenceManager::optionChanged, this, &BackgroundWidget::settingUpdated);
    update();
}

void BackgroundWidget::settingUpdated(SETTING setting)
{
    switch ( setting )
    {
    case SETTING::BACKGROUND_STYLE:
        update();
        break;
    case SETTING::SHADOW:
        update();
        break;
    default:
        break;
    }
}

void BackgroundWidget::paintEvent(QPaintEvent *)
{
    bool hasShadow = mPrefs->isOn(SETTING::SHADOW);

    QString sStyle = getBackgroundStyle();

    setStyleSheet(sStyle);

    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    if (hasShadow) {
        drawShadow(painter);
    }
}

QString BackgroundWidget::getBackgroundStyle()
{
    QString bgName = mPrefs->getString(SETTING::BACKGROUND_STYLE);
    QString style = "background-color:white; border: 1px solid lightGrey;";

    if ( bgName == "white" )
    {
        style = "background-color:white; border: 1px solid lightGrey;";
    }
    else if ( bgName == "grey" )
    {
        style = "background-color:lightGrey; border: 1px solid grey;";
    }
    else if ( bgName == "checkerboard" )
    {
        style = "background-image: url(:background/checkerboard.png); background-repeat: repeat-xy; border: 1px solid lightGrey;";
    }
    else if ( bgName == "dots" )
    {
        style = "background-image: url(:background/dots.png); background-repeat: repeat-xy; border: 1px solid lightGrey;";
    }
    else if ( bgName == "weave" )
    {
        style = "background-image: url(:background/weave.jpg); background-repeat: repeat-xy; border: 1px solid lightGrey;";
    }
    else if ( bgName == "grid" )
    {
        style = "background-image: url(:background/grid.jpg); background-repeat: repeat-xy; border: 1px solid lightGrey;";
    }

    return style;
}

void BackgroundWidget::drawShadow( QPainter& painter )
{
    int radius1 = 12;
    int radius2 = 8;

    QColor colour = Qt::black;
    qreal opacity = 0.15;

    QLinearGradient shadow = QLinearGradient( 0, 0, 0, radius1 );

    int r = colour.red();
    int g = colour.green();
    int b = colour.blue();
    qreal a = colour.alphaF();
    shadow.setColorAt( 0.0, QColor( r, g, b, qRound( a * 255 * opacity ) ) );
    shadow.setColorAt( 1.0, QColor( r, g, b, 0 ) );


    painter.setPen( Qt::NoPen );
    painter.setBrush( shadow );
    painter.drawRect( QRect( 0, 0, width(), radius1 ) );

    shadow.setFinalStop( radius1, 0 );
    painter.setBrush( shadow );
    painter.drawRect( QRect( 0, 0, radius1, height() ) );

    shadow.setStart( 0, height() );
    shadow.setFinalStop( 0, height() - radius2 );
    painter.setBrush( shadow );
    painter.drawRect( QRect( 0, height() - radius2, width(), height() ) );

    shadow.setStart( width(), 0 );
    shadow.setFinalStop( width() - radius2, 0 );
    painter.setBrush( shadow );
    painter.drawRect( QRect( width() - radius2, 0, width(), height() ) );
}
