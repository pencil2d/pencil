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

#include <QDomDocument>
#include <QTextStream>
#include <QMessageBox>
#include <QProgressDialog>

#include "object.h"
#include "layer.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layersound.h"
#include "layercamera.h"

//#include "flash.h"
#include "editor.h"
#include "bitmapimage.h"

// ******* Mac-specific: ******** (please comment (or reimplement) the lines below to compile on Windows or Linux
//#include <CoreFoundation/CoreFoundation.h>
// ******************************

Object::Object( QObject* parent ) : QObject( parent )
{
    modified = false;
    mMirror = false;
}

Object::~Object()
{
    while ( !mLayers.empty() )
    {
        delete mLayers.takeLast();
    }
}

void Object::init()
{
    // default layers
    addNewCameraLayer();//TODO Add Camera Layer at beginning for Quick Preview
    addNewVectorLayer();
    addNewBitmapLayer();

    // default palette
    loadDefaultPalette();
}

QDomElement Object::createDomElement( QDomDocument& doc )
{
    QDomElement tag = doc.createElement( "object" );
    qDebug( "  Create Object Node!" );

    int layerCount = getLayerCount();
    qDebug( "  Total LayerCount = %d", layerCount );
    for ( int i = 0; i < getLayerCount(); i++ )
    {
        Layer* layer = getLayer( i );
        QDomElement layerTag = layer->createDomElement( doc );
        tag.appendChild( layerTag );
        qDebug( "  Append Layer %d", i );
    }
    return tag;
}

bool Object::loadDomElement( QDomElement docElem, QString dataDirPath )
{
    if ( docElem.isNull() )
    {
        return false;
    }
    int layerNumber = -1;
    
    bool someRelevantData = false;
    for ( QDomNode node = docElem.firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        QDomElement element = node.toElement(); // try to convert the node to an element.
        if ( element.tagName() == "layer" )
        {
            someRelevantData = true;
            if ( element.attribute( "type" ).toInt() == Layer::BITMAP )
            {
                addNewBitmapLayer();
                layerNumber++;
                getLayer( layerNumber )->loadDomElement( element, dataDirPath );
            }
            else if ( element.attribute( "type" ).toInt() == Layer::VECTOR )
            {
                addNewVectorLayer();
                layerNumber++;
                getLayer( layerNumber )->loadDomElement( element, dataDirPath );
            }
            else if ( element.attribute( "type" ).toInt() == Layer::SOUND )
            {
                addNewSoundLayer();
                layerNumber++;
                getLayer( layerNumber )->loadDomElement( element, dataDirPath );
            }
            else if ( element.attribute( "type" ).toInt() == Layer::CAMERA )
            {
                addNewCameraLayer();
                layerNumber++;
                getLayer( layerNumber )->loadDomElement( element, dataDirPath );
            }
        }
    }
    return someRelevantData;
}

LayerBitmap* Object::addNewBitmapLayer()
{
    LayerBitmap* layerBitmap = new LayerBitmap( this );
    layerBitmap->mId = 1 + getMaxID();
    mLayers.append( layerBitmap );

    return layerBitmap;
}

LayerVector* Object::addNewVectorLayer()
{
    LayerVector* layerVector = new LayerVector( this );
    layerVector->mId = 1 + getMaxID();
    mLayers.append( layerVector );

    return layerVector;
}

LayerSound* Object::addNewSoundLayer()
{
    LayerSound* layerSound = new LayerSound( this );
    layerSound->mId = 1 + getMaxID();
    mLayers.append( layerSound );
    return layerSound;
}

LayerCamera* Object::addNewCameraLayer()
{
    LayerCamera* layerCamera = new LayerCamera( this );
    layerCamera->mId = 1 + getMaxID();
    mLayers.append( layerCamera );

    return layerCamera;
}

int Object::getMaxID()
{
    int result = 0;
    for ( int i = 0; i< getLayerCount(); i++ )
    {
        Layer* layeri = getLayer( i );
        if ( layeri->mId > result ) result = layeri->mId;
    }
    return result;
}

