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

#include <climits>
#include <QtDebug>
#include <QInputDialog>
#include <QLineEdit>
#include "layer.h"
#include "object.h"
#include "timeline.h"

Layer::Layer(Object* object) : QObject(0)
{
    this->object = object;
    m_eType = Layer::UNDEFINED;
    id = 0;
    name = QString("Undefined Layer");
    visible = true;
}

Layer::~Layer() {}

bool Layer::hasKeyframeAtPosition(int frameIndex)
{
    Q_UNUSED(frameIndex);
    return false;
}

int Layer::getPreviousKeyframePosition(int frameIndex)
{
    Q_UNUSED(frameIndex);
    return NO_KEYFRAME;
}

int Layer::getNextKeyframePosition(int frameIndex)
{
    Q_UNUSED(frameIndex);
    return NO_KEYFRAME;
}

int Layer::getFirstKeyframePosition()
{
    return getNextKeyframePosition(NO_KEYFRAME);
}

int Layer::getLastKeyframePosition()
{
    return getPreviousKeyframePosition(INT_MAX);
}

QDomElement Layer::createDomElement(QDomDocument& doc)
{
    QDomElement layerTag = doc.createElement("layer");
    layerTag.setAttribute("name", name);
    layerTag.setAttribute("visibility", visible);
    layerTag.setAttribute( "type", m_eType );

    qDebug( ) << "    Layer name=" << name << " visi=" << visible << " type=" << m_eType;
    return layerTag;
}

void Layer::loadDomElement(QDomElement element)
{
    name = element.attribute("name");
    visible = (element.attribute("visibility") == "1");
    m_eType = static_cast<LAYER_TYPE>( element.attribute( "type" ).toInt() );
}

