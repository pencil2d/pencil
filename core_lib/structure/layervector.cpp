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
    addNewKeyFrameAt( 1 );
}

LayerVector::~LayerVector()
{
}

// ------
/*
QImage* LayerVector::getImageAtIndex( int index,
                                      QSize size,
                                      bool simplified,
                                      bool showThinLines,
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
                                     antialiasing );
            vectorImage->setModified(false);
        }
        return image;
    }
}
*/

bool LayerVector::usesColour(int colorIndex)
{
    bool bUseColor = false;
    foreachKeyFrame( [&] ( KeyFrame* pKeyFrame )
    {
        auto pVecImage = static_cast< VectorImage* >( pKeyFrame );
        
        bUseColor = bUseColor || pVecImage->usesColour( colorIndex );
    } );

    return bUseColor;
}

void LayerVector::removeColour( int colorIndex )
{
    foreachKeyFrame( [=]( KeyFrame* pKeyFrame )
    {
        auto pVecImage = static_cast< VectorImage* >( pKeyFrame );
        pVecImage->removeColour( colorIndex );
    } );
}

bool LayerVector::addNewKeyFrameAt( int frameNumber )
{
    return addKeyFrame( frameNumber, new VectorImage( object() ) );
}

void LayerVector::loadImageAtFrame(QString path, int frameNumber)
{
    if ( getIndexAtFrame( frameNumber ) == -1 )
    {
        addNewKeyFrameAt( frameNumber );
    }
}

bool LayerVector::saveKeyFrame( KeyFrame* pKeyFrame, QString path )
{
    VectorImage* pVecImage = static_cast< VectorImage* >( pKeyFrame );

    QString theFileName = fileName( pKeyFrame->pos() );
    QString strFilePath = QDir( path ).filePath( theFileName );
    pVecImage->write( strFilePath, "VEC" );

    return true;
}

QString LayerVector::fileName( int frame )
{
    int layerID = id;
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
    
    foreachKeyFrame( [&] ( KeyFrame* pKeyFrame )
    {
        VectorImage* pImg = static_cast< VectorImage* >( pKeyFrame );
        //QDomElement imageTag = framesVector[index]->createDomElement(doc); // if we want to embed the data
        QDomElement imageTag = doc.createElement("image");
        //imageTag.setAttribute("frame", framesPosition.at(index));
        //imageTag.setAttribute("src", framesFilename.at(index)); // if we want to link the data to an external file
        layerTag.appendChild(imageTag);
    } );
    
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
                    QString path =  dataDirPath +"/" + imageElement.attribute("src"); // the file is supposed to be in the data directory
      //qDebug() << "LAY_VECTOR  dataDirPath=" << dataDirPath << "   ;path=" << path;  //added for debugging puproses
                    QFileInfo fi(path);
                    if (!fi.exists()) path = imageElement.attribute("src");
                    int position = imageElement.attribute("frame").toInt();
                    loadImageAtFrame( path, position );
                }
                else
                {
                    int frame = imageElement.attribute("frame").toInt();
                    addNewKeyFrameAt( frame );
                    getVectorImageAtFrame( frame )->loadDomElement(imageElement);
                }
            }
        }
        imageTag = imageTag.nextSibling();
    }
}

VectorImage* LayerVector::getVectorImageAtFrame( int frameNumber )
{
    return static_cast< VectorImage* >( getKeyFrameAtPosition( frameNumber ) );
}

VectorImage* LayerVector::getLastVectorImageAtFrame( int frameNumber, int increment )
{
    return static_cast< VectorImage* >( getLastKeyFrameAtPosition( frameNumber + increment ) );
}

void LayerVector::setModified( bool trueOrFalse )
{
    // TODO:
}
