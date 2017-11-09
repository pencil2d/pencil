/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "beziercurve.h"

#include <cmath>
#include <QList>
#include "object.h"
#include "pencilerror.h"

BezierCurve::BezierCurve()
{
}

BezierCurve::BezierCurve(QList<QPointF> pointList)
{
    QList<qreal> pressureList;
	for (int i = 0; i < pointList.size(); i++)
    {
        pressureList << 0.5; // default pressure
    }
    createCurve(pointList, pressureList);
}

BezierCurve::BezierCurve(QList<QPointF> pointList, QList<qreal> pressureList, double tol)
{
    int n = pointList.size();

    // Simplify path
    QList<bool> markList;
    for(int i=0; i<n; i++) { markList.append(false); }
    markList.replace(0, true);
    markList.replace(n-1, true);
    BezierCurve::simplify(tol, pointList, 0, n-1, markList);

    QList<QPointF> simplifiedPointList;
    QList<qreal> simplifiedPressureList;
    for(int i=0; i<n; i++)
    {
        if (markList.at(i)==true)
        {
            simplifiedPointList.append(pointList.at(i));
            if (pressureList.size() > i)
            {
                // Make sure that the stroke point always has a pressure (and a width)
                //
                qreal currentPressure = pressureList.at(i);
                if (currentPressure < 0.1) {
                    currentPressure = 0.1;
                }
                simplifiedPressureList.append(currentPressure);
            }
            else
            {
                simplifiedPressureList.append(0.5); // default pressure
            }
        }
    }

    // Create curve from the simplified path
    createCurve(simplifiedPointList, simplifiedPressureList);
}


Status BezierCurve::createDomElement( QXmlStreamWriter& xmlStream )
{
    xmlStream.writeStartElement( "curve" );
    xmlStream.writeAttribute( "width", QString::number( width ) );
    xmlStream.writeAttribute( "variableWidth", variableWidth ? "true" : "false" );
    if (feather>0) xmlStream.writeAttribute( "feather", QString::number( feather ) );
    xmlStream.writeAttribute( "invisible", invisible ? "true" : "false" );
    xmlStream.writeAttribute( "filled", mFilled ? "true" : "false" );
    xmlStream.writeAttribute( "colourNumber", QString::number( colourNumber ) );
    xmlStream.writeAttribute( "originX", QString::number( origin.x() ) );
    xmlStream.writeAttribute( "originY", QString::number( origin.y() ) );
    xmlStream.writeAttribute( "originPressure", QString::number( pressure.at(0) ) );

    int errorLocation = -1;
    for ( int i = 0; i < c1.size() ; i++ )
    {
        xmlStream.writeEmptyElement( "segment" );
        xmlStream.writeAttribute( "c1x", QString::number( c1.at( i ).x() ) );
        xmlStream.writeAttribute( "c1y", QString::number( c1.at( i ).y() ) );
        xmlStream.writeAttribute( "c2x", QString::number( c2.at( i ).x() ) );
        xmlStream.writeAttribute( "c2y", QString::number( c2.at( i ).y() ) );
        xmlStream.writeAttribute( "vx", QString::number( vertex.at( i ).x() ) );
        xmlStream.writeAttribute( "vy", QString::number( vertex.at( i ).y() ) );
        xmlStream.writeAttribute( "pressure", QString::number( pressure.at( i + 1 ) ) );
        if ( errorLocation < 0 && xmlStream.hasError() )
        {
            errorLocation = i;
        }
    }

    xmlStream.writeEndElement(); // Close curve element

    if ( xmlStream.hasError() && errorLocation >= 0 )
    {
        QStringList debugInfo = QStringList() << "BezierCurve::createDomElement"
                                              << QString( "width = %1" ).arg( width )
                                              << QString( "variableWidth = %1" ).arg( "variableWidth" )
                                              << QString( "feather = %1" ).arg( feather )
                                              << QString( "invisible = %1" ).arg( invisible )
                                              << QString( "filled = %1" ).arg( mFilled )
                                              << QString( "colourNumber = %1" ).arg( colourNumber )
                                              << QString( "originX = %1" ).arg( origin.x() )
                                              << QString( "originY = %1" ).arg( origin.y() )
                                              << QString( "originPressure = %1" ).arg( pressure.at( 0 ) )
                                              << QString( "- segmentTag[%1] has failed to write" ).arg( errorLocation )
                                              << QString( "&nbsp;&nbsp;c1x = %1" ).arg( c1.at( errorLocation ).x() )
                                              << QString( "&nbsp;&nbsp;c1y = %1" ).arg( c1.at( errorLocation ).y() )
                                              << QString( "&nbsp;&nbsp;c2x = %1" ).arg( c2.at( errorLocation ).x() )
                                              << QString( "&nbsp;&nbsp;c2y = %1" ).arg( c2.at( errorLocation ).y() )
                                              << QString( "&nbsp;&nbsp;vx = %1" ).arg( vertex.at( errorLocation ).x() )
                                              << QString( "&nbsp;&nbsp;vy = %1" ).arg( vertex.at( errorLocation ).y() )
                                              << QString( "&nbsp;&nbsp;pressure = %1" ).arg( pressure.at( errorLocation + 1 ) );

        return Status( Status::FAIL, debugInfo );
    }

    return Status::OK;
}

