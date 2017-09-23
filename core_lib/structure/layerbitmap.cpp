/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

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
}

LayerBitmap::~LayerBitmap()
{
}

BitmapImage* LayerBitmap::getBitmapImageAtFrame( int frameNumber )
{
    Q_ASSERT( frameNumber >= 1 );
    return static_cast< BitmapImage* >( getKeyFrameAt( frameNumber ) );
}

BitmapImage* LayerBitmap::getLastBitmapImageAtFrame( int frameNumber, int increment )
{
    Q_ASSERT( frameNumber >= 1 );
    return static_cast< BitmapImage* >( getLastKeyFrameAtPosition( frameNumber + increment ) );
}

void LayerBitmap::loadImageAtFrame( QString path, QPoint topLeft, int frameNumber )
{
    BitmapImage* pKeyFrame = new BitmapImage( path, topLeft );
    pKeyFrame->setPos( frameNumber );
    loadKey( pKeyFrame );
}

Status LayerBitmap::saveKeyFrame( KeyFrame* pKeyFrame, QString path )
{
    QStringList debugInfo = QStringList() << "LayerBitmap::saveKeyFrame" << QString( "pKeyFrame.pos() = %1" ).arg( pKeyFrame->pos() ) << QString( "path = %1" ).arg( path );
    BitmapImage* pBitmapImage = static_cast< BitmapImage* >( pKeyFrame );

    QString theFileName = fileName( pKeyFrame->pos() );
    QString strFilePath = QDir( path ).filePath( theFileName );
    debugInfo << QString( "strFilePath = " ).arg( strFilePath );
    if ( !pBitmapImage->image()->save( strFilePath ) && !pBitmapImage->image()->isNull() )
    {
        return Status( Status::FAIL, debugInfo << QString( "pBitmapImage could not be saved" ) );
    }

    return Status::OK;
}

QString LayerBitmap::fileName( int frame )
{
    QString layerNumberString = QString::number( id() );
    QString frameNumberString = QString::number( frame );
    while ( layerNumberString.length() < 3 ) layerNumberString.prepend( "0" );
    while ( frameNumberString.length() < 3 ) frameNumberString.prepend( "0" );
    return layerNumberString + "." + frameNumberString + ".png";
}

QDomElement LayerBitmap::createDomElement( QDomDocument& doc )
{
    QDomElement layerTag = doc.createElement( "layer" );
    layerTag.setAttribute( "id", id() );
    layerTag.setAttribute( "name", mName );
    layerTag.setAttribute( "visibility", mVisible );
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
        int id = element.attribute( "id" ).toInt();
        setId( id );
    }
    mName = element.attribute( "name" );
    mVisible = ( element.attribute( "visibility" ).toInt() == 1 );

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
