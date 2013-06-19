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
#include <QtDebug>
#include "layerimage.h"
#include "object.h"
#include "timeline.h"
#include "timelinecells.h"

LayerImage::LayerImage(Object* object) : Layer(object)
{
    //imageSize = desiredSize;
    //addImageAtFrame(1);
    frameClicked = -1;
    frameOffset = 0;
}

LayerImage::~LayerImage()
{
}

int LayerImage::getIndexAtFrame(int frameNumber)
{
    int index = -1;
    for(int i=0; i< framesPosition.size(); i++)
    {
        if(framesPosition.at(i) == frameNumber) index = i;
    }
    return index;
}

int LayerImage::getLastIndexAtFrame(int frameNumber)
{
    int position  = -1;
    int index = -1;
    for(int i=0; i < framesPosition.size(); i++)
    {
        if(framesPosition.at(i) > position && framesPosition.at(i) <= frameNumber)
        {
            position = framesPosition.at(i);
            index = i;
        }
    }
    return index;
}

QImage* LayerImage::getImageAtIndex(int index)
{
    return NULL; // no image -> implemented in subclasses
}

QImage* LayerImage::getImageAtFrame(int frameNumber)
{
    int index = getIndexAtFrame(frameNumber);
    if(index == -1)
    {
        return NULL;
    }
    else
    {
        return getImageAtIndex(index);
    }
}


QImage* LayerImage::getLastImageAtFrame(int frameNumber, int increment)
{
    int index = getLastIndexAtFrame(frameNumber);
    if(index == -1)
    {
        return NULL;
    }
    else
    {
        return getImageAtIndex(index + increment);
    }
}