void BezierCurve::loadDomElement(QDomElement element)
{
    width = element.attribute("width").toDouble();
    variableWidth = (element.attribute("variableWidth") == "1") || (element.attribute("variableWidth") == "true");
    feather = element.attribute("feather").toDouble();
    invisible = (element.attribute("invisible") == "1") || (element.attribute("invisible") == "true");
    mFilled = (element.attribute("filled") == "1") || (element.attribute("filled") == "true");
    if (width == 0) invisible = true;

    colourNumber = element.attribute("colourNumber").toInt();
    origin = QPointF( element.attribute("originX").toFloat(), element.attribute("originY").toFloat() );
    pressure.append( element.attribute("originPressure").toFloat() );
    selected.append(false);

    QDomNode segmentTag = element.firstChild();
    while (!segmentTag.isNull())
    {
        QDomElement segmentElement = segmentTag.toElement();
        if (!segmentElement.isNull())
        {
            if (segmentElement.tagName() == "segment")
            {
                QPointF c1Point = QPointF(segmentElement.attribute("c1x").toFloat(), segmentElement.attribute("c1y").toFloat());
                QPointF c2Point = QPointF(segmentElement.attribute("c2x").toFloat(), segmentElement.attribute("c2y").toFloat());
                QPointF vertexPoint = QPointF(segmentElement.attribute("vx").toFloat(), segmentElement.attribute("vy").toFloat());
                qreal pressureValue = segmentElement.attribute("pressure").toFloat();
                appendCubic(c1Point, c2Point, vertexPoint, pressureValue);
            }
        }
        segmentTag = segmentTag.nextSibling();
    }
}


void BezierCurve::setOrigin(const QPointF& point)
{
    origin = point;
}

void BezierCurve::setOrigin(const QPointF& point, const qreal& pressureValue, const bool& trueOrFalse)
{
    origin = point;
    pressure[0] = pressureValue;
    selected[0] = trueOrFalse;
}

void BezierCurve::setC1(int i, const QPointF& point)
{
    if ( i >= 0 || i < c1.size() )
    {
        c1[i] = point;
    }
    else
    {
        qDebug() << "BezierCurve::setC1! index out of bounds:" << i;
    }
}

void BezierCurve::setC2(int i, const QPointF& point)
{
    if ( i >= 0 || i < c2.size() )
    {
        c2[i] = point;
    }
    else
    {
        qDebug() << "BezierCurve::setC2! index out of bounds:" << i;
    }
}

void BezierCurve::setVertex(int i, const QPointF& point)
{
    if (i==-1) { origin = point; }
    else
    {
        if ( i >= 0 || i < vertex.size() )
        {
            vertex[i] = point;
        }
        else
        {
            qDebug() << "BezierCurve::setVertex! index out of bounds:" << i;
        }
    }
}

void BezierCurve::setLastVertex(const QPointF& point)
{
    if (vertex.size()>0)
    {
        vertex[vertex.size()-1] = point;
    }
    else
    {
        qDebug() << "BezierCurve::setLastVertex! curve has less than 2 vertices";
    }
}


