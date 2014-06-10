#include "pencildef.h"
#include "JlCompress.h"
#include "fileformat.h"
#include "object.h"
#include "objectsaveloader.h"

ObjectSaveLoader::ObjectSaveLoader( QObject *parent ) :
    QObject( parent ),
    m_strLastTempWorkingFolder( "" )
{
}

Object* ObjectSaveLoader::loadFromFile( QString strFilename )
{
    // ---- test before opening ----

    if ( !isFileExists( strFilename ) )
    {
        m_error = PencilError( PCL_ERROR_FILE_NOT_EXIST );
        return NULL;
    }

    QString strMainXMLFilePath = strFilename;
    QStringList zippedFileList = JlCompress::getFileList( strFilename );

    // -- Test file format: new zipped pclx or old pcl ?
    bool bIsOldPencilFile = zippedFileList.empty();
    if ( !bIsOldPencilFile )
    {
        strMainXMLFilePath = extractZipToTempFolder( strFilename );
        qDebug() << "Recognized New zipped Pencil File Format !";
    }
    else
    {
        qDebug() << "Recognized Old Pencil File Format !";
    }

    // -- test before opening
    QScopedPointer<QFile> file( new QFile( strMainXMLFilePath ) );

    if ( !file->open( QFile::ReadOnly ) )
    {
        //m_strLastErrorMessage = tr("Cannot open file.");
        m_error = PencilError( PCL_ERROR_FILE_CANNOT_OPEN );
        cleanUpTempFolder();
        return NULL;
    }

    QDomDocument xmlDoc;
    if ( !xmlDoc.setContent( file.data() ) )
    {
        //m_strLastErrorMessage = tr("This file is not a valid XML document.");
        m_error = PencilError( PCL_ERROR_INVALID_XML_FILE );
        cleanUpTempFolder();
        return NULL;
    }

    QDomDocumentType type = xmlDoc.doctype();
    if ( type.name() != "PencilDocument" && type.name() != "MyObject" )
    {
        //m_strLastErrorMessage = tr("This file is not a Pencil2D document.");
        m_error = PencilError( PCL_ERROR_INVALID_PENCIL_FILE );
        cleanUpTempFolder();
        return NULL; // this is not a Pencil document
    }

    Object* pObject = new Object();

    QString strDataLayersDirPath;
    if ( bIsOldPencilFile )
    {
        // ex. aaa.pcl  => aaa.pcl.data
        strDataLayersDirPath = strMainXMLFilePath + "." + PFF_LAYERS_DIR;
    }
    else
    {
        QDir workingDir = QFileInfo( strMainXMLFilePath ).dir(); // get the parent folder
        workingDir.cd( PFF_LAYERS_DIR );
        strDataLayersDirPath = workingDir.absolutePath();
    }

    Object* newObject = pObject;
    if ( !newObject->loadPalette( strDataLayersDirPath ) )
    {
        newObject->loadDefaultPalette();
    }

    // ------- reads the XML file -------
    bool ok = true;
    int prog = 0;
    QDomElement docElem = xmlDoc.documentElement();
    if ( docElem.isNull() )
    {
        return NULL;
    }

    if ( docElem.tagName() == "document" )
    {
        qDebug( "Object Loader: start." );

        QDomNode tag = docElem.firstChild();
        while ( !tag.isNull() )
        {
            QDomElement element = tag.toElement(); // try to convert the node to an element.
            if ( !element.isNull() )
            {
                prog += std::min( prog + 10, 100 );
                //progress.setValue(prog);
                emit progressValueChanged( prog );

                if ( element.tagName() == "editor" )
                {
                    qDebug( "  Load editor" );
                    //loadDomElement( element );
                }
                else if ( element.tagName() == "object" )
                {
                    qDebug( "  Load object" );
                    ok = newObject->loadDomElement( element, strDataLayersDirPath );
                    qDebug() << "    dataDir:" << strDataLayersDirPath;
                }
            }
            tag = tag.nextSibling();
        }
    }
    else
    {
        if ( docElem.tagName() == "object" || docElem.tagName() == "MyOject" )   // old Pencil format (<=0.4.3)
        {
            ok = newObject->loadDomElement( docElem, strFilename );
        }
    }

    if ( ok )
    {
        /*
        if (!openingTheOLDWAY)
        {
        removePFFTmpDirectory( tmpFilePath ); // --removes temporary decompression directory
        }
        */
    }
    else
    {
        return NULL;
    }

    return pObject;
}

bool ObjectSaveLoader::saveToFile( Object* object, QString strFileName )
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
    removePFFTmpDirectory( m_strLastTempWorkingFolder );
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
    //qDebug() << "tmpFilePath" << tmpFilePath ;

    // --removes an old decompression directory first  - better approach
    removePFFTmpDirectory( strTempWorkingPath );

    // --creates a new decompression directory
    QDir dir( QDir::tempPath() );
    dir.mkpath( strTempWorkingPath );

    JlCompress::extractDir( strZipFile, strTempWorkingPath );

    m_strLastTempWorkingFolder = strTempWorkingPath;
    return strTempWorkingPath + "/" + PFF_XML_FILE_NAME;
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