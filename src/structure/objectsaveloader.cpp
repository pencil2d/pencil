
#include "pencildef.h"
#include "JlCompress.h"
#include "object.h"
#include "objectsaveloader.h"

ObjectSaveLoader::ObjectSaveLoader(QObject *parent) :
    QObject(parent)
{
}

Object* ObjectSaveLoader::loadFile(QString strFilename, PencilError* error)
{
    Q_ASSERT_X(error != NULL, "loadFile()", "error is NULL!");

    // ---- test before opening ----

    QFileInfo fileInfo(strFilename);

    if ( !fileInfo.exists() )
    {
        error->message = tr("File doesn't exist.");
        return NULL;
    }

    QString workDirectory;

    if ( fileInfo.suffix() == "pclx" )
    {

        // decompress file
        QString strTmpFilePath = QDir::homePath() + "/" + fileInfo.completeBaseName() + PFF_TMP_DECOMPRESS_EXT;

        QDir dir(QDir::tempPath());
        if (dir.exists())
        {
            dir.rmpath(strTmpFilePath); // --removes an old decompression directory
        }
        dir.mkpath(strTmpFilePath); // --creates a new decompression directory

        workDirectory = strTmpFilePath;
    }
    else if ( fileInfo.suffix() == "pcl" )
    {
        //workDirectory =
    }
    else
    {

    }

    QScopedPointer<QFile> file(new QFile(strFilename));

    if ( !file->open(QFile::ReadOnly) )
    {
        error->message = tr("Cannot open file.");
        return NULL;
    }

    /*
    QDomDocument doc;
    if (!doc.setContent(file.data()))
    {
        return false; // this is not a XML file
    }
    QDomDocumentType type = doc.doctype();
    if (type.name() != "PencilDocument" && type.name() != "MyObject")
    {
        return false; // this is not a Pencil document
    }

    // -----------------------------


    //QProgressDialog progress("Opening document...", "Abort", 0, 100, this);
    //progress.setWindowModality(Qt::WindowModal);
    //progress.show();

    emit loadingProgressUpdated( 0.0f );

    Object* newObject = new Object();
    if (!newObject->loadPalette(filePath+".data"))
    {
        newObject->loadDefaultPalette();
    }
    editor->setObject(newObject);

    newObject->strCurrentFilePath = filePath;

    // ------- reads the XML file -------
    bool ok = true;
    int prog = 0;
    QDomElement docElem = doc.documentElement();
    if (docElem.isNull())
    {
        return false;
    }

    if (docElem.tagName() == "document")
    {
        qDebug("Object Loader: start.");

        QDomNode tag = docElem.firstChild();
        while (!tag.isNull())
        {
            QDomElement element = tag.toElement(); // try to convert the node to an element.
            if (!element.isNull())
            {
                prog += std::min(prog + 10, 100);
                progress.setValue(prog);

                if (element.tagName() == "editor")
                {
                    qDebug("  Load editor");
                    loadDomElement(element, filePath);
                }
                else if (element.tagName() == "object")
                {
                    qDebug("  Load object");
                    ok = newObject->loadDomElement(element, filePath);
                    qDebug() << "    filePath:" << filePath;
                }
            }
            tag = tag.nextSibling();
        }
    }
    else
    {
        if (docElem.tagName() == "object" || docElem.tagName() == "MyOject")   // old Pencil format (<=0.4.3)
        {
            ok = newObject->loadDomElement(docElem, filePath);
        }
    }

    // ------------------------------
    if (ok)
    {
        editor->updateObject();

        m_recentFileMenu->addRecentFile(filePath);
        m_recentFileMenu->saveToDisk();

        qDebug() << "Current File Path=" << newObject->strCurrentFilePath;
        setWindowTitle(newObject->strCurrentFilePath);

        // FIXME: need to free the old object. but delete object will crash app, don't know why.
        m_object = newObject;
    }

    progress.setValue(100);
    */
    return NULL;
}

bool ObjectSaveLoader::saveFile(Object* object, QString strFileName, PencilError* error)
{
    Q_UNUSED(object);
    Q_UNUSED(strFileName);
    return true;
}
