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
#include "layervector.h"
#include <QtDebug>

LayerVector::LayerVector(Object* object) : LayerImage( object, Layer::VECTOR )
{
    name = QString(tr("Vector Layer"));
    addImageAtFrame(1);
}

LayerVector::~LayerVector()
{
    while (!framesVector.empty())
    {
        delete framesVector.takeFirst();
    }
}

// ------

QImage* LayerVector::getImageAtIndex( int index,
                                      QSize size,
                                      bool simplified,
                                      bool showThinLines,
                                      qreal curveOpacity,
                                      bool antialiasing)
{
    if ( index < 0 || index >= framesImage.size() )
    {
        return NULL;
    }
    else
    {
        VectorImage* vectorImage = getVectorImageAtIndex(index);
        QImage* image = framesImage.at(index);
        if (vectorImage->isModified() || size != image->size() )
        {
            if ( image->size() != size)
            {
                delete image;
                framesImage[index] = image = new QImage(size, QImage::Format_ARGB32_Premultiplied);
            }
            vectorImage->outputImage(image, size, myView,
                                     simplified, showThinLines,
                                     curveOpacity, antialiasing );
            vectorImage->setModified(false);
        }
        return image;
    }
}

QImage* LayerVector::getLastImageAtFrame(int frameNumber,
                                         int increment,
                                         QSize size,
                                         bool simplified, bool showThinLines,
                                         qreal curveOpacity,
                                         bool antialiasing )
{
    int index = getLastIndexAtFrame(frameNumber);
    if (index == -1)
    {
        return NULL;
    }
    else
    {
        return getImageAtIndex(index + increment, size, simplified, showThinLines, curveOpacity, antialiasing );
    }
}

// ------

VectorImage* LayerVector::getVectorImageAtIndex(int index)
{
    if ( index < 0 || index >= framesVector.size() )
    {
        return NULL;
    }
    else
    {
        return framesVector.at(index);
    }
}

VectorImage* LayerVector::getVectorImageAtFrame(int frameNumber)
{
    int index = getIndexAtFrame(frameNumber);
    return getVectorImageAtIndex(index);
}

VectorImage* LayerVector::getLastVectorImageAtFrame(int frameNumber, int increment)
{
    int index = getLastIndexAtFrame(frameNumber);
    return getVectorImageAtIndex(index + increment);
}

void LayerVector::setView(QMatrix view)
{
    myView = view;
    setModified(true);
}

void LayerVector::setModified(bool trueOrFalse)
{
    for(int i=0; i < framesVector.size(); i++)
    {
        framesVector[i]->setModified(trueOrFalse);
    }
}

void LayerVector::setModified(int frameNumber, bool trueOrFalse)
{
    LayerImage::setModified(frameNumber, trueOrFalse);
    getLastVectorImageAtFrame(frameNumber, 0)->setModified(trueOrFalse);
}

// -----

bool LayerVector::usesColour(int index)
{
    for(int i=0; i < framesVector.size(); i++)
    {
        if ( framesVector[i]->usesColour(index) ) return true;
    }
    return false;
}

void LayerVector::removeColour(int index)
{
    for(int i=0; i < framesVector.size(); i++)
    {
        framesVector[i]->removeColour(index);
    }
}


bool LayerVector::addNewKeyFrameAt( int frameNumber )
{
    return addKeyFrame( frameNumber, new VectorImage( object() ) );
}

void LayerVector::loadImageAtFrame(QString path, int frameNumber)
{
    if (getIndexAtFrame(frameNumber) == -1) addImageAtFrame(frameNumber);
    int index = getIndexAtFrame(frameNumber);
    framesVector[index]->read(path);
    QFileInfo fi(path);
    framesFilename[index] = fi.fileName();
}

/*void LayerVector::loadImageAtFrame(VectorImage* picture, int frameNumber) {
    if (getIndexAtFrame(frameNumber) == -1) addImageAtFrame(frameNumber);
    int index = getIndexAtFrame(frameNumber);
    //image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    framesVector[index] = picture;
}*/

void LayerVector::swap(int i, int j)
{
    LayerImage::swap(i, j);
    framesVector.swap(i,j);
    framesImage.swap(i,j);
}


bool LayerVector::saveImage(int index, QString path, int layerNumber)
{
    Q_UNUSED(layerNumber);
    int theFrame = framesPosition.at(index);
    QString theFileName = fileName(theFrame, id);
    framesFilename[index] = theFileName;
    //qDebug() << "Write " << theFileName;
    framesVector[index]->write(path +"/"+ theFileName,"VEC");
    framesModified[index] = false;

    return true;
}

QString LayerVector::fileName(int frame, int layerID)
{
    QString layerNumberString = QString::number(layerID);
    QString frameNumberString = QString::number(frame);
    while ( layerNumberString.length() < 3) layerNumberString.prepend("0");
    while ( frameNumberString.length() < 3) frameNumberString.prepend("0");
    return layerNumberString+"."+frameNumberString+".vec";
}

QDomElement LayerVector::createDomElement(QDomDocument& doc)
{
    QDomElement layerTag = doc.createElement("layer");
    layerTag.setAttribute("id", id);
    layerTag.setAttribute("name", name);
    layerTag.setAttribute("visibility", visible);
    layerTag.setAttribute("type", type());
    for(int index=0; index < framesPosition.size() ; index++)
    {
        //QDomElement imageTag = framesVector[index]->createDomElement(doc); // if we want to embed the data
        QDomElement imageTag = doc.createElement("image");
        imageTag.setAttribute("frame", framesPosition.at(index));
        imageTag.setAttribute("src", framesFilename.at(index)); // if we want to link the data to an external file
        layerTag.appendChild(imageTag);
    }
    return layerTag;
}

void LayerVector::loadDomElement(QDomElement element, QString dataDirPath)
{
    if (!element.attribute("id").isNull()) id = element.attribute("id").toInt();
    name = element.attribute("name");
    visible = (element.attribute("visibility") == "1");

    QDomNode imageTag = element.firstChild();
    while (!imageTag.isNull())
    {
        QDomElement imageElement = imageTag.toElement();
        if (!imageElement.isNull())
        {
            if (imageElement.tagName() == "image")
            {
                if (!imageElement.attribute("src").isNull())
                {
                    QString path =  dataDirPath +"/" + imageElement.attribute("src"); // the file is supposed to be in the data irectory
      //qDebug() << "LAY_VECTOR  dataDirPath=" << dataDirPath << "   ;path=" << path;  //added for debugging puproses
                    QFileInfo fi(path);
                    if (!fi.exists()) path = imageElement.attribute("src");
                    int position = imageElement.attribute("frame").toInt();
                    loadImageAtFrame( path, position );
                }
                else
                {
                    int frame = imageElement.attribute("frame").toInt();
                    addImageAtFrame( frame );
                    getVectorImageAtFrame( frame )->loadDomElement(imageElement);
                }
            }
        }
        imageTag = imageTag.nextSibling();
    }
}
