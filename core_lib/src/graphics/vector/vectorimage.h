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
#ifndef VECTORIMAGE_H
#define VECTORIMAGE_H

#include <QTransform>

#include "bezierarea.h"
#include "beziercurve.h"
#include "vertexref.h"
#include "keyframe.h"

class Object;
class QPainter;
class QImage;


class VectorImage : public KeyFrame
{
public:
    VectorImage();
    VectorImage(const VectorImage&);
    ~VectorImage() override;
    VectorImage& operator=(const VectorImage& a);

    VectorImage* clone() const override;

    void setObject(Object* pObj) { mObject = pObj; }

    bool read(QString filePath);
    Status write(QString filePath, QString format);

    Status createDomElement(QXmlStreamWriter& doc);
    void loadDomElement(QDomElement element);

    BezierCurve& curve(int i);

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
    bool isAnyCurveSelected();
    void setAreaSelected(int areaNumber, bool YesOrNo);
    bool isAreaSelected(int areaNumber);
    bool isPathFilled();

    int getFirstSelectedCurve();
    int getFirstSelectedArea();
    int getNumOfCurvesSelected();
    void selectAll();
    void deselectAll();
    QRectF getSelectionRect() { return mSelectionRect; }
    void setSelectionRect(QRectF rectange);
    void calculateSelectionRect();
    void deleteSelection();
    void deleteSelectedPoints();
    void removeVertex(int curve, int vertex);

    bool isEmpty() const { return mCurves.isEmpty(); }

    void paste(VectorImage&);

    QColor getColor(int i);
    int  getColorNumber(QPointF point);
    bool usesColor(int index);
    void removeColor(int index);
    int getCurvesColor(int curve);
    bool isCurveVisible(int curve);
    void moveColor(int start, int end);

    void paintImage(QPainter& painter, bool simplified, bool showThinCurves, bool antialiasing);
    void outputImage(QImage* image, QTransform myView, bool simplified, bool showThinCurves, bool antialiasing); // uses paintImage

    void clear();
    void clean();
    void setSelectionTransformation(QTransform transform);
    void applySelectionTransformation();
    void applySelectionTransformation(QTransform transform);
    void applyColorToSelectedCurve(int colorNumber);
    void applyColorToSelectedArea(int colorNumber);
    void applyWidthToSelection(qreal width);
    void applyFeatherToSelection(qreal feather);
    void applyOpacityToSelection(qreal opacity);
    void applyInvisibilityToSelection(bool YesOrNo);
    void applyVariableWidthToSelection(bool YesOrNo);
    void fillContour(QList<QPointF> contourPath, int color);
    void fillSelectedPath(int color);
    //    void fill(QPointF point, int color, float tolerance);
    void addArea(BezierArea bezierArea);
    int  getFirstAreaNumber(QPointF point);
    int  getLastAreaNumber(QPointF point);
    int  getLastAreaNumber(QPointF point, int maxAreaNumber);
    int getLastCurveNumber();
    BezierCurve getLastCurve();
    void removeArea(QPointF point);
    void removeAreaInCurve(int curve, int areaNumber);
    void updateArea(BezierArea& bezierArea);

    QList<int> getCurvesCloseTo(QPointF thisPoint, qreal maxDistance);
    QList<BezierCurve> getSelectedCurves();
    QList<int> getSelectedCurveNumbers();
    BezierArea getSelectedArea(QPointF currentPoint);
    VertexRef getClosestVertexTo(const BezierCurve& curve, int curveNum, QPointF thisPoint);
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

    QPainterPath getStrokedPath() { return mGetStrokedPath; }

    QList<BezierArea> mArea;
    QList<int> mCurveDisplayOrders;

    qreal getDistance(VertexRef r1, VertexRef r2);

    QSize getSize() { return mSize; }

    void setOpacity(qreal opacity) { mOpacity = opacity; }
    qreal getOpacity() const { return mOpacity; }

private:
    void addPoint(int curveNumber, int vertexNumber, qreal fraction);

    void checkCurveExtremity(BezierCurve& newCurve, qreal tolerance);
    void checkCurveIntersections(BezierCurve& newCurve, qreal tolerance);

    void updateImageSize(BezierCurve& updatedCurve);
    QPainterPath mGetStrokedPath;

private:
    QList<BezierCurve> mCurves;

    Object* mObject = nullptr;
    QRectF mSelectionRect;
    QTransform mSelectionTransformation;
    QSize mSize;
    qreal mOpacity = 1.0;
};

#endif

