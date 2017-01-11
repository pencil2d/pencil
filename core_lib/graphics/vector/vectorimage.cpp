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
#include <QDebug>
#include <cmath>
#include <QImage>
#include <QStringList>
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

    QFile file{filePath};
    if (!file.open(QFile::ReadOnly))
    {
        //QMessageBox::warning(this, "Warning", "Cannot read file");
        return false;
    }

    QDomDocument doc;
    if (!doc.setContent(&file)) return false; // this is not a XML file
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

Status VectorImage::write(QString filePath, QString format)
{
    QStringList debugInfo = QStringList() << "VectorImage::write" << QString( "filePath = " ).append( filePath ) << QString( "format = " ).append( format );
    QFile file{filePath};
    bool result = file.open(QIODevice::WriteOnly);
    if (!result)
    {
        //QMessageBox::warning(this, "Warning", "Cannot write file");
        qDebug() << "VectorImage - Cannot write file" << filePath << file.error();
        return Status( Status::FAIL, debugInfo << QString("file.error() = ").append( file.errorString() ) );
    }

    if (format == "VEC")
    {
        QXmlStreamWriter xmlStream( &file );
        xmlStream.setAutoFormatting( true);
        xmlStream.writeStartDocument();
        xmlStream.writeDTD( "<!DOCTYPE PencilVectorImage>" );

        xmlStream.writeStartElement( "image" );
        xmlStream.writeAttribute( "type", "vector" );
        Status st = createDomElement( xmlStream );
        if( !st.ok() )
        {
            QStringList xmlDetails = st.detailsList();
            for ( QString detail : xmlDetails )
            {
                detail.prepend( "&nbsp;&nbsp;" );
            }
            return Status( Status::FAIL, debugInfo << "- xml creation failed" << xmlDetails );
        }

        xmlStream.writeEndElement(); // Close image element
        xmlStream.writeEndDocument();

        return Status::OK;
    }
    else
    {
        qDebug() << "--- Not the VEC format!";
        return Status( Status::FAIL, debugInfo << "Unrecognized format" );
    }
}

