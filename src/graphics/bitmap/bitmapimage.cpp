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
#include "bitmapimage.h"
#include "blur.h"
#include "object.h"
#include <math.h>

BitmapImage::BitmapImage() {
	// nothing
	image = NULL; extendable = true;
}

BitmapImage::BitmapImage(Object *parent) {
	myParent = parent;
	image = new QImage(0, 0, QImage::Format_ARGB32_Premultiplied);
	boundaries = QRect(0,0,0,0);
	extendable = true;
}

BitmapImage::BitmapImage(Object *parent, QRect rectangle, QColor colour) {
	myParent = parent;
	boundaries = rectangle;
	image = new QImage( boundaries.size(), QImage::Format_ARGB32_Premultiplied);
	image->fill(colour.rgba());
	extendable = true;
}

BitmapImage::BitmapImage(Object *parent, QRect rectangle, QImage image) {
	myParent = parent;
	boundaries = rectangle.normalized();
	extendable = true;
	this->image = new QImage(image);
	if(this->image->width() != rectangle.width() || this->image->height() != rectangle.height()) qDebug() << "Error instancing bitmapImage.";
}

/*BitmapImage::BitmapImage(Object *parent, QImage image, QPoint topLeft) {
	myParent = parent;
	this->image = new QImage(image);
	boundaries =  QRect( topLeft, image.size() );
}*/

BitmapImage::BitmapImage(const BitmapImage &a) {
  myParent=a.myParent;
  boundaries=a.boundaries;
  image=new QImage(*a.image);
	extendable = true;
}

BitmapImage::BitmapImage(Object *parent, QString path, QPoint topLeft) {
	myParent = parent;
	image = new QImage(path);
	if (image->isNull()) qDebug() << "ERROR: Image " << path << " not loaded";
	boundaries = QRect( topLeft, image->size() );
	extendable = true;
}

BitmapImage::~BitmapImage() {
	if(image) delete image;
}

BitmapImage &BitmapImage::operator=(const BitmapImage &a) {
  myParent=a.myParent;
  boundaries=a.boundaries;
  image=new QImage(*a.image);
  return *this;
}

QDomElement BitmapImage::createDomElement(QDomDocument &doc) {
	return QDomElement();  // empty
}

void BitmapImage::loadDomElement(QDomElement imageElement, QString filePath) {
	QString path =  filePath +".data/" + imageElement.attribute("src"); // the file is supposed to be in the data irectory
	QFileInfo fi(path);
	if(!fi.exists()) path = imageElement.attribute("src");
	int x = imageElement.attribute("topLeftX").toInt();
	int y = imageElement.attribute("topLeftY").toInt();
	//loadImageAtFrame( path, position );
	image = new QImage(path);
	if( !image->isNull() ) {
		boundaries = QRect( QPoint(x, y), image->size() );
	}
}



void BitmapImage::modification() {
}

bool BitmapImage::isModified() {
	return false;
}

void BitmapImage::setModified(bool) {
}

void BitmapImage::paintImage(QPainter &painter) {
	painter.drawImage(topLeft(), *image);
}

void outputImage(QImage* image, QSize size, QMatrix myView) {
}

BitmapImage BitmapImage::copy() {
	return BitmapImage(myParent, boundaries, QImage(*image));
}

BitmapImage BitmapImage::copy(QRect rectangle) {
	//QRect intersection = boundaries.intersected( rectangle );
	QRect intersection2  = rectangle.translated( -topLeft() );
	BitmapImage result = BitmapImage(myParent, rectangle, image->copy(intersection2));
	return result;
}

void BitmapImage::paste(BitmapImage* bitmapImage) {
	paste(bitmapImage, QPainter::CompositionMode_SourceOver);
}

void BitmapImage::paste(BitmapImage* bitmapImage, QPainter::CompositionMode cm) {
	QImage* image2 = bitmapImage->image;
	QRect newBoundaries;
	if( image->width() == 0 || image->height() == 0 ) {
		newBoundaries = bitmapImage->boundaries;
	} else {
		newBoundaries = boundaries.united( bitmapImage->boundaries );
	}
	extend( newBoundaries );
	QPainter painter(image);
	painter.setCompositionMode(cm);
	painter.drawImage( bitmapImage->boundaries.topLeft() - boundaries.topLeft(), *image2);
	painter.end();
}

