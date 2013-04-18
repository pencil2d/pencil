/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include <QtGui>
#include "style.h"


int AquaStyle::pixelMetric(PixelMetric metric,
                           const QStyleOption* option,
                           const QWidget* widget) const
{
    switch (metric)
    {
    case QStyle::PM_DockWidgetSeparatorExtent:
        return 3;
    case QStyle::PM_DockWidgetTitleBarButtonMargin:
        return 1;
    default:
        return QMacStyle::pixelMetric(metric, option, widget);
    }
}


void AquaStyle::drawPrimitive(PrimitiveElement element,
                              const QStyleOption* option,
                              QPainter* painter,
                              const QWidget* widget) const
{
    /*const QDockWidget *truc = qobject_cast<QDockWidget *>(widget);
    if (truc) {
     qDebug() << "DOCK! -- element" << element;
    }*/
    /*const QStyleOptionDockWidget *dockWidgetOption = qstyleoption_cast<const QStyleOptionDockWidget *>(option);
    if(dockWidgetOption) {
    	qDebug() << "DOCK!" << element << PE_FrameDockWidget;
    }*/

    switch (element)
    {
    case PE_FrameDockWidget:
    {
        int x, y, width, height;
        option->rect.getRect(&x, &y, &width, &height);
        //painter->setBrush( Qt::red );
        //painter->drawRect( QRect(0,0, width, height) );


        //if(dockWidgetOption->isFloating) {
        painter->setBrush( Qt::NoBrush );
        painter->setPen( QColor(142,142,142) );
        painter->drawRect( QRect( QPoint(0,0), QSize(width-1, height-1) ) );
        //}
    }
    break;
    /*case PE_PanelButtonCommand:
        {
            int delta = (option->state & State_MouseOver) ? 64 : 0;
            QColor slightlyOpaqueBlack(0, 0, 0, 63);
            QColor semiTransparentWhite(255, 255, 255, 127 + delta);
            QColor semiTransparentBlack(0, 0, 0, 127 - delta);

            int x, y, width, height;
            option->rect.getRect(&x, &y, &width, &height);

            QPainterPath roundRect = roundRectPath(option->rect);
            int radius = qMin(width, height) / 2;

            QBrush brush;
            bool darker;

            const QStyleOptionButton *buttonOption =
                    qstyleoption_cast<const QStyleOptionButton *>(option);
            if (buttonOption
                    && (buttonOption->features & QStyleOptionButton::Flat)) {
                brush = option->palette.background();
                darker = (option->state & (State_Sunken | State_On));
            } else {
                if (option->state & (State_Sunken | State_On)) {
                    brush = option->palette.mid();
                    darker = !(option->state & State_Sunken);
                } else {
                    brush = option->palette.button();
                    darker = false;
                }
            }

            painter->save();
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->fillPath(roundRect, brush);
            if (darker)
                painter->fillPath(roundRect, slightlyOpaqueBlack);

            int penWidth;
            if (radius < 10)
                penWidth = 3;
            else if (radius < 20)
                penWidth = 5;
            else
                penWidth = 7;

            QPen topPen(semiTransparentWhite, penWidth);
            QPen bottomPen(semiTransparentBlack, penWidth);

            if (option->state & (State_Sunken | State_On))
                qSwap(topPen, bottomPen);

            int x1 = x;
            int x2 = x + radius;
            int x3 = x + width - radius;
            int x4 = x + width;

            if (option->direction == Qt::RightToLeft) {
                qSwap(x1, x4);
                qSwap(x2, x3);
            }

            QPolygon topHalf;
            topHalf << QPoint(x1, y)
                    << QPoint(x4, y)
                    << QPoint(x3, y + radius)
                    << QPoint(x2, y + height - radius)
                    << QPoint(x1, y + height);

            painter->setClipPath(roundRect);
            painter->setClipRegion(topHalf, Qt::IntersectClip);
            painter->setPen(topPen);
            painter->drawPath(roundRect);

            QPolygon bottomHalf = topHalf;
            bottomHalf[0] = QPoint(x4, y + height);

            painter->setClipPath(roundRect);
            painter->setClipRegion(bottomHalf, Qt::IntersectClip);
            painter->setPen(bottomPen);
            painter->drawPath(roundRect);

            painter->setPen(option->palette.foreground().color());
            painter->setClipping(false);
            painter->drawPath(roundRect);

            painter->restore();
        }
        break;*/
    default:
        QMacStyle::drawPrimitive(element, option, painter, widget);
    }
}


