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
#include <QtGui>
#include <QDomDocument>
#include <QTextStream>
#include <QMessageBox>

#include "object.h"
#include "layer.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layersound.h"
#include "layercamera.h"

#include "editor.h"
#include "bitmapimage.h"

// ******* Mac-specific: ******** (please comment (or reimplement) the lines below to compile on Windows or Linux
//#include <CoreFoundation/CoreFoundation.h>
// ******************************

Object::Object() : QObject(0) {
	// default name
	name = "Object";
	modified = false;
	mirror = false;
}

Object::~Object() {
	while(!layer.empty()) {
		delete layer.takeLast();
	}
}

QDomElement Object::createDomElement(QDomDocument &doc) {
	QDomElement tag = doc.createElement("object");

	for(int i=0; i < getLayerCount(); i++) {
		Layer* layer = getLayer(i);
		QDomElement layerTag = layer->createDomElement(doc);
		tag.appendChild(layerTag);
	}
	return tag;
}

bool Object::loadDomElement(QDomElement docElem, QString filePath) {
	if(docElem.isNull()) return false;
	int layerNumber = -1;
	QDomNode tag = docElem.firstChild();
	bool someRelevantData = false;
	while(!tag.isNull()) {
		QDomElement element = tag.toElement(); // try to convert the node to an element.
		if(!element.isNull()) {
			if(element.tagName() == "layer") {
				someRelevantData = true;
				// --- bitmap layer ---
				if(element.attribute("type").toInt() == Layer::BITMAP) {
					addNewBitmapLayer();
					layerNumber++;
					((LayerBitmap*)(getLayer(layerNumber)))->loadDomElement( element, filePath );
				}
				// --- vector layer ---
				if(element.attribute("type").toInt() == Layer::VECTOR) {
					addNewVectorLayer();
					layerNumber++;
					((LayerVector*)(getLayer(layerNumber)))->loadDomElement( element, filePath );
				}
				// --- sound layer ---
				if(element.attribute("type").toInt() == Layer::SOUND) {
					addNewSoundLayer();
					layerNumber++;
					((LayerSound*)(getLayer(layerNumber)))->loadDomElement( element, filePath );
				}
				// --- camera layer ---
				if(element.attribute("type").toInt() == Layer::CAMERA) {
					addNewCameraLayer();
					layerNumber++;
					((LayerCamera*)(getLayer(layerNumber)))->loadDomElement( element, filePath );
				}

			}
		}
		tag = tag.nextSibling();
	}
	return someRelevantData;
}


bool Object::read(QString filePath) {
	QFileInfo fileInfo(filePath);
	if( fileInfo.isDir() ) return false;

	QFile* file = new QFile(filePath);
	if (!file->open(QFile::ReadOnly)) return false;

	QDomDocument doc;
	doc.setContent(file);

	QDomElement docElem = doc.documentElement();
	loadDomElement(docElem, filePath);

	/*
	// old code: list all the files beginning with the same name
	QStringList list = name.split(".");
	QStringList filtername(list.at(0)+"*.*");
	QDir dir(fileInfo.absolutePath());
	QStringList entries = dir.entryList(filtername,QDir::Files,QDir::Type);
	*/
	return true;
}

bool Object::write(QString filePath) {
	QFile* file = new QFile(filePath);
	if (!file->open(QFile::WriteOnly | QFile::Text)) {
		//QMessageBox::warning(this, "Warning", "Cannot write file");
		qDebug() << "Object - Cannot write file" << filePath;
		return false;
	}
	QTextStream out(file);

	QDomDocument doc("PencilDocument");
	QDomElement root = createDomElement(doc);
	doc.appendChild(root);

	int IndentSize = 2;
	qDebug() << "--- Starting to write XML file...";
	doc.save(out, IndentSize);
	file->close();
	qDebug() << "--- Writing XML file done.";
	return true;
}

