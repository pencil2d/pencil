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
#include <QImage>
#include "object.h"
#include "util.h"
#include "vectorimage.h"


VectorImage::VectorImage()
{
    deselectAll();
}

VectorImage::~VectorImage()
{
}

bool VectorImage::read(QString filePath)
{
    QFileInfo fileInfo(filePath);
	if ( fileInfo.isDir() )
	{
		return false;
	}

    QFile* file = new QFile(filePath);
    if (!file->open(QFile::ReadOnly))
    {
        //QMessageBox::warning(this, "Warning", "Cannot read file");
        return false;
    }

    QDomDocument doc;
    if (!doc.setContent(file)) return false; // this is not a XML file
    QDomDocumentType type = doc.doctype();
    if (type.name() != "PencilVectorImage") return false; // this is not a Pencil document

    QDomElement element = doc.documentElement();
    if (element.tagName() == "image")
    {
        // --- vector image ---
        if (element.attribute("type") == "vector")
        {
            loadDomElement( element );
        }
    }
    return true;
}

bool VectorImage::write(QString filePath, QString format)
{
    QFile* file = new QFile(filePath);
    bool result = file->open(QIODevice::WriteOnly);
    if (!result)
    {
        //QMessageBox::warning(this, "Warning", "Cannot write file");
        qDebug() << "VectorImage - Cannot write file" << filePath << file->error();
        return false;
    }
    QTextStream out(file);

    if (format == "VEC")
    {
        QDomDocument doc("PencilVectorImage");
        //QDomElement root = doc.createElement("vectorImage");
        //doc.appendChild(root);

        QDomElement imageTag = createDomElement(doc);
        doc.appendChild(imageTag);

        int IndentSize = 2;
        qDebug() << "--- Starting to write XML file...";
        doc.save(out, IndentSize);
        qDebug() << "--- Writing XML file done.";
        file->close();
        return true;
    }
    else
    {
        file->close();
        qDebug() << "--- Not the VEC format!";
        return false;
    }
}

QDomElement VectorImage::createDomElement(QDomDocument& doc)
{
    QDomElement imageTag = doc.createElement("image");
    imageTag.setAttribute("type", "vector");
    for(int i=0; i < m_curves.size() ; i++)
    {
        QDomElement curveTag = m_curves[i].createDomElement(doc);
        imageTag.appendChild(curveTag);
    }
    for(int i=0; i < area.size() ; i++)
    {
        QDomElement areaTag = area[i].createDomElement(doc);
        imageTag.appendChild(areaTag);
    }
    return imageTag;
}

void VectorImage::loadDomElement(QDomElement element)
{
    QDomNode atomTag = element.firstChild(); // an atom in a vector picture is a curve or an area
    while (!atomTag.isNull())
    {
        QDomElement atomElement = atomTag.toElement();
        if (!atomElement.isNull())
        {
            if (atomElement.tagName() == "curve")
            {
                BezierCurve newCurve = BezierCurve();
                newCurve.loadDomElement(atomElement);
                m_curves.append(newCurve);
            }
            if (atomElement.tagName() == "area")
            {
                BezierArea newArea = BezierArea();
                newArea.loadDomElement(atomElement);
                addArea(newArea);
            }
        }
        atomTag = atomTag.nextSibling();
    }
    clean();
    modification();
}

void VectorImage::addPoint(int curveNumber, int vertexNumber, qreal t)
{
    //curve[curveNumber].addPoint(vertexNumber, point);
    m_curves[curveNumber].addPoint(vertexNumber, t);
    // updates the bezierAreas
    for(int j=0; j < area.size(); j++)
    {
        // shift the references of all the points beyond the new point
        for(int k=0; k< area.at(j).vertex.size(); k++)
        {
            if (area[j].getVertexRef(k).curveNumber == curveNumber)
            {
                if (area[j].getVertexRef(k).vertexNumber >= vertexNumber)
                {
                    area[j].vertex[k].vertexNumber++;
                }
            }
        }
        // insert the new point in the area if necessary
        for(int k=1; k< area.at(j).vertex.size(); k++)
        {
            if ( VertexRef(curveNumber, vertexNumber+1) == area.at(j).vertex.at(k) )   // area[j].vertex[k] == VertexRef(curveNumber, vertexNumber+1)
            {
                if ( VertexRef(curveNumber, vertexNumber-1) == area.at(j).vertex.at(k-1) )
                {
                    area[j].vertex.insert(k, VertexRef(curveNumber, vertexNumber) );
                }
            }
            if ( VertexRef(curveNumber, vertexNumber-1) == area.at(j).vertex.at(k) )
            {
                if ( VertexRef(curveNumber, vertexNumber+1) == area.at(j).vertex.at(k-1) )
                {
                    area[j].vertex.insert(k, VertexRef(curveNumber, vertexNumber) );
                }
            }
        }
    }
}

void VectorImage::removeCurveAt(int i)
{
    // first change the curve numbers in the areas
    for(int j=0; j < area.size(); j++)
    {
        for(int k=0; k< area.at(j).vertex.size(); k++)
        {
            if (area.at(j).vertex[k].curveNumber > i) { area[j].vertex[k].curveNumber--; }
        }
    }
    // then remove curve
    m_curves.removeAt(i);
}