void LayerImage::paintTrack(QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int frameSize)
{
    painter.setFont(QFont("helvetica", height/2));
    if(visible)
    {
        QColor col;
        if(type == BITMAP) col = QColor(130,130,245);
        if(type == VECTOR) col = QColor(100,205,150);
        if(type == SOUND) col = QColor(245,130,130);
        if(type == CAMERA) col = QColor(100,128,140);
        if(!selected) col = QColor( (1*col.red() + 2*200)/3, (1*col.green()+2*200)/3, (1*col.blue()+2*200)/3 );
        //QColor lcol = QColor( (col.red() + 2*255)/3, (col.green()+2*255)/3, (col.blue()+2*255)/3 );
        //QColor mcol = QColor( (3*col.red() + 200)/4, (3*col.green()+200)/4, (3*col.blue()+200)/4 );
        painter.setBrush( col );
        painter.setPen(QPen(QBrush(QColor(100,100,100)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
        painter.drawRect(x, y-1, width, height);
        //painter.setFont(QFont("helvetica", height/2));
        paintImages(painter, cells, x, y, width, height, selected, frameSize);
        //painter.drawText(QPoint(10, y+(2*height)/3), name);

        // changes the apparence if selected
        if(selected)
        {
            paintSelection(painter, x, y, width, height);
            /*QLinearGradient linearGrad(QPointF(0, y), QPointF(0, y + height));
            linearGrad.setColorAt(0, QColor(255,255,255,128) );
            linearGrad.setColorAt(0.40, QColor(255,255,255,0) );
            linearGrad.setColorAt(0.60, QColor(0,0,0,0) );
            linearGrad.setColorAt(1, QColor(0,0,0,64) );
            painter.setBrush( linearGrad );
            painter.setPen(QPen(QBrush(QColor(70,70,70)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
            painter.drawRect(x, y-1, width, height);*/
            /*for(int i=0; i< 4; i++) {
            	//painter.setBrush(QColor(0,0,0,(80*(8-i))/8));
            	painter.setPen(QColor(255, 255, 255, (255*(4-i))/4));
            	painter.drawLine(0, y+i, width, y+i);
            	painter.setPen(QColor(0, 0, 0, (80*(4-i))/4));
            	painter.drawLine(0, y+height-2-i, width, y+height-2-i);
            }*/
            //painter.drawRect(0, y, width, height);
        }
    }
    else
    {
        painter.setBrush(Qt::gray);
        painter.setPen(QPen(QBrush(QColor(100,100,100)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
        painter.drawRect(x, y-1, width, height); // empty rectangle  by default
        //painter.setFont(QFont("helvetica", height/2));
        //painter.drawText(QPoint(10, y+(2*height)/3), name+" (hidden)");
    }
    //painter.drawText(QPoint(10, y+(2*height)/3),"Undefined Layer");
}

void LayerImage::paintImages(QPainter& painter, TimeLineCells* cells, int x, int y, int width, int height, bool selected, int frameSize)
{
    painter.setPen(QPen(QBrush(QColor(40,40,40)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
    if(visible)
    {
        for(int i=0; i < framesPosition.size(); i++)
        {
            if(framesSelected.at(i))
            {
                painter.setBrush(QColor(60,60,60));
                //painter.drawRect(x+(framesPosition.at(i)+frameOffset-1)*frameSize+2, y+1, frameSize-2, height-4);
                painter.drawRect( cells->getFrameX(framesPosition.at(i)+frameOffset)-frameSize+2, y+1, frameSize-2, height-4);
            }
            else
            {
                if(selected)
                    painter.setBrush(QColor(125,125,125));
                else
                    painter.setBrush(QColor(125,125,125,125));
                if(framesModified.at(i)) painter.setBrush(QColor(255,125,125,125));
                painter.drawRect( cells->getFrameX(framesPosition.at(i))-frameSize+2, y+1, frameSize-2, height-4 );
                //painter.drawRect(x+(framesPosition.at(i)-1)*frameSize+2, y+1, frameSize-2, height-4);
                //painter.drawText(QPoint( (framesPosition.at(i)-1)*frameSize+5, y+(2*height)/3), QString::number(i) );
            }
        }
    }
}

void LayerImage::mousePress(QMouseEvent* event, int frameNumber)
{
    frameClicked = frameNumber;
    int index = getIndexAtFrame(frameNumber);
    if(index == -1)
    {
        deselectAllFrames();
    }
    else
    {
        if( (event->modifiers() != Qt::ShiftModifier) && (!framesSelected.at(index)) && (event->buttons() != Qt::RightButton) )
        {
            deselectAllFrames();
        }
        framesSelected[index] = true;
    }
    if(event->modifiers() == Qt::AltModifier)
    {
        for(int i=index; i < framesPosition.size(); i++)
        {
            framesSelected[i] = true;
        }
    }
}

void LayerImage::mouseDoubleClick(QMouseEvent* event, int frameNumber)
{
    int index = getIndexAtFrame(frameNumber);
    if(index != -1)
    {
        for(int i=index; i < framesPosition.size(); i++)
        {
            framesSelected[i] = true;
        }
    }
}


void LayerImage::mouseMove(QMouseEvent* event, int frameNumber)
{
    frameOffset = frameNumber - frameClicked;
    bool ok = true;
    for(int i=0; i < framesPosition.size(); i++)
    {
        if(framesSelected.at(i))
        {
            if(framesPosition.at(i) + frameOffset < 1) ok = false;
            for(int j=0; j < framesPosition.size(); j++)
            {
                if(!framesSelected.at(j))
                {
                    if(framesPosition.at(i) + frameOffset == framesPosition.at(j))
                    {
                        ok = false;
                    }
                }
            }
        }
    }
    if(ok == false) frameOffset = 0;
}

void LayerImage::mouseRelease(QMouseEvent* event, int frameNumber)
{
    for(int i=0; i < framesPosition.size(); i++)
    {
        if(framesSelected.at(i) && frameOffset != 0)
        {
            int originalFrame = framesPosition[i];
            framesPosition[i] = originalFrame + frameOffset;
            //framesModified[i] = true;
            emit imageRemoved(originalFrame); // this is to indicate to the cache that an image have been removed here
            emit imageAdded(originalFrame + frameOffset); // this is to indicate to the cache that an image have been added here
            object->modification();
        }
    }
    bubbleSort();
    frameOffset = 0;
}

bool LayerImage::addImageAtFrame(int frameNumber)
{
    int index = getIndexAtFrame(frameNumber);
    if(index == -1)
    {
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

void LayerImage::removeImageAtFrame(int frameNumber)
{
    int index = getIndexAtFrame(frameNumber);
    if(index != -1)
    {
        framesPosition.removeAt(index);
        framesOriginalPosition.removeAt(index);
        framesSelected.removeAt(index);
        framesFilename.removeAt(index);
        framesModified.removeAt(index);
        emit imageRemoved(frameNumber);
    }
    bubbleSort();
}

void LayerImage::bubbleSort()
{
    // from http://fr.wikipedia.org/wiki/Tri_a_bulles
    int i   = 0; /* Indice de repetition du tri */
    int j   = 0; /* Variable de boucle */
    int MAX = framesPosition.size(); /* taille du tableau */
    /* Booleen marquant l'arret du tri si le tableau est ordonne */
    bool en_desordre = true;
    /* Boucle de repetition du tri et le test qui arrete le tri des que le tableau est ordonne */
    for(i = 0 ; (i < MAX ) && en_desordre; i++)
    {
        /* Supposons le tableau ordonne */
        en_desordre = false;
        /* Verification des elements des places j et j-1 */
        for(j = 1 ; j < MAX - i ; j++)
        {
            /* Si les 2 elements sont mal tries */
            if(framesPosition[j] < framesPosition[j-1])
            {
                /* Inversion des 2 elements */
                swap(j-1,j);
                /* Le tableau n'est toujours pas trie */
                en_desordre = true;
            }
        }
    }
}

void LayerImage::swap(int i, int j)
{
    framesPosition.swap(i,j);
    framesOriginalPosition.swap(i,j);
    framesSelected.swap(i,j);
    framesFilename.swap(i,j);
    framesModified.swap(i,j);
}

void LayerImage::setModified(int frameNumber, bool trueOrFalse)
{
    int index = getLastIndexAtFrame(frameNumber);
    if(index != -1)
    {
        framesModified[index] = trueOrFalse;
        object->modification();
    }
}

void LayerImage::deselectAllFrames()
{
    for(int i=0; i < framesPosition.size(); i++)
    {
        framesSelected[i] = false;
    }
}

bool LayerImage::saveImages(QString path, int layerNumber)
{
    qDebug() << "Saving images of layer n. " << layerNumber;
    QDir dir(path);
    //qDebug() << dir.exists() << dir.path();
    //qDebug() << framesPosition;
    //qDebug() << framesOriginalPosition;

    /*
    // commented because this optimization does not work as expected
    // --- we test if all the files already exists
    for(int i=0; i < framesPosition.size(); i++) {
    	QString fileName = framesFilename.at(i);
    	qDebug() << "Testing if (" << i << ") " << fileName << " exists";
    	bool test = dir.exists(fileName);
    	if(!test) {
            qDebug() << "The file (" << i << ", frame " << framesPosition.at(i) << " ) " << fileName << " does not exist";
            framesModified[i] = true;
    	} else {
    	    qDebug() << "The file (" << i << ", frame " << framesPosition.at(i) << " ) " << fileName << " exists";
    	}
    }
    // --- we rename the files for the images which have been moved (if such files exist)
    // --- we do that in two steps, with temporary names in the first step, in order to avoid conflicting names
    // don't rename audio files
    if (this->type != Layer::SOUND ) {
    for(int i=0; i < framesPosition.size(); i++) {
    	int frame1 = framesPosition.at(i);
    	int frame0 = framesOriginalPosition.at(i);
    	if(frame1 != frame0 && framesFilename.at(i) != "") {
    		QString fileName0 = fileName(frame0,layerNumber);
    		QString fileName1 = fileName(frame1,layerNumber);
    		//qDebug() << fileName0 << fileName1;
    		bool rename = dir.rename( fileName0, "tmp"+fileName0 );
    		if(rename) { }
    	}
    }
    for(int i=0; i < framesPosition.size(); i++) {
    	int frame1 = framesPosition.at(i);
    	int frame0 = framesOriginalPosition.at(i);
    	if(frame1 != frame0 && framesFilename.at(i) != "") {
    		QString fileName0 = fileName(frame0,layerNumber);
    		QString fileName1 = fileName(frame1,layerNumber);
    		bool rename = dir.rename( "tmp"+fileName0, fileName1 );
    		if(rename) {
    			framesOriginalPosition[i] = frame1;
    			framesFilename[i] = fileName1;
    			qDebug() << "File " << fileName0 << " renamed to " << fileName1 << " " << framesFilename.at(i);
    		} else { // the file doesn't exist, we probably need to create it
    			qDebug() << "Could not rename to " << framesFilename.at(i);
    			framesFilename[i] = "";
    			framesModified[i] = true;
    		}
    		}
    	}
    }
    // --- we now save the files for the images which have been modified
    for(int i=0; i < framesPosition.size(); i++) {
    	framesOriginalPosition[i]=framesPosition[i];
    	QString fileName1 = fileName(framesPosition.at(i),layerNumber);
    	if ( fileName1.compare(framesFilename.at(i)))
    	qDebug() << "Inconsistent filename: " << framesFilename.at(i);
    	if(framesModified.at(i)) {
    		qDebug() << "Trying to save " << framesFilename.at(i);
    		saveImage(i, path, layerNumber);
    }
    }*/

    // always saves all frames, no optimization
    for(int i=0; i < framesPosition.size(); i++)
    {
        framesOriginalPosition[i]=framesPosition[i];
        qDebug() << "Trying to save " << framesFilename.at(i) << " of layer n. " << layerNumber;
        saveImage(i, path, layerNumber);
    }
    qDebug() << "Layer " << layerNumber << "done";
    return true;
}

bool LayerImage::saveImage(int index, QString path, int layerNumber)
{
    // implemented in subclasses
    return true;
}

QString LayerImage::fileName(int index, int layerNumber)
{
    // implemented in subclasses
    return "";
}