void AquaStyle::drawComplexControl ( ComplexControl control, const QStyleOptionComplex* option,
                                     QPainter* painter, const QWidget* widget ) const
{

    //const QStyleOptionTitleBar *titleBarOption = qstyleoption_cast<const QStyleOptionTitleBar *>(option);
    //if(titleBarOption) {
    //			qDebug() << "DOCK2!" << control;
    //}

    //qDebug() << "control" << control;
    if(control == CC_TitleBar)
    {
        //qDebug() << "coucou";
    }
    else
    {
        QMacStyle::drawComplexControl(control, option, painter, widget);
    }
}

void AquaStyle::drawControl(ControlElement element,
                            const QStyleOption* option,
                            QPainter* painter,
                            const QWidget* widget) const
{
    //const QStyleOptionDockWidget *dockWidgetOption = qstyleoption_cast<const QStyleOptionDockWidget *>(option);
    //if(dockWidgetOption) {
    //		qDebug() << "DOCK!" << element << CE_DockWidgetTitle;
    //}

    switch (element)
    {
    case CE_DockWidgetTitle:
    {
        /*QStyleOptionButton option;
        option.initFrom(this);
        option.state = isDown() ? QStyle::State_Sunken : QStyle::State_Raised;
        if (isDefault())
        	option.features |= QStyleOptionButton::DefaultButton;
        option.text = text();
        option.icon = icon();*/

        int x, y, width, height;
        option->rect.getRect(&x, &y, &width, &height);
        width = width+6;
        height = height - 1;

        const QStyleOptionDockWidget* dockWidgetOption = qstyleoption_cast<const QStyleOptionDockWidget*>(option);

        painter->setPen( QColor(142,142,142) );
        QLinearGradient gradient(0,1,0,height);

        QSettings settings("Pencil","Pencil");
        QString style = settings.value("style").toString();
        if(style=="") style = "default";
        if(style=="default")
        {
            gradient.setColorAt(0, QColor(247,247,247));
            gradient.setColorAt(0.0625, QColor(235,235,235));
            gradient.setColorAt(1, QColor(207,207,207));
            painter->setBrush( gradient );
        }
        if(style=="aqua")
        {
            gradient.setColorAt(0, QColor(208,226,245));
            gradient.setColorAt(0.25, QColor(142,187,232));
            gradient.setColorAt(0.5, QColor(134,186,237));
            gradient.setColorAt(0.51, QColor(104,169,234));
            gradient.setColorAt(0.76, QColor(104,169,234));
            gradient.setColorAt(0.90, QColor(145,210,240));
            gradient.setColorAt(1, QColor(187,252,255));

            /*gradient.setColorAt(0, QColor(238,239,241));
            gradient.setColorAt(0.25, QColor(200,205,210));
            gradient.setColorAt(0.5, QColor(194,200,206));
            gradient.setColorAt(0.51, QColor(180,188,195));
            gradient.setColorAt(1, QColor(240,247,255));*/

            painter->setBrush( QPixmap(":icons/aqua.png") );
        }

        painter->drawRect( QRect( QPoint(-1,0), QSize(width+1, height) ) );

        if(style=="aqua")
        {
            painter->setPen( QColor(0,78,193) );
            painter->drawLine( QPoint(0,0), QPoint(width,0) );
            painter->setPen( QColor(102,147,192) );
            painter->drawLine( QPoint(0,height), QPoint(width, height) );

            /*painter->setPen( QColor(93,109,124) );
            painter->drawLine( QPoint(0,0), QPoint(width,0) );
            painter->setPen( QColor(153,159,166) );
            painter->drawLine( QPoint(0,16), QPoint(width,16) );*/

            painter->setPen( QColor(0,78,193,128) );
            painter->drawLine( QPoint(0,0), QPoint(0,height) );
            painter->drawLine( QPoint(width-1,0), QPoint(width-1, height) );

            painter->setPen( QColor(0,0,0,160) );
            for(int i=0; i<height/2-3; i++)
            {
                painter->drawPoint(4,3+2*i);
                painter->drawPoint(7,3+2*i);
            }
            painter->setPen( QColor(255,255,255,240) );
            for(int i=0; i<height/2-3; i++)
            {
                painter->drawPoint(5,4+2*i);
                painter->drawPoint(8,4+2*i);
            }
        }
        if(style=="default")
        {
            painter->setPen( QColor(100,100,100,128) );
            painter->drawLine( QPoint(0,0), QPoint(0, height) );
            painter->drawLine( QPoint(width-1,0), QPoint(width-1, height) );
        }

        painter->setPen( QColor(0,0,0) );
        painter->setFont( QFont("Helvetica", 12) );
        if(style == "aqua")
        {
            painter->drawText(QPoint(13,14), dockWidgetOption->title);
        }
        if(style == "default")
        {
            painter->drawText(QPoint(6,14), dockWidgetOption->title);
        }
        //QStyleOptionButton option;
        //option.initFrom(this);
        //option.backgroundColor = palette().color(QPalette::Background);
        //QMacStyle::drawControl(QStyle::CE_PushButton, option, painter, widget);

        /*QStyleOptionButton myButtonOption;
        const QStyleOptionButton *buttonOption =
                qstyleoption_cast<const QStyleOptionButton *>(option);
        if (buttonOption) {
            myButtonOption = *buttonOption;
            if (myButtonOption.palette.currentColorGroup()
                    != QPalette::Disabled) {
                if (myButtonOption.state & (State_Sunken | State_On)) {
                    myButtonOption.palette.setBrush(QPalette::ButtonText,
                            myButtonOption.palette.brightText());
                }
            }
        }
        QMotifStyle::drawControl(element, &myButtonOption, painter, widget);*/
    }
    break;
    default:
        QMacStyle::drawControl(element, option, painter, widget);
    }
}