void BezierCurve::setWidth(qreal desiredWidth)
{
    width = desiredWidth;
}

void BezierCurve::setFeather(qreal desiredFeather)
{
    feather = desiredFeather;
}

void BezierCurve::setVariableWidth(bool YesOrNo)
{
    variableWidth = YesOrNo;
}

void BezierCurve::setInvisibility(bool YesOrNo)
{
    invisible = YesOrNo;
}

void BezierCurve::setSelected(int i, bool YesOrNo)
{
    selected[i+1] = YesOrNo;
}

/**
 * @brief BezierCurve::setFilled
 * @param YesOrNo: bool
 * setFilled doesn't do anything on its own, but we use it
 * to see if a curve has been filled with an BezierArea.
 */
void BezierCurve::setFilled(bool YesOrNo)
{
    mFilled = YesOrNo;
}

BezierCurve BezierCurve::transformed(QTransform transformation)
{
    BezierCurve transformedCurve = *this; // copy the curve
    if (isSelected(-1)) { transformedCurve.setOrigin(transformation.map(origin)); }
    for(int i=0; i< vertex.size(); i++)
    {
        if (isSelected(i-1)) { transformedCurve.setC1(i, transformation.map(c1.at(i))); }
        if (isSelected(i))
        {
            transformedCurve.setC2(i, transformation.map(c2.at(i)));
            transformedCurve.setVertex(i, transformation.map(vertex.at(i)));
        }
    }
    //transformedCurve.smoothCurve();
    /*QPointF newOrigin = origin;
    if (isSelected(-1)) { newOrigin =  transformation.map(newOrigin); }
    transformedCurve.setOrigin( newOrigin );
    for(int i=0; i< vertex.size(); i++) {
    	QPointF newC1 = c1.at(i);
    	QPointF newC2 = c2.at(i);
    	QPointF newVertex = vertex.at(i);
    	if (isSelected(i-1)) { newC1 = transformation.map(newC1); }
    	if (isSelected(i)) { newC2 = transformation.map(newC2);  newVertex = transformation.map(newVertex); }
    	transformedCurve.appendCubic( newC1, newC2, newVertex, pressure.at(i) );
    	if (isSelected(i)) { transformedCurve.setSelected(i, true); }
    }
    transformedCurve.setWidth( width);
    transformedCurve.setVariableWidth( variableWidth );
    //transformedCurve.setSelected(true); // or select only the selected elements of the orginal curve?
    */
    return transformedCurve;
}

void BezierCurve::transform(QTransform transformation)
{
    if (isSelected(-1)) setOrigin( transformation.map(origin) );
    for(int i=0; i< vertex.size(); i++)
    {
        if (isSelected(i-1)) c1[i] = transformation.map(c1.at(i));
        if (isSelected(i))
        {
            c2[i] = transformation.map(c2.at(i));
            vertex[i] = transformation.map(vertex.at(i));
        }
    }
    //smoothCurve();
}

void BezierCurve::appendCubic(const QPointF& c1Point, const QPointF& c2Point, const QPointF& vertexPoint, qreal pressureValue)
{
    c1.append(c1Point);
    c2.append(c2Point);
    vertex.append(vertexPoint);
    pressure.append(pressureValue);
    selected.append(false);
}

void BezierCurve::addPoint(int position, const QPointF point)
{
    if ( position > -1 && position < getVertexSize() )
    {
        QPointF v1 = getVertex(position-1);
        QPointF v2 = getVertex(position);
        QPointF c1o = getC1(position);
        QPointF c2o = getC2(position);

        c1[position] = point + 0.2*(v2-v1);
        c2[position] = v2 + (c2o-v2)*(0.5);

        c1.insert(position, v1 + (c1o-v1)*(0.5) );
        c2.insert(position, point - 0.2*(v2-v1));
        vertex.insert(position, point);
        pressure.insert(position, getPressure(position));
        selected.insert(position, isSelected(position) && isSelected(position-1));

        //smoothCurve();
    }
    else
    {
        qDebug() << "Error BezierCurve::addPoint(int, QPointF)";
    }
}