Layer* Object::getLayer( int i )
{
    if ( i > -1 && i < getLayerCount() )
    {
        return mLayers.at( i );
    }
    else
    {
        return NULL;
    }
}

void Object::moveLayer( int i, int j )
{
    if ( i != j )
    {
        mLayers.insert( j, mLayers.at( i ) );
        if ( i > j )
        {
            mLayers.removeAt( i + 1 );
        }
        else
        {
            mLayers.removeAt( i );
        }
    }
}

void Object::deleteLayer( int i )
{
    if ( i > -1 && i < mLayers.size() )
    {
        //layer.removeAt(i);
        disconnect( mLayers[ i ], 0, this, 0 ); // disconnect the layer from this object
        delete mLayers.takeAt( i );
    }
}

void Object::playSoundIfAny( int frame, int fps )
{
    for ( int i = 0; i < getLayerCount(); i++ )
    {
        Layer* layer = getLayer( i );
        if ( layer->type() == Layer::SOUND )
        {
            ( ( LayerSound* )layer )->playSound( frame, fps );
        }
    }
}

void Object::stopSoundIfAny()
{
    for ( int i = 0; i < getLayerCount(); i++ )
    {
        Layer* layer = getLayer( i );
        if ( layer->type() == Layer::SOUND )
        {
            ( ( LayerSound* )layer )->stopSound();
        }
    }
}

ColourRef Object::getColour( int i )
{
    ColourRef result( Qt::white, "error" );
    if ( i > -1 && i < mPalette.size() )
    {
        result = mPalette.at( i );
    }
    return result;
}

void Object::addColour( QColor colour )
{
    addColour( ColourRef( colour, "Colour " + QString::number( mPalette.size() ) ) );
}

bool Object::removeColour( int index )
{
    for ( int i = 0; i < getLayerCount(); i++ )
    {
        Layer* layer = getLayer( i );
        if ( layer->type() == Layer::VECTOR )
        {
            LayerVector* layerVector = ( ( LayerVector* )layer );
            if ( layerVector->usesColour( index ) ) return false;
        }
    }
    for ( int i = 0; i < getLayerCount(); i++ )
    {
        Layer* layer = getLayer( i );
        if ( layer->type() == Layer::VECTOR )
        {
            LayerVector* layerVector = ( ( LayerVector* )layer );
            layerVector->removeColour( index );
        }
    }
    mPalette.removeAt( index );
    return true;
    // update the vector pictures using that colour !
}

void Object::renameColour( int i, QString text )
{
    mPalette[ i ].name = text;
}

bool Object::savePalette( QString filePath )
{
    return exportPalette( filePath + "/palette.xml" );
}

bool Object::exportPalette( QString filePath )
{
    QFile* file = new QFile( filePath );
    if ( !file->open( QFile::WriteOnly | QFile::Text ) )
    {
        //QMessageBox::warning(this, "Warning", "Cannot write file");
        return false;
    }
    QTextStream out( file );

    QDomDocument doc( "PencilPalette" );
    QDomElement root = doc.createElement( "palette" );
    doc.appendChild( root );
    for ( int i = 0; i < mPalette.size(); i++ )
    {
        QDomElement tag = doc.createElement( "Colour" );
        tag.setAttribute( "name", mPalette.at( i ).name );
        tag.setAttribute( "red", mPalette.at( i ).colour.red() );
        tag.setAttribute( "green", mPalette.at( i ).colour.green() );
        tag.setAttribute( "blue", mPalette.at( i ).colour.blue() );
        root.appendChild( tag );
        //QDomText t = doc.createTextNode( myPalette.at(i).name );
        //tag.appendChild(t);
    }
    //QString xml = doc.toString();

    int IndentSize = 2;
    doc.save( out, IndentSize );
    return true;
}

bool Object::loadPalette( QString filePath )
{
    return importPalette( filePath + "/palette.xml" );
}

