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
#include "layerbitmap.h"
#include <QtDebug>

LayerBitmap::LayerBitmap(Object* object) : LayerImage(object)
{
    m_eType = Layer::BITMAP;
    name = QString("Bitmap Layer");
    addImageAtFrame(1);

    //imageSize = desiredSize;
    //frameClicked = -1;
    //frameOffset = 0;
}

LayerBitmap::~LayerBitmap()
{
    while (!framesBitmap.empty())
        delete framesBitmap.takeFirst();
}

// ------

BitmapImage* LayerBitmap::getBitmapImageAtIndex(int index)
{
    if ( index < 0 || index >= framesBitmap.size() )
    {
        return NULL;
    }
    else
    {
        return framesBitmap.at(index);
    }
}

BitmapImage* LayerBitmap::getBitmapImageAtFrame(int frameNumber)
{
    int index = getIndexAtFrame(frameNumber);
    return getBitmapImageAtIndex(index);
}

BitmapImage* LayerBitmap::getLastBitmapImageAtFrame(int frameNumber, int increment)
{
    int index = getLastIndexAtFrame(frameNumber);
    return getBitmapImageAtIndex(index + increment);
}


// -----

QImage* LayerBitmap::getImageAtIndex(int index)
{
    Q_UNUSED(index);
    return NULL;
}

bool LayerBitmap::addImageAtFrame(int frameNumber)
{
    int index = getIndexAtFrame(frameNumber);
    if (index == -1)
    {
        //framesImage.append(new QImage(imageSize, QImage::Format_ARGB32_Premultiplied));
        framesBitmap.append(new BitmapImage(m_pObject));
        framesPosition.append(frameNumber);
        framesOriginalPosition.append(frameNumber);
        framesSelected.append(false);
        framesFilename.append("");
        framesModified.append(false);
        bubbleSort();
        emit imageAdded(frameNumber);
        return true;
    }
    else
    {
        return false;
    }
}

void LayerBitmap::removeImageAtFrame(int frameNumber)
{
    int index = getIndexAtFrame(frameNumber);
    if (index != -1  && framesPosition.size() != 1)
    {
        delete framesBitmap.at(index);
        framesBitmap.removeAt(index);
        framesPosition.removeAt(index);
        framesOriginalPosition.removeAt(index);
        framesSelected.removeAt(index);
        framesFilename.removeAt(index);
        framesModified.removeAt(index);
        bubbleSort();
        emit imageRemoved(frameNumber);
    }
}

void LayerBitmap::loadImageAtFrame(QString path, QPoint topLeft, int frameNumber)
{
    //qDebug() << path;
    if (getIndexAtFrame(frameNumber) == -1) addImageAtFrame(frameNumber);
    int index = getIndexAtFrame(frameNumber);
    framesBitmap[index] = new BitmapImage(m_pObject, path, topLeft);
    QFileInfo fi(path);
    framesFilename[index] = fi.fileName();
}

void LayerBitmap::swap(int i, int j)
{
    LayerImage::swap(i, j);
    framesBitmap.swap(i,j);
}

bool LayerBitmap::saveImage(int index, QString path, int layerNumber)
{
    Q_UNUSED(layerNumber);
    int theFrame = framesPosition.at(index);
    QString theFileName = fileName(theFrame, id);
    framesFilename[index] = theFileName;
    //qDebug() << "Write " << theFileName;
    framesBitmap[index]->image->save(path +"/"+ theFileName,"PNG");
    framesModified[index] = false;

    return true;
}

QString LayerBitmap::fileName(int frame, int layerID)
{
    QString layerNumberString = QString::number(layerID);
    QString frameNumberString = QString::number(frame);
    while ( layerNumberString.length() < 3) layerNumberString.prepend("0");
    while ( frameNumberString.length() < 3) frameNumberString.prepend("0");
    return layerNumberString+"."+frameNumberString+".png";
}

QDomElement LayerBitmap::createDomElement(QDomDocument& doc)
{
    QDomElement layerTag = doc.createElement("layer");
    layerTag.setAttribute("id", id);
    layerTag.setAttribute("name", name);
    layerTag.setAttribute("visibility", visible);
    layerTag.setAttribute("type", type());
    for(int index=0; index < framesPosition.size() ; index++)
    {
        QDomElement imageTag = doc.createElement("image");
        imageTag.setAttribute("frame", framesPosition.at(index));
        imageTag.setAttribute("src", framesFilename.at(index));
        imageTag.setAttribute("topLeftX", framesBitmap[index]->topLeft().x());
        imageTag.setAttribute("topLeftY", framesBitmap[index]->topLeft().y());
        layerTag.appendChild(imageTag);
    }
    return layerTag;
}

void LayerBitmap::loadDomElement(QDomElement element, QString dataDirPath)
{
    if (!element.attribute("id").isNull()) id = element.attribute("id").toInt();
    name = element.attribute("name");
    visible = (element.attribute("visibility") == "1");
    m_eType = static_cast<LAYER_TYPE>( element.attribute("type").toInt() );

    QDomNode imageTag = element.firstChild();
    while (!imageTag.isNull())
    {
        QDomElement imageElement = imageTag.toElement();
        if (!imageElement.isNull())
        {
            if (imageElement.tagName() == "image")
            {
                QString path =  dataDirPath +"/" + imageElement.attribute("src"); // the file is supposed to be in the data directory
     //qDebug() << "LAY_BITMAP  dataDirPath=" << dataDirPath << "   ;path=" << path;  //added for debugging puproses
                QFileInfo fi(path);
                if (!fi.exists()) path = imageElement.attribute("src");
                int position = imageElement.attribute("frame").toInt();
                int x = imageElement.attribute("topLeftX").toInt();
                int y = imageElement.attribute("topLeftY").toInt();
                loadImageAtFrame( path, QPoint(x,y), position );
            }
            /*if (imageElement.tagName() == "image") {
            	int frame = imageElement.attribute("frame").toInt();
            	addImageAtFrame( frame );
            	getBitmapImageAtFrame( frame )->loadDomElement(imageElement, filePath);
            }*/
        }
        imageTag = imageTag.nextSibling();
    }
}