void VectorImage::addCurve(BezierCurve& newCurve, qreal factor)
{
    if (newCurve.getVertexSize() < 1) return; // security - a new curve should have a least 2 vertices
    qreal tol = qMax(newCurve.getWidth() / factor, 3.0 / factor); // tolerance for taking the intersection as an existing vertex on a curve
    //qDebug() << "tolerance" << tol;
    // finds if the new curve interesects itself
    for(int k=0; k < newCurve.getVertexSize(); k++)   // for each cubic section of the new curve
    {
        for(int j=k+1; j < newCurve.getVertexSize(); j++)   // for each other cubic section of the new curve
        {
            QList<Intersection> intersections;
            bool intersection = BezierCurve::findIntersection(newCurve, k, newCurve, j, intersections);
            if (intersection)
            {
                //qDebug() << "INTERSECTION" << intersectionPoint << t1 << t2;
                //newCurve.addPoint(k, intersectionPoint);
                newCurve.addPoint(k, intersections[0].t1); //qDebug() << "--a " << newCurve.getVertex(k) << newCurve.getVertex(k+1);
                k++;
                j++;
                //newCurve.addPoint(j, intersectionPoint);
                newCurve.addPoint(j, intersections[0].t2); //qDebug() << "--a " << newCurve.getVertex(j) << newCurve.getVertex(j+1);
                j++;
            }
        }
    }
    // finds if the new curve is closed
    QPointF P = newCurve.getVertex(-1);
    QPointF Q = newCurve.getVertex(newCurve.getVertexSize()-1);
    if ( BezierCurve::eLength(P-Q) < tol)
    {
        newCurve.setVertex(newCurve.getVertexSize()-1, P);
    }
    // finds if the first or last point of the new curve is close to other curves
    for(int i=0; i < m_curves.size(); i++)   // for each other curve
    {
        for(int j=0; j < m_curves.at(i).getVertexSize(); j++)   // for each cubic section of the other curve
        {
            QPointF P = newCurve.getVertex(-1);
            QPointF Q = newCurve.getVertex(newCurve.getVertexSize()-1);
            QPointF P1 = m_curves.at(i).getVertex(j-1);
            QPointF P2 = m_curves.at(i).getVertex(j);
            qreal tol3 = 2.0*sqrt(  0.25*((P1-P2).x()*(P1-P2).x() + (P1-P2).y()*(P1-P2).y())  + tol*tol );
            qreal dist1 = BezierCurve::eLength(P-P1);
            qreal dist2 = BezierCurve::eLength(P-P2);
            if (dist1 <= 0.2*tol)
            {
                newCurve.setVertex(-1, P1); //qDebug() << "--b " << P1;
            }
            else
            {
                if (dist2 <= 0.2*tol)
                {
                    newCurve.setVertex(-1, P2); //qDebug() << "--c " << P2;
                }
                else
                {
                    if ( dist1+dist2 <= 3*tol3 )   // preselection, to speed up
                    {
                        QPointF nearestPoint = P;
                        qreal t = -1.0;
                        qreal distance = BezierCurve::findDistance(m_curves[i], j, P, nearestPoint, t);
                        if (distance < tol)
                        {
                            newCurve.setOrigin(nearestPoint); //qDebug() << "--d " << nearestPoint;
                            addPoint(i, j, t);
                            //j++;
                        }
                    }
                }
                //qDebug() << "Modif first";
            }

            dist1 = BezierCurve::eLength(Q-P1);
            dist2 = BezierCurve::eLength(Q-P2);
            if (dist1 <= 0.2*tol)
            {
                newCurve.setLastVertex(P1); //qDebug() << "--e " << P1;
            }
            else
            {
                if (dist2 <= 0.2*tol)
                {
                    newCurve.setLastVertex(P2); //qDebug() << "--f " << P2;
                }
                else
                {
                    if ( dist1+dist2 <= 3*tol3 )   // preselection, to speed up
                    {
                        QPointF nearestPoint = Q;
                        qreal t = -1.0;;
                        qreal distance = BezierCurve::findDistance(m_curves[i], j, Q, nearestPoint, t);
                        if (distance < tol)
                        {
                            newCurve.setLastVertex(nearestPoint); //qDebug() << "--g " << nearestPoint;
                            addPoint(i, j, t);
                            //j++;
                        }
                    }
                }
                //qDebug() << "Modif last";
            }
        }
    }

    // finds if the new curve interesects other curves
    for(int k=0; k < newCurve.getVertexSize(); k++)   // for each cubic section of the new curve
    {
        //if (k==0) L1 = QLineF(P1 + 1.5*tol*(P1-Q1)/BezierCurve::eLength(P1-Q1), Q1);  // we extend slightly the line for the near point
        //if (k==newCurve.getVertexSize()-1) L1 = QLineF(P1, Q1- 1.5*tol*(P1-Q1)/BezierCurve::eLength(P1-Q1));  // we extend slightly the line for the last point
        //QPointF extension1 = 1.5*tol*(P1-Q1)/BezierCurve::eLength(P1-Q1);
        //L1 = QLineF(P1 + extension1, Q1 - extension1);
        for(int i=0; i < m_curves.size(); i++)   // for each other curve // TO DO: should only loop on "nearby" curves instead of all
        {
            //BezierCurve otherCurve;
            //if (i==-1) { otherCurve = newCurve; } else {  otherCurve = curve.at(i); }

            // ---- finds if the first or last point of the other curve is close to the current cubic section of the new curve
            QPointF P = m_curves.at(i).getVertex(-1);
            QPointF Q = m_curves.at(i).getVertex(m_curves.at(i).getVertexSize()-1);
            QPointF P1 = newCurve.getVertex(k-1);
            QPointF P2 = newCurve.getVertex(k);
            qreal tol3 = 2.0*sqrt(  0.25*((P1-P2).x()*(P1-P2).x() + (P1-P2).y()*(P1-P2).y())  + tol*tol );
            qreal dist1 = BezierCurve::eLength(P-P1);
            qreal dist2 = BezierCurve::eLength(P-P2);
            if (dist1 < 0.2*tol)
            {
                m_curves[i].setVertex(-1, P1);  // memo: curve.at(i) is just a copy which can be read, curve[i] is a reference which can be modified
            }
            else
            {
                if (dist2 < 0.2*tol)
                {
                    m_curves[i].setVertex(-1, P2);
                }
                else
                {
                    if ( dist1+dist2 < 3*tol3 )
                    {
                        // TO DO: find a better intersection point
                        QPointF nearestPoint = P;
                        qreal t = -1.0;
                        qreal distance = BezierCurve::findDistance(newCurve, k, P, nearestPoint, t);
                        //qDebug() << "OK1" << t;
                        if (distance < tol)
                        {
                            P = nearestPoint;
                            m_curves[i].setOrigin(P);
                            newCurve.addPoint(k, P); //qDebug() << "--i " << P;
                        }
                    }
                }
                //qDebug() << "Modif first";
            }
            dist1 = BezierCurve::eLength(Q-P1);
            dist2 = BezierCurve::eLength(Q-P2);
            if (dist1 < 0.2*tol)
            {
                m_curves[i].setVertex(m_curves.at(i).getVertexSize()-1, P1);
            }
            else
            {
                if (dist2 < 0.2*tol)
                {
                    m_curves[i].setVertex(m_curves.at(i).getVertexSize()-1, P2);
                }
                else
                {
                    if ( dist1+dist2 < 3*tol3 )
                    {
                        // TO DO: find a better intersection point
                        QPointF nearestPoint = Q;
                        qreal t = -1.0;;
                        qreal distance = BezierCurve::findDistance(newCurve, k, Q, nearestPoint, t);
                        //qDebug() << "OK2" << t;
                        if (distance < tol)
                        {
                            Q = nearestPoint;
                            m_curves[i].setLastVertex(Q);
                            newCurve.addPoint(k, Q); //qDebug() << "--j " << Q;
                        }
                    }
                }
                //qDebug() << "Modif first";
            }

            // ---- finds if any cubic section of the other curve intersects the current cubic section of the new curve
            for(int j=0; j < m_curves.at(i).getVertexSize(); j++)   // for each cubic section of the other curve
            {
                QList<Intersection> intersections;
                bool intersection = BezierCurve::findIntersection(newCurve, k, m_curves.at(i), j, intersections);
                if (intersection)
                {
                    //qDebug() << "Found " << intersections.size() << " intersections";
                    QPointF intersectionPoint = intersections[0].point;
                    qreal t1 = intersections[0].t1;
                    qreal t2 = intersections[0].t2;
                    if ( BezierCurve::eLength(intersectionPoint - newCurve.getVertex(k-1)) <= 0.1*tol )   // the first point is close to the intersection
                    {
                        newCurve.setVertex(k-1, intersectionPoint); //qDebug() << "--k " << intersectionPoint;
                        //qDebug() << "--------- recal " << k-1 << intersectionPoint;
                    }
                    else
                    {
                        if ( BezierCurve::eLength(intersectionPoint - newCurve.getVertex(k)) <= 0.1*tol )   // the second point is close to the intersection
                        {
                            newCurve.setVertex(k, intersectionPoint); //qDebug() << "--l " << intersectionPoint;
                            //qDebug() << "-------- recal " << k << intersectionPoint;
                        }
                        else     // none of the point is close to the intersection -> we add a new point
                        {
                            //newCurve.addPoint(k, intersectionPoint);
                            newCurve.addPoint(k, t1); //qDebug() << "--m " << newCurve.getVertex(k);
                            //qDebug() << "----- add " << k << newCurve.getVertex(k);
                            //k++;
                        }
                    }
                    if ( BezierCurve::eLength(intersectionPoint - m_curves.at(i).getVertex(j-1)) <= 0.1*tol )   // the first point is close to the intersection
                    {
                        m_curves[i].setVertex(j-1, intersectionPoint); //qDebug() << "--n " << intersectionPoint;
                        //qDebug() << "-------- recal2 " << j-1 << intersectionPoint;
                    }
                    else
                    {
                        if ( BezierCurve::eLength(intersectionPoint - m_curves.at(i).getVertex(j)) <= 0.1*tol )   // the second point is close to the intersection
                        {
                            m_curves[i].setVertex(j, intersectionPoint); //qDebug() << "--o " << intersectionPoint;
                            //qDebug() << "-------- recal2 " << j << intersectionPoint;
                        }
                        else     // none of the point is close to the intersection -> we add a new point
                        {
                            addPoint(i, j, t2);
                            //qDebug() << "----- add2 " << j << curve[i].getVertex(j);
                            //j++;
                        }
                    }
                }
            }
        }
    }
    m_curves.append(newCurve);
    modification();
    //QPainter painter(&image);
    //painter.setRenderHint(QPainter::Antialiasing, true);
    //newCurve.drawPath(&painter);
}

