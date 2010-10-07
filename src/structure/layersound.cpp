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
#include "layersound.h"
#include "object.h"
#include <phonon>

 LayerSound::LayerSound(Object* object) : LayerImage(object) {
	type = Layer::SOUND;
	//modified = false;
	name = QString("Sound Layer");
}

LayerSound::~LayerSound() {
  while(!sound.empty())
    delete sound.takeFirst();
}


void LayerSound::paintImages(QPainter &painter, TimeLineCells *cells, int x, int y, int width, int height, bool selected, int frameSize) {

		for(int i=0; i < sound.size(); i++) {
			qreal h = x + (framesPosition.at(i)-1)*frameSize+2;
			if(framesSelected.at(i)) {
				painter.setBrush(QColor(60,60,60));
				h = h + frameOffset*frameSize;
				//	painter.drawRect((framesPosition.at(i)+frameOffset-1)*frameSize+2, verticalPosition+1, frameSize-2, layerHeight-4);
			}
			else {
			//	if(framesModified.at(i))
			//		painter.setBrush(QColor(255,125,125));
			//	else
			//painter.setPen(QPen(QBrush(QColor(40,40,40)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
				painter.setBrush(QColor(125,125,125));
			}
			QPointF points[3] = { QPointF(h, y+4), QPointF(h, y+height-4), QPointF(h+15, y+0.5*height) };
			painter.drawPolygon( points, 3 );
			//painter.drawRect((startingFrame.at(i)-1)*frameSize+2, verticalPosition+1, frameSize-2, layerHeight-4);
			painter.drawText(QPoint( h + 20, y+(2*height)/3), framesFilename.at(i) );
			//}
		}
}

bool LayerSound::addImageAtFrame(int frameNumber) {
	int index = getIndexAtFrame(frameNumber);
	if(index == -1) {
		sound.append(NULL);
		soundFilepath.append("");
		framesPosition.append(frameNumber);
		framesSelected.append(false);
		framesFilename.append("");
		framesModified.append(false);
		bubbleSort();
		return true;
	} else {
		return false;
	}
}

void LayerSound::removeImageAtFrame(int frameNumber) {
	int index = getIndexAtFrame(frameNumber);
	if(index != -1  && framesPosition.size() != 1) {
		delete sound.at(index);
		soundFilepath.removeAt(index);
		framesPosition.removeAt(index);
		framesSelected.removeAt(index);
		framesFilename.removeAt(index);
		framesModified.removeAt(index);
		bubbleSort();
	}
}

void LayerSound::loadSoundAtFrame(QString filePathString, int frameNumber) {
//	if(getIndexAtFrame(frameNumber) == -1) addImageAtFrame(frameNumber);
 	int index = getIndexAtFrame(frameNumber);
	if(index == -1)
        addImageAtFrame(frameNumber);
	index = getIndexAtFrame(frameNumber);

 	QFileInfo fi(filePathString);
 	if(fi.exists()) {
//		sound[index] = new QSound(filePathString, NULL);
       Phonon::MediaObject *media = new Phonon::MediaObject();
        connect(media, SIGNAL(totalTimeChanged(qint64)), this, SLOT(addTimelineKey(qint64)));
       media->setCurrentSource(filePathString);
        sound[index] = media;
 		soundFilepath[index] = filePathString;
 		framesFilename[index] = fi.fileName();
 	} else {
//		sound[index] = NULL;
        sound[index] = NULL;
 		soundFilepath[index] = "Wrong file";
 		framesFilename[index] = "Wrong file" + filePathString;
 	}
}

 void LayerSound::swap(int i, int j) {
 	 	LayerImage::swap(i, j);
 	//	sound.swap(i,j);
 	    sound.swap(i, j);
 	 	soundFilepath.swap(i,j);
 	 }