bool Object::importPalette( QString filePath )
{
    QFile* file = new QFile( filePath );
    if ( !file->open( QFile::ReadOnly ) )
    {
        //QMessageBox::warning(this, "Warning", "Cannot read file");
        return false;
    }

    QDomDocument doc;
    doc.setContent( file );

    mPalette.clear();
    QDomElement docElem = doc.documentElement();
    QDomNode tag = docElem.firstChild();
    while ( !tag.isNull() )
    {
        QDomElement e = tag.toElement(); // try to convert the node to an element.
        if ( !e.isNull() )
        {
            QString name = e.attribute( "name" );
            int r = e.attribute( "red" ).toInt();
            int g = e.attribute( "green" ).toInt();
            int b = e.attribute( "blue" ).toInt();
            mPalette.append( ColourRef( QColor( r, g, b ), name ) );
            //qDebug() << name << r << g << b << endl; // the node really is an element.
        }
        tag = tag.nextSibling();
    }
    return true;
}


void Object::loadDefaultPalette()
{
    mPalette.clear();
    addColour( ColourRef( QColor( Qt::black ), QString( tr( "Black" ) ) ) );
    addColour( ColourRef( QColor( Qt::red ), QString( tr( "Red" ) ) ) );
    addColour( ColourRef( QColor( Qt::darkRed ), QString( tr( "Dark Red" ) ) ) );
    addColour( ColourRef( QColor( 255, 128, 0 ), QString( tr( "Orange" ) ) ) );
    addColour( ColourRef( QColor( 128, 64, 0 ), QString( tr( "Dark Orange" ) ) ) );
    addColour( ColourRef( QColor( Qt::yellow ), QString( tr( "Yellow" ) ) ) );
    addColour( ColourRef( QColor( Qt::darkYellow ), QString( tr( "Dark Yellow" ) ) ) );
    addColour( ColourRef( QColor( Qt::green ), QString( tr( "Green" ) ) ) );
    addColour( ColourRef( QColor( Qt::darkGreen ), QString( tr( "Dark Green" ) ) ) );
    addColour( ColourRef( QColor( Qt::cyan ), QString( tr( "Cyan" ) ) ) );
    addColour( ColourRef( QColor( Qt::darkCyan ), QString( tr( "Dark Cyan" ) ) ) );
    addColour( ColourRef( QColor( Qt::blue ), QString( tr( "Blue" ) ) ) );
    addColour( ColourRef( QColor( Qt::darkBlue ), QString( tr( "Dark Blue" ) ) ) );
    addColour( ColourRef( QColor( 255, 255, 255 ), QString( tr( "White" ) ) ) );
    addColour( ColourRef( QColor( 220, 220, 229 ), QString( tr( "Very Light Grey" ) ) ) );
    addColour( ColourRef( QColor( Qt::lightGray ), QString( tr( "Light Grey" ) ) ) );
    addColour( ColourRef( QColor( Qt::gray ), QString( tr( "Grey" ) ) ) );
    addColour( ColourRef( QColor( Qt::darkGray ), QString( tr( "Dark Grey" ) ) ) );
    addColour( ColourRef( QColor( 255, 227, 187 ), QString( tr( "Light Skin" ) ) ) );
    addColour( ColourRef( QColor( 221, 196, 161 ), QString( tr( "Light Skin - shade" ) ) ) );
    addColour( ColourRef( QColor( 255, 214, 156 ), QString( tr( "Skin" ) ) ) );
    addColour( ColourRef( QColor( 207, 174, 127 ), QString( tr( "Skin - shade" ) ) ) );
    addColour( ColourRef( QColor( 255, 198, 116 ), QString( tr( "Dark Skin" ) ) ) );
    addColour( ColourRef( QColor( 227, 177, 105 ), QString( tr( "Dark Skin - shade" ) ) ) );
}