void VectorImage::select(QRectF rectangle)
{
    for(int i=0; i< m_curves.size(); i++)
    {
        if ( m_curves[i].intersects(rectangle) )
        {
            setSelected(i, true);
        }
        else
        {
            setSelected(i, false);
        }
    }
    for(int i=0; i< area.size(); i++)
    {
        if ( rectangle.contains(area[i].path.boundingRect()) )
        {
            setAreaSelected(i, true);
        }
        else
        {
            setAreaSelected(i, false);
        }
    }
    modification();
}

void VectorImage::setSelected(int curveNumber, bool YesOrNo)
{
    m_curves[curveNumber].setSelected(YesOrNo);
    if (YesOrNo) selectionRect |= m_curves[curveNumber].getBoundingRect();
    modification();
}

void VectorImage::setSelected(int curveNumber, int vertexNumber, bool YesOrNo)
{
    m_curves[curveNumber].setSelected(vertexNumber, YesOrNo);
    QPointF vertex = getVertex(curveNumber, vertexNumber);
    if (YesOrNo) selectionRect |= QRectF(vertex.x(), vertex.y(), 0.0, 0.0);
    modification();
}

void VectorImage::setSelected(VertexRef vertexRef, bool YesOrNo)
{
    setSelected(vertexRef.curveNumber, vertexRef.vertexNumber, YesOrNo);
}