void Layer::paintTrack(QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int frameSize)
{
    Q_UNUSED(cells);
    Q_UNUSED(frameSize);
    painter.setBrush(Qt::lightGray);
    painter.setPen(QPen(QBrush(QColor(100,100,100)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
    painter.drawRect(x, y, width, height); // empty rectangle  by default
    //painter.setFont(QFont("helvetica", height/2));
    //painter.drawText(QPoint(10, y+(2*height)/3), name);
    //if (selected) {
    //	painter.setBrush(QColor(0,0,0,80));
    //	painter.drawRect(x, y-1, width, height);
    //}
    if (selected)
    {
        QLinearGradient linearGrad(QPointF(0, y), QPointF(0, y + height));
        linearGrad.setColorAt(0, QColor(255,255,255,128) );
        linearGrad.setColorAt(0.40, QColor(255,255,255,0) );
        linearGrad.setColorAt(0.60, QColor(0,0,0,0) );
        linearGrad.setColorAt(1, QColor(0,0,0,64) );
        painter.setBrush( linearGrad );
        painter.setPen(QPen(QBrush(QColor(70,70,70)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
        painter.drawRect(x, y-1, width, height);
    }
}

void Layer::paintLabel(QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int allLayers)
{
    Q_UNUSED(cells);
    painter.setBrush(Qt::lightGray);
    painter.setPen(QPen(QBrush(QColor(100,100,100)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
    painter.drawRect(x, y-1, width, height); // empty rectangle  by default

    if (visible)
    {
        if (allLayers==0)  painter.setBrush(Qt::NoBrush);
        if (allLayers==1)   painter.setBrush(Qt::darkGray);
        if ((allLayers==2) || selected)  painter.setBrush(Qt::black);
    }
    else
    {
        painter.setBrush(Qt::NoBrush);
    }
    painter.setPen(Qt::black);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawEllipse(x+6, y+4, 9, 9);
    painter.setRenderHint(QPainter::Antialiasing, false);
    /*if (selected) {
    	painter.setPen(Qt::NoPen);
    	painter.setBrush(QColor(0,0,0,80));
    	painter.drawRect(x, y-1, width, height);
    }*/
    if (selected)
    {
        paintSelection(painter, x, y, width, height);
    }

    if ( type() == BITMAP ) painter.drawPixmap( QPoint( 20, y + 2 ), QPixmap( ":/icons/layer-bitmap.png" ) );
    if ( type() == VECTOR ) painter.drawPixmap( QPoint( 20, y + 2 ), QPixmap( ":/icons/layer-vector.png" ) );
    if ( type() == SOUND ) painter.drawPixmap( QPoint( 21, y + 2 ), QPixmap( ":/icons/layer-sound.png" ) );
    if ( type() == CAMERA ) painter.drawPixmap( QPoint( 21, y + 2 ), QPixmap( ":/icons/layer-camera.png" ) );

    painter.setFont(QFont("helvetica", height/2));
    painter.setPen(Qt::black);
    painter.drawText(QPoint(45, y+(2*height)/3), name);

}

void Layer::paintSelection(QPainter& painter, int x, int y, int width, int height)
{
    QLinearGradient linearGrad(QPointF(0, y), QPointF(0, y + height));
    QSettings settings("Pencil","Pencil");
    QString style = settings.value("style").toString();
    if (style == "aqua")
    {
        linearGrad.setColorAt(0, QColor(225,225,255,100) );
        linearGrad.setColorAt(0.10, QColor(225,225,255,80) );
        linearGrad.setColorAt(0.20, QColor(225,225,255,64) );
        linearGrad.setColorAt(0.35, QColor(225,225,255,20) );
        linearGrad.setColorAt(0.351, QColor(0,0,0,32) );
        linearGrad.setColorAt(0.66, QColor(245,255,235,32) );
        linearGrad.setColorAt(1, QColor(245,255,235,128) );
    }
    else
    {
        linearGrad.setColorAt(0, QColor(255,255,255,128) );
        linearGrad.setColorAt(0.49, QColor(255,255,255,0) );
        linearGrad.setColorAt(0.50, QColor(0,0,0,0) );
        linearGrad.setColorAt(1, QColor(0,0,0,48) );

        /*linearGrad.setColorAt(0, QColor(255,255,255,128) );
        linearGrad.setColorAt(0.10, QColor(255,255,255,64) );
        linearGrad.setColorAt(0.49, QColor(0,0,0,32) );
        linearGrad.setColorAt(0.50, QColor(0,0,0,32) );
        linearGrad.setColorAt(0.70, QColor(245,255,245,32) );
        linearGrad.setColorAt(1, QColor(245,255,245,128) );*/

        /*linearGrad.setColorAt(0, QColor(255,255,255,128) );
        linearGrad.setColorAt(0.10, QColor(255,255,255,64) );
        linearGrad.setColorAt(0.20, QColor(0,0,0,32) );
        linearGrad.setColorAt(0.40, QColor(0,0,0,0) );
        linearGrad.setColorAt(0.41, QColor(255,255,255,0) );
        linearGrad.setColorAt(1, QColor(255,255,255,128) );*/
    }
    painter.setBrush( linearGrad );
    painter.setPen( Qt::NoPen );
    painter.drawRect(x, y, width, height-1);
    //painter.setBrush( Qt::NoBrush );
    //painter.setPen(QPen(QBrush(QColor(0,0,0,100)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
    //painter.drawRect(x, y-1, width, height);
    /*if (style == "aqua") {
    	QColor col;
    	if (type == BITMAP) col = QColor(65,65,122);
    	if (type == VECTOR) col = QColor(50,102,75);
    	if (type == SOUND) col = QColor(122,65,65);
    	painter.setPen(col);
    	painter.drawLine(x,y-1, x+width, y-1);
    }*/
}

void Layer::mousePress(QMouseEvent* event, int frameNumber)
{
    Q_UNUSED(event);
    Q_UNUSED(frameNumber);
}

void Layer::mouseDoubleClick(QMouseEvent* event, int frameNumber)
{
    Q_UNUSED(event);
    Q_UNUSED(frameNumber);
}

void Layer::mouseMove(QMouseEvent* event, int frameNumber)
{
    Q_UNUSED(event);
    Q_UNUSED(frameNumber);
}

void Layer::mouseRelease(QMouseEvent* event, int frameNumber)
{
    Q_UNUSED(event);
    Q_UNUSED(frameNumber);
}

void Layer::editProperties()
{
    bool ok;
    QString text = QInputDialog::getText(NULL, tr("Layer Properties"),
                                         tr("Layer name:"), QLineEdit::Normal,
                                         name, &ok);
    if (ok && !text.isEmpty())
    {
        name = text;
        //palette->updateList();
    }
}