void BitmapImage::add(BitmapImage* bitmapImage) {
	QImage* image2 = bitmapImage->image;
	QRect newBoundaries;
	if( image->width() == 0 || image->height() == 0 ) {
		newBoundaries = bitmapImage->boundaries;
	} else {
		newBoundaries = boundaries.united( bitmapImage->boundaries );
	}
	extend( newBoundaries );
	QPoint offset = bitmapImage->boundaries.topLeft() - boundaries.topLeft();
	for(int y=0; y<image2->height(); y++) {
		for(int x=0; x<image2->width(); x++) {
			/*QRgb p2 = image2->pixel(x,y);
			int r2 = qRed(p2);
			int g2 = qGreen(p2);
			int b2 = qBlue(p2);
			int a2 = qAlpha(p2);
			int r, g, b, a;
			r=0; g=0; b=0; a=0;
			for(int u=0; u<1; u++) {
				for(int v=0; v<1;v++) {
				  if(boundaries.contains(  bitmapImage->boundaries.topLeft() + QPoint(x+u,y+v) )) {
						QRgb p1  = image->pixel(offset.x()+x+u,offset.y()+y+v);
						int r1 = qRed(p1);
						int g1 = qGreen(p1);
						int b1 = qBlue(p1);
						int a1 = qAlpha(p1);
						r = r + r1;
						g = g + g1;
						b = b + b1;
						a = a + a1;
					}
				}
			}
			r = r/1;
			g = g/1;
			b = b/1;
			a = a/1;
			//r = 255;
			//g = 0;
			//b = 0;
			a = 255;
					QRgb p1  = image->pixel(offset.x()+x,offset.y()+y);
					int r1 = qRed(p1);
					int g1 = qGreen(p1);
					int b1 = qBlue(p1);
					int a1 = qAlpha(p1);
			r = (r1*(255-r2) + r2*r)/255;
			g = (g1*(255-g2) + g2*g)/255;
			b = (b1*(255-b2) + b2*b)/255;
			a = (a1*(255-a2) + a2*a)/255;*/
			QRgb p1  = image->pixel(offset.x()+x,offset.y()+y);
			QRgb p2 = image2->pixel(x,y);

			int a1 = qAlpha(p1); int a2 = qAlpha(p2);
			int r1 = qRed(p1);   int r2 = qRed(p2); // remember that the bitmap format is RGB32 Premultiplied
			int g1 = qGreen(p1); int g2 = qGreen(p2);
			int b1 = qBlue(p1);  int b2 = qBlue(p2);

			/*qreal a1 = qAlpha(p1); qreal a2 = qAlpha(p2);
			qreal r1 = qRed(p1);   qreal r2 = qRed(p2); // remember that the bitmap format is RGB32 Premultiplied
			qreal g1 = qGreen(p1); qreal g2 = qGreen(p2);
			qreal b1 = qBlue(p1);  qreal b2 = qBlue(p2);*/

			// unite
			int a = qMax(a1, a2);
			int r = qMax(r1, r2);
			int g = qMax(g1, g2);
			int b = qMax(b1, b2);

			// blend
			/*int a = a2 + a1*(255-a2)/255;
			int r = r2 + r1*(255-a2)/255;
			int g = g2 + g1*(255-a2)/255;
			int b = b2 + b1*(255-a2)/255;*/

			// source
			/*int a = a2;
			int r = r2;
			int g = g2;
			int b = b2;*/

			/*int a = qRound(a1+a2);
			int r = qRound((a1+a2)*((r1+0.)/a1+(r2+0.)/a2)/1);
			int g = qRound((a1+a2)*((g1+0.)/a1+(g2+0.)/a2)/1);
			int b = qRound((a1+a2)*((b1+0.)/a1+(b2+0.)/a2)/1);*/

			// add
			/*int a = qMin(255, qRound(1.0*(a1+a2)));
			int r = qMin(255, qRound(0.5*(r1+r2)));
			int g = qMin(255, qRound(0.5*(g1+g2)));
			int b = qMin(255, qRound(0.5*(b1+b2)));*/

			/*int a = qMin(255, qRound((1.0*a1+0.32*a2)));
			int r = qMin(255, qRound((1.0*r1+0.32*r2)));
			int g = qMin(255, qRound((1.0*g1+0.32*g2)));
			int b = qMin(255, qRound((1.0*b1+0.32*b2)));*/

			QRgb mix = qRgba(r, g, b, a);
			/*qDebug() << "------";
			qDebug() << r1 << g1 << b1 << a1;
			qDebug() << r2 << g2 << b2 << a2;
			qDebug() << r << g << b << a;
			qDebug() << qRed(mix) << qGreen(mix) << qBlue(mix) << qAlpha(mix);*/
			//QRgb mix = qRgba(r2, g2, b2, a);
			if(a2 != 0)
				image->setPixel(offset.x()+x,offset.y()+y, mix);
		}
	}
}