void VectorImage::setSelected(QList<int> curveList, bool YesOrNo)
{
    for(int i=0; i<curveList.size(); i++)
    {
        setSelected(curveList.at(i), YesOrNo);
    }
}

void VectorImage::setSelected(QList<VertexRef> vertexList, bool YesOrNo)
{
    for(int i=0; i<vertexList.size(); i++)
    {
        setSelected(vertexList.at(i), YesOrNo);
    }
}

void VectorImage::setAreaSelected(int areaNumber, bool YesOrNo)
{
    area[areaNumber].setSelected(YesOrNo);
    if (YesOrNo) selectionRect |= area[areaNumber].path.boundingRect();
    modification();
}

bool VectorImage::isAreaSelected(int areaNumber)
{
    return area[areaNumber].isSelected();
}

bool VectorImage::isSelected(int curveNumber)
{
    return m_curves[curveNumber].isSelected();
}

bool VectorImage::isSelected(int curveNumber, int vertexNumber)
{
    return m_curves[curveNumber].isSelected(vertexNumber);
}

bool VectorImage::isSelected(VertexRef vertexRef)
{
    return isSelected(vertexRef.curveNumber, vertexRef.vertexNumber);
}

bool VectorImage::isSelected(QList<int> curveList)
{
    bool result = true;
    for(int i=0; i<curveList.size(); i++)
    {
        result &= isSelected(curveList.at(i));
    }
    return result;
}

bool VectorImage::isSelected(QList<VertexRef> vertexList)
{
    bool result = true;
    for(int i=0; i<vertexList.size(); i++)
    {
        result &= isSelected(vertexList.at(i));
    }
    return result;
}

int VectorImage::getFirstSelectedCurve()
{
    int result = -1;
    for(int i=0; i<m_curves.size() && result == -1; i++)
    {
        if ( isSelected(i) ) result = i;
    }
    return result;
}

int VectorImage::getFirstSelectedArea()
{
    int result = -1;
    for(int i=0; i<area.size() && result == -1; i++)
    {
        if ( isAreaSelected(i) ) result = i;
    }
    return result;
}

void VectorImage::selectAll()
{
    for(int i=0; i< m_curves.size(); i++)
    {
        //curve[i].setSelected(true);
        setSelected(i, true);
    }
    selectionTransformation.reset();
    //modification();
}

void VectorImage::deselectAll()
{
    for(int i=0; i< m_curves.size(); i++)
    {
        m_curves[i].setSelected(false);
    }
    for(int i=0; i< area.size(); i++)
    {
        area[i].setSelected(false);
    }
    selectionRect = QRectF(0,0,0,0);
    selectionTransformation.reset();
    modification();
}

void VectorImage::setSelectionRect(QRectF rectangle)
{
    selectionRect = rectangle;
    select(rectangle);
}

void VectorImage::calculateSelectionRect()
{
    selectionRect = QRectF(0,0,0,0);
    for(int i=0; i< m_curves.size(); i++)
    {
        if ( m_curves.at(i).isPartlySelected()) selectionRect |= m_curves[i].getBoundingRect();
    }
}

void VectorImage::setSelectionTransformation(QMatrix transform)
{
    selectionTransformation = transform;
    modification();
}

void VectorImage::deleteSelection()
{
    // ---- deletes areas
    for(int i=0; i< area.size(); i++)
    {
        if ( area[i].isSelected())
        {
            area.removeAt(i);
            i--;
        }
    }
    // ---- deletes curves
    for(int i=0; i< m_curves.size(); i++)
    {
        if ( m_curves[i].isSelected())
        {
            // eliminates areas which are associated to this curve
            for(int j=0; j < area.size(); j++)
            {
                bool toBeDeleted = false;
                for(int k=0; k< area.at(j).vertex.size(); k++)
                {
                    if (area.at(j).vertex[k].curveNumber == i) { toBeDeleted = true; }
                    if (area.at(j).vertex[k].curveNumber > i) { area[j].vertex[k].curveNumber = area[j].vertex[k].curveNumber - 1; }
                }
                if (toBeDeleted)
                {
                    area.removeAt(j);
                    j--;
                }
            }
            m_curves.removeAt(i);
            i--;
        }
        else
        {
            /*for(int j=-1; j<curve.at(i).size(); j++) {
                if (curve.at(i).isSelected(j)) {
                    curve.at(i).removeVertex(j);
                }
            }*/
        }
    }
    modification();
}