void Object::addNewBitmapLayer() {
	LayerBitmap* layerBitmap = new LayerBitmap(this);
	layerBitmap->id = 1+getMaxID();
	layer.append( layerBitmap );
	connect( layerBitmap, SIGNAL(imageAdded(int)), this, SIGNAL(imageAdded(int)) );
	connect( layerBitmap, SIGNAL(imageRemoved(int)), this, SLOT(imageCheck(int)) );
}

void Object::addNewVectorLayer() {
	LayerVector* layerVector = new LayerVector(this);
	layerVector->id = 1+getMaxID();
	layer.append( layerVector );
	connect( layerVector, SIGNAL(imageAdded(int)), this, SIGNAL(imageAdded(int)) );
	connect( layerVector, SIGNAL(imageRemoved(int)), this, SLOT(imageCheck(int)) );
}

void Object::addNewSoundLayer() {
	LayerSound* layerSound = new LayerSound(this);
	layerSound->id = 1+getMaxID();
	layer.append( layerSound );
}

void Object::addNewCameraLayer() {
	LayerCamera* layerCamera = new LayerCamera(this);
	layerCamera->id = 1+getMaxID();
	layer.append( layerCamera );
	connect( layerCamera, SIGNAL(imageAdded(int,int)), this, SIGNAL(imageAdded(int,int)) );
	connect( layerCamera, SIGNAL(imageRemoved(int)), this, SLOT(imageCheck(int)) );
}

int Object::getMaxID() {
	int result = 0;
	for(int i=0; i< getLayerCount(); i++) {
		Layer* layeri = getLayer(i);
		if(layeri->id > result) result = layeri->id;
	}
	return result;
}

Layer* Object::getLayer(int i) {
	if(i > -1 && i < getLayerCount()) {
		return layer.at(i);
	} else {
		return NULL;
	}
}

void Object::moveLayer(int i, int j) {
	if(i != j) {
		layer.insert(j, layer.at(i));
		if(i>j) {
			layer.removeAt(i+1);
		} else {
			layer.removeAt(i);
		}
	}
}

void Object::deleteLayer(int i) {
	if(i > -1 && i < layer.size()) {
		//layer.removeAt(i);
		disconnect( layer[i], 0, this, 0); // disconnect the layer from this object
		delete layer.takeAt(i);
	}
}

void Object::playSoundIfAny(int frame) {
	for(int i=0; i < getLayerCount(); i++) {
		Layer* layer = getLayer(i);
		if( layer->type == Layer::SOUND) {
			((LayerSound*)layer)->playSound(frame);
		}
	}
}

void Object::stopSoundIfAny() {
	for(int i=0; i < getLayerCount(); i++) {
		Layer* layer = getLayer(i);
		if( layer->type == Layer::SOUND) {
			((LayerSound*)layer)->stopSound();
		}
	}
}

ColourRef Object::getColour(int i) {
	ColourRef result(Qt::white, "error");
	if( i > -1 && i < myPalette.size() ) {
		result = myPalette.at(i);
	}
	return result;
}

void Object::addColour(QColor colour) {
	addColour( ColourRef(colour, "Colour "+QString::number(myPalette.size()) ) );
}

bool Object::removeColour(int index) {
	for(int i=0; i< getLayerCount(); i++) {
		Layer* layer = getLayer(i);
		if( layer->type == Layer::VECTOR) {
			LayerVector *layerVector = ((LayerVector*)layer);
			if(layerVector->usesColour(index)) return false;
		}
	}
	for(int i=0; i< getLayerCount(); i++) {
		Layer* layer = getLayer(i);
		if( layer->type == Layer::VECTOR) {
			LayerVector *layerVector = ((LayerVector*)layer);
			layerVector->removeColour(index);
		}
	}
	myPalette.removeAt(index);
	return true;
	// update the vector pictures using that colour !
}

void Object::renameColour(int i, QString text) {
	myPalette[i].name = text;
}

bool Object::savePalette(QString filePath) {
	return exportPalette(filePath+"/palette.xml");
}

