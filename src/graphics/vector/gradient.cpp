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

#include <cmath>
#include "gradient.h"
#include "vectorimage.h"
#include "bitmapimage.h"
#include "bezierarea.h"
#include "beziercurve.h"
#include "vertexref.h"

Gradient::Gradient()
{
    // nothing
}

void Gradient::paint1(QPainter& painter, VectorImage* v, int i, int gradients)
{
    Q_UNUSED(gradients);

    QMatrix painterMatrix = painter.worldMatrix();
    //qreal scale = qAbs(painterMatrix.m11()) + qAbs(painterMatrix.m12()); // quick overestimation of sqrt( m11*m22 - m12*m21 )
    QColor colour = v->getColour(v->area[i].colourNumber);

    //bool buffer = false;
    //bool subBuffer = true;
    //QPainter painter2; QMatrix painter2Matrix; QImage* bufferImage; QRect rect;
    //QPainter painter3; QMatrix painter3Matrix; QImage* subBufferImage; QRect subRect;

    qreal gradientWidth = 0.0;
    for( int j=0; j<v->area[i].vertex.size(); j++)
    {
        qreal width = v->curve[ v->area[i].getVertexRef(j).curveNumber ].getFeather();
        if ( width > gradientWidth) gradientWidth = width;
    }

    QPainterPath path;
    VertexRef P1Ref, P2Ref;
    QPointF P1, P2, C1, C2;
    for( int j=1; j<v->area[i].vertex.size(); j++)
    {
        P1Ref = v->area[i].vertex[j-1];
        P2Ref = v->area[i].vertex[j];
        P1 = v->getVertex( P1Ref );
        P2 = v->getVertex( P2Ref );
        bool P1invisible = v->curve[ P1Ref.curveNumber ].isInvisible();
        bool P2invisible = v->curve[ P2Ref.curveNumber ].isInvisible();
        if ( P1invisible || P2invisible )
        {
            if (P1Ref.vertexNumber < P2Ref.vertexNumber )   // the points follow the curve progression
            {
                C1 = v->getC1( P2Ref );
                C2 = v->getC2( P2Ref );
            }
            else
            {
                C2 =  v->getC1( P1Ref );
                C1 =  v->getC2( P1Ref );
            }
            if ( j==1 || !P1invisible) path.moveTo(P1);
            path.cubicTo(C1,C2,P2);
        }
    }
    painter.setBrush(Qt::NoBrush);
    qreal nsteps = gradientWidth;
    for(int incr=0; incr < nsteps; incr++)
    {
        QColor colour2 = colour;
        colour2.setAlphaF( (1.0*incr)/(2*nsteps) );
        QPen pen = QPen(colour2);
        pen.setWidth( (gradientWidth*(nsteps-incr))/nsteps );
        painter.setPen( pen );
        painter.drawPath(path);
    }
}