void BitmapImage::moveTopLeft(QPoint point) {
	boundaries.moveTopLeft(point);
}

void BitmapImage::transform(QRect newBoundaries, bool smoothTransform) {
	if(boundaries != newBoundaries) {
		boundaries = newBoundaries;
		newBoundaries.moveTopLeft( QPoint(0,0) );
		QImage* newImage = new QImage( boundaries.size(), QImage::Format_ARGB32_Premultiplied);
		//newImage->fill(QColor(255,255,255).rgb());
		QPainter painter(newImage);
		painter.setRenderHint(QPainter::SmoothPixmapTransform, smoothTransform);
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		painter.fillRect( newImage->rect(), QColor(0,0,0,0) );
		painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
		painter.drawImage(newBoundaries, *image );
		painter.end();
		//if(image != NULL) delete image;
		image = newImage;
	}
}

BitmapImage BitmapImage::transformed(QRect newBoundaries, bool smoothTransform) {
	BitmapImage transformedImage(NULL, newBoundaries, QColor(0,0,0,0));
	QPainter painter(transformedImage.image);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, smoothTransform);
	newBoundaries.moveTopLeft( QPoint(0,0) );
	painter.drawImage(newBoundaries, *image );
	painter.end();
	return transformedImage;
}


void BitmapImage::extend(QPoint P) {
	if(boundaries.contains( P )) {
		// nothing
	} else {
		extend( QRect(P, QSize(0,0)) ); // can we set QSize(0,0) ?
	}
}

void BitmapImage::extend(QRect rectangle) {
	if(!extendable) return;
	if(rectangle.width() <= 0) rectangle.setWidth(1);
	if(rectangle.height() <= 0) rectangle.setHeight(1);
	if(boundaries.contains( rectangle )) {
		// nothing
	} else {
		QRect newBoundaries = boundaries.united(rectangle).normalized();
		QImage* newImage = new QImage( newBoundaries.size(), QImage::Format_ARGB32_Premultiplied);
		newImage->fill(qRgba(0,0,0,0));
		if(!newImage->isNull()) {
			QPainter painter(newImage);
			painter.drawImage(boundaries.topLeft() - newBoundaries.topLeft(), *image);
			painter.end();
		}
		if(image != NULL) delete image;
		image = newImage;
		boundaries = newBoundaries;
	}
}

QRgb BitmapImage::pixel(int x, int y) {
	return pixel( QPoint(x,y) );
}

QRgb BitmapImage::pixel(QPoint P) {
	QRgb result = qRgba(0,0,0,0); // black
	if( boundaries.contains( P ) ) result = image->pixel(P - topLeft());
	return result;
}

void BitmapImage::setPixel(int x, int y, QRgb colour) {
	setPixel( QPoint(x,y), colour);
}

void BitmapImage::setPixel(QPoint P, QRgb colour) {
	extend( P );
	if( boundaries.contains(P) ) image->setPixel(P-topLeft(), colour);
	//drawLine( QPointF(P), QPointF(P), QPen(QColor(colour)), QPainter::CompositionMode_SourceOver, false);
}


void BitmapImage::drawLine( QPointF P1, QPointF P2, QPen pen, QPainter::CompositionMode cm, bool antialiasing) {
	int width = 2+pen.width();
	extend( QRect(P1.toPoint(), P2.toPoint()).normalized().adjusted(-width,-width,width,width) );
	if(image != NULL && !image->isNull() ) {
		QPainter painter(image);
		painter.setCompositionMode(cm);
		painter.setRenderHint(QPainter::Antialiasing, antialiasing);
		painter.setPen(pen);
		painter.drawLine( P1-topLeft(), P2-topLeft());
		painter.end();
	}
}