bool Object::exportPalette(QString filePath) {
	//qDebug() << "coucou" << filePath;
	QFile* file = new QFile(filePath);
	if (!file->open(QFile::WriteOnly | QFile::Text)) {
		//QMessageBox::warning(this, "Warning", "Cannot write file");
		return false;
	}
	QTextStream out(file);

	QDomDocument doc("PencilPalette");
	QDomElement root = doc.createElement("palette");
	doc.appendChild(root);
	for(int i=0; i < myPalette.size(); i++) {
		QDomElement tag = doc.createElement("Colour");
		tag.setAttribute("name", myPalette.at(i).name);
		tag.setAttribute("red", myPalette.at(i).colour.red());
		tag.setAttribute("green", myPalette.at(i).colour.green());
		tag.setAttribute("blue", myPalette.at(i).colour.blue());
		root.appendChild(tag);
		//QDomText t = doc.createTextNode( myPalette.at(i).name );
		//tag.appendChild(t);
	}
	//QString xml = doc.toString();

	int IndentSize = 2;
    doc.save(out, IndentSize);
    return true;
}

bool Object::loadPalette(QString filePath) {
	return importPalette(filePath+"/palette.xml");
}

bool Object::importPalette(QString filePath) {
	QFile* file = new QFile(filePath);
	if (!file->open(QFile::ReadOnly)) {
		//QMessageBox::warning(this, "Warning", "Cannot read file");
		return false;
	}

	QDomDocument doc;
	doc.setContent(file);

	myPalette.clear();
	QDomElement docElem = doc.documentElement();
	QDomNode tag = docElem.firstChild();
	while(!tag.isNull()) {
		QDomElement e = tag.toElement(); // try to convert the node to an element.
		if(!e.isNull()) {
			QString name = e.attribute("name");
			int r = e.attribute("red").toInt();
			int g = e.attribute("green").toInt();
			int b = e.attribute("blue").toInt();
			myPalette.append( ColourRef( QColor(r, g, b), name) );
			//qDebug() << name << r << g << b << endl; // the node really is an element.
		}
		tag = tag.nextSibling();
	}
    return true;
}


void Object::defaultInitialisation() {
	// default layers
	addNewBitmapLayer();
	addNewVectorLayer();
	addNewCameraLayer();//TODO Add Camera Layer at beginning for Quick Preview

	// default palette
	loadDefaultPalette();
}

void Object::loadDefaultPalette() {
	myPalette.clear();
	addColour(  ColourRef(QColor(Qt::black), QString("Black"))  );
	addColour(  ColourRef(QColor(Qt::red), QString("Red"))  );
	addColour(  ColourRef(QColor(Qt::darkRed), QString("Dark Red"))  );
	addColour(  ColourRef(QColor(255,128,0), QString("Orange"))  );
	addColour(  ColourRef(QColor(128,64,0), QString("Dark Orange"))  );
	addColour(  ColourRef(QColor(Qt::yellow), QString("Yellow"))  );
	addColour(  ColourRef(QColor(Qt::darkYellow), QString("Dark Yellow"))  );
	addColour(  ColourRef(QColor(Qt::green), QString("Green"))  );
	addColour(  ColourRef(QColor(Qt::darkGreen), QString("Dark Green"))  );
	addColour(  ColourRef(QColor(Qt::cyan), QString("Cyan"))  );
	addColour(  ColourRef(QColor(Qt::darkCyan), QString("Dark Cyan"))  );
	addColour(  ColourRef(QColor(Qt::blue), QString("Blue"))  );
	addColour(  ColourRef(QColor(Qt::darkBlue), QString("Dark Blue"))  );
	addColour(  ColourRef(QColor(255,255,255), QString("White"))  );
	addColour(  ColourRef(QColor(220,220,229), QString("Very Light Grey"))  );
	addColour(  ColourRef(QColor(Qt::lightGray), QString("Light Grey"))  );
	addColour(  ColourRef(QColor(Qt::gray), QString("Grey"))  );
	addColour(  ColourRef(QColor(Qt::darkGray), QString("Dark Grey"))  );
	addColour(  ColourRef(QColor(255,227,187), QString("Light Skin"))  );
	addColour(  ColourRef(QColor(221,196,161), QString("Light Skin - shade"))  );
	addColour(  ColourRef(QColor(255,214,156), QString("Skin"))  );
	addColour(  ColourRef(QColor(207,174,127), QString("Skin - shade"))  );
	addColour(  ColourRef(QColor(255,198,116), QString("Dark Skin"))  );
	addColour(  ColourRef(QColor(227,177,105), QString("Dark Skin - shade"))  );
}