void VectorImage::removeVertex(int i, int m)   // curve number i and vertex number m
{
    // first eliminates areas which are associated to this point
    for(int j=0; j < area.size(); j++)
    {
        bool toBeDeleted = false;
        for(int k=0; k< area.at(j).vertex.size(); k++)
        {
            if (area.at(j).vertex[k].curveNumber == i && area.at(j).vertex[k].vertexNumber == m) { toBeDeleted = true; }
            //if (area.at(j).vertex[k].curveNumber > i) { area[j].vertex[k].curveNumber = area[j].vertex[k].curveNumber - 1; }
        }
        if (toBeDeleted)
        {
            area.removeAt(j);
            j--;
        }
    }
    // then eliminates the point
    if (m_curves[i].getVertexSize() > 1)
    {
        // first possibility: we just remove the point in the curve
        /*
        curve[i].removeVertex(m);
        m--;*/
        // second possibility: we split the curve into two parts:
        if ( m == -1 || m == getCurveSize(i) - 1 )   // we just remove the first or last point
        {
            m_curves[i].removeVertex(m);
            m--;
            // we also need to update the areas
            for(int j=0; j < area.size(); j++)
            {
                for(int k=0; k< area.at(j).vertex.size(); k++)
                {
                    if (area.at(j).vertex[k].curveNumber == i && area.at(j).vertex[k].vertexNumber > m) { area[j].vertex[k].vertexNumber--; }
                }
            }
        }
        else
        {
            int n = getCurveSize(i);
            BezierCurve newCurve = m_curves.at(i); // duplicate curve
            for(int p = m; p < n; p++)    // removes the end of of the curve i (after m, included) -> left part
            {
                m_curves[i].removeVertex( getCurveSize(i)-1 );
            }
            for(int p=-1; p <= m; p++)   // removes the beginning of the new curve (before m, included) -> right part
            {
                newCurve.removeVertex(-1);
            }
            //if (newCurve.getVertexSize() > 0) curve.insert(i+1, newCurve);
            if (newCurve.getVertexSize() > 0) m_curves.append( newCurve); // insert the right part if it has more than one point
            // we also need to update the areas
            for(int j=0; j < area.size(); j++)
            {
                for(int k=0; k< area.at(j).vertex.size(); k++)
                {
                    if (area.at(j).vertex[k].curveNumber == i && area.at(j).vertex[k].vertexNumber > m)
                    {
                        area[j].vertex[k].curveNumber = m_curves.size()-1;
                        area[j].vertex[k].vertexNumber = area[j].vertex[k].vertexNumber-m-1;
                    }
                }
            }

            if ( getCurveSize(i) < 1)   // the left part has less than two points so we remove it
            {
                //curve.removeAt(i);
                removeCurveAt(i);
                i--;
            }
        }
    }
    else     // there are just two points left, so we remove the whole curve
    {
        //curve.removeAt(i);
        removeCurveAt(i);
        i--;
    }
}

void VectorImage::deleteSelectedPoints()
{
    for(int i=0; i< m_curves.size(); i++)
    {
        for(int m=-1; m < getCurveSize(i); m++)
        {
            if (m_curves.at(i).isSelected(m))   // point m of curve i is selected
            {
                removeVertex(i, m);
            }
        }
    }
    modification();
}

void VectorImage::paste(VectorImage vectorImage)
{
    selectionRect = QRect(0,0,0,0);
    int n = m_curves.size();
    QList<int> selectedCurves;
    for(int i=0; i < vectorImage.m_curves.size() ; i++)
    {
        if ( vectorImage.m_curves.at(i).isSelected() )
        {
            m_curves.append( vectorImage.m_curves.at(i) );
            selectedCurves << i;
            selectionRect |= vectorImage.m_curves[i].getBoundingRect();
        }
    }
    for(int i=0; i < vectorImage.area.size() ; i++)
    {
        BezierArea newArea = vectorImage.area.at(i);
        bool ok = true;
        for(int j=0; j < newArea.vertex.size(); j++)
        {
            int curveNumber = newArea.vertex.at(j).curveNumber;
            int vertexNumber = newArea.vertex.at(j).vertexNumber;
            if ( vectorImage.m_curves.at(curveNumber).isSelected() )
            {
                newArea.vertex[j] = VertexRef( selectedCurves.indexOf(curveNumber) + n, vertexNumber );
            }
            else
            {
                ok = false;
            }
        }
        if (ok) area.append( newArea );
    }
    modification();
}

void VectorImage::modification()
{
    setModified(true);
}

bool VectorImage::isModified()
{
    return modified;
}

void VectorImage::setModified(bool trueOrFalse)
{
    modified = trueOrFalse;
}

QColor VectorImage::getColour(int colourNumber)
{
    return myParent->getColour(colourNumber).colour;
    //return Qt::blue;
}

int VectorImage::getColourNumber(QPointF point)
{
    int result = -1;
    int areaNumber = getLastAreaNumber(point);
    if (areaNumber != -1)
    {
        result = area[areaNumber].colourNumber;
    }
    return result;
}

bool VectorImage::usesColour(int index)
{
    for(int i=0; i< area.size(); i++)
    {
        if (area[i].colourNumber == index) return true;
    }
    for(int i=0; i< m_curves.size(); i++)
    {
        if (m_curves[i].getColourNumber() == index) return true;
    }
    return false;
}

void VectorImage::removeColour(int index)
{
    for(int i=0; i< area.size(); i++)
    {
        if (area[i].getColourNumber() > index) area[i].decreaseColourNumber();
    }
    for(int i=0; i< m_curves.size(); i++)
    {
        if (m_curves[i].getColourNumber() > index) m_curves[i].decreaseColourNumber();
    }
}

void VectorImage::paintImage(QPainter& painter,
							 bool simplified,
                             bool showThinCurves,
							 bool antialiasing )
{
    painter.setRenderHint(QPainter::Antialiasing, antialiasing);

    painter.setClipping(false);
    painter.setOpacity(1.0);
    QMatrix painterMatrix = painter.worldMatrix();

    QRect mappedViewRect = QRect(0,0, painter.device()->width(), painter.device()->height() );
    QRectF viewRect = painterMatrix.inverted().mapRect( mappedViewRect );

    // --- draw filled areas ----
    if (!simplified)
    {
        for(int i=0; i< area.size(); i++)
        {
            updateArea( area[i] ); // to do: if selected

            // --- fill areas ---- //
            QColor colour = getColour(area[i].colourNumber);

            if (area[i].isSelected())
            {
                painter.save();
                painter.setWorldMatrixEnabled(false);

                painter.setBrush( QBrush( QColor(255-colour.red(),255-colour.green(),255-colour.blue()), Qt::Dense6Pattern) );
                painter.drawPath( painter.worldMatrix().map( area[i].path ) );
                painter.restore();
                painter.setWorldMatrixEnabled(true);
            }
            // --
            painter.setRenderHint(QPainter::Antialiasing, antialiasing);
            painter.setClipping(false);
        }
    }

    // ---- draw curves ----
    //simplified = true;
    //painter.setClipRect( viewRect );
    //painter.setClipping(true);
    for ( BezierCurve curve : m_curves )
    {
        curve.drawPath( painter, myParent, selectionTransformation, simplified, showThinCurves );
    }
    
    painter.setClipping(false);
}

