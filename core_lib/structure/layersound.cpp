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


LayerSound::LayerSound( Object* object ) : Layer( object, Layer::SOUND )
{
    mName = QString( tr( "Sound Layer" ) );
}

LayerSound::~LayerSound()
{
}

void LayerSound::paintImages(QPainter& painter,
                             TimeLineCells* cells,
                             int x, int y,
                             int width, int height,
                             bool selected, int frameSize)
{
    Q_UNUSED( cells );
    Q_UNUSED( width );
    Q_UNUSED( selected );

    // TODO: re-write here.
    /*
    for (int i = 0; i < sound.size(); i++)
    {
    qreal h = x + (framesPosition.at(i)-1)*frameSize+2;
    if (framesSelected.at(i))
    {
    painter.setBrush(QColor(60,60,60));
    h = h + frameOffset*frameSize;
    }
    else
    {
    painter.setBrush(QColor(125,125,125));
    }
    QPointF points[3] = { QPointF(h, y+4), QPointF(h, y+height-4), QPointF(h+15, y+0.5*height) };
    painter.drawPolygon( points, 3 );
    painter.drawText(QPoint( h + 20, y+(2*height)/3), framesFilename.at(i) );
    }
    */
}

bool LayerSound::addNewKeyAt( int frameNumber )
{
    return addKeyFrame( frameNumber, new SoundClip );
}

Status LayerSound::loadSoundAtFrame( QString strFilePath, int frameNumber )
{
    if ( !QFile::exists( strFilePath ) )
    {
        return Status::NOT_FOUND;
    }
    
    QMediaPlayer* pPlayer = new QMediaPlayer( this );
    pPlayer->setMedia( QUrl::fromLocalFile( strFilePath ) );

    if ( pPlayer->error() != QMediaPlayer::NoError )
    {
        return Status::ERROR_LOAD_SOUND_FILE;
    }

    

    return Status::OK;
}

bool LayerSound::saveImage( int index, QString path, int layerNumber )
{
    /*
    Q_UNUSED(layerNumber);

    QFile originalFile( soundFilepath.at(index) );
    originalFile.copy( path + "/" + framesFilename.at(index) );
    framesModified[index] = false;
    */
    return true;
}

void LayerSound::playSound( int frame )
{
    if ( keyExists( frame ) )
    {
    }
}



void LayerSound::stopSound()
{
    /*
    for(int i=0; i < sound.size(); i++)
    {
    Q_ASSERT( sound[i] );
    sound[i]->stop();
    }
    */
}


QDomElement LayerSound::createDomElement( QDomDocument& doc )
{
    QDomElement layerTag = doc.createElement( "layer" );
    /*
    layerTag.setAttribute("id",id);
    layerTag.setAttribute("name", name);
    layerTag.setAttribute("visibility", visible);
    layerTag.setAttribute("type", type());
    for (int index=0; index < framesPosition.size() ; index++)
    {
    QDomElement soundTag = doc.createElement("sound");
    soundTag.setAttribute("position", framesPosition.at(index));
    soundTag.setAttribute("src", framesFilename.at(index));
    layerTag.appendChild(soundTag);
    }
    */
    return layerTag;
}

void LayerSound::loadDomElement( QDomElement element, QString dataDirPath )
{
    /*
    if (!element.attribute("id").isNull()) id = element.attribute("id").toInt();
    name = element.attribute("name");
    visible = (element.attribute("visibility") == "1");

    QDomNode soundTag = element.firstChild();
    while (!soundTag.isNull())
    {
    QDomElement soundElement = soundTag.toElement();
    if (!soundElement.isNull())
    {
    if (soundElement.tagName() == "sound")
    {
    QString path = dataDirPath + "/" + soundElement.attribute("src"); // the file is supposed to be in the data directory
    //qDebug() << "LAY_SOUND  dataDirPath=" << dataDirPath << "   ;path=" << path;  //added for debugging puproses
    QFileInfo fi(path);
    if (!fi.exists()) path = soundElement.attribute("src");
    int position = soundElement.attribute("position").toInt();
    loadSoundAtFrame( path, position );
    }
    }
    soundTag = soundTag.nextSibling();
    }
    */
}

bool LayerSound::saveKeyFrame( KeyFrame*, QString path )
{
    return true;
}