//void Object::paintImage(QPainter &painter, int frameNumber, const QRectF &source, const QRectF &target, bool background, qreal curveOpacity, bool antialiasing, bool niceGradients) {
void Object::paintImage(QPainter &painter, int frameNumber, bool background, qreal curveOpacity, bool antialiasing, int gradients) {
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

	//painter.setWorldMatrix(matrix);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

	// paints the background
	if(background) {
		painter.setPen(Qt::NoPen);
		painter.setBrush(Qt::white);
		painter.setWorldMatrixEnabled(false);
		painter.drawRect( QRect(0,0, painter.device()->width(), painter.device()->height() ) );
		painter.setWorldMatrixEnabled(true);
	}

	for(int i=0; i < getLayerCount(); i++) {
		Layer* layer = getLayer(i);
		if(layer->visible) {
			painter.setOpacity(1.0);

			// paints the bitmap images
			if(layer->type == Layer::BITMAP) {
				LayerBitmap* layerBitmap = (LayerBitmap*)layer;
				/*BitmapImage* bitmapImage = layerBitmap->getLastBitmapImageAtFrame(frameNumber, 0);
				// TO BE FIXED
				if(bitmapImage != NULL) {
					if( mirror) {
						painter.drawImage(target, (*(bitmapImage->image)).mirrored(true, false), source);
					} else {
						painter.drawImage(target, *(bitmapImage->image), source);
					}
				}*/
				layerBitmap->getLastBitmapImageAtFrame(frameNumber, 0)->paintImage(painter);
			}
			// paints the vector images
			if(layer->type == Layer::VECTOR) {
				LayerVector* layerVector = (LayerVector*)layer;
				layerVector->getLastVectorImageAtFrame(frameNumber, 0)->paintImage(painter, false, false, curveOpacity, antialiasing, gradients);
			}
		}
	}
}

void Object::exportFrames(int frameStart, int frameEnd, QMatrix view, Layer* currentLayer, QSize exportSize, QString filePath, const char* format, int quality, bool background, bool antialiasing, int gradients) {

	QSettings settings("Pencil","Pencil");
	qreal curveOpacity = (100-settings.value("curveOpacity").toInt())/100.0; // default value is 1.0

	QString extension = "";
	QString formatStr = format;
	if( formatStr == "PNG" || formatStr == "png") {
		format = "PNG";  extension = ".png";
	}
	if( formatStr == "JPG" || formatStr == "jpg" || formatStr == "JPEG") {
		format = "JPG";  extension = ".jpg";
		background = true; // JPG doesn't support transparency so we have to include the background
	}
	if(filePath.endsWith(extension, Qt::CaseInsensitive)) {
		filePath.chop(4);
	}
	//qDebug() << "format =" << format << "extension = " << extension;

	qDebug() << "Exporting frames from " << frameStart << "to" << frameEnd << "at size " << exportSize;
	for(int currentFrame = frameStart; currentFrame <= frameEnd ; currentFrame++) {
		QImage tempImage(exportSize, QImage::Format_ARGB32_Premultiplied);
		QPainter painter(&tempImage);

		if(currentLayer->type == Layer::CAMERA) {
			QRect viewRect = ((LayerCamera*)currentLayer)->getViewRect();
			QMatrix mapView = Editor::map( viewRect, QRectF(QPointF(0,0), exportSize) );
			mapView = ((LayerCamera*)currentLayer)->getViewAtFrame(currentFrame) * mapView;
			painter.setWorldMatrix(mapView);
		} else {
			painter.setWorldMatrix(view);
		}
		paintImage(painter, currentFrame, background, curveOpacity, antialiasing, gradients);

		QString frameNumberString = QString::number(currentFrame);
		while( frameNumberString.length() < 4) frameNumberString.prepend("0");

		tempImage.save(filePath+frameNumberString+extension, format, quality);
	}
}