void BitmapImage::drawRect( QRectF rectangle, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing) {
	int width = pen.width();
	extend( rectangle.adjusted(-width,-width,width,width).toRect() );
	if(brush.style() == Qt::RadialGradientPattern) {
		QRadialGradient* gradient = (QRadialGradient*)brush.gradient();
		gradient->setCenter( gradient->center() - topLeft() );
		gradient->setFocalPoint( gradient->focalPoint() - topLeft() );
	}
	if(image != NULL && !image->isNull() ) {
		QPainter painter(image);
		painter.setCompositionMode(cm);
		painter.setRenderHint(QPainter::Antialiasing, antialiasing);
		painter.setPen(pen);
		painter.setBrush(brush);
		//painter.fillRect( rectangle.translated(-topLeft()), brush );
		//painter.fillRect( rectangle.translated(-topLeft()), QColor(255,0,0) );
		painter.drawRect( rectangle.translated(-topLeft()) );
		painter.end();
	}
}

void BitmapImage::drawEllipse( QRectF rectangle, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing) {
	int width = pen.width();
	extend( rectangle.adjusted(-width,-width,width,width).toRect() );
	if(image != NULL && !image->isNull() ) {
		QPainter painter(image);
		painter.setCompositionMode(cm);
		painter.setRenderHint(QPainter::Antialiasing, antialiasing);
		painter.setPen(pen);
		painter.setBrush(brush);
		//if(brush == Qt::NoBrush)
		painter.drawEllipse( rectangle.translated(-topLeft()) );
		painter.end();
	}
}

void BitmapImage::drawPath( QPainterPath path, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing) {
	int width = pen.width();
	extend( path.controlPointRect().adjusted(-width,-width,width,width).toRect() );
	if(image != NULL && !image->isNull() ) {
		QPainter painter(image);
		painter.setCompositionMode(cm);
		painter.setRenderHint(QPainter::Antialiasing, antialiasing);
		painter.setPen(pen);
		painter.setBrush(brush);
		painter.setWorldMatrix(QMatrix().translate(-topLeft().x(), -topLeft().y()));
		painter.setMatrixEnabled(true);
		painter.drawPath( path );
		painter.end();
	}
}

void BitmapImage::blur(qreal radius) {
	if(image == NULL) return;
	int rad = qRound(0.5*radius);
	extend( boundaries.adjusted(-rad, -rad, rad, rad) );
	Blur::fastbluralpha(*image, rad);
}

void BitmapImage::blur2(qreal radius) {
	if(image == NULL) return;
	int rad = qRound(0.5*radius);
	extend( boundaries.adjusted(-rad, -rad, rad, rad) );
	Blur::expblur(*image, rad, 16, 7);
}

void BitmapImage::clear() {
	if(image != NULL) delete image;
	image = new QImage(0, 0, QImage::Format_ARGB32_Premultiplied);
	boundaries = QRect(0,0,0,0);
}

void BitmapImage::clear(QRect rectangle) {
	QRect clearRectangle = boundaries.intersected( rectangle );
	clearRectangle.moveTopLeft( clearRectangle.topLeft() - topLeft() );
	QPainter painter(image);
	painter.setCompositionMode(QPainter::CompositionMode_Clear);
	painter.fillRect( clearRectangle, QColor(0,0,0,0) );
	painter.end();
}

int BitmapImage::sqr(int n) { // square of a number
	return n*n;
}

int BitmapImage::rgbDistance(QRgb rgba1, QRgb rgba2) {
	int result = sqr(qRed(rgba1)-qRed(rgba2)) + sqr(qGreen(rgba1)-qGreen(rgba2)) + sqr(qBlue(rgba1)-qBlue(rgba2)) + sqr(qAlpha(rgba1)-qAlpha(rgba2));
	return result;
}

