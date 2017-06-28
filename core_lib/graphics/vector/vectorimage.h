/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef VECTORIMAGE_H
#define VECTORIMAGE_H


#include <QtXml>
#include <QTransform>
#include <QDebug>
#include <QImage>
#include <QStringList>

#include "bezierarea.h"
#include "beziercurve.h"
#include "vertexref.h"
#include "keyframe.h"

class Object;
class QPainter;

class VectorImage : public KeyFrame
{
public:
    VectorImage();
    virtual ~VectorImage();

    void setObject( Object* pObj ) { mObject = pObj; }

    bool read(QString filePath);
    Status write(QString filePath, QString format);

    Status createDomElement(QXmlStreamWriter& doc);
    void loadDomElement(QDomElement element);

    void insertCurve(int position, BezierCurve& newCurve, qreal factor, bool interacts);
    void addCurve(BezierCurve& newCurve, qreal factor, bool interacts = true);
    void removeCurveAt(int i);
    void select(QRectF rectangle);
    void setSelected(int curveNumber, bool YesOrNo);
    void setSelected(int curveNumber, int vertexNumber, bool YesOrNo);
    void setSelected(VertexRef vertexRef, bool YesOrNo);
    void setSelected(QList<int> curveList, bool YesOrNo);
    void setSelected(QList<VertexRef> vertexList, bool YesOrNo);
    bool isSelected(int curveNumber);
    bool isSelected(int curveNumber, int vertexNumber);
    bool isSelected(VertexRef vertexRef);
    bool isSelected(QList<int> curveList);
    bool isSelected(QList<VertexRef> vertexList);
    void setAreaSelected(int areaNumber, bool YesOrNo);
    bool isAreaSelected(int areaNumber);

    int getFirstSelectedCurve();
    int getFirstSelectedArea();
    void selectAll();
    void deselectAll();
    QRectF getSelectionRect() { return mSelectionRect; }
    void setSelectionRect(QRectF rectange);
    void calculateSelectionRect();
    void deleteSelection();
    void deleteSelectedPoints();
    void removeVertex(int i, int m);

    void paste(VectorImage&);

    QColor getColour(int i);
    int  getColourNumber(QPointF point);
    bool usesColour(int index);
    void removeColour(int index);

    void paintImage(QPainter& painter, bool simplified, bool showThinCurves, bool antialiasing);
    void outputImage(QImage* image, QTransform myView, bool simplified, bool showThinCurves, bool antialiasing); // uses paintImage

    void clear();
    void clean();
    void setSelectionTransformation(QTransform transform);
    void applySelectionTransformation();
    void applySelectionTransformation(QTransform transform);
    void applyColourToSelection(int colourNumber);
    void applyWidthToSelection(qreal width);
    void applyFeatherToSelection(qreal feather);
    void applyOpacityToSelection(qreal opacity);
    void applyInvisibilityToSelection(bool YesOrNo);
    void applyVariableWidthToSelection(bool YesOrNo);
    void fillPath(QList<QPointF> contourPath, int colour, float tolerance);
    void fill(QPointF point, int colour, float tolerance);
    void addArea(BezierArea bezierArea);
    int  getFirstAreaNumber(QPointF point);
    int  getLastAreaNumber(QPointF point);
    int  getLastAreaNumber(QPointF point, int maxAreaNumber);
    void removeArea(QPointF point);
    void updateArea(BezierArea& bezierArea);

    QList<int> getCurvesCloseTo(QPointF thisPoint, qreal maxDistance);
    VertexRef getClosestVertexTo(QPointF thisPoint, qreal maxDistance);
    QList<VertexRef> getCurveVertices(int curveNumber);
    QList<VertexRef> getVerticesCloseTo(QPointF thisPoint, qreal maxDistance);
    QList<VertexRef> getVerticesCloseTo(QPointF thisPoint, qreal maxDistance, QList<VertexRef>* listOfPoints);
    QList<VertexRef> getVerticesCloseTo(VertexRef thisPointRef, qreal maxDistance);
    QList<VertexRef> getVerticesCloseTo(VertexRef thisPointRef, qreal maxDistance, QList<VertexRef>* listOfPoints);
    QList<VertexRef> getAndRemoveVerticesCloseTo(QPointF thisPoint, qreal maxDistance, QList<VertexRef>* listOfPoints);
    QList<VertexRef> getAndRemoveVerticesCloseTo(VertexRef thisPointRef, qreal maxDistance, QList<VertexRef>* listOfPoints);
    QPointF getVertex(int curveNumber, int vertexNumber);
    QPointF getVertex(VertexRef vertexRef);
    QPointF getC1(int curveNumber, int vertexNumber);
    QPointF getC1(VertexRef vertexRef);
    QPointF getC2(int curveNumber, int vertexNumber);
    QPointF getC2(VertexRef vertexRef);
    QList<VertexRef> getAllVertices();
    int getCurveSize(int curveNumber);

    QList<BezierCurve> m_curves;
    QList<BezierArea> area;
    QList<int> m_curveDisplayOrders;

    qreal getDistance(VertexRef r1, VertexRef r2);

    QSize getSize() {return mSize;}

private:
    void addPoint( int curveNumber, int vertexNumber, qreal t );
	
	void checkCurveExtremity(BezierCurve& newCurve, qreal tolerance);
	void checkCurveIntersections(BezierCurve& newCurve, qreal tolerance);

	QList<QPointF> getfillContourPoints(QPoint point);
	void updateImageSize(BezierCurve& updatedCurve);

private:
    Object* mObject = nullptr;
    QRectF mSelectionRect;
    QTransform mSelectionTransformation;
    QSize mSize;
};

#endif