void Object::exportX(int frameStart, int frameEnd, QMatrix view, QSize exportSize, QString filePath, bool antialiasing, int gradients) {
	QSettings settings("Pencil","Pencil");
	qreal curveOpacity = (100-settings.value("curveOpacity").toInt())/100.0; // default value is 1.0

	int page;
	page=0;
	for(int j = frameStart; j <= frameEnd; j=j+15) {
		QImage xImg(QSize(2300,3400), QImage::Format_ARGB32_Premultiplied);
		QPainter xPainter(&xImg);
		xPainter.fillRect(0,0,2300,3400,Qt::white);
		int y = j-1;
		for(int i=j;i<15+page*15 && i<=frameEnd;i++) {
			QRect source = QRect(  QPoint(0,0)  , exportSize );
			QRect target = QRect (  QPoint((y%3)*800+30, (y/3)*680+50-page*3400)  , QSize(640,480) );
			QMatrix thumbView = view * Editor::map(source, target);
			xPainter.setWorldMatrix( thumbView );
			xPainter.setClipRegion( thumbView.inverted().map( QRegion(target) ) );
			paintImage(xPainter, i, false, curveOpacity, antialiasing, gradients);
			xPainter.resetMatrix();
			xPainter.setClipping(false);
			xPainter.setPen( Qt::black );
			xPainter.setFont(QFont("helvetica",50));
			xPainter.drawRect( target );
			xPainter.drawText(QPoint((y%3)*800+35, (y/3)*680+65-page*3400), QString::number(i));
			y++;
		}

		if(filePath.endsWith(".jpg", Qt::CaseInsensitive)) {
			filePath = filePath.remove(".jpg", Qt::CaseInsensitive);
		}
		xImg.save(filePath+QString::number(page)+".jpg", "JPG", 60);
		page++;
	}
}

void Object::exportIm(int frameStart, int frameEnd, QMatrix view, QSize exportSize, QString filePath, bool antialiasing, int gradients) {
    QSettings settings("Pencil","Pencil");
	qreal curveOpacity = (100-settings.value("curveOpacity").toInt())/100.0;
	QImage exported(exportSize, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&exported);
    painter.fillRect(exported.rect(), Qt::white);
    painter.setWorldMatrix(view);
    paintImage(painter, frameStart, false, curveOpacity, antialiasing, gradients);
    exported.save(filePath);
}

void Object::exportFlash(int startFrame, int endFrame, QMatrix view, QSize exportSize, QString filePath, int fps, int compression) {
	if(!filePath.endsWith(".swf", Qt::CaseInsensitive)) {
		filePath = filePath + ".swf";
	}
	// ************* Requires the MING Library ***************
	Flash::exportFlash(this, startFrame, endFrame, view, exportSize, filePath, fps, compression);
	// **********************************************
}

void Object::imageCheck(int frameNumber) {
	bool noImage = true;
	for(int i=0; i< layer.size() && noImage; i++) {
		if(layer[i]->type == Layer::BITMAP || layer[i]->type == Layer::VECTOR) {
			if( ((LayerImage*)layer[i])->getIndexAtFrame( frameNumber ) != -1) noImage = false;
		}
	}
	if(noImage) emit imageRemoved(frameNumber);
}
