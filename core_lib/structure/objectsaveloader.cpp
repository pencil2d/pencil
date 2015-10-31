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


#include "objectsaveloader.h"
#include "pencildef.h"
#include "JlCompress.h"
#include "fileformat.h"
#include "object.h"



ObjectSaveLoader::ObjectSaveLoader( QObject *parent ) : QObject( parent ),
mstrLastTempFolder( "" ),
mLog( "SaveLoader" )
{
    ENABLE_DEBUG_LOG( mLog, false );
}

Object* ObjectSaveLoader::load( QString strFileName )
{
    if ( !isFileExists( strFileName ) )
    {
        qCDebug( mLog ) << "ERROR - File doesn't exist.";
        mError = Status::ERROR_FILE_NOT_EXIST;
        return nullptr;
    }

    QString strMainXMLFile;
    QString strDataFolder;

    // Test file format: new zipped .pclx or old .pcl?
    QStringList zippedFileList = JlCompress::getFileList( strFileName );
    bool isOldFile = zippedFileList.empty();

    if ( isOldFile )
    {
        qCDebug( mLog ) << "Recognized Old Pencil File Format (*.pcl) !";
        strMainXMLFile = strFileName;
        strDataFolder = strMainXMLFile + "." + PFF_OLD_DATA_DIR;
    }
    else
    {
        qCDebug( mLog ) << "Recognized New zipped Pencil File Format (*.pclx) !";
        QString strTempWorkingFolder = extractZipToTempFolder( strFileName );
        qCDebug( mLog ) << "Temp Folder=" << strTempWorkingFolder;
        strMainXMLFile = QDir( strTempWorkingFolder ).filePath( PFF_XML_FILE_NAME );
        strDataFolder = QDir( strTempWorkingFolder ).filePath( PFF_OLD_DATA_DIR );
    }
    qCDebug( mLog ) << "XML=" << strMainXMLFile;
    qCDebug( mLog ) << "Data Folder=" << strDataFolder;

    QScopedPointer<QFile> file( new QFile( strMainXMLFile ) );
    if ( !file->open( QFile::ReadOnly ) )
    {
        cleanUpTempFolder();
        mError = Status::ERROR_FILE_CANNOT_OPEN;
        return nullptr;
    }

    qCDebug( mLog ) << "Checking main XML file...";
    QDomDocument xmlDoc;
    if ( !xmlDoc.setContent( file.data() ) )
    {
        cleanUpTempFolder();
        mError = Status::ERROR_INVALID_XML_FILE;
        return nullptr;
    }

    QDomDocumentType type = xmlDoc.doctype();
    if ( type.name() != "PencilDocument" && type.name() != "MyObject" )
    {
        cleanUpTempFolder();
        mError = Status::ERROR_INVALID_PENCIL_FILE;
        return nullptr;
    }

    QDomElement root = xmlDoc.documentElement();
    if ( root.isNull() )
    {
        cleanUpTempFolder();
        mError = Status::ERROR_INVALID_PENCIL_FILE;
        return nullptr;
    }

    // Create object.
    Object* object = new Object();
    if ( !object->loadPalette( strDataFolder ) )
    {
        object->loadDefaultPalette();
    }

    // ------- Reads the XML file -------

    qCDebug( mLog ) << "Start to load object.";
    
    bool ok = true;
    //int progress = 0;
    
    if ( root.tagName() == "document" )
    {
        ok = loadObject( object, root, strDataFolder );
    }
    else if ( root.tagName() == "object" || root.tagName() == "MyOject" )   // old Pencil format (<=0.4.3)
    {
        ok = loadObjectOldWay( object, root, strDataFolder );
    }

    object->setFilePath( strFileName );

    return object;
}