void Gradient::paint2(QPainter& painter, VectorImage* v, int i, int gradients)
{

    QMatrix painterMatrix = painter.worldMatrix();
    qreal scale = qAbs(painterMatrix.m11()) + qAbs(painterMatrix.m12()); // quick overestimation of sqrt( m11*m22 - m12*m21 )
    QColor colour = v->getColour(v->area[i].colourNumber);

    bool buffer = false;
    bool subBuffer = true;
    QPainter painter2;
    QMatrix painter2Matrix;
    QImage* bufferImage = NULL;
    QRect rect;
    QPainter painter3;
    QMatrix painter3Matrix;
    QImage* subBufferImage;
    QRect subRect;

    qreal gradientWidth = 0.0;
    for( int j=0; j<v->area[i].vertex.size(); j++)
    {
        qreal width = v->curve[ v->area[i].getVertexRef(j).curveNumber ].getFeather();
        if ( width > gradientWidth) gradientWidth = width;
    }

    QLinearGradient linearGrad(QPointF(0,0), QPointF(0,1));
    QRadialGradient radialGrad(QPointF(0,0), gradientWidth, QPointF(0,0));
    QColor colour2 = v->getColour(v->area[i].colourNumber);
    colour2.setAlpha(255);
    linearGrad.setColorAt(0, colour2 );
    radialGrad.setColorAt(0, colour2 );
    colour2.setAlpha(217);
    linearGrad.setColorAt(0.2, colour2 );
    radialGrad.setColorAt(0.2, colour2 );
    colour2.setAlpha(134);
    linearGrad.setColorAt(0.4, colour2 );
    radialGrad.setColorAt(0.4, colour2 );
    colour2.setAlpha(94);
    linearGrad.setColorAt(0.5, colour2 );
    radialGrad.setColorAt(0.5, colour2 );
    colour2.setAlpha(60);
    linearGrad.setColorAt(0.6, colour2 );
    radialGrad.setColorAt(0.6, colour2 );
    colour2.setAlpha(36);
    linearGrad.setColorAt(0.7, colour2 );
    radialGrad.setColorAt(0.7, colour2 );
    colour2.setAlpha(20);
    linearGrad.setColorAt(0.8, colour2 );
    radialGrad.setColorAt(0.8, colour2 );
    colour2.setAlpha(10);
    linearGrad.setColorAt(0.9, colour2 );
    radialGrad.setColorAt(0.9, colour2 );
    colour2.setAlpha(0);
    linearGrad.setColorAt(1, colour2 );
    radialGrad.setColorAt(1, colour2 );
    QBrush gradientBrush;

    painter.setRenderHint(QPainter::Antialiasing, false);

    VertexRef P1Ref, P2Ref;
    QPointF P1, P2, C1, C2;
    for( int j=1; j<=v->area[i].vertex.size(); j++)
    {
        P1Ref = v->area[i].getVertexRef(j-1);
        P2Ref = v->area[i].getVertexRef(j);
        P1 = v->getVertex( P1Ref );
        P2 = v->getVertex( P2Ref );
        bool P1invisible = v->curve[ P1Ref.curveNumber ].isInvisible();
        bool P2invisible = v->curve[ P2Ref.curveNumber ].isInvisible();
        qreal width1 = v->curve[ P1Ref.curveNumber ].getFeather();
        qreal width2 = v->curve[ P2Ref.curveNumber ].getFeather();
        gradientWidth = qMax(width1, width2);

        if (P1Ref.vertexNumber < P2Ref.vertexNumber )   // the points follow the curve progression
        {
            C1 = v->getC1( P2Ref );
            C2 = v->getC2( P2Ref );
        }
        else
        {
            C2 =  v->getC1( P1Ref );
            C1 =  v->getC2( P1Ref );
        }
        // -------------------------------------------------------------------------------------------------
        if ( (P1invisible || P2invisible) && gradientWidth != 0.0 )     //if ( QLineF(P1,P2).length() <= 0.5)
        {

            if (  (P1Ref.curveNumber != P2Ref.curveNumber) || (j==v->area[i].vertex.size()) )
            {
                QPointF prevPoint, nextPoint, prevC1, prevC2, nextC1, nextC2;
                if (true)   // j > 1 && j < v->area[i].vertex.size()-1
                {
                    if ( P1Ref.curveNumber != v->area[i].getVertexRef(j-2).curveNumber)   // just to be sure
                    {
                        VertexRef swap = P1Ref;
                        P1Ref = P2Ref;
                        P2Ref = swap;
                    }

                    int prevIndex = -1;
                    do
                    {
                        prevIndex++;
                        prevPoint = v->getVertex( v->area[i].getVertexRef(j-2-prevIndex) );
                    }
                    while ( prevPoint == P1);
                    int nextIndex = -1;
                    do
                    {
                        nextIndex++;
                        nextPoint = v->getVertex( v->area[i].getVertexRef(j+1+nextIndex) );
                    }
                    while ( nextPoint == P2);
                    //qDebug() << "indices =" << prevIndex << nextIndex;

                    if (v->area[i].getVertexRef(j-2).vertexNumber < P1Ref.vertexNumber )   // the points follow the curve progression
                    {
                        prevC2 = v->getC2( v->area[i].getVertexRef(j-1-prevIndex) );
                        prevC1 = v->getC1( v->area[i].getVertexRef(j-1-prevIndex) );
                        if (prevC2 == P1) prevC2 = prevC1;
                    }
                    else
                    {
                        prevC2 = v->getC1( v->area[i].getVertexRef(j-2-prevIndex) );
                        prevC1 = v->getC2( v->area[i].getVertexRef(j-2-prevIndex) );
                        if (prevC2 == P1) prevC2 = prevC1;
                    }
                    if (P2Ref.vertexNumber < v->area[i].getVertexRef(j+1).vertexNumber )   // the points follow the curve progression
                    {
                        nextC1 = v->getC1( v->area[i].getVertexRef(j+1+nextIndex) );
                        nextC2 = v->getC2( v->area[i].getVertexRef(j+1+nextIndex) );
                        if (nextC1 == P2) nextC1 = nextC2;
                    }
                    else
                    {
                        nextC1 = v->getC2( v->area[i].getVertexRef(j+nextIndex) );
                        nextC2 = v->getC1( v->area[i].getVertexRef(j+nextIndex) );
                        if (nextC1 == P2) nextC1 = nextC2;
                    }
                    QLineF tangent1, tangent2, normal1, normal2;
                    tangent1 = QLineF(prevC2, P1).unitVector();
                    QPointF tangentVec1 = (tangent1.p2() - tangent1.p1());
                    tangent2 = QLineF(P2, nextC1).unitVector();
                    QPointF tangentVec2 = (tangent2.p2() - tangent2.p1());
                    normal1 = tangent1.normalVector();
                    QPointF normalVec1 = (normal1.p2() - normal1.p1());
                    normal2 = tangent2.normalVector();
                    QPointF normalVec2 = (normal2.p2() - normal2.p1());

                    qreal t1 = 0.2;
                    QPointF prevClosePoint = P1*(1-t1)*(1-t1)*(1-t1) + 3*prevC2*(1-t1)*(1-t1)*t1 + 3*prevC1*(1-t1)*t1*t1 + t1*t1*t1*prevPoint;
                    if ( v->area[i].path.contains( prevClosePoint + 0.01*normalVec1 ) )  	// we make sure the normal vector points outwards
                    {
                        normalVec1 = -normalVec1;
                    }
                    QPointF nextClosePoint = P2*(1-t1)*(1-t1)*(1-t1) + 3*nextC1*(1-t1)*(1-t1)*t1 + 3*nextC2*(1-t1)*t1*t1 + t1*t1*t1*nextPoint;
                    if ( v->area[i].path.contains( nextClosePoint + 0.01*normalVec2 ) )  	// we make sure the normal vector points outwards
                    {
                        normalVec2 = -normalVec2;
                    }

                    QPointF midPoint = 0.5*(P1+P2);
                    //QLineF midLine = QLineF( midPoint, midPoint + vec1+vec2 ).unitVector();
                    //QPointF midVector = midLine.p2() - midLine.p1();

                    QPointF vec1, vec2, midVector;
                    if (P1invisible && P2invisible)
                    {
                        vec1 = normalVec1;
                        vec2 = normalVec2;
                        midVector = normalVec1 + normalVec2;
                    }
                    if (P1invisible && !P2invisible)
                    {
                        vec1 = normalVec1;
                        vec2 = -tangentVec2;
                        midVector = -tangentVec1 - tangentVec2;
                    }
                    if (!P1invisible && P2invisible)
                    {
                        vec1 = tangentVec1;
                        vec2 = normalVec2;
                        midVector = tangentVec1 + tangentVec2;
                    }
                    //qDebug() << "----->" << midVector << tangentVec1  << prevC2 << P1 << prevPoint << nextPoint << (prevC2 == P1);

                    QPainterPath extension;
                    extension.moveTo( P1 );
                    extension.lineTo( P1 + gradientWidth*vec1);
                    extension.lineTo( 0.5*(P1+P2) + 0.7*gradientWidth*(vec1+vec2) );
                    extension.lineTo( P2 + gradientWidth*vec2);
                    extension.lineTo( P2);
                    extension.closeSubpath();

                    radialGrad.setCenter(P1);
                    radialGrad.setFocalPoint(P1);
                    radialGrad.setRadius(gradientWidth);

                    if (buffer)
                    {
                        // -- mask --
                        //int areaNumber = -1;
                        //int areaNumber = getAreaNumber(P1 + 0.1*(-tangentVec1 + 0.1*normalVec1) );
                        //int areaNumber = getAreaNumber(prevClosePoint + 0.01*normalVec1 );
                        //if ( areaNumber == -1 || areaNumber == i) areaNumber = getAreaNumber(nextClosePoint + 0.01*normalVec2 );
                        int areaNumber = v->getFirstAreaNumber(midPoint + 0.01*midVector);
                        QPainterPath maskPath;
                        if ( areaNumber != -1 && areaNumber != i)
                        {
                            maskPath = painterMatrix.map( v->area[areaNumber].path );
                        }
                        // ----------
                        subRect = painterMatrix.mapRect( extension.controlPointRect() ).toRect();
                        subRect = subRect.intersected( rect );
                        subBufferImage = new QImage(subRect.size(), QImage::Format_ARGB32_Premultiplied );
                        if (!subBufferImage->isNull())
                        {
                            subBufferImage->fill(qRgba(0,0,0,0));
                            painter3.begin(subBufferImage);
                            painter3.setCompositionMode(QPainter::CompositionMode_Source);
                            painter3.setRenderHint(QPainter::Antialiasing, false);
                            painter3Matrix = painterMatrix;
                            painter3.setWorldMatrix( painter3Matrix.translate( -subRect.left()/scale, -subRect.top()/scale ) );

                            painter3.fillPath( extension, QBrush(radialGrad) );
                            //painter3.strokePath( extension, QPen(Qt::black) );
                            painter3.end();

                            for(int x=0; x < subRect.width(); x++ )
                            {
                                for(int y=0; y < subRect.height(); y++ )
                                {
                                    int newX = subRect.left()-rect.left()+x;
                                    int newY = subRect.top()-rect.top()+y;
                                    Q_ASSERT(bufferImage != NULL);
                                    if ( newX >= 0 && newX < bufferImage->width() && newY >=0 && newY < bufferImage->height())
                                    {
                                        if ( qAlpha(subBufferImage->pixel(x,y)) > qAlpha(bufferImage->pixel(newX,newY)) )
                                        {
                                            if (areaNumber == -1 || areaNumber == i)
                                            {
                                                bufferImage->setPixel( newX, newY, subBufferImage->pixel(x,y) );
                                            }
                                            else
                                            {
                                                if ( maskPath.contains(subRect.topLeft() + QPointF(x,y)) )
                                                {
                                                    bufferImage->setPixel( newX, newY, subBufferImage->pixel(x,y) );
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        //qDebug() << x << y << subRect << newX << newY << rect << bufferImage->width() << bufferImage->height();
                                    }
                                }
                            }
                        }
                        delete subBufferImage;

                    }
                    else
                    {
                        //painter.fillPath( extension, QBrush(radialGrad) );
                        painter.strokePath( extension, QPen(Qt::gray) );

                    }
                    /*qDebug() << "coucou---";
                    qDebug() << "coucou" << midPoint << midVector;
                    painter.setBrush( QBrush(Qt::gray) );
                    painter.setPen( Qt::green );
                    painter.drawRect( QRectF( midPoint + 0.01*midVector , QSizeF(5/scale, 5/scale)));
                    painter.drawLine( midPoint, midPoint + 0.01*midVector );
                    //painter.drawRect( QRectF( prevClosePoint + 0.01*normalVec1 , QSizeF(5/scale, 5/scale)));
                    //painter.drawLine( prevClosePoint, prevClosePoint + 0.01*normalVec1 );
                    //painter.setPen( Qt::blue );
                    //painter.drawRect( QRectF( nextClosePoint + 0.01*normalVec2 , QSizeF(5/scale, 5/scale)));
                    //painter.drawLine( nextClosePoint, nextClosePoint + 0.01*normalVec2 );
                    painter.setFont( QFont("Helvetica", 12) );
                    painter.setMatrixEnabled(false);
                    //painter.drawText( painterMatrix.map(prevClosePoint + 0.01*normalVec1) + QPointF(5,5), QString::number(i)+">"+QString::number(getAreaNumber(prevClosePoint + 0.01*normalVec1)) );
                    //painter.drawText( painterMatrix.map(nextClosePoint + 0.01*normalVec2) + QPointF(5,5), QString::number(i)+">"+QString::number(getAreaNumber(nextClosePoint + 0.01*normalVec2)) );
                    //painter.drawText( painterMatrix.map(midPoint + 0.01*midVector) + QPointF(5,5), QString::number(i)+">"+QString::number(getAreaNumber(midPoint + 0.01*midVector)) );
                    painter.setMatrixEnabled(true);*/
                }
            }
            // -------------------------------------------------------------------------------------------------
            if ( (P1Ref.curveNumber == P2Ref.curveNumber) && (j!=v->area[i].vertex.size()) && (P1 != P2))   // therefore both P1 and P2 are invisible
            {
                QLineF normal = QLineF(P1, P2).normalVector().unitVector();
                QPointF normalVec = normal.p2() - normal.p1();

                QPointF midPoint = 0.125*(P1 + 3*C1 + 3*C2 + P2);
                int sign = 1;
                if ( v->area[i].path.contains( midPoint - 0.01*normalVec ) )  	// the path of the area (probably) has a negative orientation
                {
                    sign = -1;
                }

                // -- mask --
                //int areaNumber = -1;
                int areaNumber = v->getFirstAreaNumber(midPoint - 0.01*sign*normalVec);
                QPainterPath maskPath;
                if ( areaNumber != -1 )
                {
                    maskPath = painterMatrix.map( v->area[areaNumber].path );
                }
                // ----------

                int subdivisions = 1;
                if (gradients == 1) subdivisions = 4;
                for(int incr = 0; incr < subdivisions; incr++)
                {
                    qreal t1 = (incr-0.0001/scale)/subdivisions;
                    qreal t2 = (incr+1+0.0001/scale)/subdivisions;
                    if (buffer)
                    {
                        t1 = (incr-0.03/scale)/subdivisions;
                        t2 = (incr+1+0.03/scale)/subdivisions;
                    }
                    QPointF Q1 = P1*(1-t1)*(1-t1)*(1-t1) + 3*C1*(1-t1)*(1-t1)*t1 + 3*C2*(1-t1)*t1*t1 + t1*t1*t1*P2;
                    QPointF Q2 = P1*(1-t2)*(1-t2)*(1-t2) + 3*C1*(1-t2)*(1-t2)*t2 + 3*C2*(1-t2)*t2*t2 + t2*t2*t2*P2;
                    QPointF dQ1 = P1*(t1-1)*(1-t1) + C1*(1-t1)*(1-3*t1) + C2*t1*(2-3*t1) + P2*t1*t1;
                    QPointF dQ2 = P1*(t2-1)*(1-t2) + C1*(1-t2)*(1-3*t2) + C2*t2*(2-3*t2) + P2*t2*t2;
                    QLineF normal1 = QLineF(Q1, Q1+dQ1).normalVector().unitVector();
                    QPointF normalVec1 = sign*(normal1.p2() - normal1.p1());
                    QLineF normal2 = QLineF(Q2, Q2+dQ2).normalVector().unitVector();
                    QPointF normalVec2 = sign*(normal2.p2() - normal2.p1());
                    QLineF normal3 = QLineF(Q1, Q2).normalVector().unitVector();
                    QPointF normalVec3 = sign*(normal3.p2() - normal3.p1());

                    normal1 = QLineF(Q1, Q1 - gradientWidth*normalVec1);
                    normal2 = QLineF(Q2, Q2 - gradientWidth*normalVec2);
                    QPointF* intersectionPoint = new QPointF();

                    QPainterPath extension;
                    extension.moveTo(Q1 + 4.0*normalVec1/subdivisions);
                    if ( normal1.intersect(normal2, intersectionPoint) == QLineF::BoundedIntersection)
                    {
                        extension.lineTo( *intersectionPoint );
                    }
                    else
                    {
                        extension.lineTo(Q1 - gradientWidth*normalVec1);
                        extension.lineTo(Q2 - gradientWidth*normalVec2);
                    }
                    extension.lineTo(Q2 + 4.0*normalVec2/subdivisions);
                    //extension.cubicTo(C2+0.5*normalVec, C1+0.5*normalVec, prevPoint+0.5*normalVec);
                    extension.closeSubpath();

                    linearGrad.setStart(Q1);
                    linearGrad.setFinalStop(Q1 - gradientWidth*normalVec3);

                    if (buffer)
                    {
                        if (subBuffer)
                        {

                            subRect = painterMatrix.mapRect( extension.controlPointRect() ).toRect().adjusted(-1,-1,1,1);
                            subRect = subRect.intersected( rect );
                            subBufferImage = new QImage(subRect.size(), QImage::Format_ARGB32_Premultiplied );
                            if (!subBufferImage->isNull())
                            {
                                subBufferImage->fill(qRgba(0,0,0,0));
                                painter3.begin(subBufferImage);
                                painter3.setCompositionMode(QPainter::CompositionMode_Source);
                                painter3.setRenderHint(QPainter::Antialiasing, false);
                                painter3Matrix = painterMatrix;
                                painter3.setWorldMatrix( painter3Matrix.translate( -subRect.left()/scale, -subRect.top()/scale ) );

                                painter3.fillPath( extension, QBrush(linearGrad) );
                                //painter3.strokePath( extension, QPen(Qt::gray) );
                                painter3.end();

                                for(int x=0; x < subRect.width(); x++ )
                                {
                                    for(int y=0; y < subRect.height(); y++ )
                                    {
                                        int newX = subRect.left()-rect.left()+x;
                                        int newY = subRect.top()-rect.top()+y;
                                        if ( newX >= 0 && newX < bufferImage->width() && newY >=0 && newY < bufferImage->height())
                                        {
                                            if ( qAlpha(subBufferImage->pixel(x,y)) > qAlpha(bufferImage->pixel(newX,newY)) )
                                            {
                                                //bufferImage->setPixel( newX, newY, subBufferImage->pixel(x,y) );
                                                if (areaNumber == -1)
                                                {
                                                    bufferImage->setPixel( newX, newY, subBufferImage->pixel(x,y) );
                                                }
                                                else
                                                {
                                                    if ( maskPath.contains(subRect.topLeft() + QPointF(x,y)) )
                                                    {
                                                        bufferImage->setPixel( newX, newY, subBufferImage->pixel(x,y) );
                                                    }
                                                }

                                            }
                                        }
                                        else
                                        {
                                            //qDebug() << x << y << subRect << newX << newY << rect << bufferImage->width() << bufferImage->height();
                                        }
                                    }
                                }
                            }
                            delete subBufferImage;

                        }
                        else
                        {
                            painter2.fillPath( extension, QBrush(linearGrad));
                        }

                    }
                    else
                    {
                        //painter.strokePath( extension, QPen(Qt::gray) );
                        painter.fillPath( extension, QBrush(linearGrad) );
                    }
                    painter.setPen( colour );
                    painter.drawLine( Q1, Q2 );

                    //painter.setPen( Qt::black );
                    //painter.setBrush( QBrush(linearGrad) );
                    //painter.strokePath( extension, QPen(Qt::black) );
                    //painter.setPen( Qt::blue );
                    //painter.drawRect( QRectF(midPoint - 0.01*sign*normalVec, QSizeF(5/scale, 5/scale)));
                }
            }
            //P1 = P2;
            //P1Ref.curveNumber = i;
            //P1Ref.vertexNumber = j;
        }
    }

    if (buffer && !bufferImage->isNull())
    {
        painter2.end();
        painter.setWorldMatrixEnabled(false);
        painter.drawImage( rect.topLeft(), *bufferImage);
        delete bufferImage;
        painter.setWorldMatrixEnabled(true);
    }

}

void Gradient::paint3(QPainter& painter, VectorImage* v, int i, int gradients)
{
    QMatrix painterMatrix = painter.worldMatrix();
    qreal scale = qAbs(painterMatrix.m11()) + qAbs(painterMatrix.m12()); // quick overestimation of sqrt( m11*m22 - m12*m21 )
    QColor colour = v->getColour(v->area[i].colourNumber);
    painter.fillPath( v->area[i].path, colour );

    BitmapImage* buffer = NULL;
    //BitmapImage* buffer = new BitmapImage(NULL);

    //bool subBuffer = true;
    //QPainter painter2; QMatrix painter2Matrix; QImage* bufferImage; QRect rect;
    //QPainter painter3; QMatrix painter3Matrix; QImage* subBufferImage; QRect subRect;

    qreal gradientWidth = 0.0;
    for( int j=0; j<v->area[i].vertex.size(); j++)
    {
        if ( v->curve[ v->area[i].getVertexRef(j).curveNumber ].isInvisible() )
        {
            qreal width = v->curve[ v->area[i].getVertexRef(j).curveNumber ].getFeather();
            if ( width > gradientWidth) gradientWidth = width;
        }
    }
    if (gradientWidth == 0.0) return;

    // ---- linear and radial gradient of exponentially decaying colour ---
    QLinearGradient linearGrad(QPointF(0,0), QPointF(0,1));
    QRadialGradient radialGrad(QPointF(0,0), gradientWidth, QPointF(0,0));
    QColor colour2 = v->getColour(v->area[i].colourNumber);
    colour2.setAlpha(qRound(255*colour.alphaF()));
    linearGrad.setColorAt(0, colour2 );
    radialGrad.setColorAt(0, colour2 );
    colour2.setAlpha(qRound(217*colour.alphaF()));
    linearGrad.setColorAt(0.2, colour2 );
    radialGrad.setColorAt(0.2, colour2 );
    colour2.setAlpha(qRound(134*colour.alphaF()));
    linearGrad.setColorAt(0.4, colour2 );
    radialGrad.setColorAt(0.4, colour2 );
    colour2.setAlpha(qRound(94*colour.alphaF()));
    linearGrad.setColorAt(0.5, colour2 );
    radialGrad.setColorAt(0.5, colour2 );
    colour2.setAlpha(qRound(60*colour.alphaF()));
    linearGrad.setColorAt(0.6, colour2 );
    radialGrad.setColorAt(0.6, colour2 );
    colour2.setAlpha(qRound(36*colour.alphaF()));
    linearGrad.setColorAt(0.7, colour2 );
    radialGrad.setColorAt(0.7, colour2 );
    colour2.setAlpha(qRound(20*colour.alphaF()));
    linearGrad.setColorAt(0.8, colour2 );
    radialGrad.setColorAt(0.8, colour2 );
    colour2.setAlpha(qRound(10*colour.alphaF()));
    linearGrad.setColorAt(0.9, colour2 );
    radialGrad.setColorAt(0.9, colour2 );
    colour2.setAlpha(0);
    linearGrad.setColorAt(1, colour2 );
    radialGrad.setColorAt(1, colour2 );
    QBrush gradientBrush;

    painter.setRenderHint(QPainter::Antialiasing, false);

    // ---- loop over all the points of the contour ---
    VertexRef P1Ref, P2Ref;
    QPointF P1, P2, C1, C2;
    for( int j=1; j<=v->area[i].vertex.size(); j++)
    {

        P1Ref = v->area[i].getVertexRef(j-1);
        P2Ref = v->area[i].getVertexRef(j);
        P1 = v->getVertex( P1Ref );
        P2 = v->getVertex( P2Ref );
        bool P1invisible = v->curve[ P1Ref.curveNumber ].isInvisible();
        bool P2invisible = v->curve[ P2Ref.curveNumber ].isInvisible();
        qreal width1 = v->curve[ P1Ref.curveNumber ].getFeather();
        qreal width2 = v->curve[ P2Ref.curveNumber ].getFeather();
        gradientWidth = qMax(width1, width2);

        if (P1Ref.vertexNumber < P2Ref.vertexNumber )   // the points follow the curve progression
        {
            C1 = v->getC1( P2Ref );
            C2 = v->getC2( P2Ref );
        }
        else
        {
            C2 =  v->getC1( P1Ref );
            C1 =  v->getC2( P1Ref );
        }
        // -------------------------- one of the two points is invisible ------------------------------------
        if ( (P1invisible || P2invisible) && gradientWidth != 0.0 )     //if ( QLineF(P1,P2).length() <= 0.5)
        {

            // ----------------------- junction between two curves ("angular point") -----------------------------
            if (  (P1Ref.curveNumber != P2Ref.curveNumber) || (j==v->area[i].vertex.size()) )
            {
                QPointF prevPoint, nextPoint, prevC1, prevC2, nextC1, nextC2;
                if (true)   // j > 1 && j < v->area[i].vertex.size()-1
                {
                    if ( P1Ref.curveNumber != v->area[i].getVertexRef(j-2).curveNumber)   // just to be sure
                    {
                        VertexRef swap = P1Ref;
                        P1Ref = P2Ref;
                        P2Ref = swap;
                    }

                    int prevIndex = -1;
                    do
                    {
                        prevIndex++;
                        prevPoint = v->getVertex( v->area[i].getVertexRef(j-2-prevIndex) );
                    }
                    while ( prevPoint == P1);
                    int nextIndex = -1;
                    do
                    {
                        nextIndex++;
                        nextPoint = v->getVertex( v->area[i].getVertexRef(j+1+nextIndex) );
                    }
                    while ( nextPoint == P2);
                    //qDebug() << "indices =" << prevIndex << nextIndex;

                    if (v->area[i].getVertexRef(j-2).vertexNumber < P1Ref.vertexNumber )   // the points follow the curve progression
                    {
                        prevC2 = v->getC2( v->area[i].getVertexRef(j-1-prevIndex) );
                        prevC1 = v->getC1( v->area[i].getVertexRef(j-1-prevIndex) );
                        if (prevC2 == P1) prevC2 = prevC1;
                    }
                    else
                    {
                        prevC2 = v->getC1( v->area[i].getVertexRef(j-2-prevIndex) );
                        prevC1 = v->getC2( v->area[i].getVertexRef(j-2-prevIndex) );
                        if (prevC2 == P1) prevC2 = prevC1;
                    }
                    if (P2Ref.vertexNumber < v->area[i].getVertexRef(j+1).vertexNumber )   // the points follow the curve progression
                    {
                        nextC1 = v->getC1( v->area[i].getVertexRef(j+1+nextIndex) );
                        nextC2 = v->getC2( v->area[i].getVertexRef(j+1+nextIndex) );
                        if (nextC1 == P2) nextC1 = nextC2;
                    }
                    else
                    {
                        nextC1 = v->getC2( v->area[i].getVertexRef(j+nextIndex) );
                        nextC2 = v->getC1( v->area[i].getVertexRef(j+nextIndex) );
                        if (nextC1 == P2) nextC1 = nextC2;
                    }
                    QLineF tangent1, tangent2, normal1, normal2;
                    tangent1 = QLineF(prevC2, P1).unitVector();
                    QPointF tangentVec1 = (tangent1.p2() - tangent1.p1());
                    tangent2 = QLineF(P2, nextC1).unitVector();
                    QPointF tangentVec2 = (tangent2.p2() - tangent2.p1());
                    normal1 = tangent1.normalVector();
                    QPointF normalVec1 = (normal1.p2() - normal1.p1());
                    normal2 = tangent2.normalVector();
                    QPointF normalVec2 = (normal2.p2() - normal2.p1());

                    qreal t1 = 0.2;
                    QPointF prevClosePoint = P1*(1-t1)*(1-t1)*(1-t1) + 3*prevC2*(1-t1)*(1-t1)*t1 + 3*prevC1*(1-t1)*t1*t1 + t1*t1*t1*prevPoint;
                    if ( v->area[i].path.contains( prevClosePoint + 0.01*normalVec1 ) )  	// we make sure the normal vector points outwards
                    {
                        normalVec1 = -normalVec1;
                    }
                    QPointF nextClosePoint = P2*(1-t1)*(1-t1)*(1-t1) + 3*nextC1*(1-t1)*(1-t1)*t1 + 3*nextC2*(1-t1)*t1*t1 + t1*t1*t1*nextPoint;
                    if ( v->area[i].path.contains( nextClosePoint + 0.01*normalVec2 ) )  	// we make sure the normal vector points outwards
                    {
                        normalVec2 = -normalVec2;
                    }

                    QPointF midPoint = 0.5*(P1+P2);
                    //QLineF midLine = QLineF( midPoint, midPoint + vec1+vec2 ).unitVector();
                    //QPointF midVector = midLine.p2() - midLine.p1();

                    QPointF vec1, vec2, midVector;
                    if (P1invisible && P2invisible)
                    {
                        vec1 = normalVec1;
                        vec2 = normalVec2;
                        midVector = normalVec1 + normalVec2;
                    }
                    if (P1invisible && !P2invisible)
                    {
                        vec1 = normalVec1;
                        vec2 = -tangentVec2;
                        midVector = -tangentVec1 - tangentVec2;
                    }
                    if (!P1invisible && P2invisible)
                    {
                        vec1 = tangentVec1;
                        vec2 = normalVec2;
                        midVector = tangentVec1 + tangentVec2;
                    }
                    //qDebug() << "----->" << midVector << tangentVec1  << prevC2 << P1 << prevPoint << nextPoint << (prevC2 == P1);

                    QPainterPath extension;
                    extension.moveTo( P1 );
                    extension.lineTo( P1 + gradientWidth*vec1);
                    extension.lineTo( 0.5*(P1+P2) + 0.7*gradientWidth*(vec1+vec2) );
                    extension.lineTo( P2 + gradientWidth*vec2);
                    extension.lineTo( P2);
                    extension.closeSubpath();

                    radialGrad.setCenter(P1);
                    radialGrad.setFocalPoint(P1);
                    radialGrad.setRadius(gradientWidth);

                    int areaNumber = v->getFirstAreaNumber(midPoint + 1.01*(vec1+vec2) );
                    if (areaNumber != -1)
                    {
                        painter.setClipping(true);
                        painter.setClipPath( v->area[areaNumber].path );
                        //painter.fillPath( extension, QBrush(radialGrad) );
                        //painter.setClipPath( v->area[i].path, Qt::UniteClip );
                        painter.fillPath( extension, QBrush(radialGrad) );
                        painter.setClipping(false);
                        //if (niceGradients) painter.strokePath( extension, QPen(Qt::gray) );
                    }
                    //painter.strokePath( extension, QPen(Qt::gray) );
                    //if (areaNumber!=-1) painter.fillRect( QRectF(midPoint + 1.01*(vec1+vec2), QSizeF(0.2,0.2) ), Qt::red );
                    //if (areaNumber==-1) painter.fillRect( QRectF(midPoint + 1.01*(vec1+vec2), QSizeF(0.2,0.2) ), Qt::blue );
                }
            }

            // -------------------- the two points are separated and on the same curve ------------------------------
            if ( (P1Ref.curveNumber == P2Ref.curveNumber) && (j!=v->area[i].vertex.size()) && (P1 != P2))   // therefore both P1 and P2 are invisible
            {
                QLineF normal = QLineF(P1, P2).normalVector().unitVector();
                QPointF normalVec = normal.p2() - normal.p1();

                QPointF midPoint = 0.125*(P1 + 3*C1 + 3*C2 + P2);
                int sign = 1;
                if ( v->area[i].path.contains( midPoint - 0.01*normalVec ) )  	// the path of the area (probably) has a negative orientation
                {
                    sign = -1;
                }
                //sign = -sign;

                // -- mask --
                //int areaNumber = -1;
                int areaNumber = v->getFirstAreaNumber(midPoint - 0.01*sign*normalVec);
                //QPainterPath maskPath;
                /*if ( areaNumber != -1 ) {
                	//maskPath = painterMatrix.map( v->area[areaNumber].path );
                	painter.setClipping(true);
                	painter.setClipPath( v->area[areaNumber].path );
                } else {
                	painter.setClipping(false);
                }*/
                //painter.setClipping(true);
                //painter.setClipPath( v->area[i].path );

                // ----------

                int subdivisions = 1;
                if (gradients == 1) subdivisions = 4;
                for(int incr = 0; incr < subdivisions; incr++)
                {
                    qreal t1 = (incr-0.0001/scale)/subdivisions;
                    qreal t2 = (incr+1+0.0001/scale)/subdivisions;
                    //if (buffer) {
                    //	t1 = (incr-0.03/scale)/subdivisions;
                    //	t2 = (incr+1+0.03/scale)/subdivisions;
                    //}
                    QPointF Q1 = P1*(1-t1)*(1-t1)*(1-t1) + 3*C1*(1-t1)*(1-t1)*t1 + 3*C2*(1-t1)*t1*t1 + t1*t1*t1*P2;
                    QPointF Q2 = P1*(1-t2)*(1-t2)*(1-t2) + 3*C1*(1-t2)*(1-t2)*t2 + 3*C2*(1-t2)*t2*t2 + t2*t2*t2*P2;
                    QPointF dQ1 = P1*(t1-1)*(1-t1) + C1*(1-t1)*(1-3*t1) + C2*t1*(2-3*t1) + P2*t1*t1;
                    QPointF dQ2 = P1*(t2-1)*(1-t2) + C1*(1-t2)*(1-3*t2) + C2*t2*(2-3*t2) + P2*t2*t2;
                    QLineF normal1 = QLineF(Q1, Q1+dQ1).normalVector().unitVector();
                    QPointF normalVec1 = sign*(normal1.p2() - normal1.p1());
                    QLineF normal2 = QLineF(Q2, Q2+dQ2).normalVector().unitVector();
                    QPointF normalVec2 = sign*(normal2.p2() - normal2.p1());
                    QLineF normal3 = QLineF(Q1, Q2).normalVector().unitVector();
                    QPointF normalVec3 = sign*(normal3.p2() - normal3.p1());

                    normal1 = QLineF(Q1, Q1 - gradientWidth*normalVec1);
                    normal2 = QLineF(Q2, Q2 - gradientWidth*normalVec2);
                    QPointF* intersectionPoint = new QPointF();

                    QPainterPath extension;
                    extension.moveTo(Q1 + 4.0*normalVec1/subdivisions);
                    if ( normal1.intersect(normal2, intersectionPoint) == QLineF::BoundedIntersection)
                    {
                        extension.lineTo( *intersectionPoint );
                    }
                    else
                    {
                        extension.lineTo(Q1 - gradientWidth*normalVec1);
                        extension.lineTo(Q2 - gradientWidth*normalVec2);
                    }
                    extension.lineTo(Q2 + 4.0*normalVec2/subdivisions);
                    //extension.cubicTo(C2+0.5*normalVec, C1+0.5*normalVec, prevPoint+0.5*normalVec);
                    extension.closeSubpath();

                    linearGrad.setStart(Q1);
                    linearGrad.setFinalStop(Q1 - gradientWidth*normalVec3);

                    if (buffer)
                    {
                        /*if (subBuffer) {

                        	subRect = painterMatrix.mapRect( extension.controlPointRect() ).toRect().adjusted(-1,-1,1,1);
                        	subRect = subRect.intersected( rect );
                        	subBufferImage = new QImage(subRect.size(), QImage::Format_ARGB32_Premultiplied );
                        	if (!subBufferImage->isNull()) {
                        		subBufferImage->fill(qRgba(0,0,0,0));
                        		painter3.begin(subBufferImage);
                        		painter3.setCompositionMode(QPainter::CompositionMode_Source);
                        		painter3.setRenderHint(QPainter::Antialiasing, false);
                        		painter3Matrix = painterMatrix;
                        		painter3.setWorldMatrix( painter3Matrix.translate( -subRect.left()/scale, -subRect.top()/scale ) );

                        		painter3.fillPath( extension, QBrush(linearGrad) );
                        		//painter3.strokePath( extension, QPen(Qt::gray) );
                        		painter3.end();

                        		for(int x=0; x < subRect.width(); x++ ) {
                        			for(int y=0; y < subRect.height(); y++ ) {
                        				int newX = subRect.left()-rect.left()+x;
                        				int newY = subRect.top()-rect.top()+y;
                        				if ( newX >= 0 && newX < bufferImage->width() && newY >=0 && newY < bufferImage->height()) {
                        					if ( qAlpha(subBufferImage->pixel(x,y)) > qAlpha(bufferImage->pixel(newX,newY)) ) {
                        						//bufferImage->setPixel( newX, newY, subBufferImage->pixel(x,y) );
                        							if (areaNumber == -1) {
                        								bufferImage->setPixel( newX, newY, subBufferImage->pixel(x,y) );
                        							} else {
                        								if ( maskPath.contains(subRect.topLeft() + QPointF(x,y)) ) {
                        									bufferImage->setPixel( newX, newY, subBufferImage->pixel(x,y) );
                        								}
                        							}

                        					}
                        				} else {
                        					//qDebug() << x << y << subRect << newX << newY << rect << bufferImage->width() << bufferImage->height();
                        				}
                        			}
                        		}
                        	}
                        	delete subBufferImage;

                        } else {
                        	painter2.fillPath( extension, QBrush(linearGrad));
                        }*/

                    }
                    else
                    {
                        if ( areaNumber != -1 )
                        {
                            painter.setClipping(true);
                            painter.setClipPath( v->area[areaNumber].path );
                            painter.fillPath( extension, QBrush(linearGrad) );
                            painter.setClipPath( v->area[i].path );
                            painter.fillPath( extension, QBrush(linearGrad) );
                            painter.setClipping(false);
                        }
                        else
                        {
                            //painter.fillPath( extension, QBrush(linearGrad) );
                        }
                        //if (niceGradients) painter.strokePath( extension, QPen(Qt::gray) );
                    }
                    painter.setPen( colour );
                    painter.drawLine( Q1, Q2 );

                    //painter.setPen( Qt::black );
                    //painter.setBrush( QBrush(linearGrad) );
                    //painter.strokePath( extension, QPen(Qt::black) );
                    //painter.setPen( Qt::blue );
                    //painter.drawRect( QRectF(midPoint - 0.01*sign*normalVec, QSizeF(5/scale, 5/scale)));
                }
            }
            //P1 = P2;
            //P1Ref.curveNumber = i;
            //P1Ref.vertexNumber = j;
        }
    }

    /*if (buffer && !bufferImage->isNull()) {
    	painter2.end();
    	painter.setWorldMatrixEnabled(false);
    	painter.drawImage( rect.topLeft(), *bufferImage);
    	delete bufferImage;
    	painter.setWorldMatrixEnabled(true);
    }*/

}

void Gradient::paint4(QPainter& painter, VectorImage* v, int i, int gradients)
{
    Q_UNUSED(gradients);

    //QMatrix painterMatrix = painter.worldMatrix();
    //qreal scale = qAbs(painterMatrix.m11()) + qAbs(painterMatrix.m12()); // quick overestimation of sqrt( m11*m22 - m12*m21 )
    QColor colour = v->getColour(v->area[i].colourNumber);
    BitmapImage* buffer = new BitmapImage(NULL);
    //BitmapImage* buffer = NULL;

    painter.fillPath( v->area[i].path, colour );
    // ---- find the gradient width
    qreal gradientWidth = 0.0;
    for( int j=0; j<v->area[i].vertex.size(); j++)
    {
        if ( v->curve[ v->area[i].getVertexRef(j).curveNumber ].isInvisible() )
        {
            qreal width = v->curve[ v->area[i].getVertexRef(j).curveNumber ].getFeather();
            if ( width > gradientWidth) gradientWidth = width;
        }
    }
    if (gradientWidth == 0.0)
    {
        return;
    }

    // ---- find the area just below that area
    VertexRef P1Ref, P2Ref;
    QPointF midPoint;
    bool pointInside = false;
    int j=1;
    while (!pointInside && j<=v->area[i].vertex.size())
    {
        P1Ref = v->area[i].getVertexRef(0);
        P2Ref = v->area[i].getVertexRef(j);
        midPoint = 0.5*( v->getVertex( P1Ref ) + v->getVertex( P2Ref ) );
        pointInside = v->area[i].path.contains( midPoint );
        j++;
    }
    if (!pointInside) return;

    int areaNumber = v->getFirstAreaNumber(midPoint);
    if (areaNumber != -1)
    {
        painter.setClipping(true);
        painter.setClipPath( v->area[areaNumber].path );
    }
    else
    {
        //painter.setClipping(false);
        return;
    }

    /*qreal gradientWidth = 0.0;
    for( int j=0; j<v->area[i].vertex.size(); j++) {
    	if ( v->curve[ v->area[i].getVertexRef(j).curveNumber ].isInvisible() ) {
    		qreal width = v->curve[ v->area[i].getVertexRef(j).curveNumber ].getFeather();
    		if ( width > gradientWidth) gradientWidth = width;
    	}
    }
    if (gradientWidth == 0.0) return;*/

    // ---- radial gradient with gaussian decaying ---
    QRadialGradient radialGrad(QPointF(0,0), gradientWidth, QPointF(0,0));
    QColor colour2 = v->getColour(v->area[i].colourNumber);
    //colour2 = Qt::black;
    colour2.setAlpha(255);
    radialGrad.setColorAt(0, colour2 );
    colour2.setAlpha(217);
    radialGrad.setColorAt(0.2, colour2 );
    colour2.setAlpha(134);
    radialGrad.setColorAt(0.4, colour2 );
    colour2.setAlpha(94);
    radialGrad.setColorAt(0.5, colour2 );
    colour2.setAlpha(60);
    radialGrad.setColorAt(0.6, colour2 );
    colour2.setAlpha(36);
    radialGrad.setColorAt(0.7, colour2 );
    colour2.setAlpha(20);
    radialGrad.setColorAt(0.8, colour2 );
    colour2.setAlpha(10);
    radialGrad.setColorAt(0.9, colour2 );
    colour2.setAlpha(0);
    radialGrad.setColorAt(1, colour2 );

    /*colour2.setAlpha(255);
    radialGrad.setColorAt(0, colour2 );
    colour2.setAlpha(212);
    radialGrad.setColorAt(0.2, colour2 );
    colour2.setAlpha(142);
    radialGrad.setColorAt(0.4, colour2 );
    colour2.setAlpha(107);
    radialGrad.setColorAt(0.5, colour2 );
    colour2.setAlpha(77);
    radialGrad.setColorAt(0.6, colour2 );
    colour2.setAlpha(32);
    radialGrad.setColorAt(0.7, colour2 );
    colour2.setAlpha(16);
    radialGrad.setColorAt(0.8, colour2 );
    colour2.setAlpha(3);
    radialGrad.setColorAt(0.9, colour2 );
    colour2.setAlpha(0);
    radialGrad.setColorAt(1, colour2 );*/

    /*colour2.setAlpha(255);
    radialGrad.setColorAt(0, colour2 );
    colour2.setAlpha(204);
    radialGrad.setColorAt(0.2, colour2 );
    colour2.setAlpha(153);
    radialGrad.setColorAt(0.4, colour2 );
    colour2.setAlpha(127);
    radialGrad.setColorAt(0.5, colour2 );
    colour2.setAlpha(102);
    radialGrad.setColorAt(0.6, colour2 );
    colour2.setAlpha(76);
    radialGrad.setColorAt(0.7, colour2 );
    colour2.setAlpha(51);
    radialGrad.setColorAt(0.8, colour2 );
    colour2.setAlpha(25);
    radialGrad.setColorAt(0.9, colour2 );
    colour2.setAlpha(0);
    radialGrad.setColorAt(1, colour2 );*/

    // gradient for 1/2 spacings blended
    /*colour2.setAlpha(255);
    radialGrad.setColorAt(0, colour2 );
    colour2.setAlpha(234);
    radialGrad.setColorAt(0.1, colour2 );
    colour2.setAlpha(190);
    radialGrad.setColorAt(0.2, colour2 );
    colour2.setAlpha(92);
    radialGrad.setColorAt(0.4, colour2 );
    colour2.setAlpha(58);
    radialGrad.setColorAt(0.5, colour2 );
    colour2.setAlpha(36);
    radialGrad.setColorAt(0.6, colour2 );
    colour2.setAlpha(23);
    radialGrad.setColorAt(0.7, colour2 );
    colour2.setAlpha(14);
    radialGrad.setColorAt(0.8, colour2 );
    colour2.setAlpha(7);
    radialGrad.setColorAt(0.9, colour2 );
    colour2.setAlpha(0);
    radialGrad.setColorAt(1, colour2 );*/

    // gradient for 1/3 spacings blended
    /*colour2.setAlpha(255);
    radialGrad.setColorAt(0, colour2 );
    colour2.setAlpha(213);
    radialGrad.setColorAt(0.1, colour2 );
    colour2.setAlpha(168);
    radialGrad.setColorAt(0.2, colour2 );
    colour2.setAlpha(75);
    radialGrad.setColorAt(0.4, colour2 );
    colour2.setAlpha(44);
    radialGrad.setColorAt(0.5, colour2 );
    colour2.setAlpha(25);
    radialGrad.setColorAt(0.6, colour2 );
    colour2.setAlpha(14);
    radialGrad.setColorAt(0.7, colour2 );
    colour2.setAlpha(8);
    radialGrad.setColorAt(0.8, colour2 );
    colour2.setAlpha(3);
    radialGrad.setColorAt(0.9, colour2 );
    colour2.setAlpha(0);
    radialGrad.setColorAt(1, colour2 );*/

    // gradient for 1/2 spacings added
    /*colour2.setAlpha(147);
    radialGrad.setColorAt(0, colour2 );
    colour2.setAlpha(142);
    radialGrad.setColorAt(0.1, colour2 );
    colour2.setAlpha(125);
    radialGrad.setColorAt(0.2, colour2 );
    colour2.setAlpha(76);
    radialGrad.setColorAt(0.4, colour2 );
    colour2.setAlpha(52);
    radialGrad.setColorAt(0.5, colour2 );
    colour2.setAlpha(34);
    radialGrad.setColorAt(0.6, colour2 );
    colour2.setAlpha(20);
    radialGrad.setColorAt(0.7, colour2 );
    colour2.setAlpha(11);
    radialGrad.setColorAt(0.8, colour2 );
    colour2.setAlpha(5);
    radialGrad.setColorAt(0.9, colour2 );
    colour2.setAlpha(0);
    radialGrad.setColorAt(1, colour2 );*/

    painter.setRenderHint(QPainter::Antialiasing, false);

    // ---- loop over all the points of the contour ---
    //VertexRef P1Ref, P2Ref;
    QPointF P1, P2, C1, C2;
    QPointF lastPoint;
    bool lastPointExists = false;
    for( int j=1; j<=v->area[i].vertex.size(); j++)
    {

        P1Ref = v->area[i].getVertexRef(j-1);
        P2Ref = v->area[i].getVertexRef(j);
        P1 = v->getVertex( P1Ref );
        P2 = v->getVertex( P2Ref );
        bool P1invisible = v->curve[ P1Ref.curveNumber ].isInvisible();
        bool P2invisible = v->curve[ P2Ref.curveNumber ].isInvisible();

        Q_UNUSED(P1invisible);
        Q_UNUSED(P2invisible);

        qreal width1 = v->curve[ P1Ref.curveNumber ].getFeather();
        qreal width2 = v->curve[ P2Ref.curveNumber ].getFeather();
        gradientWidth = qMax(width1, width2);

        // -------------------------------------------------------------------------------------------------
        //if ( (P1invisible || P2invisible) && gradientWidth != 0.0 ) {   //if ( QLineF(P1,P2).length() <= 0.5)
        if (gradientWidth != 0.0)
        {
            if (!lastPointExists)
            {
                lastPoint = P1;
            }

            QPointF midPoint;
            if (P1Ref.curveNumber == P1Ref.curveNumber)
            {
                if (P1Ref.vertexNumber < P2Ref.vertexNumber )   // the points follow the curve progression
                {
                    C1 = v->getC1( P2Ref );
                    C2 = v->getC2( P2Ref );
                }
                else
                {
                    C2 =  v->getC1( P1Ref );
                    C1 =  v->getC2( P1Ref );
                }
                midPoint = 0.125*(P1 + 3*C1 + 3*C2 + P2);
            }
            else
            {
                midPoint = 0.5*(P1+P2);
            }
            /*QLineF normal = QLineF(P1, P2).normalVector().unitVector();
            QPointF normalVec = normal.p2() - normal.p1();
            if ( v->area[i].path.contains( midPoint + 0.1*normalVec ) ) {
            	normalVec = -normalVec;
            }
            int areaNumber = v->getAreaNumber(midPoint + 0.1*normalVec);

            if (areaNumber != -1 && areaNumber != i) {
            	painter.setClipping(true);
            	painter.setClipPath( v->area[areaNumber].path );
            } else {
            	painter.setClipping(false);
            }*/

            qreal distance = 4.0*QLineF(P2, lastPoint).length();
            int steps = qRound( distance /gradientWidth );
            for(int i=0; i<steps; i++)
            {
                //if (i>0 || !lastPointExists) {
                if (true)
                {
                    //QPointF thePoint = lastPoint + (i)*(gradientWidth)*(P2 -lastPoint)/distance;
                    qreal t1 = i*gradientWidth/distance;
                    QPointF thePoint = P1*(1-t1)*(1-t1)*(1-t1) + 3*C1*(1-t1)*(1-t1)*t1 + 3*C2*(1-t1)*t1*t1 + t1*t1*t1*P2;

                    //QPointF thePoint2 = thePoint - 0.00*gradientWidth*normalVec;
                    QPointF thePoint2 = thePoint;
                    radialGrad.setCenter( thePoint2 );
                    radialGrad.setFocalPoint( thePoint2 );
                    radialGrad.setRadius( gradientWidth );
                    QRectF rectangle(thePoint2.x()-gradientWidth, thePoint2.y()-gradientWidth, 2*gradientWidth, 2*gradientWidth);


                    if (buffer)
                    {
                        BitmapImage* subBuffer = new BitmapImage(NULL);
                        subBuffer->drawRect( rectangle, Qt::NoPen, radialGrad, QPainter::CompositionMode_SourceOver, false);
                        buffer->add(subBuffer);
                        delete subBuffer;
                    }
                    else
                    {
                        painter.fillRect( rectangle, radialGrad );
                    }

                    //painter.setPen( Qt::black );
                    //painter.drawEllipse( rectangle );
                    //painter.setPen( Qt::red );
                    //painter.drawRect( QRectF(thePoint, QSizeF(2,2)) );

                    if (i==steps-1) lastPoint = thePoint;
                }
            }
            if (!lastPointExists)
            {
                lastPointExists = true;
            }
        }
        else
        {
            //lastPointExists = false;
        }
    }
    if (buffer)
    {
        if (buffer->image) painter.drawImage(buffer->topLeft(), *(buffer->image));
        delete buffer;
    }
}

void Gradient::paint5(QPainter& painter, VectorImage* v, int i, int gradients)
{
    //BitmapImage* buffer = new BitmapImage(NULL);
    //BitmapImage* buffer = NULL;
    QColor colour = v->getColour(v->area[i].colourNumber);

    // ---- find the gradient width
    qreal gradientWidth = 0.0;
    for( int j=0; j<v->area[i].vertex.size(); j++)
    {
        if ( v->curve[ v->area[i].getVertexRef(j).curveNumber ].isInvisible() )
        {
            qreal width = v->curve[ v->area[i].getVertexRef(j).curveNumber ].getFeather();
            if ( width > gradientWidth) gradientWidth = width;
        }
    }
    if (gradientWidth == 0.0)
    {
        painter.fillPath( v->area[i].path, colour );
        return;
    }
    gradientWidth = gradientWidth * sqrt( painter.worldMatrix().determinant() );

    // ---- find the area just below that area
    VertexRef P1Ref, P2Ref;
    QPointF midPoint;
    bool pointInside = false;
    int j=1;
    while (!pointInside && j<=v->area[i].vertex.size())
    {
        P1Ref = v->area[i].getVertexRef(0);
        P2Ref = v->area[i].getVertexRef(j);
        midPoint = 0.5*( v->getVertex( P1Ref ) + v->getVertex( P2Ref ) );
        pointInside = v->area[i].path.contains( midPoint );
        j++;
    }
    if (!pointInside) return;

    int areaNumber = v->getFirstAreaNumber(midPoint);
    if (areaNumber != -1)
    {
        painter.setClipping(true);
        painter.setClipPath( v->area[areaNumber].path );
    }
    else
    {
        //painter.setClipping(false);
        return;
    }

    // ---- blur the area
    QMatrix painterMatrix = painter.worldMatrix();
    QPainterPath path = painterMatrix.map( v->area[i].path );
    BitmapImage* buffer = new BitmapImage(NULL);
    //buffer->drawPath( path, Qt::NoPen, colour, QPainter::CompositionMode_SourceOver, false);
    buffer->drawPath( path, QPen(colour, gradientWidth), colour, QPainter::CompositionMode_SourceOver, false);
    if (gradients == 3) buffer->blur2(0.9*gradientWidth);
    if (gradients == 4) buffer->blur(0.9*gradientWidth);


    /*QPointF P1, P2, C1, C2;
    int localGradientWidth;
    for( int j=1; j<v->area[i].vertex.size(); j++) {

    	P1Ref = painterMatrix.map( v->area[i].getVertexRef(j-1) );
    	P2Ref = painterMatrix.map( v->area[i].getVertexRef(j) );
    	P1 = v->getVertex( P1Ref );
    	P2 = v->getVertex( P2Ref );
    	bool P1invisible = v->curve[ P1Ref.curveNumber ].isInvisible();
    	bool P2invisible = v->curve[ P2Ref.curveNumber ].isInvisible();
    	qreal width1 = v->curve[ P1Ref.curveNumber ].getFeather();
    	qreal width2 = v->curve[ P2Ref.curveNumber ].getFeather();
    	localGradientWidth = qMax(width1, width2);
    	if (localGradientWidth == 0.0) localGradientWidth = gradientWidth;

    	// -------------------------------------------------------------------------------------------------
    	if ( (P1invisible || P2invisible) && gradientWidth != 0.0 ) {   //if ( QLineF(P1,P2).length() <= 0.5)
    	//if (localGradientWidth != 0.0) {
    		//if (!lastPointExists) {
    		//	lastPoint = P1;
    		//	lastPointExists = true;
    		//}

    			if (P1Ref.curveNumber == P1Ref.curveNumber) {
    				if (P1Ref.vertexNumber < P2Ref.vertexNumber ) { // the points follow the curve progression
    					C1 = v->getC1( P2Ref );
    					C2 = v->getC2( P2Ref );
    				} else {
    					C2 =  v->getC1( P1Ref );
    					C1 =  v->getC2( P1Ref );
    				}
    			}

    		qreal distance = 2.0*QLineF(P2, lastPoint).length();
    		int steps = qRound( distance /localGradientWidth );
    		for(int i=0; i<steps; i++) {
    			if (i>=0) {
    				QPointF thePoint = P1 + (i)*(localGradientWidth)*(P2 -P1)/distance;
    				QPointF thePoint2 = thePoint;
    				radialGrad.setCenter( thePoint2 );
    				radialGrad.setFocalPoint( thePoint2 );
    				radialGrad.setRadius( localGradientWidth );
    				QRectF rectangle(thePoint2.x()-localGradientWidth, thePoint2.y()-localGradientWidth, 2*localGradientWidth, 2*localGradientWidth);


    				if (buffer) {
    					BitmapImage* subBuffer = new BitmapImage(NULL);
    					subBuffer->drawRect( rectangle, Qt::NoPen, radialGrad, QPainter::CompositionMode_Source, false);
    					buffer->add(subBuffer);
    					delete subBuffer;
    				} else {
    					painter.fillRect( rectangle, radialGrad );
    				}

    				//painter.setPen( Qt::black );
    				//painter.drawEllipse( rectangle );
    				if (i==steps-1) lastPoint = thePoint;
    			}
    		}
    		//if (!lastPointExists) {
    		//	lastPointExists = true;
    		//}
    	}
    	//} else {
    	//	lastPointExists = false;
    	//}
    }*/







    painter.setWorldMatrixEnabled(false);
    if (buffer->image) painter.drawImage(buffer->topLeft(), *(buffer->image));
    painter.setWorldMatrixEnabled(true);

    delete buffer;

    /*
    // ---- radial gradient with gaussian decaying ---
    QRadialGradient radialGrad(QPointF(0,0), gradientWidth, QPointF(0,0));
    QColor colour2 = v->getColour(v->area[i].colourNumber);
    //colour2 = Qt::black;
    				colour2.setAlpha(255);
    				radialGrad.setColorAt(0, colour2 );
    				colour2.setAlpha(217);
    				radialGrad.setColorAt(0.2, colour2 );
    				colour2.setAlpha(134);
    				radialGrad.setColorAt(0.4, colour2 );
    				colour2.setAlpha(94);
    				radialGrad.setColorAt(0.5, colour2 );
    				colour2.setAlpha(60);
    				radialGrad.setColorAt(0.6, colour2 );
    				colour2.setAlpha(36);
    				radialGrad.setColorAt(0.7, colour2 );
    				colour2.setAlpha(20);
    				radialGrad.setColorAt(0.8, colour2 );
    				colour2.setAlpha(10);
    				radialGrad.setColorAt(0.9, colour2 );
    				colour2.setAlpha(0);
    				radialGrad.setColorAt(1, colour2 );

    				colour2.setAlpha(255);
    				radialGrad.setColorAt(0, colour2 );
    				colour2.setAlpha(212);
    				radialGrad.setColorAt(0.2, colour2 );
    				colour2.setAlpha(142);
    				radialGrad.setColorAt(0.4, colour2 );
    				colour2.setAlpha(107);
    				radialGrad.setColorAt(0.5, colour2 );
    				colour2.setAlpha(77);
    				radialGrad.setColorAt(0.6, colour2 );
    				colour2.setAlpha(32);
    				radialGrad.setColorAt(0.7, colour2 );
    				colour2.setAlpha(16);
    				radialGrad.setColorAt(0.8, colour2 );
    				colour2.setAlpha(3);
    				radialGrad.setColorAt(0.9, colour2 );
    				colour2.setAlpha(0);
    				radialGrad.setColorAt(1, colour2 );

    				colour2.setAlpha(255);
    				radialGrad.setColorAt(0, colour2 );
    				colour2.setAlpha(204);
    				radialGrad.setColorAt(0.2, colour2 );
    				colour2.setAlpha(153);
    				radialGrad.setColorAt(0.4, colour2 );
    				colour2.setAlpha(127);
    				radialGrad.setColorAt(0.5, colour2 );
    				colour2.setAlpha(102);
    				radialGrad.setColorAt(0.6, colour2 );
    				colour2.setAlpha(76);
    				radialGrad.setColorAt(0.7, colour2 );
    				colour2.setAlpha(51);
    				radialGrad.setColorAt(0.8, colour2 );
    				colour2.setAlpha(25);
    				radialGrad.setColorAt(0.9, colour2 );
    				colour2.setAlpha(0);
    				radialGrad.setColorAt(1, colour2 );

    				// gradient for 1/2 spacings blended
    				colour2.setAlpha(255);
    				radialGrad.setColorAt(0, colour2 );
    				colour2.setAlpha(234);
    				radialGrad.setColorAt(0.1, colour2 );
    				colour2.setAlpha(190);
    				radialGrad.setColorAt(0.2, colour2 );
    				colour2.setAlpha(92);
    				radialGrad.setColorAt(0.4, colour2 );
    				colour2.setAlpha(58);
    				radialGrad.setColorAt(0.5, colour2 );
    				colour2.setAlpha(36);
    				radialGrad.setColorAt(0.6, colour2 );
    				colour2.setAlpha(23);
    				radialGrad.setColorAt(0.7, colour2 );
    				colour2.setAlpha(14);
    				radialGrad.setColorAt(0.8, colour2 );
    				colour2.setAlpha(7);
    				radialGrad.setColorAt(0.9, colour2 );
    				colour2.setAlpha(0);
    				radialGrad.setColorAt(1, colour2 );

    				// gradient for 1/3 spacings blended
    				colour2.setAlpha(255);
    				radialGrad.setColorAt(0, colour2 );
    				colour2.setAlpha(213);
    				radialGrad.setColorAt(0.1, colour2 );
    				colour2.setAlpha(168);
    				radialGrad.setColorAt(0.2, colour2 );
    				colour2.setAlpha(75);
    				radialGrad.setColorAt(0.4, colour2 );
    				colour2.setAlpha(44);
    				radialGrad.setColorAt(0.5, colour2 );
    				colour2.setAlpha(25);
    				radialGrad.setColorAt(0.6, colour2 );
    				colour2.setAlpha(14);
    				radialGrad.setColorAt(0.7, colour2 );
    				colour2.setAlpha(8);
    				radialGrad.setColorAt(0.8, colour2 );
    				colour2.setAlpha(3);
    				radialGrad.setColorAt(0.9, colour2 );
    				colour2.setAlpha(0);
    				radialGrad.setColorAt(1, colour2 );

    				// gradient for 1/2 spacings added
    				colour2.setAlpha(147);
    				radialGrad.setColorAt(0, colour2 );
    				colour2.setAlpha(142);
    				radialGrad.setColorAt(0.1, colour2 );
    				colour2.setAlpha(125);
    				radialGrad.setColorAt(0.2, colour2 );
    				colour2.setAlpha(76);
    				radialGrad.setColorAt(0.4, colour2 );
    				colour2.setAlpha(52);
    				radialGrad.setColorAt(0.5, colour2 );
    				colour2.setAlpha(34);
    				radialGrad.setColorAt(0.6, colour2 );
    				colour2.setAlpha(20);
    				radialGrad.setColorAt(0.7, colour2 );
    				colour2.setAlpha(11);
    				radialGrad.setColorAt(0.8, colour2 );
    				colour2.setAlpha(5);
    				radialGrad.setColorAt(0.9, colour2 );
    				colour2.setAlpha(0);
    				radialGrad.setColorAt(1, colour2 );

    painter.setRenderHint(QPainter::Antialiasing, false);

    // ---- loop over all the points of the contour ---
    QPointF lastPoint; bool lastPointExists = false;
    QPointF P1, P2, C1, C2;
    int localGradientWidth;
    for( int j=1; j<v->area[i].vertex.size(); j++) {

    	P1Ref = v->area[i].getVertexRef(j-1);
    	P2Ref = v->area[i].getVertexRef(j);
    	P1 = v->getVertex( P1Ref );
    	P2 = v->getVertex( P2Ref );
    	//bool P1invisible = v->curve[ P1Ref.curveNumber ].isInvisible();
    	//bool P2invisible = v->curve[ P2Ref.curveNumber ].isInvisible();
    	qreal width1 = v->curve[ P1Ref.curveNumber ].getFeather();
    	qreal width2 = v->curve[ P2Ref.curveNumber ].getFeather();
    	localGradientWidth = qMax(width1, width2);
    	if (localGradientWidth == 0.0) localGradientWidth = gradientWidth;

    	// -------------------------------------------------------------------------------------------------
    	//if ( (P1invisible || P2invisible) && gradientWidth != 0.0 ) {   //if ( QLineF(P1,P2).length() <= 0.5)
    	if (localGradientWidth != 0.0) {
    		if (!lastPointExists) {
    			lastPoint = P1;
    			lastPointExists = true;
    		}

    			if (P1Ref.curveNumber == P1Ref.curveNumber) {
    				if (P1Ref.vertexNumber < P2Ref.vertexNumber ) { // the points follow the curve progression
    					C1 = v->getC1( P2Ref );
    					C2 = v->getC2( P2Ref );
    				} else {
    					C2 =  v->getC1( P1Ref );
    					C1 =  v->getC2( P1Ref );
    				}
    			}

    		qreal distance = 2.0*QLineF(P2, lastPoint).length();
    		int steps = qRound( distance /localGradientWidth );
    		for(int i=0; i<steps; i++) {
    			if (i>=0) {
    				QPointF thePoint = lastPoint + (i)*(localGradientWidth)*(P2 -lastPoint)/distance;
    				QPointF thePoint2 = thePoint;
    				radialGrad.setCenter( thePoint2 );
    				radialGrad.setFocalPoint( thePoint2 );
    				radialGrad.setRadius( localGradientWidth );
    				QRectF rectangle(thePoint2.x()-localGradientWidth, thePoint2.y()-localGradientWidth, 2*localGradientWidth, 2*localGradientWidth);


    				if (buffer) {
    					BitmapImage* subBuffer = new BitmapImage(NULL);
    					subBuffer->drawRect( rectangle, Qt::NoPen, radialGrad, QPainter::CompositionMode_Source, false);
    					buffer->add(subBuffer);
    					delete subBuffer;
    				} else {
    					painter.fillRect( rectangle, radialGrad );
    				}

    				//painter.setPen( Qt::black );
    				//painter.drawEllipse( rectangle );
    				if (i==steps-1) lastPoint = thePoint;
    			}
    		}
    		//if (!lastPointExists) {
    		//	lastPointExists = true;
    		//}
    	}
    	//} else {
    	//	lastPointExists = false;
    	//}
    }
    if (buffer) {
    	if (buffer->image) painter.drawImage(buffer->topLeft(), *(buffer->image));
    	delete buffer;
    }*/

}
