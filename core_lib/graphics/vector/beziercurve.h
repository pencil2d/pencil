/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include <QtXml>
#include <QPainter>

class Object;
class Status;

struct Intersection
{
    QPointF point;
    qreal t1 = 0.0;
    qreal t2 = 0.0;
};

class BezierCurve
{
public:
    BezierCurve();
    BezierCurve(QList<QPointF> pointList);
    BezierCurve(QList<QPointF> pointList, QList<qreal> pressureList, double tol);

    Status createDomElement(QXmlStreamWriter &xmlStream);
    void loadDomElement(QDomElement element);

    qreal getWidth() const { return width; }
    qreal getFeather() const { return feather; }
    bool getVariableWidth() const { return variableWidth; }
    int getColourNumber() const { return colourNumber; }
    void decreaseColourNumber() { colourNumber--; }
    int getVertexSize() const { return vertex.size(); }
    QPointF getOrigin() const {	return origin; }
    QPointF getVertex(int i) const { if (i==-1) { return origin; } else { return vertex.at(i);} }
    QPointF getC1(int i) const { return c1.at(i); }
    QPointF getC2(int i) const { return c2.at(i); }
    qreal getPressure(int i) const { return pressure.at(i); }
    bool isSelected(int i) const { return selected.at(i+1); }
    bool isSelected() const { bool result=true; for(int i=0; i<selected.size(); i++) result = result && selected[i]; return result; }
    bool isPartlySelected() const { bool result=false; for(int i=0; i<selected.size(); i++) result = result || selected[i]; return result; }
    bool isInvisible() const { return invisible; }
    bool intersects(QPointF point, qreal distance);
    bool intersects(QRectF rectangle);

    void setOrigin(const QPointF& point);
    void setOrigin(const QPointF& point, const qreal& pressureValue, const bool& trueOrFalse);
    void setC1(int i, const QPointF& point);
    void setC2(int i, const QPointF& point);
    void setVertex(int i, const QPointF& point);
    void setLastVertex(const QPointF& point);
    void setWidth(qreal desiredWidth);
    void setFeather(qreal desiredFeather);
    void setVariableWidth(bool YesOrNo);
    void setInvisibility(bool YesOrNo);
    void setColourNumber(int colourNumber) { this->colourNumber = colourNumber; }
    void setSelected(bool YesOrNo) { for(int i=0; i<selected.size(); i++) { selected[i] = YesOrNo; } }
    void setSelected(int i, bool YesOrNo);

    BezierCurve transformed(QTransform transformation);
    void transform(QTransform transformation);

    void appendCubic(const QPointF& c1Point, const QPointF& c2Point, const QPointF& vertexPoint, qreal pressureValue);
    void addPoint(int position, const QPointF point);
    void addPoint(int position, const qreal t);
    QPointF getPointOnCubic(int i, qreal t);
    void removeVertex(int i);
    QPainterPath getStraightPath();
    QPainterPath getSimplePath();
    QPainterPath getStrokedPath();
    QPainterPath getStrokedPath(qreal width);
    QPainterPath getStrokedPath(qreal width, bool pressure);
    QRectF getBoundingRect();

    void drawPath(QPainter& painter, Object* object, QTransform transformation, bool simplified, bool showThinLines );
    void createCurve(QList<QPointF>& pointList, QList<qreal>& pressureList );
    void smoothCurve();

    static void simplify(double tol, QList<QPointF>& inputList, int j, int k, QList<bool>& markList);

    // general useful functions -> to be placed elsewhere?
    static qreal eLength(const QPointF point); // returns the Euclidean length of a point (seen as a vector)
    static qreal mLength(const QPointF point); // returns the Manhattan length of a point (seen as a vector)
    static void normalise(QPointF& point); // normalises a point (seen as a vector);
    static qreal findDistance(BezierCurve curve, int i, QPointF P, QPointF& nearestPoint, qreal& t); //finds the distance between a cubic section and a point
    static bool findIntersection(BezierCurve curve1, int i1, BezierCurve curve2, int i2, QList<Intersection>& intersections); //finds the intersection between two cubic sections

private:
    QPointF origin;
    QList<QPointF> c1;
    QList<QPointF> c2;
    QList<QPointF> vertex;
    QList<float> pressure; // this list has one more element than the other list (the first element is for the origin)
    int colourNumber = 0;
    float width = 0.f;
    float feather = 0.f;
    bool variableWidth = 0.f;
    bool invisible = false;
    QList<bool> selected; // this list has one more element than the other list (the first element is for the origin)
};

#endif
