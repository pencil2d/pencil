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

    qDebug() << "  XML=" << strMainXMLFile;
    qDebug() << "  Data Folder=" << strDataFolder;
    qDebug() << "  Working Folder=" << obj->workingDir();

    obj->setDataDir( strDataFolder );
    obj->setMainXMLFile( strMainXMLFile );

    QFile file( strMainXMLFile );
    if ( !file.open( QFile::ReadOnly ) )
    {
        return cleanUpWithErrorCode( Status::ERROR_FILE_CANNOT_OPEN );
    }

    QDomDocument xmlDoc;
    if ( !xmlDoc.setContent( &file ) )
    {
        qCDebug(mLog) << "Couldn't open the main XML file.";
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
    
    verifyObject( obj );
    
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
			isOK = object->loadXML( element, [this] ( float f )
			{
				emit progressUpdated( f );
			} );

            if (!isOK) qCDebug(mLog) << "Failed to Load object";

        }
        else if ( element.tagName() == "editor" || element.tagName() == "projectdata" )
        {
            ObjectData* projectData = loadProjectData( element );
            object->setData( projectData );
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
	return object->loadXML( root, [this]( float f )
	{
		emit progressUpdated( f );
	} );
}

bool FileManager::isOldForamt( const QString& fileName )
{
    QStringList zippedFileList = JlCompress::getFileList( fileName );
    return ( zippedFileList.empty() );
}

Status FileManager::save( Object* object, QString strFileName )
{
    QStringList debugDetails = QStringList() << "FileManager::save" << QString( "strFileName = " ).append( strFileName );
    if ( object == nullptr )
    {
        return Status( Status::INVALID_ARGUMENT, debugDetails << "object parameter is null" );
    }

    QFileInfo fileInfo( strFileName );
    if ( fileInfo.isDir() )
    { 
        debugDetails << "strFileName points to a directory";
        return Status( Status::INVALID_ARGUMENT,
                       debugDetails,
                       tr( "Invalid Save Path" ),
                       tr( "The file path you have specified (\"%1\") points to a directory, so the file cannot be saved." ).arg( fileInfo.absoluteFilePath() ) );
    }
    if ( fileInfo.exists() && !fileInfo.isWritable() )
    {
        debugDetails << "strFileName points to a file that is not writable";
        return Status( Status::INVALID_ARGUMENT,
                       debugDetails ,
                       tr( "Invalid Save Path" ),
                       tr( "The file path you have specified (\"%1\") cannot be written to, so the file cannot be saved. Please make sure that you have sufficient permissions to save to that location and try again." ).arg( fileInfo.absoluteFilePath() ) );
    }

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
        debugDetails << QString( "strTempWorkingFolder = " ).append( strTempWorkingFolder );

        qCDebug( mLog ) << "Temp Folder=" << strTempWorkingFolder;
        strMainXMLFile = QDir( strTempWorkingFolder ).filePath( PFF_XML_FILE_NAME );
        strDataFolder = QDir( strTempWorkingFolder ).filePath( PFF_OLD_DATA_DIR );
    }

    QFileInfo dataInfo( strDataFolder );
    if ( !dataInfo.exists() )
    {
        QDir dir( strDataFolder ); // the directory where filePath is or will be saved
        
        // creates a directory with the same name +".data"
        if( !dir.mkpath( strDataFolder ) )
        {
            debugDetails << QString( "dir.absolutePath() = %1" ).arg( dir.absolutePath() );
            if( isOldFile ) {
                return Status( Status::ERROR_FILE_CANNOT_OPEN, debugDetails, tr( "Cannot Create Data Directory" ), tr( "Cannot create the data directory at \"%1\". Please make sure that you have sufficient permissions to save to that location and try again. Alternatively try saving as pclx format." ).arg( strDataFolder ) );
            }
            else {
                return Status( Status::FAIL, debugDetails, tr("Internal Error"), tr( "Cannot create the data directory at temporary location \"%1\". Please make sure that you have sufficient permissions to save to that location and try again. Alternatively try saving as pcl format." ).arg( strDataFolder ) );
            }
        }
    }
    if( !dataInfo.isDir() )
    {
        debugDetails << QString( "dataInfo.absoluteFilePath() = ").append(dataInfo.absoluteFilePath());
        if( isOldFile )
        {
            return Status( Status::ERROR_FILE_CANNOT_OPEN, debugDetails, tr( "Cannot Create Data Directory" ), tr( "Cannot use the path \"%1\" as a data directory since that currently points to a file. Please move or delete that file and try again. Alternatively try saving with the pclx format." ).arg( dataInfo.absoluteFilePath() ) );
        }
        else
        {
            return Status( Status::FAIL, debugDetails, tr( "Internal Error" ), tr( "Cannot use the data directory at temporary location \"%1\" since it is a file. Please move or delete that file and try again. Alternatively try saving with the pcl format." ).arg( dataInfo.absoluteFilePath() ) );
        }
    }

    // save data
    int layerCount = object->getLayerCount();
    debugDetails << QString("layerCount = %1").arg(layerCount);
    qCDebug( mLog ) << QString( "Total layers = %1" ).arg( layerCount );

    bool isOkay = true;
    for ( int i = 0; i < layerCount; ++i )
    {
        Layer* layer = object->getLayer( i );
        qCDebug( mLog ) << QString( "Saving Layer %1" ).arg( i ).arg( layer->mName );

        //progressValue = (i * 100) / nLayers;
        //progress.setValue( progressValue );
        debugDetails << QString("layer[%1] = Layer[id=%2, name=%3, type=%4]").arg( i ).arg( layer->id() ).arg( layer->name() ).arg( layer->type() );
        switch ( layer->type() )
        {
        case Layer::BITMAP:
        case Layer::VECTOR:
        case Layer::SOUND:
        {
            Status st = layer->save( strDataFolder );
            if( !st.ok() )
            {
                isOkay = false;
                QStringList layerDetails = st.detailsList();
                for ( QString detail : layerDetails )
                {
                    detail.prepend( "&nbsp;&nbsp;" );
                }
                debugDetails << QString( "- Layer[%1] failed to save" ).arg( i ) << layerDetails;
            }
            break;
        }
        case Layer::CAMERA:
            break;
        case Layer::UNDEFINED:
        case Layer::MOVIE:
            Q_ASSERT( false );
            break;
        }
        if( !isOkay )
        {
            return Status( Status::FAIL, debugDetails, tr( "Internal Error" ), tr( "An internal error occurred while trying to save the file. Some or all of your file may not have saved." ) );
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
    QDomProcessingInstruction encoding = xmlDoc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    xmlDoc.appendChild( encoding );
    xmlDoc.appendChild( root );

    // save editor information
    QDomElement projectDataElement = saveProjectData( object->data(), xmlDoc );
    root.appendChild( projectDataElement );
    qCDebug( mLog ) << "Save Project Data";

    // save object
    QDomElement objectElement = object->saveXML( xmlDoc );
    root.appendChild( objectElement );
    qCDebug( mLog ) << "Save Object Node";

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

ObjectData* FileManager::loadProjectData( const QDomElement& docElem )
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
        
        extractProjectData( element, data );
     
        tag = tag.nextSibling();
    }
    return data;
}


