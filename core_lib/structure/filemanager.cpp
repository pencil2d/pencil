/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2014 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/


#include "filemanager.h"
#include "pencildef.h"
#include "JlCompress.h"
#include "fileformat.h"
#include "object.h"


FileManager::FileManager( QObject *parent ) : QObject( parent ),
    mLog( "SaveLoader" )
{
    ENABLE_DEBUG_LOG( mLog, true );
}

Object* FileManager::load( QString strFileName )
{
    if ( !QFile::exists( strFileName ) )
    {
        qCDebug( mLog ) << "ERROR - File doesn't exist.";
        return cleanUpWithErrorCode( Status::FILE_NOT_FOUND );
    }

    emit progressUpdated( 0.f );

    Object* obj = new Object;
    obj->setFilePath( strFileName );
    obj->createWorkingDir();

    QString strMainXMLFile;	
    QString strDataFolder;

    // Test file format: new zipped .pclx or old .pcl?
    bool oldFormat = isOldForamt( strFileName );

    if ( oldFormat )
    {
        qCDebug( mLog ) << "Recognized Old Pencil File Format (*.pcl) !";

        strMainXMLFile = strFileName;
        strDataFolder  = strMainXMLFile + "." + PFF_OLD_DATA_DIR;
    }
    else
    {
        qCDebug( mLog ) << "Recognized New zipped Pencil File Format (*.pclx) !";

        unzip( strFileName, obj->workingDir() );

        strMainXMLFile = QDir( obj->workingDir() ).filePath( PFF_XML_FILE_NAME );
        strDataFolder  = QDir( obj->workingDir() ).filePath( PFF_DATA_DIR );
    }

    qDebug() << "XML=" << strMainXMLFile;
    qDebug() << "Data Folder=" << strDataFolder;
    qDebug() << "Working Folder=" << obj->workingDir();

    obj->setDataDir( strDataFolder );
    obj->setMainXMLFile( strMainXMLFile );

    QFile file( strMainXMLFile );
    if ( !file.open( QFile::ReadOnly ) )
    {
        return cleanUpWithErrorCode( Status::ERROR_FILE_CANNOT_OPEN );
    }

    qCDebug( mLog ) << "Checking main XML file...";
    QDomDocument xmlDoc;
    if ( !xmlDoc.setContent( &file ) )
    {
        return cleanUpWithErrorCode( Status::ERROR_INVALID_XML_FILE );
    }

    QDomDocumentType type = xmlDoc.doctype();
    if ( !( type.name() == "PencilDocument" || type.name() == "MyObject" ) )
    {
        return cleanUpWithErrorCode( Status::ERROR_INVALID_PENCIL_FILE );
    }

    QDomElement root = xmlDoc.documentElement();
    if ( root.isNull() )
    {
        return cleanUpWithErrorCode( Status::ERROR_INVALID_PENCIL_FILE );
    }
    
    // Create object.
    qCDebug( mLog ) << "Start to load object..";

    loadPalette( obj );

    bool ok = true;
    
    if ( root.tagName() == "document" )
    {
        ok = loadObject( obj, root );
    }
    else if ( root.tagName() == "object" || root.tagName() == "MyOject" ) // old Pencil format (<=0.4.3)
    {
        ok = loadObjectOldWay( obj, root );
    }

    if ( !ok )
    {
        delete obj;
        return cleanUpWithErrorCode( Status::ERROR_INVALID_PENCIL_FILE );
    }
    
    return obj;
}

bool FileManager::loadObject( Object* object, const QDomElement& root )
{
    QDomElement e = root.firstChildElement( "object" );
    if ( e.isNull() )
    {
        return false;
    }
    
    bool isOK = true;
    for ( QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        QDomElement element = node.toElement(); // try to convert the node to an element.
        if ( element.isNull() )
        { 
            continue;
        }

        if ( element.tagName() == "object" )
        {
            qCDebug( mLog ) << "Load object";
            isOK = object->loadXML( element );
        }
        else if ( element.tagName() == "editor" )
        {
            ObjectData* editorData = loadEditorState( element );
            object->setData( editorData );
        }
        else
        {
            Q_ASSERT( false );
        }
        //progress = std::min( progress + 10, 100 );
        //emit progressValueChanged( progress );
    }

    return isOK;
}

