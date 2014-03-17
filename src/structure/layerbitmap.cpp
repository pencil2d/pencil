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

LayerBitmap::LayerBitmap( Object* object ) : LayerImage( object, Layer::BITMAP )
{
    name = QString(tr("Bitmap Layer"));
    addImageAtFrame(1);
}

LayerBitmap::~LayerBitmap()
{
    while (!m_framesBitmap.empty())
        delete m_framesBitmap.takeFirst();
}

// ------

BitmapImage* LayerBitmap::getBitmapImageAtIndex(int index)
{
    if ( index < 0 || index >= m_framesBitmap.size() )
    {
        return NULL;
    }
    else
    {
        return m_framesBitmap.at(index);
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

bool LayerBitmap::addImageAtFrame( int frameNumber )
{
    if ( frameNumber <= 0 )
    {
        return false;
    }

    int index = getIndexAtFrame(frameNumber);
    if (index == -1)
    {
        m_framesBitmap.append(new BitmapImage);
        framesPosition.append(frameNumber);
        framesSelected.append(false);
        framesFilename.append("");
        framesModified.append(false);
        bubbleSort();

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
    if (index != -1  && framesPosition.size() > 1)  // TODO: maybe size=0 is acceptable?
    {
        delete m_framesBitmap.at(index);
        m_framesBitmap.removeAt(index);
        framesPosition.removeAt(index);
        framesSelected.removeAt(index);
        framesFilename.removeAt(index);
        framesModified.removeAt(index);
        bubbleSort();
    }
}

void LayerBitmap::loadImageAtFrame(QString path, QPoint topLeft, int frameNumber)
{
    //qDebug() << path;
    if (getIndexAtFrame(frameNumber) == -1) addImageAtFrame(frameNumber);
    int index = getIndexAtFrame(frameNumber);
    m_framesBitmap[index] = new BitmapImage(path, topLeft);
    QFileInfo fi(path);
    framesFilename[index] = fi.fileName();
}

void LayerBitmap::swap(int i, int j)
{
    LayerImage::swap(i, j);
    m_framesBitmap.swap(i,j);
}

bool LayerBitmap::saveImage(int index, QString path, int layerNumber)
{
    Q_UNUSED(layerNumber);
    int theFrame = framesPosition.at(index);
    QString theFileName = fileName(theFrame, id);
    framesFilename[index] = theFileName;
    //qDebug() << "Write " << theFileName;
    m_framesBitmap[index]->m_pImage->save(path +"/"+ theFileName,"PNG");
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
        imageTag.setAttribute("topLeftX", m_framesBitmap[index]->topLeft().x());
        imageTag.setAttribute("topLeftY", m_framesBitmap[index]->topLeft().y());
        layerTag.appendChild(imageTag);
    }
    return layerTag;
}

void LayerBitmap::loadDomElement(QDomElement element, QString dataDirPath)
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
