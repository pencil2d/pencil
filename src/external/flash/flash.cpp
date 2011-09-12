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
#include <QtCore>
#include <QtGui>
#include <QtDebug>

#include "flash.h"

#include "object.h"
#include "beziercurve.h"
#include "colourref.h"


void Flash::exportFlash(Object* object, int startFrame, int endFrame, QMatrix view, QSize exportSize, QString filePath, int fps, int compression) {
	qDebug() << "------Flash------" << compression;

	// ************* Requires the MING Library ***************
	Ming_init();

  SWFMovie *movie = new SWFMovie();

	movie->setDimension(exportSize.width(), exportSize.height());

	movie->setRate(fps); // 12 frames per seconds

  //SWFShape *shape = new SWFShape();
        SWFSprite *objectSprite = new SWFSprite();

	for(int i=0; i < object->getLayerCount(); i++) {
		Layer* layer = object->getLayer(i);
		if(layer->visible) {
			// paints the bitmap images
			if(layer->type == Layer::BITMAP) {
				LayerBitmap* layerBitmap = (LayerBitmap*)layer;
                                SWFSprite *layerSprite = new SWFSprite();
				SWFDisplayItem *previousItem = NULL;
				for(int frameNumber = startFrame; frameNumber <= endFrame; frameNumber++) {
					BitmapImage* bitmapImage = layerBitmap->getBitmapImageAtFrame(frameNumber);
					if(bitmapImage != NULL) {
						if(previousItem != NULL) layerSprite->remove( previousItem );
                                                SWFSprite *imageSprite = new SWFSprite();
						convertToSWFSprite( bitmapImage, object, view, imageSprite);
						previousItem = layerSprite->add( imageSprite );
					}
					layerSprite->nextFrame();
				}
					//layerSprite->add( new SWFAction("stop();") );
					//layerSprite->nextFrame();
				objectSprite->add(layerSprite);
			}
			// paints the vector images
			if(layer->type == Layer::VECTOR) {
				LayerVector* layerVector = (LayerVector*)layer;
                                SWFSprite *layerSprite = new SWFSprite();
				SWFDisplayItem *previousItem = NULL;
				for(int frameNumber = startFrame; frameNumber <= endFrame; frameNumber++) {
					VectorImage* vectorImage = layerVector->getVectorImageAtFrame(frameNumber);
					if(vectorImage != NULL) {
						if(previousItem != NULL) layerSprite->remove( previousItem );
                                                SWFSprite *sprite = new SWFSprite();
						convertToSWFSprite( vectorImage, object, view, sprite);
						previousItem = layerSprite->add( sprite );
					}
					layerSprite->nextFrame();
				}
				//layerSprite->add( new SWFAction("stop();") );
				//layerSprite->nextFrame();

				objectSprite->add(layerSprite);
			}
		}
	}

	objectSprite->nextFrame();
	//objectSprite->add( new SWFAction("stop();") );
	//objectSprite->nextFrame();

	movie->add(objectSprite);
	movie->nextFrame();
	movie->add( new SWFAction("gotoFrame(0);") );
	movie->nextFrame();
//	int b;
	//movie->soundobject=new SWFSound(fopen("foo.mp3","rb"),b);

	QByteArray byteArray(filePath.toLatin1()); // is there any problem with accented characters?
  movie->save(byteArray.data(), compression);

	qDebug() << "done.";
}


void Flash::convertToSWFSprite( BitmapImage* bitmapImage, Object* object, QMatrix view, SWFSprite* sprite ) {
	QString tempPath = QDir::tempPath()+"/penciltemp.png";
	QByteArray tempPath2( tempPath.toLatin1());
	bitmapImage->image->save( tempPath , "PNG");
	SWFShape *shape = new SWFShape();
	SWFFill* fill = shape->addBitmapFill( new SWFBitmap( tempPath2.data() ) );
	fill->moveTo(static_cast<float>(bitmapImage->topLeft().x()), static_cast<float>(bitmapImage->topLeft().y()));
	shape->setRightFill(fill);
		shape->movePenTo(bitmapImage->left(), bitmapImage->top());
		shape->drawLineTo(bitmapImage->right(), bitmapImage->top());
		shape->drawLineTo(bitmapImage->right(), bitmapImage->bottom());
		shape->drawLineTo(bitmapImage->left(), bitmapImage->bottom());
		shape->drawLineTo(bitmapImage->left(), bitmapImage->top());
	SWFDisplayItem *item = sprite->add( shape );
	//item->setMatrix( view.m11(), view.m12(), view.m21(), view.m22(), view.dx(), view.dy() ); // this C++ method is not defined in mingpp.h version 0.3
	SWFDisplayItem_setMatrix( item->item, view.m11(), view.m12(), view.m21(), view.m22(), view.dx(), view.dy() ); // we use the C function instead
	sprite->nextFrame();
}