bool FileManager::loadObjectOldWay( Object* object, const QDomElement& root )
{
    return object->loadXML( root );
}

bool FileManager::isOldForamt( const QString& fileName )
{
    QStringList zippedFileList = JlCompress::getFileList( fileName );
    return ( zippedFileList.empty() );
}

Status FileManager::save( Object* object, QString strFileName )
{
    if ( object == nullptr ) { return Status::INVALID_ARGUMENT; }

    QFileInfo fileInfo( strFileName );
    if ( fileInfo.isDir() ) { return Status::INVALID_ARGUMENT; }

    QString strTempWorkingFolder;
    QString strMainXMLFile;
    QString strDataFolder;

    bool isOldFile = strFileName.endsWith( PFF_OLD_EXTENSION );
    if ( isOldFile )
    {
        qCDebug( mLog ) << "Save in Old Pencil File Format (*.pcl) !";

        strMainXMLFile = strFileName;
        strDataFolder = strMainXMLFile + "." + PFF_OLD_DATA_DIR;
    }
    else
    {
        qCDebug( mLog ) << "Save in New zipped Pencil File Format (*.pclx) !";

        strTempWorkingFolder = object->workingDir();
        Q_ASSERT( QDir( strTempWorkingFolder ).exists() );

        qCDebug( mLog ) << "Temp Folder=" << strTempWorkingFolder;
        strMainXMLFile = QDir( strTempWorkingFolder ).filePath( PFF_XML_FILE_NAME );
        strDataFolder = QDir( strTempWorkingFolder ).filePath( PFF_OLD_DATA_DIR );
    }

    QFileInfo dataInfo( strDataFolder );
    if ( !dataInfo.exists() )
    {
        QDir dir( strDataFolder ); // the directory where filePath is or will be saved
        dir.mkpath( strDataFolder ); // creates a directory with the same name +".data"
    }

    // save data
    int layerCount = object->getLayerCount();
    qCDebug( mLog ) << QString( "Total layers = %1" ).arg( layerCount );

    for ( int i = 0; i < layerCount; ++i )
    {
        Layer* layer = object->getLayer( i );
        qCDebug( mLog ) << QString( "Saving Layer %1" ).arg( i ).arg( layer->mName );

        //progressValue = (i * 100) / nLayers;
        //progress.setValue( progressValue );
        switch ( layer->type() )
        {
        case Layer::BITMAP:
        case Layer::VECTOR:
        case Layer::SOUND:
            layer->save( strDataFolder );
            break;
        case Layer::CAMERA:
            break;
        case Layer::UNDEFINED:
        case Layer::MOVIE:
            Q_ASSERT( false );
            break;
        }
    }

    // save palette
    object->savePalette( strDataFolder );

    // -------- save main XML file -----------
    QScopedPointer<QFile> file( new QFile( strMainXMLFile ) );
    if ( !file->open( QFile::WriteOnly | QFile::Text ) )
    {
        return Status::ERROR_FILE_CANNOT_OPEN;
    }

    QDomDocument xmlDoc( "PencilDocument" );
    QDomElement root = xmlDoc.createElement( "document" );
    xmlDoc.appendChild( root );

    // save editor information
    //QDomElement editorElement = createDomElement( xmlDoc );
    //root.appendChild( editorElement );
    qCDebug( mLog ) << "Save Editor Node.";

    // save object
    QDomElement objectElement = object->saveXML( xmlDoc );
    root.appendChild( objectElement );
    qCDebug( mLog ) << "Save Object Node.";

    const int IndentSize = 2;

    QTextStream out( file.data() );
    xmlDoc.save( out, IndentSize );

    if ( !isOldFile )
    {
        qCDebug( mLog ) << "Now compressing data to PFF - PCLX ...";

        bool ok = JlCompress::compressDir( strFileName, strTempWorkingFolder );
        if ( !ok )
        {
            return Status::FAIL;
        }

        qCDebug( mLog ) << "Compressed. File saved.";
    }

    object->setFilePath( strFileName );
    object->setModified( false );

    return Status::OK;
}