bool LayerSound::saveImage(int index, QString path, int layerNumber) {
	/*QString layerNumberString = QString::number(layerNumber);
	QString frameNumberString = QString::number(framesPosition.at(index));
	while( layerNumberString.length() < 3) layerNumberString.prepend("0");
	while( frameNumberString.length() < 3) frameNumberString.prepend("0");*/
	//framesFilename[index] = path+"/"+layerNumberString+"."+frameNumberString+".png";
	//framesFilename[index] = layerNumberString+"."+frameNumberString+".png";
	//framesFilename[index] = layerNumberString+"."+frameNumberString+".png";
	//qDebug() << "Write " << framesFilename.at(index);
	//framesImage[index]->save(framesFilename.at(index),"PNG");
	QFile originalFile( soundFilepath.at(index) );
	originalFile.copy( path + "/" + framesFilename.at(index) );
	framesModified[index] = false;

    return true;
}

/*#
void LayerSound::playSound(int frame) {
	for(int i=0; i < sound.size(); i++) {
		if(frame == framesPosition.at(i)) {
			if(sound.at(i) != NULL && visible) sound[i]->play();
		}
	}
}
#*/
void LayerSound::playSound(int frame) {
    static QSettings settings("Pencil","Pencil");
    int fps = settings.value("fps").toInt();

    for (int i = 0; i < sound.size(); ++i) {
        Phonon::MediaObject *media = sound.at(i);
        if (media != NULL && visible) {
            int position = framesPosition.at(i);
            if (frame < position) {
                media->stop();
            } else {
                Phonon::AudioOutput * audioOutput = NULL;
                if (outputDevices.size() <= i) {
                    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
                    outputDevices.push_back(audioOutput);
                } else {
                    audioOutput = outputDevices.at(i);
                }

                int offsetInMs = floor((frame - position) * float(1000) / fps);
                if (media->state() == Phonon::PlayingState) {
                    if (fabs(media->currentTime() - offsetInMs) > float(500))
                        media->seek(offsetInMs);
                } else {
                    if (frame > position) {
                        media->pause();
                        media->seek(float(offsetInMs) / fps);
                    }
                    Phonon::createPath(media, outputDevices.at(i));
                    media->play();
               }
            }
        }
    }
}



void LayerSound::stopSound() {
	for(int i=0; i < sound.size(); i++) {
		if(sound.at(i) != NULL) sound[i]->stop();
	}
}


QDomElement LayerSound::createDomElement(QDomDocument &doc) {
	QDomElement layerTag = doc.createElement("layer");
	layerTag.setAttribute("id",id);
	layerTag.setAttribute("name", name);
	layerTag.setAttribute("visibility", visible);
	layerTag.setAttribute("type", type);
	for(int index=0; index < framesPosition.size() ; index++) {
		QDomElement soundTag = doc.createElement("sound");
		soundTag.setAttribute("position", framesPosition.at(index));
		soundTag.setAttribute("src", framesFilename.at(index));
		layerTag.appendChild(soundTag);
	}
	return layerTag;
}

void LayerSound::loadDomElement(QDomElement element, QString filePath) {
	if(!element.attribute("id").isNull()) id = element.attribute("id").toInt();
	name = element.attribute("name");
	visible = (element.attribute("visibility") == "1");
	type = element.attribute("type").toInt();

	QDomNode soundTag = element.firstChild();
	while(!soundTag.isNull()) {
		QDomElement soundElement = soundTag.toElement();
		if(!soundElement.isNull()) {
			if(soundElement.tagName() == "sound") {
				QString path = filePath + ".data/" + soundElement.attribute("src"); // the file is supposed to be in the data irectory
				QFileInfo fi(path);
				if(!fi.exists()) path = soundElement.attribute("src");
				int position = soundElement.attribute("position").toInt();
				loadSoundAtFrame( path, position );
			}
		}
		soundTag = soundTag.nextSibling();
	}
}

void LayerSound::addTimelineKey(qint64 newTotalTime) {
  QSettings settings("Pencil","Pencil");
    int fps = settings.value("fps").toInt();
   addImageAtFrame(fps * ((newTotalTime / 1000) + 2));
}