void Flash::convertToSWFSprite( VectorImage* vectorImage, Object* object, QMatrix view, SWFSprite* sprite ) {

	// add filled areas
	for(int i=0; i< vectorImage->area.size(); i++) {
		QColor colour = object->getColour(vectorImage->area[i].getColourNumber()).colour;
		QPainterPath path = view.map( vectorImage->area[i].path );
		addShape(sprite, path, colour, colour, 0, true);
	}
	// add curves
	for(int i=0; i< vectorImage->curve.size(); i++) {
		if(!vectorImage->curve[i].isInvisible()) {
			QColor colour = object->getColour(vectorImage->curve[i].getColourNumber()).colour;
			if(vectorImage->curve[i].getVariableWidth()) {
				QPainterPath path = view.map( vectorImage->curve[i].getStrokedPath() );
				addShape(sprite, path, colour, colour, 0, true);
			} else {
				QPainterPath path = view.map( vectorImage->curve[i].getSimplePath() );
				qreal width = vectorImage->curve[i].getWidth();
				addShape(sprite, path, colour, colour, width, false);
			}
		}
	}
	sprite->nextFrame();
}




void Flash::addShape( SWFSprite* sprite, QPainterPath path, QColor fillColour, QColor borderColour, qreal width, bool fill ) {
	SWFShape* shape = new SWFShape();
	//float widthf = static_cast< float >(width);
	if(width == 0.0) {
		shape->setLine( static_cast<unsigned short>(width), borderColour.red(), borderColour.green(), borderColour.blue(), 0);
	} else {
		shape->setLine(static_cast<unsigned short>(width), borderColour.red(), borderColour.green(), borderColour.blue() );
	}
	if(fill) {
		SWFFill* fill = shape->addSolidFill( fillColour.red(), fillColour.green(), fillColour.blue() );
		shape->setRightFill(fill);
	}

	qreal memoP0x = 0.0;
	qreal memoP0y = 0.0;
	qreal memoP1x = 0.0;
	qreal memoP1y = 0.0;
	qreal memoP2x = 0.0;
	qreal memoP2y = 0.0;
	qreal memoP3x = 0.0;
	qreal memoP3y = 0.0;
	qreal memo = 0.0;
	for(int i=0; i < path.elementCount() ; i++) {
		QPainterPath::Element element = path.elementAt(i);
		if( element.type == QPainterPath::CurveToDataElement ) {
			memo++;
			if(memo == 1) {
				memoP2x = element.x;
				memoP2y = element.y;
			}
			if(memo == 2) {
				memoP3x = element.x;
				memoP3y = element.y;
			}
		}
		if( (element.type != QPainterPath::CurveToDataElement) || ((element.type == QPainterPath::CurveToDataElement) && (i==path.elementCount()-1)) ) {
			if(memo == 1) {
				shape->drawCurveTo( static_cast<float>(memoP1x), static_cast<float>(memoP1y), static_cast<float>(memoP2x), static_cast<float>(memoP2y) );
				//---
				memo = 0;
				memoP0x = memoP2x;
				memoP0y = memoP2y;
			}
			if(memo == 2) {
				// now we need to approximate a cubic Bezier curve by 4 quadratic Bezier curves (Flash API uses only quadratic Bezier curves)
				// we use the fixed midpoint method
				// Excellent explanations here: http://timotheegroleau.com/Flash/articles/cubic_bezier_in_flash.htm
				QPointF P0(memoP0x, memoP0y);
				QPointF P1(memoP1x, memoP1y);
				QPointF P2(memoP2x, memoP2y);
				QPointF P3(memoP3x, memoP3y);

				QPointF M2 = (P0+3*P1+3*P2+P3)/8;
				QPointF C1 = (5*P0+3*P1)/8;
				QPointF C4 = (5*P3+3*P2)/8;
				QPointF C2 = (3*(P0+2*P1+P2)/4+M2)/4;
				QPointF C3 = (3*(P1+2*P2+P3)/4+M2)/4;
				QPointF M1 = 0.5*(C1+C2);
				QPointF M3 = 0.5*(C3+C4);
				shape->drawCurveTo( static_cast<float>(C1.x()), static_cast<float>(C1.y()), static_cast<float>(M1.x()), static_cast<float>(M1.y()) );
				shape->drawCurveTo( static_cast<float>(C2.x()), static_cast<float>(C2.y()), static_cast<float>(M2.x()), static_cast<float>(M2.y()) );
				shape->drawCurveTo( static_cast<float>(C3.x()), static_cast<float>(C3.y()), static_cast<float>(M3.x()), static_cast<float>(M3.y()) );
				shape->drawCurveTo( static_cast<float>(C4.x()), static_cast<float>(C4.y()), static_cast<float>(P3.x()), static_cast<float>(P3.y()) );
				//---
				memo = 0;
				memoP0x = memoP3x;
				memoP0y = memoP3y;
			}
		}
		if( element.type == QPainterPath::MoveToElement ) {
			shape->movePenTo( static_cast<float>(element.x), static_cast<float>(element.y) );
			memoP0x = element.x;
			memoP0y = element.y;
		}
		if( element.type == QPainterPath::LineToElement ) {
			shape->drawLineTo( static_cast<float>(element.x), static_cast<float>(element.y) );
			memoP0x = element.x;
			memoP0y = element.y;
		}
		if( element.type == QPainterPath::CurveToElement ) {
			memoP1x = element.x;
			memoP1y = element.y;
		}
	}
	sprite->add( shape );
}
