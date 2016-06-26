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
#include <QMediaPlayer>
#include "object.h"
#include "layersound.h"
#include "soundclip.h"


LayerSound::LayerSound( Object* object ) : Layer( object, Layer::SOUND )
{
    mName = QString( tr( "Sound Layer" ) );
}

LayerSound::~LayerSound()
{
}

Status LayerSound::loadSoundAtFrame( QString strFilePath, int frameNumber )
{
    if ( !QFile::exists( strFilePath ) )
    {
        return Status::FILE_NOT_FOUND;
    }
    
    QFileInfo info( strFilePath );
    if ( !info.isFile() )
    {
        strFilePath = "";
    }

    SoundClip* clip = new SoundClip;
    clip->init( strFilePath );
    clip->setPos( frameNumber );
    loadKey( clip );
    return Status::OK;
}

QDomElement LayerSound::createDomElement( QDomDocument& doc )
{
    QDomElement layerTag = doc.createElement( "layer" );
    
    layerTag.setAttribute( "id", id() );
    layerTag.setAttribute( "name", name() );
    layerTag.setAttribute( "visibility", visible() );
    layerTag.setAttribute( "type", type() );

    foreachKeyFrame( [ &doc, &layerTag ]( KeyFrame* pKeyFrame )
    {
        QDomElement imageTag = doc.createElement( "sound" );
        imageTag.setAttribute( "frame", pKeyFrame->pos() );

        QFileInfo info( pKeyFrame->fileName() );
        qDebug() << "Save=" << info.fileName();
        imageTag.setAttribute( "src", info.fileName() );
        layerTag.appendChild( imageTag );
    } );
    
    return layerTag;
}

void LayerSound::loadDomElement( QDomElement element, QString dataDirPath )
{   
    if ( !element.attribute( "id" ).isNull() )
    {
        int myId = element.attribute( "id" ).toInt();
        setId( myId );
    }
    mName = element.attribute( "name" );
    mVisible = ( element.attribute( "visibility" ).toInt() == 1 );

    QDomNode soundTag = element.firstChild();
    while ( !soundTag.isNull() )
    {
        QDomElement soundElement = soundTag.toElement();
        if ( soundElement.isNull() )
        {
            continue;
        }

        if ( soundElement.tagName() == "sound" )
        {
            QString soundFile = soundElement.attribute( "src" );

            // the file is supposed to be in the data directory
            QString fullPath = QDir( dataDirPath ).filePath( soundFile );
            qDebug() << "Load Sound path = " << fullPath;  //added for debugging puproses

            int position = soundElement.attribute( "frame" ).toInt();
            Status st = loadSoundAtFrame( fullPath, position );
            Q_ASSERT( st.ok() );
        }
        
        soundTag = soundTag.nextSibling();
    }
}

bool LayerSound::saveKeyFrame( KeyFrame*, QString path )
{
    return true;
}
