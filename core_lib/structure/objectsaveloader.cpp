#include "objectsaveloader.h"
#include "pencildef.h"
#include "JlCompress.h"
#include "fileformat.h"
#include "object.h"



ObjectSaveLoader::ObjectSaveLoader( QObject *parent ) : QObject( parent ),
mstrLastTempFolder( "" ),
mLog( "SaveLoader" )
{
}

Object* ObjectSaveLoader::load( QString strFileName )
{
    if ( !isFileExists( strFileName ) )
    {
        qCDebug( mLog, "ERROR - File doesn't exist." );
        mError = PencilError( PCL_ERROR_FILE_NOT_EXIST );
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
        strDataFolder = QDir( strTempWorkingFolder ).filePath( PFF_XML_FILE_NAME );
    }
    qCDebug( mLog ) << "XML=" << strMainXMLFile;
    qCDebug( mLog ) << "Data Folder=" << strDataFolder;

    QScopedPointer<QFile> file( new QFile( strMainXMLFile ) );
    if ( !file->open( QFile::ReadOnly ) )
    {
        cleanUpTempFolder();
        mError = PencilError( PCL_ERROR_FILE_CANNOT_OPEN );
        return nullptr;
    }

    qCDebug( mLog ) << "Checking main XML file...";
    QDomDocument xmlDoc;
    if ( !xmlDoc.setContent( file.data() ) )
    {
        cleanUpTempFolder();
        mError = PencilError( PCL_ERROR_INVALID_XML_FILE );
        return nullptr;
    }

    QDomDocumentType type = xmlDoc.doctype();
    if ( type.name() != "PencilDocument" && type.name() != "MyObject" )
    {
        cleanUpTempFolder();
        mError = PencilError( PCL_ERROR_INVALID_PENCIL_FILE );
        return nullptr;
    }

    QDomElement root = xmlDoc.documentElement();
    if ( root.isNull() )
    {
        cleanUpTempFolder();
        mError = PencilError( PCL_ERROR_INVALID_PENCIL_FILE );
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
    int progress = 0;
    
    if ( root.tagName() == "document" )
    {
        ok = loadObject( object, root, strDataFolder );
    }
    else if ( root.tagName() == "object" || root.tagName() == "MyOject" )   // old Pencil format (<=0.4.3)
    {
        ok = loadObjectOladWay( object, root, strDataFolder );
    }

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

bool ObjectSaveLoader::loadObjectOladWay( Object* object, const QDomElement& root, const QString& strDataFolder )
{
    return object->loadDomElement( root, strDataFolder );
}

bool ObjectSaveLoader::save( Object* object, QString strFileName )
{
    Q_UNUSED( object );
    Q_UNUSED( strFileName );
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
                //m_pScribbleArea->setMyView( QMatrix(m11,m12,m21,m22,dx,dy) );
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

QString ObjectSaveLoader::extractZipToTempFolder( QString strZipFile )
{
    // ---- now decompress PFF -----
    QFileInfo zipFileInfo( strZipFile );

    QString strTempWorkingPath = QDir::tempPath() + "/" + zipFileInfo.completeBaseName() + PFF_TMP_DECOMPRESS_EXT;

    // --removes an old decompression directory first  - better approach
    removePFFTmpDirectory( strTempWorkingPath );

    // --creates a new decompression directory
    QDir dir( QDir::tempPath() );
    dir.mkpath( strTempWorkingPath );

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