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

Status LayerSound::loadSoundClipAtFrame( const QString& sSoundClipName,
                                     const QString& strFilePath,
                                     int frameNumber )
{
    if ( !QFile::exists( strFilePath ) )
    {
        return Status::FILE_NOT_FOUND;
    }
    
    QFileInfo info( strFilePath );
    if ( !info.isFile() )
    {
        return Status::ERROR_LOAD_SOUND_FILE;
    }

    SoundClip* clip = new SoundClip;
    clip->setSoundClipName( sSoundClipName );
    clip->init( strFilePath );
    clip->setPos( frameNumber );
    loadKey( clip );
    return Status::OK;
}

void LayerSound::updateFrameLengths(int fps)
{
    foreachKeyFrame( [&fps] (KeyFrame* pKeyFrame)
    {
        auto soundClip = dynamic_cast<SoundClip *>(pKeyFrame);
        soundClip->updateLength(fps);
    } );
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
        SoundClip* clip = static_cast<SoundClip*>(pKeyFrame);

        QDomElement imageTag = doc.createElement( "sound" );
        imageTag.setAttribute( "frame", clip->pos() );
        imageTag.setAttribute( "name", clip->soundClipName() );

        QFileInfo info( clip->fileName() );
        //qDebug() << "Save=" << info.fileName();
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
            const QString soundFile = soundElement.attribute( "src" );
            const QString sSoundClipName = soundElement.attribute( "name", "My Sound Clip" );

            // the file is supposed to be in the data directory
            const QString sFullPath = QDir( dataDirPath ).filePath( soundFile );

            int position = soundElement.attribute( "frame" ).toInt();
            Status st = loadSoundClipAtFrame( sSoundClipName, sFullPath, position );
            Q_ASSERT( st.ok() );
        }
        
        soundTag = soundTag.nextSibling();
    }
}

Status LayerSound::saveKeyFrame( KeyFrame*, QString path )
{
    Q_UNUSED(path)

    return Status::OK;
}

SoundClip* LayerSound::getSoundClipWhichCovers( int frameNumber )
{
    KeyFrame* key = getKeyFrameWhichCovers( frameNumber );
    return static_cast< SoundClip* >( key );
}