ObjectData* FileManager::loadEditorState( QDomElement docElem )
{
    ObjectData* data = new ObjectData;
    if ( docElem.isNull() )
    {
        return data;
    }

    QDomNode tag = docElem.firstChild();

    while ( !tag.isNull() )
    {
        QDomElement element = tag.toElement(); // try to convert the node to an element.
        if ( element.isNull() )
        {
            continue;
        }

     
        
        tag = tag.nextSibling();
    }
    return data;
}


void FileManager::extractEditorStateData( const QDomElement& element, ObjectData* data )
{
    Q_ASSERT( data );

    QString strName = element.tagName();
    if ( strName == "currentFrame" )
    {
        data->setCurrentFrame( element.attribute( "value" ).toInt() );
    }
    else  if ( strName == "currentColor" )
    {
        int r = element.attribute( "r", "255" ).toInt();
        int g = element.attribute( "g", "255" ).toInt();
        int b = element.attribute( "b", "255" ).toInt();
        int a = element.attribute( "a", "255" ).toInt();

        data->setCurrentColor( QColor( r, g, b, a ) );
    }
    else if ( strName == "currentLayer" )
    {
        data->setCurrentLayer( element.attribute( "value", "0" ).toInt() );
    }
    else if ( strName == "currentView" )
    {
        double m11 = element.attribute( "m11", "1" ).toDouble();
        double m12 = element.attribute( "m12", "0" ).toDouble();
        double m21 = element.attribute( "m21", "0" ).toDouble();
        double m22 = element.attribute( "m22", "1" ).toDouble();
        double dx = element.attribute( "dx", "0" ).toDouble();
        double dy = element.attribute( "dy", "0" ).toDouble();
        
        data->setCurrentView( QTransform( m11, m12, m21, m22, dx, dy ) );
    }
    else if ( strName == "fps" )
    {
        data->setFrameRate( element.attribute( "value", "12" ).toInt() );
    }
    else if ( strName == "isLoop" )
    {
        data->setLooping ( element.attribute( "value", "false" ) == "true" );
    }
    else if ( strName == "isRangedPlayback" )
    {
        data->setRangedPlayback( ( element.attribute( "value", "false" ) == "true" ) );
    }
    else if ( strName == "markInFrame" )
    {
        data->setMarkInFrameNumber( element.attribute( "value", "0" ).toInt() );
    }
    else if ( strName == "markOutFrame" )
    {
        data->setMarkOutFrameNumber( element.attribute( "value", "15" ).toInt() );
    }
}

Object* FileManager::cleanUpWithErrorCode( Status error )
{
    mError = error;
    removePFFTmpDirectory( mstrLastTempFolder );
    return nullptr;
}

bool FileManager::loadPalette( Object* obj )
{
    qCDebug( mLog ) << "Load Palette..";

    QString paletteFilePath = obj->dataDir() + "/" + PFF_PALETTE_FILE;
    if ( !obj->importPalette( paletteFilePath ) )
    {
        obj->loadDefaultPalette();
    }
    return true;
}

void FileManager::unzip( const QString& strZipFile, const QString& strUnzipTarget )
{
    // --removes an old decompression directory first  - better approach
    removePFFTmpDirectory( strUnzipTarget );

    // --creates a new decompression directory
    JlCompress::extractDir( strZipFile, strUnzipTarget );

    mstrLastTempFolder = strUnzipTarget;
}

QList<ColourRef> FileManager::loadPaletteFile( QString strFilename )
{
    QFileInfo fileInfo( strFilename );
    if ( !fileInfo.exists() )
    {
        return QList<ColourRef>();
    }

    // TODO: Load Palette.
    return QList<ColourRef>();
}