QRect AquaStyle::subControlRect ( ComplexControl control, const QStyleOptionComplex* option,
                                  SubControl subControl, const QWidget* widget ) const
{
    //qDebug() << "ComplexControl" << control << "subControl" << subControl << QMacStyle::subControlRect(control, option, subControl, widget);
    if(control == CC_ToolButton)
    {
        if(subControl == SC_TitleBarNormalButton)
        {
            qDebug() << "ComplexControl" << control << "subControl" << subControl;
        }
        //return QRect(30,5,20,20);
    }
    // else {
    //return QRect(2,2,32,32);
    return QMacStyle::subControlRect(control, option, subControl, widget);
    //}
}

QPixmap AquaStyle::standardPixmap ( StandardPixmap standardPixmap, const QStyleOption* option,
                                    const QWidget* widget ) const
{
    return QMacStyle::standardPixmap(standardPixmap, option, widget);
    /*
    //qDebug() << "standardPixmap" << standardPixmap;
    //if(option) qDebug() << "option" << *option;
    switch (standardPixmap) {
    	case SP_TitleBarNormalButton:
    		{ QPixmap pixmap(":icons/titleBar/floatButton.png"); return pixmap;
    			//qDebug() << "SP_TitleBarNormalButton" << QMacStyle::standardPixmap(standardPixmap, option, widget).size();
    			//return QMacStyle::standardPixmap(standardPixmap, option, widget);
    		}
    		break;
    	case SP_TitleBarCloseButton:
    		{ QPixmap pixmap(":icons/titleBar/closeButton.png"); return pixmap;
    			//qDebug() << "SP_TitleBarCloseButton" << QMacStyle::standardPixmap(standardPixmap, option, widget).size();
    			//return QMacStyle::standardPixmap(standardPixmap, option, widget);
    		}
    		break;
    	default:
    		return QMacStyle::standardPixmap(standardPixmap, option, widget);
    }
    */
}