void BitmapImage::floodFill(BitmapImage* targetImage, BitmapImage* fillImage, QPoint point, QRgb targetColour, QRgb replacementColour, int tolerance, bool extendFillImage) {
	QList<QPoint> queue; // queue all the pixels of the filled area (as they are found)
	int j, k; bool condition;
	BitmapImage* replaceImage;
	if(extendFillImage) {
		replaceImage = new BitmapImage(NULL, targetImage->boundaries.united(fillImage->boundaries), QColor(0,0,0,0));
	} else {
		targetImage->extend(fillImage->boundaries); // not necessary - here just to prevent some bug when we draw outside the targetImage - to be fixed
		replaceImage = new BitmapImage(NULL, fillImage->boundaries, QColor(0,0,0,0));
		replaceImage->extendable = false;
	}
	//QPainter painter1(replaceImage->image);
	//QPainter painter2(fillImage->image);
	//painter1.setPen( QColor(replacementColour) );
	QPen myPen;
	myPen = QPen( QColor(replacementColour) , 1.0, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin);

	targetColour = targetImage->pixel(point.x(), point.y());
	//if(  rgbDistance(targetImage->pixel(point.x(), point.y()), targetColour) > tolerance ) return;
	queue.append( point );
	// ----- flood fill
	// ----- from the standard flood fill algorithm
	// ----- http://en.wikipedia.org/wiki/Flood_fill
	j = -1; k = 1;
	for(int i=0; i< queue.size(); i++ ) {
		point = queue.at(i);
		if(  replaceImage->pixel(point.x(), point.y()) != replacementColour  && rgbDistance(targetImage->pixel(point.x(), point.y()), targetColour) < tolerance ) {
			j = -1; condition =  (point.x() + j > targetImage->left());
			if(!extendFillImage) condition = condition && (point.x() + j > replaceImage->left());
			while( replaceImage->pixel(point.x()+j, point.y()) != replacementColour  && rgbDistance(targetImage->pixel( point.x()+j, point.y() ), targetColour) < tolerance && condition) {
				j = j - 1;
				condition =  (point.x() + j > targetImage->left());
				if(!extendFillImage) condition = condition && (point.x() + j > replaceImage->left());
			}

			k = 1; condition = ( point.x() + k < targetImage->right()-1);
			if(!extendFillImage) condition = condition && (point.x() + k < replaceImage->right()-1);
			while( replaceImage->pixel(point.x()+k, point.y()) != replacementColour  && rgbDistance(targetImage->pixel( point.x()+k, point.y() ), targetColour) < tolerance && condition) {
				k = k + 1;
				condition = ( point.x() + k < targetImage->right()-1);
				if(!extendFillImage) condition = condition && (point.x() + k < replaceImage->right()-1);
			}

			//painter1.drawLine( point.x()+j, point.y(), point.x()+k+1, point.y() );

			replaceImage->drawLine( QPointF(point.x()+j, point.y()), QPointF(point.x()+k, point.y()), myPen, QPainter::CompositionMode_SourceOver, false);
			//for(int l=0; l<=k-j+1 ; l++) {
			//	replaceImage->setPixel( point.x()+j, point.y(), replacementColour );
			//}

			for(int x = j+1; x < k; x++) {
				//replaceImage->setPixel( point.x()+x, point.y(), replacementColour);
				condition = point.y() - 1 > targetImage->top();
				if(!extendFillImage) condition = condition && (point.y() - 1 > replaceImage->top());
				if( condition && queue.size() < targetImage->height() * targetImage->width() ) {
					if( replaceImage->pixel(point.x()+x, point.y()-1) != replacementColour) {
						if(rgbDistance(targetImage->pixel( point.x()+x, point.y() - 1), targetColour) < tolerance) {
							queue.append( point + QPoint(x,-1) );
						} else {
							replaceImage->setPixel( point.x()+x, point.y()-1, replacementColour);
						}
					}
				}
				condition = point.y() + 1 < targetImage->bottom();
				if(!extendFillImage) condition = condition && (point.y() + 1 < replaceImage->bottom());
				if( condition && queue.size() < targetImage->height() * targetImage->width() ) {
					if( replaceImage->pixel(point.x()+x, point.y()+1) != replacementColour) {
						if(rgbDistance(targetImage->pixel( point.x()+x, point.y() + 1), targetColour) < tolerance) {
							queue.append( point + QPoint(x, 1) );
						} else {
							replaceImage->setPixel( point.x()+x, point.y()+1, replacementColour);
						}
					}
				}
			}
		}
	}		
	//painter2.drawImage( QPoint(0,0), *replaceImage );
	//bool memo = fillImage->extendable;
	//fillImage->extendable = false;
	fillImage->paste(replaceImage);
	//fillImage->extendable = memo;
	//replaceImage->fill(qRgba(0,0,0,0));
	//painter1.end();
	//painter2.end();
	delete replaceImage;
	//update();
}