Status VectorImage::createDomElement( QXmlStreamWriter& xmlStream )
{
    QStringList debugInfo = QStringList() << "VectorImage::createDomElement";
    bool isOkay = true;
    for ( int i = 0; i < m_curves.size(); i++ )
    {
        Status st = m_curves[ i ].createDomElement( xmlStream );
        if ( !st.ok() )
        {
            isOkay = false;
            QStringList curveDetails = st.detailsList();
            for ( QString detail : curveDetails )
            {
                detail.prepend( "&nbsp;&nbsp;" );
            }
            return Status( Status::FAIL, debugInfo << QString( "- m_curves[%1] failed to write" ).arg( i ) << curveDetails );
        }
    }
    for ( int i = 0; i < area.size(); i++ )
    {
        Status st = area[ i ].createDomElement( xmlStream );
        if ( !st.ok() )
        {
            isOkay = false;
            QStringList areaDetails = st.detailsList();
            for ( QString detail : areaDetails )
            {
                detail.prepend( "&nbsp;&nbsp;" );
            }
            return Status( Status::FAIL, debugInfo << QString( "- area[%1] failed to write" ).arg( i ) << areaDetails );
        }
    }
    return Status::OK;
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
        for(int k=0; k< area.at(j).mVertex.size(); k++)
        {
            if (area[j].getVertexRef(k).curveNumber == curveNumber)
            {
                if (area[j].getVertexRef(k).vertexNumber >= vertexNumber)
                {
                    area[j].mVertex[k].vertexNumber++;
                }
            }
        }
        // insert the new point in the area if necessary
        for(int k=1; k< area.at(j).mVertex.size(); k++)
        {
            if ( VertexRef(curveNumber, vertexNumber+1) == area.at(j).mVertex.at(k) )   // area[j].vertex[k] == VertexRef(curveNumber, vertexNumber+1)
            {
                if ( VertexRef(curveNumber, vertexNumber-1) == area.at(j).mVertex.at(k-1) )
                {
                    area[j].mVertex.insert(k, VertexRef(curveNumber, vertexNumber) );
                }
            }
            if ( VertexRef(curveNumber, vertexNumber-1) == area.at(j).mVertex.at(k) )
            {
                if ( VertexRef(curveNumber, vertexNumber+1) == area.at(j).mVertex.at(k-1) )
                {
                    area[j].mVertex.insert(k, VertexRef(curveNumber, vertexNumber) );
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
        for(int k=0; k< area.at(j).mVertex.size(); k++)
        {
            if (area.at(j).mVertex[k].curveNumber > i) { area[j].mVertex[k].curveNumber--; }
        }
    }
    // then remove curve
    m_curves.removeAt(i);
}

void VectorImage::insertCurve(int position, BezierCurve& newCurve, qreal factor, bool interacts)
{
    if (newCurve.getVertexSize() < 1) {

        // security - a new curve should have a least 2 vertices
        return;
    }


    // Does the curve should interact with others or with itself?
    //
    if (interacts) {
        // tolerance for taking the intersection as an existing vertex on a curve
        //
        qreal tol = qMax( newCurve.getWidth() / factor, 3.0 / factor);
        //qDebug() << "tolerance" << tol;

        checkCurveExtremity(newCurve, tol);
        checkCurveIntersections(newCurve, tol);
    }


    // Append or insert the curve in the list
    //
    if (position < 0 || position > m_curves.size() - 1) {
        m_curves.append(newCurve);
    }
    else {
        // If it's an insert we have to shift the curve numbers in the areas
        //
        for(int i=0; i < area.size(); i++)
        {
            for(int j=0; j< area.at(i).mVertex.size(); j++)
            {
                if (area.at(i).mVertex[j].curveNumber >= position) {
                    area[i].mVertex[j].curveNumber++;
                }
            }
        }
        m_curves.insert(position, newCurve);
    }


    updateImageSize(newCurve);
    modification();
    //QPainter painter(&image);
    //painter.setRenderHint(QPainter::Antialiasing, true);
    //newCurve.drawPath(&painter);
}

void VectorImage::addCurve(BezierCurve& newCurve, qreal factor, bool interacts)
{
    insertCurve(-1, newCurve, factor, interacts);
}

void VectorImage::checkCurveExtremity(BezierCurve& newCurve, qreal tolerance)
{
    // finds if the new curve is closed
    QPointF P = newCurve.getVertex(-1);
    QPointF Q = newCurve.getVertex(newCurve.getVertexSize()-1);
    if ( BezierCurve::eLength(P-Q) < tolerance)
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
            qreal tol3 = 2.0*sqrt(  0.25*((P1-P2).x()*(P1-P2).x() + (P1-P2).y()*(P1-P2).y())  + tolerance*tolerance );
            qreal dist1 = BezierCurve::eLength(P-P1);
            qreal dist2 = BezierCurve::eLength(P-P2);
            if (dist1 <= 0.2*tolerance)
            {
                newCurve.setVertex(-1, P1); //qDebug() << "--b " << P1;
            }
            else
            {
                if (dist2 <= 0.2*tolerance)
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
                        if (distance < tolerance)
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
            if (dist1 <= 0.2*tolerance)
            {
                newCurve.setLastVertex(P1); //qDebug() << "--e " << P1;
            }
            else
            {
                if (dist2 <= 0.2*tolerance)
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
                        if (distance < tolerance)
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
}

void VectorImage::checkCurveIntersections(BezierCurve& newCurve, qreal tolerance)
{

    // finds if the new curve interesects itself
    //
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
            qreal tol3 = 2.0*sqrt(  0.25*((P1-P2).x()*(P1-P2).x() + (P1-P2).y()*(P1-P2).y())  + tolerance*tolerance );
            qreal dist1 = BezierCurve::eLength(P-P1);
            qreal dist2 = BezierCurve::eLength(P-P2);

            if (dist1 < 0.2*tolerance)
            {
                m_curves[i].setVertex(-1, P1);  // memo: curve.at(i) is just a copy which can be read, curve[i] is a reference which can be modified
            }
            else
            {
                if (dist2 < 0.2*tolerance)
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
                        if (distance < tolerance)
                        {
                            P = nearestPoint;
                            //m_curves[i].setOrigin(P);
                            //newCurve.addPoint(k, P); //qDebug() << "--i " << P;
                        }
                    }
                }
                //qDebug() << "Modif first";
            }
            dist1 = BezierCurve::eLength(Q-P1);
            dist2 = BezierCurve::eLength(Q-P2);
            if (dist1 < 0.2*tolerance)
            {
                m_curves[i].setVertex(m_curves.at(i).getVertexSize()-1, P1);
            }
            else
            {
                if (dist2 < 0.2*tolerance)
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
                        if (distance < tolerance)
                        {
                            Q = nearestPoint;
                            //m_curves[i].setLastVertex(Q);
                            //newCurve.addPoint(k, Q); //qDebug() << "--j " << Q;
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
                    if ( BezierCurve::eLength(intersectionPoint - newCurve.getVertex(k-1)) <= 0.1*tolerance )   // the first point is close to the intersection
                    {
                        newCurve.setVertex(k-1, intersectionPoint); //qDebug() << "--k " << intersectionPoint;
                        //qDebug() << "--------- recal " << k-1 << intersectionPoint;
                    }
                    else
                    {
                        if ( BezierCurve::eLength(intersectionPoint - newCurve.getVertex(k)) <= 0.1*tolerance )   // the second point is close to the intersection
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
                    if ( BezierCurve::eLength(intersectionPoint - m_curves.at(i).getVertex(j-1)) <= 0.1*tolerance )   // the first point is close to the intersection
                    {
                        m_curves[i].setVertex(j-1, intersectionPoint); //qDebug() << "--n " << intersectionPoint;
                        //qDebug() << "-------- recal2 " << j-1 << intersectionPoint;
                    }
                    else
                    {
                        if ( BezierCurve::eLength(intersectionPoint - m_curves.at(i).getVertex(j)) <= 0.1*tolerance )   // the second point is close to the intersection
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
        if ( rectangle.contains(area[i].mPath.boundingRect()) )
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
    if (YesOrNo) selectionRect |= area[areaNumber].mPath.boundingRect();
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

void VectorImage::setSelectionTransformation(QTransform transform)
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
                for(int k=0; k< area.at(j).mVertex.size(); k++)
                {
                    if (area.at(j).mVertex[k].curveNumber == i) { toBeDeleted = true; }
                    if (area.at(j).mVertex[k].curveNumber > i) { area[j].mVertex[k].curveNumber = area[j].mVertex[k].curveNumber - 1; }
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
        for(int k=0; k< area.at(j).mVertex.size(); k++)
        {
            if (area.at(j).mVertex[k].curveNumber == i && area.at(j).mVertex[k].vertexNumber == m) { toBeDeleted = true; }
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
                for(int k=0; k< area.at(j).mVertex.size(); k++)
                {
                    if (area.at(j).mVertex[k].curveNumber == i && area.at(j).mVertex[k].vertexNumber > m) { area[j].mVertex[k].vertexNumber--; }
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
                for(int k=0; k< area.at(j).mVertex.size(); k++)
                {
                    if (area.at(j).mVertex[k].curveNumber == i && area.at(j).mVertex[k].vertexNumber > m)
                    {
                        area[j].mVertex[k].curveNumber = m_curves.size()-1;
                        area[j].mVertex[k].vertexNumber = area[j].mVertex[k].vertexNumber-m-1;
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

void VectorImage::paste(VectorImage& vectorImage)
{
    selectionRect = QRect(0,0,0,0);
    int n = m_curves.size();
    QList<int> selectedCurves;

    bool hasSelection = getFirstSelectedCurve() < -1;

    for(int i=0; i < vectorImage.m_curves.size() ; i++)
    {
        // If nothing is selected, paste everything
        //
        if ( !hasSelection || vectorImage.m_curves.at(i).isSelected() )
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
        for(int j=0; j < newArea.mVertex.size(); j++)
        {
            int curveNumber = newArea.mVertex.at(j).curveNumber;
            int vertexNumber = newArea.mVertex.at(j).vertexNumber;

            // If nothing is selected, paste everything
            //
            if ( !hasSelection || vectorImage.m_curves.at(curveNumber).isSelected() )
            {
                newArea.mVertex[j] = VertexRef( selectedCurves.indexOf(curveNumber) + n, vertexNumber );
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
        result = area[areaNumber].mColourNumber;
    }
    return result;
}

bool VectorImage::usesColour(int index)
{
    for(int i=0; i< area.size(); i++)
    {
        if (area[i].mColourNumber == index) return true;
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
    QTransform painterMatrix = painter.transform();

    QRect mappedViewRect = QRect(0,0, painter.device()->width(), painter.device()->height() );
    QRectF viewRect = painterMatrix.inverted().mapRect( mappedViewRect );

    // --- draw filled areas ----
    if (!simplified)
    {
        for(int i=0; i< area.size(); i++)
        {
            updateArea( area[i] ); // to do: if selected

            // --- fill areas ---- //
            QColor colour = getColour(area[i].mColourNumber);

            painter.save();
            painter.setWorldMatrixEnabled( false );

            if (area[i].isSelected())
            {
                painter.setBrush( QBrush( QColor(255-colour.red(),255-colour.green(),255-colour.blue()), Qt::Dense6Pattern) );
            }
            else {
                painter.setPen(QPen(QBrush(colour), 1, Qt::NoPen, Qt::RoundCap,Qt::RoundJoin));
                painter.setBrush( QBrush( colour, Qt::SolidPattern ));
            }

            painter.drawPath( painter.transform().map( area[ i ].mPath ) );
            painter.restore();
            painter.setWorldMatrixEnabled( true );

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
        painter.setClipping(false);
    }
}

void VectorImage::outputImage(QImage* image,
							  QTransform myView,
							  bool simplified,
                              bool showThinCurves,
							  bool antialiasing)
{
	image->fill(qRgba(0,0,0,0));
    QPainter painter( image );
    painter.setTransform( myView );
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

void VectorImage::applySelectionTransformation(QTransform transf)
{
    for(int i=0; i< m_curves.size(); i++)
    {
        if ( m_curves.at(i).isPartlySelected()) {
            m_curves[i].transform(transf);

        }
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

QList<VertexRef> VectorImage::getCurveVertices(int curveNumber)
{
    QList<VertexRef> result;

    if (curveNumber > -1 && curveNumber < m_curves.size())
    {
        BezierCurve myCurve = m_curves.at(curveNumber);

        for(int k=-1; k<myCurve.getVertexSize(); k++)
        {
            VertexRef vertexRef = VertexRef(curveNumber, k);
            result.append(vertexRef);
        }
    }

    return result;
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
    // Add Area vertices
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

void VectorImage::fill(QList<QPointF> contourPath, int colour, float tolerance)
{
    QList<VertexRef> vertexPath;

    for (QPointF point : contourPath) {
        VertexRef vertex = getClosestVertexTo(point, tolerance);
        if (vertex.curveNumber != -1 && !vertexPath.contains(vertex)) {
            vertexPath.append(vertex);
        }
    }

    BezierArea bezierArea(vertexPath, colour);
    addArea( bezierArea );
    modification();
}


QList<QPointF> VectorImage::getfillContourPoints(QPoint point)
{
    int error = -1;

    // We get the contour points from a bitmap version of the vector layer as it is much faster to process
    //
    QImage* image = new QImage( mSize, QImage::Format_ARGB32_Premultiplied );
    image->fill(Qt::white);
    QPainter painter( image );

    // Adapt the QWidget view coordinates to the QImage coordinates
    QTransform translate;
    translate.translate( mSize.width() / 2.f , mSize.height() / 2.f );
    painter.setTransform( translate );

    paintImage( painter, true, true, false );


    QList<QPoint> queue; // queue all the pixels of the filled area (as they are found)
    QList<QPointF> contourPoints; // refs of points near the contour pixels

    qreal maxWidth = mSize.width();
    qreal maxHeight = mSize.height();

    // To keep track of the highest y contour point to make sure it is on the main contour and not inside.
    int highestY = point.y();

    // Convert point to image coordinates as the image doesn't have the same coordinates origin as the
    // QWidget view
    //
    QPointF startPoint((maxWidth / 2) + point.x(), (maxHeight / 2) + point.y());
    queue.append( startPoint.toPoint() );


    // Check the colour of the clicked point
    QRgb colouFrom = image->pixel(startPoint.x(), startPoint.y());
    QRgb colourTo = Qt::green;


    // ----- flood fill and remember the contour pixels -> contourPixels
    // ----- from the standard flood fill algorithm
    // ----- http://en.wikipedia.org/wiki/Flood_fill
    //
    while ( queue.size() > 0 )
    {
        // Get the first point in the queue
        QPoint currentPoint = queue.at(0);

        // Remove this point from the queue as we process it.
        queue.removeAt(0);

        // Inspect a line from edge to edge

        if (image->pixel(currentPoint.x(), currentPoint.y()) == colouFrom) {

            int leftX = currentPoint.x();
            int rightX = currentPoint.x();

            bool foundLeftBound = false;
            bool foundRightBound = false;


            while (!foundLeftBound) {
                leftX--;

                // Are we getting to the end of the document ?
                //
                if ( leftX < 1) {
                    error = 1;
                    qWarning() << " Out of bound left ";
                    QList<QPointF> emptylist;
                    return emptylist;
                }

                QPoint leftPoint = QPoint(leftX, currentPoint.y());

                // Are we getting to a curve ?
                //
                if ( image->pixel(leftPoint.x(), leftPoint.y()) != colouFrom &&
                     image->pixel(leftPoint.x(), leftPoint.y()) != colourTo ) {

                    foundLeftBound = true;

                    // Convert point to view coordinates
                    QPointF contourPoint( leftPoint.x() - (maxWidth / 2), leftPoint.y() - (maxHeight / 2));

                    // Check if the left bound is just a line crossing the main shape
                    //
                    bool foundFillAfter = false;
                    int increment = 1;

                    while (leftPoint.x() - increment > 0 && increment < 3 && !foundFillAfter) {
                        QPoint pointAfter = QPoint(leftPoint.x() - increment, leftPoint.y());

                        if (image->pixel(pointAfter.x(), pointAfter.y()) == colourTo) {
                            foundFillAfter = true;
                        }

                        increment ++;
                    }


                    if (foundFillAfter) {

                        // If the bound is not a contour, we must ignore it
                        //
                        contourPoints.removeOne(contourPoint);
                    }
                    else {
                        contourPoints.append(contourPoint);
                    }
                }
            }

            while (!foundRightBound) {
                rightX++;

                // Are we getting to the end of the document ?
                //
                if ( rightX > maxWidth - 1 ) {
                    error = 1;
                    qWarning() << " Out of bound right ";
                    QList<QPointF> emptylist;
                    return emptylist;
                }

                QPoint rightPoint = QPoint(rightX, currentPoint.y());

                // Are we getting to a curve ?
                //
                if ( image->pixel(rightPoint.x(), rightPoint.y()) != colouFrom &&
                     image->pixel(rightPoint.x(), rightPoint.y()) != colourTo) {

                    foundRightBound = true;

                    // Convert point to view coordinates
                    QPointF contourPoint( rightPoint.x() - (maxWidth / 2), rightPoint.y() - (maxHeight / 2));


                    // Check if the left bound is just a line crossing the main shape
                    //
                    bool foundFillAfter = false;
                    int increment = 1;

                    while (rightPoint.x() + increment < maxWidth && increment < 3 && !foundFillAfter) {
                        QPoint pointAfter = QPoint(rightPoint.x() + increment, rightPoint.y());

                        if (image->pixel(pointAfter.x(), pointAfter.y()) == colourTo) {
                            foundFillAfter = true;
                        }

                        increment ++;
                    }


                    if (foundFillAfter) {

                        // If the bound is not a contour, we must ignore it
                        //
                        contourPoints.removeOne(contourPoint);
                    }
                    else {
                        contourPoints.append(contourPoint);
                    }
                }
            }

            int lineY = currentPoint.y();
            int topY = lineY - 1;
            int bottomY = lineY + 1;

            if ( topY < 1 || bottomY > maxHeight - 1 ) {
                error = 1;
                qWarning() << " Out of bound top / bottom ";
                QList<QPointF> emptylist;
                return emptylist;
            }

            // Check if top point relative to left point is to be filled
            //
            for (int x = leftX + 1; x < rightX; x++) {

                // The current line point is checked (Coloured)
                //
                QPoint linePoint = QPoint(x, lineY);
                image->setPixel(linePoint.x(), linePoint.y(), colourTo);

                // Top point
                //
                QPoint topPoint = QPoint(x, topY);

                if ( image->pixel(topPoint.x(), topPoint.y()) != colouFrom &&
                     image->pixel(topPoint.x(), topPoint.y()) != colourTo) {

                    // Convert point to view coordinates
                    QPointF contourPoint( topPoint.x() - (maxWidth / 2), topPoint.y() - (maxHeight / 2));


                    // Check if the left bound is just a line crossing the main shape
                    //
                    bool foundFillAfter = false;
                    int increment = 1;

                    while (topPoint.y() - increment > 0 && increment < 3 && !foundFillAfter) {
                        QPoint pointAfter = QPoint(topPoint.x(), topPoint.y() - increment);

                        if (image->pixel(pointAfter.x(), pointAfter.y()) == colourTo) {
                            foundFillAfter = true;
                        }

                        increment ++;
                    }


                    if (foundFillAfter) {
                        // If the bound is not a contour, we must ignore it
                        //
                        contourPoints.removeOne(contourPoint);
                    }
                    else {
                        contourPoints.append(contourPoint);
                    }
                }
                else {
                    queue.append(topPoint);
                }

                // Bottom point
                //
                QPoint bottomPoint = QPoint(x, bottomY);

                if ( image->pixel(bottomPoint.x(), bottomPoint.y()) != colouFrom &&
                     image->pixel(bottomPoint.x(), bottomPoint.y()) != colourTo ) {

                    // Convert point to view coordinates
                    QPointF contourPoint( bottomPoint.x() - (maxWidth / 2), bottomPoint.y() - (maxHeight / 2));

                    // Check if the left bound is just a line crossing the main shape
                    //
                    bool foundFillAfter = false;
                    int increment = 1;

                    while (bottomPoint.y() + increment < maxHeight && increment < 3 && !foundFillAfter) {
                        QPoint pointAfter = QPoint(bottomPoint.x(), bottomPoint.y() + increment);

                        if (image->pixel(pointAfter.x(), pointAfter.y()) == colourTo) {
                            foundFillAfter = true;
                        }

                        increment ++;
                    }


                    if (foundFillAfter) {

                        // If the bound is not a contour, we must ignore it
                        //
                        contourPoints.removeOne(contourPoint);
                    }
                    else {

                        // Keep track of the highest Y position (lowest point) at the beginning of the list
                        // so that we can parse the list from a point that is a real extremity.
                        // of the area.
                        //
                        if (highestY < bottomY) {

                            highestY = bottomY;
                            contourPoints.insert(0, contourPoint);
                        }
                        else {
                            contourPoints.append(contourPoint);
                        }
                    }

                }
                else {
                    queue.append(bottomPoint);
                }
            }
        }
    }

    return contourPoints;
}

void VectorImage::fill(QPointF point, int colour, float tolerance)
{
    // Check if we clicked on a curve. In that case, we change its color.
    //
    QList<int> closestCurves = getCurvesCloseTo( point, tolerance );

    if (closestCurves.size() > 0) // the user click on one or more curves
    {
        // For each clicked curves, we change the color if requiered
        //
        for (int i = 0; i < closestCurves.size(); i++) {
            int curveNumber = closestCurves[i];
            m_curves[curveNumber].setColourNumber(colour);
        }

        return;
    }

    // Check if we clicked on an area of the same color.
    // We don't want to create another area.
    //
    int areaNum = getLastAreaNumber(point);
    if (areaNum > -1 && area[areaNum].mColourNumber == colour) {
        return;
    }

    // Get the contour points
    //
    QList<QPointF> contourPoints = getfillContourPoints(point.toPoint());

    // Make a path from the external contour points.
    // Put the points in the right order.
    //
    QList<QPointF> mainContourPath;
    QPointF currentPoint;

    if (contourPoints.size() > 0) {
        currentPoint = QPointF(contourPoints[0].x(), contourPoints[0].y());
        mainContourPath.append(currentPoint);
        contourPoints.removeAt(0);
    }

    bool completedPath = false;
    bool foundError = (contourPoints.size() < 1 && !completedPath);

    int maxDelta = 2;
    int minDelta = -2;

    while (!completedPath && !foundError) {

        bool foundNextPoint = false;

        int i = 0;
        while (i < contourPoints.size() && !foundNextPoint) {
            QPointF point = contourPoints.at(i);

            if (mainContourPath.contains(point)) {
                contourPoints.removeAt(i);
            }
            else {
                qreal deltaX = currentPoint.x() - point.x();
                qreal deltaY = currentPoint.y() - point.y();

                if ( (deltaX < maxDelta && deltaX > minDelta) &&
                     (deltaY < maxDelta && deltaY > minDelta)) {

                    currentPoint = QPointF(point.x(), point.y());
                    mainContourPath.append(currentPoint);
                    contourPoints.removeAt(i);

                    foundNextPoint = true;

                    maxDelta = 2;
                    minDelta = -2;
                }
                i++;
            }
        }

        // Check if we have looped
        //
        if (!foundNextPoint) {

            qreal deltaX = currentPoint.x() - mainContourPath[0].x();
            qreal deltaY = currentPoint.y() - mainContourPath[0].y();

            if ( (deltaX < maxDelta && deltaX > minDelta) &&
                 (deltaY < maxDelta && deltaY > minDelta)) {
                completedPath = true;
                foundNextPoint = true;
            }
            else if (maxDelta == 2){
                // Check if we can find the point after
                //
                maxDelta = 3;
                minDelta = -3;
                foundNextPoint = true;
            }
            else {
                qWarning() << " couldn't find next point after " << currentPoint.x() << ", " << currentPoint.y();
            }
        }
        else if (contourPoints.size() < 1) {
            // If we found the next point and we have no more points, it means, we have the end of the path
            completedPath = true;
            foundNextPoint = true;
        }

        foundError = ( (contourPoints.size() < 1 && !completedPath) || !foundNextPoint );
    }


    // Add exclude paths


    // Fill the path if we have one.
    //
    if (completedPath) {
        fill(mainContourPath, colour, 3.0);
    }
    else {
        // Check if we clicked on an area in this position and as we couldn't create one,
        // we update this one. It may be an area drawn from a stroke path.
        //
        int areaNum = getLastAreaNumber(point);
        if (areaNum > -1) {

            int clickedColorNum = area[areaNum].getColourNumber();

            if (clickedColorNum != colour) {
                area[areaNum].setColourNumber(colour);
            }
        }
    }


}

// Creates an area based on the contour of a curve
//
void VectorImage::fill(int curveNumber, int colour)
{
    BezierArea bezierArea(getCurveVertices(curveNumber), colour);
    addArea(bezierArea);
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
        if ( area[i].mPath.controlPointRect().contains( point ) )
        {
            if ( area[i].mPath.contains( point ) )
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
        if ( area[i].mPath.controlPointRect().contains( point ) )
        {
            if ( area[i].mPath.contains( point ) )
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
    for(int i=0; i<bezierArea.mVertex.size(); i++)
    {
        QPointF myPoint = getVertex(bezierArea.mVertex[i]);
        QPointF myC1;
        QPointF myC2;

        if (i==0)
        {
            newPath.moveTo( myPoint );
        }
        else
        {
            if (bezierArea.mVertex[i-1].curveNumber == bezierArea.mVertex[i].curveNumber )   // the two points are on the same curve
            {
                if (bezierArea.mVertex[i-1].vertexNumber < bezierArea.mVertex[i].vertexNumber )   // the points follow the curve progression
                {
                    myC1 =  getC1(bezierArea.mVertex[i]);
                    myC2 =  getC2(bezierArea.mVertex[i]);
                }
                else
                {
                    myC1 = getC2(bezierArea.mVertex[i-1]);
                    myC2 = getC1(bezierArea.mVertex[i-1]);
                }
                newPath.cubicTo(myC1, myC2, myPoint);
            }
            else      // the two points are not the same curve
            {
                if ( bezierArea.mVertex[i].vertexNumber == -1)   // the current point is the first point in the new curve
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
    bezierArea.mPath = newPath;
    bezierArea.mPath.setFillRule( Qt::WindingFill );
}

qreal VectorImage::getDistance(VertexRef r1, VertexRef r2)
{
    qreal dist = BezierCurve::eLength(getVertex(r1)-getVertex(r2));
    return dist;
}

void VectorImage::updateImageSize(BezierCurve& updatedCurve) {

    // Set the current width of the document based on the extremity of the drawing.
    //
    // It calculates the size of the document in a way that the center point from
    // the view (0, 0) is always the center point of the document.
    //
    // It adds a point to the 4 sides of the document size in order
    // make sure that any curve, any vertex stays within the document.
    //

    QRectF rect = updatedCurve.getBoundingRect();

    QPoint topLeft      = rect.topLeft().toPoint();
    QPoint bottomRight  = rect.bottomRight().toPoint();

    int widthFromLeft = ((topLeft.x() * -1) * 2) + 2;

    if (widthFromLeft > mSize.width()) {
        mSize.setWidth(widthFromLeft);
    }

    int widthFromRight = (bottomRight.x() * 2) + 2;

    if (widthFromRight > mSize.width()) {
        mSize.setWidth(widthFromRight);
    }

    int heightFromTop = ((topLeft.y() * -1) * 2) + 2;

    if (heightFromTop > mSize.height()) {
        mSize.setHeight(heightFromTop);
    }

    int heightFromBottom = (bottomRight.y() * 2) + 2;

    if (heightFromBottom > mSize.height()) {
        mSize.setHeight(heightFromBottom);
    }
}