void BezierCurve::addPoint(int position, const qreal fraction) // fraction is where to split the bezier curve (ex: fraction=0.5)
{
    // de Casteljau's method is used
    // http://en.wikipedia.org/wiki/De_Casteljau%27s_algorithm
    // http://www.damtp.cam.ac.uk/user/na/PartIII/cagd2002/halve.ps
    if ( position > -1 && position < getVertexSize() )
    {
        QPointF vA = getVertex(position-1);
        QPointF vB = getVertex(position);
        QPointF c1o = getC1(position);
        QPointF c2o = getC2(position);
        QPointF c12 = (1-fraction)*c1o + fraction*c2o;
        QPointF cA1 = (1-fraction)*vA + fraction*c1o;
        QPointF cB2 = (1-fraction)*c2o + fraction*vB;
        QPointF cA2 = (1-fraction)*cA1 + fraction*c12;
        QPointF cB1 = (1-fraction)*c12 + fraction*cB2;
        QPointF vM = (1-fraction)*cA2 + fraction*cB1;

        setC1(position, cB1);
        setC2(position, cB2);

        c1.insert(position, cA1);
        c2.insert(position, cA2);
        vertex.insert(position, vM);
        pressure.insert(position, getPressure(position));
        selected.insert(position, isSelected(position) && isSelected(position-1));

        //smoothCurve();
    }
    else
    {
        qDebug() << "Error BezierCurve::addPoint(int, qreal)";
    }
}

void BezierCurve::removeVertex(int i)
{
    int n = vertex.size();
    if (i>-2 && i< n)
    {
        if (i== -1)
        {
            origin = vertex.at(0);
            vertex.removeAt(0);
            c1.removeAt(0);
            c2.removeAt(0);
            pressure.removeAt(0);
            selected.removeAt(0);
        }
        else
        {
            vertex.removeAt(i);
            c2.removeAt(i);
            pressure.removeAt(i+1);
            selected.removeAt(i+1);
            if ( i != n-1 )
            {
                c1.removeAt(i+1);
            }
            else
            {
                c1.removeAt(i);
            }
        }
    }
}

