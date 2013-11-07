
#include "pencildef.h"
#include "JlCompress.h"
#include "fileformat.h"
#include "object.h"
#include "objectsaveloader.h"

ObjectSaveLoader::ObjectSaveLoader(QObject *parent) :
    QObject(parent),
    m_strLastErrorMessage( "" ),
    m_strLastTempWorkingFolder( "" )
{
}

Object* ObjectSaveLoader::loadFromFile(QString strFilename)
{
    // ---- test before opening ----
    QFileInfo fileInfo(strFilename);
    if ( !fileInfo.exists() )
    {
        //m_strLastErrorMessage = tr("File doesn't exist.");
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
    QScopedPointer<QFile> file(new QFile(strMainXMLFilePath));

    if ( !file->open(QFile::ReadOnly) )
    {
        //m_strLastErrorMessage = tr("Cannot open file.");
        m_error = PencilError( PCL_ERROR_FILE_CANNOT_OPEN );
        cleanUpTempFolder();
        return NULL;
    }

    QDomDocument xmlDoc;
    if ( !xmlDoc.setContent(file.data()) )
    {
        //m_strLastErrorMessage = tr("This file is not a valid XML document.");
        m_error = PencilError( PCL_ERROR_INVALID_XML_FILE );
        cleanUpTempFolder();
        return NULL;
    }

    QDomDocumentType type = xmlDoc.doctype();
    if (type.name() != "PencilDocument" && type.name() != "MyObject")
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


}

bool ObjectSaveLoader::saveToFile(Object* object, QString strFileName)
{
    Q_UNUSED(object);
    Q_UNUSED(strFileName);
    return true;
}

void ObjectSaveLoader::cleanUpTempFolder()
{
    removePFFTmpDirectory( m_strLastTempWorkingFolder );
}

QString ObjectSaveLoader::extractZipToTempFolder(QString strZipFile)
{
    // ---- now decompress PFF -----
    QFileInfo zipFileInfo(strZipFile);

    QString strTempWorkingPath = QDir::tempPath() + "/" + zipFileInfo.completeBaseName() + PFF_TMP_DECOMPRESS_EXT;
    //qDebug() << "tmpFilePath" << tmpFilePath ;

    // --removes an old decompression directory first  - better approach
    removePFFTmpDirectory( strTempWorkingPath );

    // --creates a new decompression directory
    QDir dir(QDir::tempPath());
    dir.mkpath( strTempWorkingPath );

    JlCompress::extractDir(strZipFile, strTempWorkingPath);

    m_strLastTempWorkingFolder = strTempWorkingPath;
    return strTempWorkingPath + "/" + PFF_XML_FILE_NAME;
}