void Object::paintImage( QPainter& painter, int frameNumber,
                         bool background,
                         bool antialiasing )
{
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setRenderHint( QPainter::SmoothPixmapTransform, true );

    //painter.setTransform(matrix);
    painter.setCompositionMode( QPainter::CompositionMode_SourceOver );

    // paints the background
    if ( background )
    {
        painter.setPen( Qt::NoPen );
        painter.setBrush( Qt::white );
        painter.setViewTransformEnabled( false );
        painter.drawRect( QRect( 0, 0, painter.device()->width(), painter.device()->height() ) );
        painter.setViewTransformEnabled( true );
    }

    for ( int i = 0; i < getLayerCount(); i++ )
    {
        Layer* layer = getLayer( i );
        if ( layer->visible )
        {
            painter.setOpacity( 1.0 );

            // paints the bitmap images
            if ( layer->type() == Layer::BITMAP )
            {
                LayerBitmap* layerBitmap = ( LayerBitmap* )layer;
                layerBitmap->getLastBitmapImageAtFrame( frameNumber, 0 )->paintImage( painter );
            }
            // paints the vector images
            if ( layer->type() == Layer::VECTOR )
            {
                LayerVector* layerVector = ( LayerVector* )layer;
                layerVector->getLastVectorImageAtFrame( frameNumber, 0 )->paintImage( painter,
                                                                                      false,
                                                                                      false,
                                                                                      antialiasing );
            }
        }
    }
}

bool Object::exportFrames( int frameStart, int frameEnd,
                           Layer* currentLayer,
                           QSize exportSize, QString filePath,
                           const char* format,
                           int quality,
                           bool background,
                           bool antialiasing,
                           QProgressDialog* progress = NULL,
                           int progressMax = 50 )
{
    QSettings settings( "Pencil", "Pencil" );

    QString extension = "";
    QString formatStr = format;
    if ( formatStr == "PNG" || formatStr == "png" )
    {
        format = "PNG";
        extension = ".png";
    }
    if ( formatStr == "JPG" || formatStr == "jpg" || formatStr == "JPEG" || formatStr == "jpeg" )
    {
        format = "JPG";
        extension = ".jpg";
        background = true; // JPG doesn't support transparency so we have to include the background
    }
    if ( filePath.endsWith( extension, Qt::CaseInsensitive ) )
    {
        filePath.chop( extension.size() );
    }
    //qDebug() << "format =" << format << "extension = " << extension;

    qDebug() << "Exporting frames from " << frameStart << "to" << frameEnd << "at size " << exportSize;
    for ( int currentFrame = frameStart; currentFrame <= frameEnd; currentFrame++ )
    {
        if ( progress != NULL ) progress->setValue( ( currentFrame - frameStart )*progressMax / ( frameEnd - frameStart ) );
        QImage tempImage( exportSize, QImage::Format_ARGB32_Premultiplied );
        QPainter painter( &tempImage );

        // Make sure that old frame is erased before exporting a new one
        tempImage.fill( 0x00000000 );

        QRect viewRect = ( ( LayerCamera* )currentLayer )->getViewRect();
        QTransform mapView = Editor::map( viewRect, QRectF( QPointF( 0, 0 ), exportSize ) );
        mapView = ( ( LayerCamera* )currentLayer )->getViewAtFrame( currentFrame ) * mapView;
        painter.setWorldTransform( mapView );

        paintImage( painter, currentFrame, background, antialiasing );

        QString frameNumberString = QString::number( currentFrame );
        while ( frameNumberString.length() < 4 )
        {
            frameNumberString.prepend( "0" );
        }
        tempImage.save( filePath + frameNumberString + extension, format, quality );
    }

    return true;
}


void convertNFrames( int fps, int exportFps, int* frameRepeat, int* frameReminder, int* framePutEvery, int* frameSkipEvery )
{
    /// --- simple conversion ---
    *frameRepeat = exportFps / fps;     // identic frames to export per frame
    *frameReminder = exportFps % fps;   // additional frames to export in an fps cycle (= 1 second)

    /// --- modulo frames and their redistribution in time ---
    if ( *frameReminder == 0 )                            /// frames left = 0 -> no need to add extra frames
    {
        *framePutEvery = 0; *frameSkipEvery = 0;
    }        //  so, frameSkipEvery and framePutEvery will not be used.
    else if ( *frameReminder > ( fps - *frameReminder ) )   /// frames to add > frames to skip -> frameSkipEvery will be used.
    {
        *frameSkipEvery = fps / ( fps - *frameReminder ); *framePutEvery = 0;
    }
    else                                                /// Frames to add < frames to skip -> framePutEvery will be used.
    {
        *framePutEvery = fps / *frameReminder; *frameSkipEvery = 0;
    }
    qDebug() << "-->convertedNFrames";
}