void BezierCurve::drawPath(QPainter& painter, Object* object, QTransform transformation, bool simplified, bool showThinLines )
{
    QColor colour = object->getColour(colourNumber).colour;

    BezierCurve myCurve;
    if (isPartlySelected()) { myCurve = (transformed(transformation)); }
    else { myCurve = *this; }

    if ( variableWidth && !simplified && !invisible)
    {
        painter.setPen(QPen(QBrush(colour), 1, Qt::NoPen, Qt::RoundCap,Qt::RoundJoin));
        painter.setBrush(colour);
        painter.drawPath(myCurve.getStrokedPath());
    }
    else
    {
        qreal renderedWidth = width;
        if (simplified)
        {
            renderedWidth = 1.0/painter.matrix().m11();
        }
        painter.setBrush(Qt::NoBrush);
        if ( invisible )
        {
            if (showThinLines)
            {
                if (simplified)
                {
                    // Set simplified lines to black for the fill function to define contours.
                    painter.setPen(QPen(QBrush(Qt::black), renderedWidth, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
                }
                else
                {
                    painter.setPen(QPen(QBrush(colour), 0, Qt::DotLine, Qt::RoundCap,Qt::RoundJoin));
                }
            }
            else
            {
                painter.setPen(Qt::NoPen);
            }
        }
        else
        {
            painter.setPen( QPen( QBrush( colour ), renderedWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
            //painter.setPen( QPen( Qt::darkYellow , 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        }
        QPainterPath path = myCurve.getSimplePath();
        painter.drawPath( path );
    }

    if (!simplified)
    {
        // highlight the selected elements
        colour = QColor(100,150,255);  // highlight colour
        painter.setBrush(Qt::NoBrush);
        qreal lineWidth = 1.5/painter.matrix().m11();
        painter.setPen(QPen(QBrush(colour), lineWidth, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
        if (isSelected()) painter.drawPath(myCurve.getSimplePath());


        for(int i=-1; i< vertex.size(); i++)
        {
            if (isSelected(i))
            {
//                painter.fillRect(myCurve.getVertex(i).x()-0.5*squareWidth, myCurve.getVertex(i).y()-0.5*squareWidth, squareWidth, squareWidth, colour);

                //painter.fillRect(QRectF(myCurve.getVertex(i).x()-0.5*squareWidth, myCurve.getVertex(i).y()-0.5*squareWidth, squareWidth, squareWidth), colour);

                /*painter.setFont( QFont("Arial", floor(12.0/painter.matrix().m11()), -1, false) );
                //painter.drawText(myCurve.getVertex(i)+QPointF(4.0,0.0), QString::number(i)+"-"+QString::number(myCurve.getVertex(i).x())+","+QString::number(myCurve.getVertex(i).y()));
                QPointF normale = QPointF(4.0, 0.0);
                if (i>-1) { normale = (myCurve.getVertex(i)-myCurve.getC2(i)); } else { normale = (myCurve.getC1(i+1)-myCurve.getVertex(i)); }
                normale = QPointF(-normale.y(), normale.x());
                normale = 8.0*normale/eLength(normale)/painter.matrix().m11();
                painter.drawLine(myCurve.getVertex(i), myCurve.getVertex(i)+normale);
                painter.drawText(myCurve.getVertex(i)+2*normale, QString::number(i));*/
            }
        }
    }
}

// Without curve fitting
QPainterPath BezierCurve::getStraightPath()
{
    QPainterPath path;
    path.moveTo(origin);
    for(int i=0; i<vertex.size(); i++)
    {
        path.lineTo(vertex[i]);
    }
    return path;
}

// With bezier curve fitting
QPainterPath BezierCurve::getSimplePath()
{
    QPainterPath path;
    path.moveTo(origin);
    for(int i=0; i<vertex.size(); i++)
    {
        path.cubicTo(c1.at(i), c2.at(i), vertex.at(i));
    }
    return path;
}

QPainterPath BezierCurve::getStrokedPath()
{
    return getStrokedPath( width );
}

QPainterPath BezierCurve::getStrokedPath(qreal width)
{
    return getStrokedPath(width, true);
}

// this function is a mess and outputs buggy results randomly...
QPainterPath BezierCurve::getStrokedPath(qreal width, bool usePressure)
{
    QPainterPath path;
    QPointF tangentVec, normalVec, normalVec2, normalVec2_1, normalVec2_2;
    qreal width2 = width;
    int n = vertex.size();
    path.setFillRule(Qt::WindingFill);

    normalVec = QPointF(-(c1.at(0) - origin).y(), (c1.at(0) - origin).x());
    normalise(normalVec);
    if (usePressure) width2 = width * 0.5 * pressure.at(0);
    if (n==1 && width2 == 0.0)  width2 = 0.15 * width;
    path.moveTo(origin + width2*normalVec);
    for(int i=0; i<n; i++)
    {
        if (i==n-1)
        {
            normalVec2 = QPointF(-(vertex.at(i) - c2.at(i)).y(), (vertex.at(i) - c2.at(i)).x());
        }
        else
        {
            normalVec2_1 = QPointF(-(vertex.at(i) - c2.at(i)).y(), (vertex.at(i) - c2.at(i)).x());
            normalise(normalVec2_1);
            normalVec2_2 = QPointF(-(c1.at(i+1) - vertex.at(i)).y(), (c1.at(i+1) - vertex.at(i)).x());
            normalise(normalVec2_2);
            normalVec2 = normalVec2_1 + normalVec2_2;
        }
        normalise(normalVec2);
        if (usePressure) width2 = width * 0.5 * pressure.at(i);
        if (n==1 && width2 == 0.0)  width2 = 0.15 * width;
        //if (i==n-1) width2 = 0.0;
        path.cubicTo(c1.at(i) + width2*normalVec, c2.at(i) + width2*normalVec2, vertex.at(i) + width2*normalVec2);
        //path.moveTo(vertex.at(i) + width*normalVec2);
        //path.lineTo(vertex.at(i) - width*normalVec2);
        normalVec = normalVec2;
    }
    if (usePressure) width2 = width * 0.5 * pressure.at(n-1);
    if (n==1 && width2 == 0.0)  width2 = 0.15 * width;

    //path.lineTo(vertex.at(n-1) - width2*normalVec);
    tangentVec = (vertex.at(n-1)-c2.at(n-1));
    normalise(tangentVec);
    path.cubicTo(vertex.at(n-1) + width2*(normalVec+1.8*tangentVec), vertex.at(n-1) + width2*(-normalVec+1.8*tangentVec), vertex.at(n-1) - width2*normalVec);

    for(int i=n-2; i>=0; i--)
    {
        normalVec2_1 = QPointF((vertex.at(i) - c1.at(i+1)).y(), -(vertex.at(i) - c1.at(i+1)).x());
        normalise(normalVec2_1);
        normalVec2_2 = QPointF((c2.at(i) - vertex.at(i)).y(), -(c2.at(i) - vertex.at(i)).x());
        normalise(normalVec2_2);
        normalVec2 = normalVec2_1 + normalVec2_2;
        normalise(normalVec2);
        if (usePressure) width2 = width * 0.5 * pressure.at(i);
        if (n==1 && width2 == 0.0)  width2 = 0.15 * width;
        path.cubicTo(c2.at(i+1) - width2*normalVec, c1.at(i+1) - width2*normalVec2, vertex.at(i) - width2*normalVec2);
        normalVec = normalVec2;
    }
    normalVec2 = QPointF((origin - c1.at(0)).y(), -(origin - c1.at(0)).x());
    normalise(normalVec2);
    if (usePressure) width2 = width * 0.5 * pressure.at(0);
    if (n==1 && width2 == 0.0)  width2 = 0.15 * width;
    path.cubicTo(c2.at(0) - width2*normalVec, c1.at(0) - width2*normalVec2, origin - width2*normalVec2);

    tangentVec = (origin-c1.at(0));
    normalise(tangentVec);
    path.cubicTo(origin + width2*(-normalVec+1.8*tangentVec), origin + width2*(normalVec+1.8*tangentVec), origin + width2*normalVec);

    path.closeSubpath();
    return path;
}

QRectF BezierCurve::getBoundingRect()
{
    return getSimplePath().boundingRect();
}

void BezierCurve::createCurve(QList<QPointF>& pointList, QList<qreal>& pressureList )
{
    int p = 0;
    int n = pointList.size();
    // generate the Bezier (cubic) curve from the simplified path and mouse pressure
    // first, empty everything
    while (c1.size()>0) c1.removeAt(0);
    while (c2.size()>0) c2.removeAt(0);
    while (vertex.size()>0) vertex.removeAt(0);
    while (selected.size()>0) selected.removeAt(0);
    while (pressure.size()>0) pressure.removeAt(0);

    setOrigin( pointList.at(0) );
    selected.append(false);
    pressure.append(pressureList.at(0));

    for(p=1; p<n; p++)
    {
        c1.append(pointList.at(p));
        c2.append(pointList.at(p));
        vertex.append(pointList.at(p));
        pressure.append(pressureList.at(p));
        selected.append(false);

    }
    smoothCurve();
    //colourNumber = 0;
    feather = 0;
}


void BezierCurve::smoothCurve()
{
    QPointF c1, c2, c2old, tangentVec, normalVec;
    int n = vertex.size();
    c2old = QPointF(-100,-100); // bogus point
    for(int p=0; p<n-1; p++)
    {
        QPointF D = getVertex(p);
        QPointF Dprev = getVertex(p-1);
        QPointF Dnext = getVertex(p+1);
        qreal L1 = mLength(D-Dprev);
        qreal L2 = mLength(D-Dnext);

        tangentVec = 0.4*(Dnext - Dprev);
        normalVec = QPointF(-tangentVec.y(), tangentVec.x())/eLength(tangentVec);
        if (  ((D-Dprev).x()*(D-Dnext).x()+(D-Dprev).y()*(D-Dnext).y())/(1.0*L1*L2) < 0  )
        {
            // smooth point
            c1 =  D - tangentVec*(L1+0.0)/(L1+L2);
            c2 =  D + tangentVec*(L2+0.0)/(L1+L2);
        }
        else
        {
            // sharp point
            c1 = 0.6*D + 0.4*Dprev;
            c2 = 0.6*D + 0.4*Dnext;
        }

        if (p==0)
        {
            c2old  = 0.5*(vertex.at(0)+c1);
        }

        this->c1[p] = c2old;
        this->c2[p] = c1;
        //appendCubic(c2old, c1, D, pressureList->at(p));
        c2old = c2;
    }
    if (n>2)
    {
        this->c1[n-1] = c2old;
        this->c2[n-1] = 0.5*(c2old+vertex.at(n-1));
    }
}

void BezierCurve::simplify(double tol, QList<QPointF>& inputList, int j, int k, QList<bool>& markList)
{
    // -- Douglas-Peucker simplification algorithm
    // from http://geometryalgorithms.com/Archive/algorithm_0205/
    if (k <= j+1)   //there is nothing to simplify
    {
        // return immediately
    }
    else
    {
        // test distance of intermediate vertices from segment Vj to Vk
        double maxd = 0.0; //is the distance of farthest vertex from segment jk
        int maxi = j;  //is the index of the vertex farthest from segement jk
        for(int i=j+1; i<k-1; i++)  // each intermediate vertex Vi
        {
            QPointF Vij = inputList.at(i)-inputList.at(j);
            QPointF Vjk = inputList.at(j)-inputList.at(k);
            double Vijx = Vij.x();
            double Vijy = Vij.y();
            double Vjkx = Vjk.x();
            double Vjky = Vjk.y();
            double dv = (Vjkx*Vjkx+Vjky*Vjky);
            if ( dv != 0.0)
            {
                dv = sqrt( Vijx*Vijx+Vijy*Vijy  -  pow(Vijx*Vjkx+Vijy*Vjky,2)/dv );
            }
            //qDebug() << "distance = "+QString::number(dv);
            if (dv < maxd)
            {
                //Vi is not farther away, so continue to the next vertex
            }
            else     //Vi is a new max vertex
            {
                maxd = dv;
                maxi = i; //to remember the farthest vertex
            }
        }
        if (maxd >= tol)   //a vertex is farther than tol from Sjk
        {
            // split the polyline at the farthest vertex
            //Mark Vmaxi as part of the simplified polyline
            markList.replace(maxi, true);
            //and recursively simplify the two subpolylines
            simplify(tol, inputList, j, maxi, markList);
            simplify(tol, inputList, maxi, k, markList);
        }
    }
}

// general useful functions -> to be placed elsewhere
qreal BezierCurve::eLength(const QPointF point)    // calculates the Euclidean Length (of a point seen as a vector)
{
    qreal result = sqrt( point.x()*point.x() + point.y()*point.y() );  // could also use QLine.length()... is it any faster?
    //if (result == 0.0) result = 1.0;
    return result;
}

qreal BezierCurve::mLength(const QPointF point)   // calculates the Manhattan Length (of a point seen as a vector)
{
    qreal result = qAbs(point.x()) + qAbs(point.y());
    if (result == 0.0) result = 1.0;
    return result;
}

void BezierCurve::normalise(QPointF& point)
{
    qreal length = eLength(point);
    if (length > 1.0e-6)
    {
        point = point/length;
    }
}

qreal BezierCurve::findDistance(BezierCurve curve, int i, QPointF P, QPointF& nearestPoint, qreal& t)   //finds the distance between a cubic section and a point
{
    //qDebug() << "---- INTER CUBIC SEGMENT";
    int nSteps = 24;
    QPointF Q;
    Q = curve.getVertex(i-1);
    qreal distMin = eLength(Q-P);
    nearestPoint = Q;
    t = 0;
    for(int k=1; k<=nSteps; k++)
    {
        qreal s = (k+0.0)/nSteps;
        Q = curve.getPointOnCubic(i, s);
        qreal dist = eLength(Q-P);
        if (dist <= distMin)
        {
            distMin = dist;
            nearestPoint = Q;
            t = s;
        }
    }
    //QPointF Q1 = curve.getPointOnCubic(i, t);
    return distMin;
}

QPointF BezierCurve::getPointOnCubic(int i, qreal t)
{
    return (1.0-t)*(1.0-t)*(1.0-t)*getVertex(i-1)
           + 3*t*(1.0-t)*(1.0-t)*getC1(i)
           + 3*t*t*(1.0-t)*getC2(i)
           + t*t*t*getVertex(i);
}


bool BezierCurve::intersects(QPointF point, qreal distance)
{
    bool result = false;
    if ( getStrokedPath(distance, false).contains(point) )
    {
        //if ( getSimplePath().controlPointRect().contains(point)) {
        result = true;
    }
    return result;
}

bool BezierCurve::intersects(QRectF rectangle)
{
    bool result = false;
    if ( getSimplePath().controlPointRect().intersects(rectangle))
    {
        for(int i=0; i<vertex.size(); i++)
        {
            if ( rectangle.contains( getVertex(i) ) ) return true;
        }
    }
    return result;
}

bool BezierCurve::findIntersection(BezierCurve curve1, int i1, BezierCurve curve2, int i2, QList<Intersection>& intersections)   //finds the intersection between two cubic sections
{
    bool result = false;
    //qDebug() << "---- INTER CUBIC CUBIC"  << i1 << i2;
    QPointF P1, Q1, P2, Q2;
    QLineF L1, L2;
    QRectF R1;
    QRectF R2;

    P1 = curve1.getVertex(i1-1);
    Q1 = curve1.getVertex(i1);
    P2 = curve2.getVertex(i2-1);
    Q2 = curve2.getVertex(i2);
    L1 = QLineF(P1, Q1);
    L2 = QLineF(P2, Q2);

    //qDebug() << "-------------------- ";

    R1.setTopLeft(P1);
    R1.setBottomRight(Q1);
    R2.setTopLeft(P2);
    R2.setBottomRight(Q2);

    QPointF intersectionPoint = QPointF(50.0, 50.0); // bogus point
    QPointF* cubicIntersection = &intersectionPoint;
    if ( R1.intersects(R2) || L2.intersect(L1, cubicIntersection) == QLineF::BoundedIntersection )
    {
        //if (L2.intersect(L1, intersection) == QLineF::BoundedIntersection) {
        //qDebug() << "                   FOUND rectangle intersection ";
        //if (intersectionPoint != curve1.getVertex(i1-1) && intersectionPoint != curve1.getVertex(i1)) {
        //	qDebug() << "                   it's not one of the points ";
        // find the cubic intersection
        int nSteps = 24;
        P1 = curve1.getVertex(i1-1);
        for(int i=1; i<=nSteps; i++)
        {
            qreal s = (i+0.0)/nSteps;
            Q1 = curve1.getPointOnCubic(i1, s);
            P2 = curve2.getVertex(i2-1);
            for(int j=1; j<=nSteps; j++)
            {
                qreal t = (j+0.0)/nSteps;
                Q2 = curve2.getPointOnCubic(i2, t);
                L1 = QLineF(P1, Q1);
                L2 = QLineF(P2, Q2);
                if (L2.intersect(L1, cubicIntersection) == QLineF::BoundedIntersection)
                {
                    QPointF intersectionPoint = *cubicIntersection;
                    if (intersectionPoint != curve1.getVertex(i1-1) && intersectionPoint != curve1.getVertex(i1))
                    {
                        qreal fraction1 = eLength(intersectionPoint-Q1)/(0.0+eLength(Q1-P1));
                        qreal fraction2 = eLength(intersectionPoint-Q2)/(0.0+eLength(Q2-P2));
                        qreal t1 = (i - fraction1)/nSteps;
                        qreal t2 = (j - fraction2)/nSteps;
                        Intersection intersection;
                        intersection.point = intersectionPoint;
                        intersection.t1 = t1;
                        intersection.t2 = t2;
                        intersections.append( intersection );
                        result = true;
                        //qDebug() << "FOUND cubic interesection " << intersectionPoint << i << j;
                    }
                }
                P2 = Q2;
            }
            P1 = Q1;
        }
    }
    else
    {
        //return false; // approximation to speed up the calculation
    }
    //qDebug() << "------";
    return result;
}