bool ObjectSaveLoader::loadObject( Object* object, const QDomElement& root, const QString& strDataFolder )
{
    bool isOK = true;
    for ( QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        QDomElement element = node.toElement(); // try to convert the node to an element.
        if ( element.isNull() ) { continue; }

        if ( element.tagName() == "object" )
        {
            qCDebug( mLog ) << "Load object";
            isOK = object->loadDomElement( element, strDataFolder );
        }
        else if ( element.tagName() == "editor" )
        {
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

bool ObjectSaveLoader::loadObjectOldWay( Object* object, const QDomElement& root, const QString& strDataFolder )
{
    return object->loadDomElement( root, strDataFolder );
}

bool ObjectSaveLoader::save( Object* object, QString strFileName )
{
    if ( object == nullptr ) { return false; }

    QFileInfo fileInfo( strFileName );
    if ( fileInfo.isDir() ) { return false; }

    bool isOldFile = strFileName.endsWith( PFF_OLD_EXTENSION );

    QString strTempWorkingFolder;
    QString strMainXMLFile;
    QString strDataFolder;
    if ( isOldFile )
    {
        qCDebug( mLog ) << "Save in Old Pencil File Format (*.pcl) !";
        strMainXMLFile = strFileName;
        strDataFolder = strMainXMLFile + "." + PFF_OLD_DATA_DIR;
    }
    else
    {
        qCDebug( mLog ) << "Save in New zipped Pencil File Format (*.pclx) !";
        strTempWorkingFolder = createTempWorkingFolder( strFileName );
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
        }
    }

    // save palette
    object->savePalette( strDataFolder );

    // -------- save main XML file -----------
    QScopedPointer<QFile> file( new QFile( strMainXMLFile ) );
    if ( !file->open( QFile::WriteOnly | QFile::Text ) )
    {
        //QMessageBox::warning(this, "Warning", "Cannot write file");
        return false;
    }

    QDomDocument xmlDoc( "PencilDocument" );
    QDomElement root = xmlDoc.createElement( "document" );
    xmlDoc.appendChild( root );

    // save editor information
    //QDomElement editorElement = createDomElement( xmlDoc );
    //root.appendChild( editorElement );
    qCDebug( mLog ) << "Save Editor Node.";

    // save object
    QDomElement objectElement = object->createDomElement( xmlDoc );
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
            return false;
        }
        //removePFFTmpDirectory( strTempWorkingFolder ); // --removing temporary files

        qCDebug( mLog ) << "Compressed. File saved.";
    }

    object->setFilePath( strFileName );
    object->setModified( false );

    return true;
}

bool ObjectSaveLoader::loadDomElement( QDomElement docElem )
{
    if ( docElem.isNull() ) return false;
    QDomNode tag = docElem.firstChild();
    while ( !tag.isNull() )
    {
        QDomElement element = tag.toElement(); // try to convert the node to an element.
        if ( !element.isNull() )
        {
            if ( element.tagName() == "currentLayer" )
            {
                int nCurrentLayerIndex = element.attribute( "value" ).toInt();
                //editor->setCurrentLayer(nCurrentLayerIndex);
            }
            if ( element.tagName() == "currentFrame" )
            {
                //editor->layerManager()->currentFrameIndex() = element.attribute("value").toInt();
            }
            if ( element.tagName() == "currentFps" )
            {
                //editor->fps = element.attribute("value").toInt();
                //timer->setInterval(1000/fps);
                //m_pTimeLine->setFps(editor->fps);
            }
            if ( element.tagName() == "currentView" )
            {
                qreal m11 = element.attribute( "m11" ).toDouble();
                qreal m12 = element.attribute( "m12" ).toDouble();
                qreal m21 = element.attribute( "m21" ).toDouble();
                qreal m22 = element.attribute( "m22" ).toDouble();
                qreal dx = element.attribute( "dx" ).toDouble();
                qreal dy = element.attribute( "dy" ).toDouble();
                //m_pScribbleArea->setMyView( QTransform(m11,m12,m21,m22,dx,dy) );
            }
        }
        tag = tag.nextSibling();
    }
    return true;
}


void ObjectSaveLoader::cleanUpTempFolder()
{
    removePFFTmpDirectory( mstrLastTempFolder );
}

bool ObjectSaveLoader::isFileExists( QString strFilename )
{
    return QFileInfo( strFilename ).exists();
}

QString ObjectSaveLoader::createTempWorkingFolder( QString strFileName )
{
    QFileInfo fileInfo( strFileName );
    QString strTempWorkingFolder = QDir( QDir::tempPath() ).filePath( fileInfo.completeBaseName() + PFF_TMP_DECOMPRESS_EXT );

    QDir dir( QDir::tempPath() );
    dir.mkpath( strTempWorkingFolder );

    return strTempWorkingFolder;
}

QString ObjectSaveLoader::extractZipToTempFolder( QString strZipFile )
{
    QString strTempWorkingPath = createTempWorkingFolder( strZipFile );

    // --removes an old decompression directory first  - better approach
    removePFFTmpDirectory( strTempWorkingPath );

    // --creates a new decompression directory
  
    JlCompress::extractDir( strZipFile, strTempWorkingPath );

    mstrLastTempFolder = strTempWorkingPath;
    return strTempWorkingPath;
}

QList<ColourRef> ObjectSaveLoader::loadPaletteFile( QString strFilename )
{
    QFileInfo fileInfo( strFilename );
    if ( !fileInfo.exists() )
    {
        return QList<ColourRef>();
    }

    // TODO: Load Palette.
    return QList<ColourRef>();
}