bool Object::exportFrames1( ExportFrames1Parameters par )
{
    int frameStart = par.frameStart;
    int frameEnd = par.frameEnd;
    QTransform view = par.view;
    Layer* currentLayer = par.currentLayer;
    QSize exportSize = par.exportSize;
    QString filePath = par.filePath;
    const char* format = par.format;
    int quality = par.quality;
    bool background = par.background;
    bool antialiasing = par.antialiasing;
    QProgressDialog* progress = par.progress;
    int progressMax = par.progressMax;
    int fps = par.fps;
    int exportFps = par.exportFps;

    int frameRepeat;
    int frameReminder, frameReminder1;
    int framePutEvery, framePutEvery1;
    int frameSkipEvery, frameSkipEvery1;
    int frameNumber;
    int framePerSecond;

    QSettings settings( "Pencil", "Pencil" );

    QString extension = "";
    QString formatStr = format;
    if ( formatStr == "PNG" || formatStr == "png" )
    {
        format = "PNG";
        extension = ".png";
    }
    if ( formatStr == "JPG" || formatStr == "jpg" || formatStr == "JPEG" )
    {
        format = "JPG";
        extension = ".jpg";
        background = true; // JPG doesn't support transparency so we have to include the background
    }
    if ( filePath.endsWith( extension, Qt::CaseInsensitive ) )
    {
        filePath.chop( extension.size() );
    }
    //qDebug() << "format =" << format << "extension = " << extension;

    qDebug() << "Exporting frames from " << frameStart << "to" << frameEnd << "at size " << exportSize;
    convertNFrames( fps, exportFps, &frameRepeat, &frameReminder, &framePutEvery, &frameSkipEvery );
    qDebug() << "fps " << fps << " exportFps " << exportFps << " frameRepeat " << frameRepeat << " frameReminder " << frameReminder << " framePutEvery " << framePutEvery << " frameSkipEvery " << frameSkipEvery;
    frameNumber = 0;
    framePerSecond = 0;
    frameReminder1 = frameReminder;
    framePutEvery1 = framePutEvery;
    frameSkipEvery1 = frameSkipEvery;
    for ( int currentFrame = frameStart; currentFrame <= frameEnd; currentFrame++ )
    {
        if ( progress != NULL ) progress->setValue( ( currentFrame - frameStart )*progressMax / ( frameEnd - frameStart ) );
        QImage tempImage( exportSize, QImage::Format_ARGB32_Premultiplied );
        QPainter painter( &tempImage );

        // Make sure that old frame is erased before exporting a new one
        tempImage.fill( 0x00000000 );

        if ( currentLayer->type() == Layer::CAMERA )
        {
            QRect viewRect = ( ( LayerCamera* )currentLayer )->getViewRect();
            QTransform mapView = Editor::map( viewRect, QRectF( QPointF( 0, 0 ), exportSize ) );
            mapView = ( ( LayerCamera* )currentLayer )->getViewAtFrame( currentFrame ) * mapView;
            painter.setWorldTransform( mapView );
        }
        else
        {
            painter.setTransform( view );
        }
        paintImage( painter, currentFrame, background, antialiasing );

        frameNumber++;
        framePerSecond++;
        QString frameNumberString = QString::number( frameNumber );
        while ( frameNumberString.length() < 4 ) frameNumberString.prepend( "0" );

        tempImage.save( filePath + frameNumberString + extension, format, quality );
        int delta = 0;
        if ( framePutEvery )
        {
            framePutEvery1--;
            qDebug() << "-->framePutEvery1" << framePutEvery1;
            if ( framePutEvery1 )
            {
                delta = 0;
            }
            else
            {
                delta = 1; framePutEvery1 = framePutEvery;
            }
        }
        if ( frameSkipEvery )
        {
            frameSkipEvery1--;
            qDebug() << "-->frameSkipEvery1" << frameSkipEvery1;
            if ( frameSkipEvery1 )
            {
                delta = 1;
            }
            else
            {
                delta = 0; frameSkipEvery1 = frameSkipEvery;
            }
        }
        if ( frameReminder1 )
        {
            frameReminder1 -= delta;
        }
        else
        {
            delta = 0;
        }
        for ( int i = 0; ( i < frameRepeat - 1 + delta ) && ( framePerSecond < exportFps ); i++ )
        {
            frameNumber++;
            framePerSecond++;
            QString frameNumberLink = QString::number( frameNumber );
            while ( frameNumberLink.length() < 4 ) frameNumberLink.prepend( "0" );
            tempImage.save( filePath + frameNumberLink + extension, format, quality );
        }
        if ( framePerSecond == exportFps )
        {
            framePerSecond = 0;
            frameReminder1 = frameReminder;
            framePutEvery1 = framePutEvery;
            frameSkipEvery1 = frameSkipEvery;
        }
    }

    // XXX no error handling yet
    return true;
}