QDomElement FileManager::saveProjectData( ObjectData* data, QDomDocument& xmlDoc )
{
	QDomElement rootTag = xmlDoc.createElement( "projectdata" );

	// Current Frame
	QDomElement currentFrameTag = xmlDoc.createElement( "currentFrame" );
	currentFrameTag.setAttribute( "value", data->getCurrentFrame() );
	rootTag.appendChild( currentFrameTag );
	
	// Current Colour
	QDomElement currentColorTag = xmlDoc.createElement( "currentColor" );
	QColor color = data->getCurrentColor();
	currentColorTag.setAttribute( "r", color.red() );
	currentColorTag.setAttribute( "g", color.green() );
	currentColorTag.setAttribute( "b", color.blue() );
	currentColorTag.setAttribute( "a", color.alpha() );
	rootTag.appendChild( currentColorTag );

	// Current Layer
	QDomElement currentLayerTag = xmlDoc.createElement( "currentLayer" );
	currentLayerTag.setAttribute( "value", data->getCurrentLayer() );
	rootTag.appendChild( currentLayerTag );

	// Current View
	QDomElement currentViewTag = xmlDoc.createElement( "currentView" );
	QTransform view = data->getCurrentView();
	currentViewTag.setAttribute( "m11", view.m11() );
	currentViewTag.setAttribute( "m12", view.m12() );
	currentViewTag.setAttribute( "m21", view.m21() );
	currentViewTag.setAttribute( "m22", view.m22() );
	currentViewTag.setAttribute( "dx", view.dx() );
	currentViewTag.setAttribute( "dy", view.dy() );
	rootTag.appendChild( currentViewTag );

	// Fps
	QDomElement fpsTag = xmlDoc.createElement( "fps" );
	fpsTag.setAttribute( "value", data->getFrameRate() );
	rootTag.appendChild( fpsTag );

	// Current Layer
	QDomElement tagIsLoop = xmlDoc.createElement( "isLoop" );
	tagIsLoop.setAttribute( "value", data->isLooping() ? "true" : "false" );
	rootTag.appendChild( tagIsLoop );

	// Current Layer
	QDomElement tagRangedPlayback = xmlDoc.createElement( "isRangedPlayback" );
	tagRangedPlayback.setAttribute( "value", data->isRangedPlayback() ? "true" : "false" );
	rootTag.appendChild( tagRangedPlayback );

	// Current Layer
	QDomElement tagMarkInFrame = xmlDoc.createElement( "markInFrame" );
	tagMarkInFrame.setAttribute( "value", data->getMarkInFrameNumber() );
	rootTag.appendChild( tagMarkInFrame );

	// Current Layer
	QDomElement tagMarkOutFrame = xmlDoc.createElement( "markOutFrame" );
	tagMarkOutFrame.setAttribute( "value", data->getMarkOutFrameNumber() );
	rootTag.appendChild( tagMarkOutFrame );

	return rootTag;
}

void FileManager::extractProjectData( const QDomElement& element, ObjectData* data )
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
    else if ( strName == "fps" || strName == "currentFps" )
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

Status FileManager::verifyObject( Object* obj )
{
    // check current layer.
    int curLayer = obj->data()->getCurrentLayer();
    int maxLayer = obj->getLayerCount();
    if ( curLayer >= maxLayer )
    {
        obj->data()->setCurrentLayer(maxLayer - 1);
    }
    
    return Status::OK;
}