void VectorImage::outputImage(QImage* image,
							  QMatrix myView,
							  bool simplified,
                              bool showThinCurves,
							  bool antialiasing)
{
	image->fill(qRgba(0,0,0,0));
    QPainter painter( image );
    painter.setWorldMatrix( myView );
    paintImage( painter, simplified, showThinCurves, antialiasing );
}

void VectorImage::clear()
{
    while (m_curves.size() > 0) { m_curves.removeAt(0); }
    while (area.size() > 0) { area.removeAt(0); }
    modification();
}

void VectorImage::clean()
{
    for(int i=0; i<m_curves.size(); i++)
    {
        if (m_curves.at(i).getVertexSize() == 0) { qDebug() << "CLEAN " << i; m_curves.removeAt(i); i--; }
    }
}

void VectorImage::applySelectionTransformation()
{
    applySelectionTransformation(selectionTransformation);
}

void VectorImage::applySelectionTransformation(QMatrix transf)
{
    for(int i=0; i< m_curves.size(); i++)
    {
        if ( m_curves.at(i).isPartlySelected()) m_curves[i].transform(transf);
    }
    calculateSelectionRect();
    selectionTransformation.reset();
    modification();
}

void VectorImage::applyColourToSelection(int colourNumber)
{
    for (int i = 0; i < m_curves.size(); i++)
    {
        if ( m_curves.at(i).isSelected()) m_curves[i].setColourNumber(colourNumber);
    }
    for (int i = 0; i < area.size(); i++)
    {
        if ( area.at(i).isSelected()) area[i].setColourNumber(colourNumber);
    }
    modification();
}

void VectorImage::applyWidthToSelection(qreal width)
{
    for(int i = 0; i < m_curves.size(); i++)
    {
        if ( m_curves.at(i).isSelected() ) m_curves[ i ].setWidth( width );
    }
    modification();
}

void VectorImage::applyFeatherToSelection(qreal feather)
{
    for(int i=0; i< m_curves.size(); i++)
    {
        if ( m_curves.at(i).isSelected() ) m_curves[i].setFeather(feather);
    }
    modification();
}

void VectorImage::applyOpacityToSelection(qreal opacity)
{
    Q_UNUSED(opacity);
    for(int i=0; i< m_curves.size(); i++)
    {
        //if ( curve.at(i).isSelected()) curve[i].setOpacity(width);
    }
    modification();
}

void VectorImage::applyInvisibilityToSelection(bool YesOrNo)
{
    for(int i=0; i< m_curves.size(); i++)
    {
        if ( m_curves.at(i).isSelected()) m_curves[i].setInvisibility(YesOrNo);
    }
    modification();
}

void VectorImage::applyVariableWidthToSelection(bool YesOrNo)
{
    for(int i=0; i< m_curves.size(); i++)
    {
        if ( m_curves.at(i).isSelected()) m_curves[i].setVariableWidth(YesOrNo);
    }
    modification();
}

QList<int> VectorImage::getCurvesCloseTo(QPointF P1, qreal maxDistance)
{
    QList<int> result;
    for(int j=0; j<m_curves.size(); j++)
    {
        BezierCurve myCurve;
        if (m_curves[j].isPartlySelected()) { myCurve = m_curves[j].transformed(selectionTransformation); }
        else { myCurve = m_curves[j]; }
        if ( myCurve.intersects(P1, maxDistance) )
        {
            result.append( j );
        }
    }
    return result;
}


VertexRef VectorImage::getClosestVertexTo(QPointF P1, qreal maxDistance)
{
    VertexRef result;
    result = VertexRef(-1, -1);  // result = [-1, -1]
    //qreal distance = image.width()*image.width(); // initial big value
    qreal distance = 400.0*400.0; // initial big value
    for(int j=0; j<m_curves.size(); j++)
    {
        for(int k=-1; k<m_curves.at(j).getVertexSize(); k++)
        {
            //QPointF P2 = selectionTransformation.map( getVertex(j, k) );
            QPointF P2 = getVertex(j, k);
            qreal distance2 = (P1.x()-P2.x())*(P1.x()-P2.x()) + (P1.y()-P2.y())*(P1.y()-P2.y());
            if ( distance2 < distance  && distance2 < maxDistance*maxDistance)
            {
                distance = distance2;
                result = VertexRef(j,k);
            }
        }
    }
    return result;
}

QList<VertexRef> VectorImage::getVerticesCloseTo(QPointF P1, qreal maxDistance)
{
    QList<VertexRef> result;
    for(int j=0; j<m_curves.size(); j++)
    {
        for(int k=-1; k<m_curves.at(j).getVertexSize(); k++)
        {
            QPointF P2 = getVertex(j, k);
            qreal distance = (P1.x()-P2.x())*(P1.x()-P2.x()) + (P1.y()-P2.y())*(P1.y()-P2.y());
            if ( distance < maxDistance*maxDistance )
            {
                result.append( VertexRef(j,k) );
            }
        }
    }
    return result;
}