bool Object::exportX( int frameStart, int frameEnd, QTransform view, QSize exportSize, QString filePath, bool antialiasing )
{
    QSettings settings( "Pencil", "Pencil" );

    int page;
    page = 0;
    for ( int j = frameStart; j <= frameEnd; j = j + 15 )
    {
        QImage xImg( QSize( 2300, 3400 ), QImage::Format_ARGB32_Premultiplied );
        QPainter xPainter( &xImg );
        xPainter.fillRect( 0, 0, 2300, 3400, Qt::white );
        int y = j - 1;
        for ( int i = j; i < 15 + page * 15 && i <= frameEnd; i++ )
        {
            QRect source = QRect( QPoint( 0, 0 ), exportSize );
            QRect target = QRect( QPoint( ( y % 3 ) * 800 + 30, ( y / 3 ) * 680 + 50 - page * 3400 ), QSize( 640, 480 ) );
            QTransform thumbView = view * Editor::map( source, target );
            xPainter.setWorldTransform( thumbView );
            xPainter.setClipRegion( thumbView.inverted().map( QRegion( target ) ) );
            paintImage( xPainter, i, false, antialiasing );
            xPainter.resetMatrix();
            xPainter.setClipping( false );
            xPainter.setPen( Qt::black );
            xPainter.setFont( QFont( "helvetica", 50 ) );
            xPainter.drawRect( target );
            xPainter.drawText( QPoint( ( y % 3 ) * 800 + 35, ( y / 3 ) * 680 + 65 - page * 3400 ), QString::number( i ) );
            y++;
        }

        if ( filePath.endsWith( ".jpg", Qt::CaseInsensitive ) )
        {
            filePath.chop( 4 );
        }
        if ( !xImg.save( filePath + QString::number( page ) + ".jpg", "JPG", 60 ) ) {
            return false;
        }
        page++;
    }

    return true;
}

bool Object::exportIm( int frameStart, int frameEnd, QTransform view, QSize exportSize, QString filePath, bool antialiasing )
{
    Q_UNUSED( frameEnd );

    QImage exported( exportSize, QImage::Format_ARGB32_Premultiplied );
    QPainter painter( &exported );
    painter.fillRect( exported.rect(), Qt::white );
    painter.setWorldTransform( view );
    paintImage( painter, frameStart, false, antialiasing );
    return exported.save( filePath );
}

bool Object::exportFlash( int startFrame, int endFrame, QTransform view, QSize exportSize, QString filePath, int fps, int compression )
{
    Q_UNUSED( exportSize );
    Q_UNUSED( startFrame );
    Q_UNUSED( endFrame );
    Q_UNUSED( view );
    Q_UNUSED( fps );
    Q_UNUSED( compression );

    if ( !filePath.endsWith( ".swf", Qt::CaseInsensitive ) )
    {
        filePath = filePath + ".swf";
    }

    // ************* Requires the MING Library ***************
    // Flash::exportFlash(this, startFrame, endFrame, view, exportSize, filePath, fps, compression);
    // **********************************************

    return false;
}

int Object::getLayerCount()
{
    Q_ASSERT_X( this != nullptr, "", "" );
    return mLayers.size();
}