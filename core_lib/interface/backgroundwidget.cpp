/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "backgroundwidget.h"

#include <QStyleOption>
#include <QPainter>


BackgroundWidget::BackgroundWidget(QWidget* parent) : QWidget(parent)
{
    setObjectName("BackgroundWidget");

    // Qt::WA_StaticContents ensure that the widget contents are rooted to the top-left corner
    // and don't change when the widget is resized.
    setAttribute( Qt::WA_StaticContents );
}

BackgroundWidget::~BackgroundWidget()
{
}

void BackgroundWidget::init(PreferenceManager *prefs)
{
    mPrefs = prefs;
    connect(mPrefs, &PreferenceManager::optionChanged, this, &BackgroundWidget::settingUpdated);

    loadBackgroundStyle();
    mHasShadow = mPrefs->isOn(SETTING::SHADOW);

    update();
}

void BackgroundWidget::settingUpdated(SETTING setting)
{
    switch ( setting )
    {
    case SETTING::BACKGROUND_STYLE:

        loadBackgroundStyle();
        update();
        break;

    case SETTING::SHADOW:

        mHasShadow = mPrefs->isOn(SETTING::SHADOW);
        update();
        break;

    default:
        break;
    }
}

void BackgroundWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    if (mHasShadow)
        drawShadow(painter);
}

void BackgroundWidget::loadBackgroundStyle()
{
    QString bgName = mPrefs->getString(SETTING::BACKGROUND_STYLE);
    mStyle = "background-color:white; border: 1px solid lightGrey;";

    if ( bgName == "white" )
    {
        mStyle = "background-color:white; border: 1px solid lightGrey;";
    }
    else if ( bgName == "grey" )
    {
        mStyle = "background-color:lightGrey; border: 1px solid grey;";
    }
    else if ( bgName == "checkerboard" )
    {
        mStyle = "background-image: url(:background/checkerboard.png); background-repeat: repeat-xy; border: 1px solid lightGrey;";
    }
    else if ( bgName == "dots" )
    {
        mStyle = "background-image: url(:background/dots.png); background-repeat: repeat-xy; border: 1px solid lightGrey;";
    }
    else if ( bgName == "weave" )
    {
        mStyle = "background-image: url(:background/weave.jpg); background-repeat: repeat-xy; border: 1px solid lightGrey;";
    }
    else if ( bgName == "grid" )
    {
        mStyle = "background-image: url(:background/grid.jpg); background-repeat: repeat-xy; border: 1px solid lightGrey;";
    }

    mStyle = QString("BackgroundWidget { %1 }").arg(mStyle);

    setStyleSheet(mStyle);
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