QList<VertexRef> VectorImage::getVerticesCloseTo(QPointF P1, qreal maxDistance, QList<VertexRef>* listOfPoints)
{
    QList<VertexRef> result;
    for(int j=0; j<listOfPoints->size(); j++)
    {
        QPointF P2 = getVertex(listOfPoints->at(j));
        qreal distance = (P1.x()-P2.x())*(P1.x()-P2.x()) + (P1.y()-P2.y())*(P1.y()-P2.y());
        if ( distance < maxDistance*maxDistance )
        {
            result.append(listOfPoints->at(j));
            //listOfPoints->removeAt(j);
        }
    }
    return result;
}

QList<VertexRef> VectorImage::getVerticesCloseTo(VertexRef P1ref, qreal maxDistance)
{
    return getVerticesCloseTo( getVertex(P1ref), maxDistance);
}

QList<VertexRef> VectorImage::getVerticesCloseTo(VertexRef P1ref, qreal maxDistance, QList<VertexRef>* listOfPoints)
{
    return getVerticesCloseTo( getVertex(P1ref), maxDistance, listOfPoints);
}

QList<VertexRef> VectorImage::getAndRemoveVerticesCloseTo(QPointF P1, qreal maxDistance, QList<VertexRef>* listOfPoints)
{
    QList<VertexRef> result;
    //qreal distance = image.width()*image.width(); // initial big value
    for(int j=0; j<listOfPoints->size(); j++)
    {
        QPointF P2 = getVertex(listOfPoints->at(j));
        qreal distance = (P1.x()-P2.x())*(P1.x()-P2.x()) + (P1.y()-P2.y())*(P1.y()-P2.y());
        if ( distance < maxDistance*maxDistance )
        {
            result.append(listOfPoints->at(j));
            listOfPoints->removeAt(j);
        }
    }
    return result;
}

QList<VertexRef> VectorImage::getAndRemoveVerticesCloseTo(VertexRef P1Ref, qreal maxDistance, QList<VertexRef>* listOfPoints)
{
    return getAndRemoveVerticesCloseTo(getVertex(P1Ref), maxDistance, listOfPoints);
}

QPointF VectorImage::getVertex(int curveNumber, int vertexNumber)
{
    QPointF result = QPointF(11.11, 11.11); // bogus point
    if (curveNumber > -1 && curveNumber < m_curves.size())
    {
        BezierCurve myCurve = m_curves.at(curveNumber);
        if ( myCurve.isPartlySelected() ) myCurve = myCurve.transformed(selectionTransformation);
        if ( vertexNumber > -2 && vertexNumber < myCurve.getVertexSize())
        {
            result = myCurve.getVertex(vertexNumber);
        }
    }
    return result;
}

QPointF VectorImage::getVertex(VertexRef vertexRef)
{
    return getVertex(vertexRef.curveNumber, vertexRef.vertexNumber);
}

QPointF VectorImage::getC1(int curveNumber, int vertexNumber)
{
    QPointF result = QPointF(11.11, 11.11); // bogus point
    if (curveNumber > -1 && curveNumber < m_curves.size())
    {
        BezierCurve myCurve = m_curves.at(curveNumber);
        if ( myCurve.isPartlySelected() ) myCurve = myCurve.transformed(selectionTransformation);
        if ( vertexNumber > -1 && vertexNumber < myCurve.getVertexSize())
        {
            result = myCurve.getC1(vertexNumber);
        }
    }
    return result;
}

QPointF VectorImage::getC1(VertexRef vertexRef)
{
    return getC1(vertexRef.curveNumber, vertexRef.vertexNumber);
}

QPointF VectorImage::getC2(int curveNumber, int vertexNumber)
{
    QPointF result = QPointF(11.11, 11.11); // bogus point
    if (curveNumber > -1 && curveNumber < m_curves.size())
    {
        BezierCurve myCurve = m_curves.at(curveNumber);
        if ( myCurve.isPartlySelected() ) myCurve = myCurve.transformed(selectionTransformation);
        if ( vertexNumber > -1 && vertexNumber < myCurve.getVertexSize())
        {
            result = myCurve.getC2(vertexNumber);
        }
    }
    return result;
}

QPointF VectorImage::getC2(VertexRef vertexRef)
{
    return getC2(vertexRef.curveNumber, vertexRef.vertexNumber);
}

QList<VertexRef> VectorImage::getAllVertices()
{
    QList<VertexRef> result;
    for(int j=0; j<m_curves.size(); j++)
    {
        for(int k=-1; k<m_curves.at(j).getVertexSize(); k++)
        {
            VertexRef vertexRef = VertexRef(j, k);
            result.append(vertexRef);
        }
    }
    return result;
}

int VectorImage::getCurveSize(int curveNumber)
{
    if ( curveNumber > -1 && curveNumber < m_curves.size())
    {
        return m_curves.at(curveNumber).getVertexSize();
    }
    else
    {
        return -1;
    }
}

