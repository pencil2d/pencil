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
#include "keyframe.h"
#include "bitmapimage.h"
#include "layerbitmap.h"


LayerBitmap::LayerBitmap( Object* object ) : Layer( object, Layer::BITMAP )
{
    mName = QString( tr( "Bitmap Layer" ) );
    addNewKeyAt( 1 );
}

LayerBitmap::~LayerBitmap()
{
}

BitmapImage* LayerBitmap::getBitmapImageAtFrame( int frameNumber )
{
    return static_cast< BitmapImage* >( getKeyFrameAtPosition( frameNumber ) );
}

BitmapImage* LayerBitmap::getLastBitmapImageAtFrame( int frameNumber, int increment )
{
    return static_cast< BitmapImage* >( getLastKeyFrameAtPosition( frameNumber + increment ) );
}

bool LayerBitmap::addNewKeyAt( int frameNumber )
{
    if ( frameNumber <= 0 )
    {
        return false;
    }
    return addKeyFrame( frameNumber, new BitmapImage );
}

void LayerBitmap::loadImageAtFrame( QString path, QPoint topLeft, int frameNumber )
{
    BitmapImage* pKeyFrame = new BitmapImage( path, topLeft );
    pKeyFrame->setPos( frameNumber );
    loadKey( pKeyFrame );
}

bool LayerBitmap::saveKeyFrame( KeyFrame* pKeyFrame, QString path )
{
    BitmapImage* pBitmapImage = static_cast< BitmapImage* >( pKeyFrame );

    QString theFileName = fileName( pKeyFrame->pos() );
    QString strFilePath = QDir( path ).filePath( theFileName );
    pBitmapImage->image()->save( strFilePath );

    return true;
}

QString LayerBitmap::fileName( int frame )
{
    int layerID = mId;
    QString layerNumberString = QString::number( layerID );
    QString frameNumberString = QString::number( frame );
    while ( layerNumberString.length() < 3 ) layerNumberString.prepend( "0" );
    while ( frameNumberString.length() < 3 ) frameNumberString.prepend( "0" );
    return layerNumberString + "." + frameNumberString + ".png";
}

QDomElement LayerBitmap::createDomElement( QDomDocument& doc )
{
    QDomElement layerTag = doc.createElement( "layer" );
    layerTag.setAttribute( "id", mId );
    layerTag.setAttribute( "name", mName );
    layerTag.setAttribute( "visibility", visible );
    layerTag.setAttribute( "type", type() );

    foreachKeyFrame( [&]( KeyFrame* pKeyFrame )
    {
        BitmapImage* pImg = static_cast< BitmapImage* >( pKeyFrame );

        QDomElement imageTag = doc.createElement( "image" );
        imageTag.setAttribute( "frame", pKeyFrame->pos() );
        imageTag.setAttribute( "src", fileName( pKeyFrame->pos() ) );
        imageTag.setAttribute( "topLeftX", pImg->topLeft().x() );
        imageTag.setAttribute( "topLeftY", pImg->topLeft().y() );
        layerTag.appendChild( imageTag );
    } );

    return layerTag;
}

void LayerBitmap::loadDomElement( QDomElement element, QString dataDirPath )
{
    if ( !element.attribute( "id" ).isNull() )
    {
        mId = element.attribute( "id" ).toInt();
    }
    mName = element.attribute( "name" );
    visible = ( element.attribute( "visibility" ) == "1" );

    QDomNode imageTag = element.firstChild();
    while ( !imageTag.isNull() )
    {
        QDomElement imageElement = imageTag.toElement();
        if ( !imageElement.isNull() )
        {
            if ( imageElement.tagName() == "image" )
            {
                QString path = dataDirPath + "/" + imageElement.attribute( "src" ); // the file is supposed to be in the data directory
                //qDebug() << "LAY_BITMAP  dataDirPath=" << dataDirPath << "   ;path=" << path;  //added for debugging puproses
                QFileInfo fi( path );
                if ( !fi.exists() ) path = imageElement.attribute( "src" );
                int position = imageElement.attribute( "frame" ).toInt();
                int x = imageElement.attribute( "topLeftX" ).toInt();
                int y = imageElement.attribute( "topLeftY" ).toInt();
                loadImageAtFrame( path, QPoint( x, y ), position );
            }
        }
        imageTag = imageTag.nextSibling();
    }
}