void VectorImage::colour(QList<QPointF> mousePath, int colour)
{
    QList<VertexRef> path;
    VertexRef vertex, lastVertex;
    int incr = 0;
    int n = 0;
    for(int i=0; i < mousePath.size(); i++)
    {
        vertex = getClosestVertexTo(mousePath.at(i), 10.0);
        if ( vertex.curveNumber != -1 && !path.contains(vertex) )   // we found a point on a curve
        {
            if ( path.size() == 0) { path.append(vertex); }
            else
            {
                if ( vertex != lastVertex )   // unnecessary precaution
                {
                    if (vertex.curveNumber == lastVertex.curveNumber)   // the two vertices are on the same curve
                    {
                        int j1 = lastVertex.vertexNumber;
                        int j2 = vertex.vertexNumber;
                        if (incr == 0)   // this is the beginning of a new curve
                        {
                            n = m_curves.at(vertex.curveNumber).getVertexSize();
                            incr = 1;
                            if ( j2 < j1 ) { incr = -1; }
                        }
                        if (incr == 1)
                        {
                            if (j1 < j2)
                            {
                                // normal case
                                for(int j=j1; j<j2; j++)
                                {
                                    //qDebug() << "[+1] " << (j+1) << " : " << j1 << " -> " << j2;
                                    VertexRef newVertex = VertexRef(vertex.curveNumber, j+1);
                                    path.append(newVertex);
                                }
                            }
                            else
                            {
                                // special case: we join the tips of the curves
                                //qDebug() << "Special case +1 " << " : " << j1 << " -> " << j2;
                                for(int j=j1; j<n-1; j++)
                                {
                                    //QList<int> newVertex; newVertex << vertex[0] << (j+1); path.append(newVertex);
                                    VertexRef newVertex = VertexRef(vertex.curveNumber, j+1);
                                    path.append(newVertex);
                                }
                                for(int j=-2; j<j2; j++)
                                {
                                    //QList<int> newVertex; newVertex << vertex[0] << (j+1); path.append(newVertex);
                                    VertexRef newVertex = VertexRef(vertex.curveNumber, j+1);
                                    path.append(newVertex);
                                }
                            }
                        }
                        if (incr == -1)
                        {
                            if (j2 < j1)
                            {
                                // normal case
                                for(int j=j1; j>j2; j--)
                                {
                                    //qDebug() << "[-1] " << (j-1) << " : " << j1 << " -> " << j2;
                                    //QList<int> newVertex; newVertex << vertex[0] << (j-1); path.append(newVertex);
                                    VertexRef newVertex = VertexRef(vertex.curveNumber, j-1);
                                    path.append(newVertex);
                                }
                            }
                            else
                            {
                                // special case: we join the tips of the curves
                                //qDebug() << "Special case -1 " << " : " << j1 << " -> " << j2;
                                for(int j=j1; j>-1; j--)
                                {
                                    //qDebug() << "[-1] " << (j-1) << " : " << j1 << " -> " << j2;
                                    //QList<int> newVertex; newVertex << vertex[0] << (j-1); path.append(newVertex);
                                    VertexRef newVertex = VertexRef(vertex.curveNumber, j+1);
                                    path.append(newVertex);
                                }
                                for(int j=n; j>j2; j--)
                                {
                                    //qDebug() << "[-1] " << (j-1) << " : " << j1 << " -> " << j2;
                                    //QList<int> newVertex; newVertex << vertex[0] << (j-1); path.append(newVertex);
                                    VertexRef newVertex = VertexRef(vertex.curveNumber, j+1);
                                    path.append(newVertex);
                                }
                            }
                        }
                    }
                    else     // the two vertices are not on the same curve
                    {
                        incr = 0;
                        path.append(vertex);
                    }
                }
            }
            lastVertex = vertex;
        }
    }
    BezierArea bezierArea(path, colour);
    addArea( bezierArea );
    modification();
    //qDebug() << path;
    //return path;
}


void VectorImage::addArea(BezierArea bezierArea)
{
    updateArea(bezierArea);
    area.append( bezierArea );
    modification();
}

int VectorImage::getFirstAreaNumber(QPointF point)
{
    int result = -1;
    for(int i=0; i<area.size() && result==-1; i++)
    {
        if ( area[i].path.controlPointRect().contains( point ) )
        {
            if ( area[i].path.contains( point ) )
            {
                result = i;
            }
        }
    }
    return result;
}

int VectorImage::getLastAreaNumber(QPointF point)
{
    return getLastAreaNumber(point, area.size()-1);
}

int VectorImage::getLastAreaNumber(QPointF point, int maxAreaNumber)
{
    int result = -1;
    for(int i=maxAreaNumber; i>-1 && result==-1; i--)
    {
        if ( area[i].path.controlPointRect().contains( point ) )
        {
            if ( area[i].path.contains( point ) )
            {
                result = i;
            }
        }
    }
    return result;
}

void VectorImage::removeArea(QPointF point)
{
    int areaNumber = getLastAreaNumber(point);
    if ( areaNumber != -1)
    {
        area.removeAt(areaNumber);
    }
    modification();
}

void VectorImage::updateArea(BezierArea& bezierArea)
{
    QPainterPath newPath;
    for(int i=0; i<bezierArea.vertex.size(); i++)
    {
        QPointF myPoint = getVertex(bezierArea.vertex[i]);
        QPointF myC1;
        QPointF myC2;

        if (i==0)
        {
            newPath.moveTo( myPoint );
        }
        else
        {
            if (bezierArea.vertex[i-1].curveNumber == bezierArea.vertex[i].curveNumber )   // the two points are on the same curve
            {
                if (bezierArea.vertex[i-1].vertexNumber < bezierArea.vertex[i].vertexNumber )   // the points follow the curve progression
                {
                    myC1 =  getC1(bezierArea.vertex[i]);
                    myC2 =  getC2(bezierArea.vertex[i]);
                }
                else
                {
                    myC1 = getC2(bezierArea.vertex[i-1]);
                    myC2 = getC1(bezierArea.vertex[i-1]);
                }
                newPath.cubicTo(myC1, myC2, myPoint);
            }
            else      // the two points are not the same curve
            {
                if ( bezierArea.vertex[i].vertexNumber == -1)   // the current point is the first point in the new curve
                {
                    newPath.lineTo( myPoint );
                }
                else
                {
                    newPath.lineTo( myPoint );
                }
            }
        }
    }
    newPath.closeSubpath();
    bezierArea.path = newPath;
    bezierArea.path.setFillRule( Qt::WindingFill );
}

qreal VectorImage::getDistance(VertexRef r1, VertexRef r2)
{
    qreal dist = BezierCurve::eLength(getVertex(r1)-getVertex(r2));
    return dist;
}
