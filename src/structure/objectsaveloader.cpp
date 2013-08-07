#include "object.h"
#include "objectsaveloader.h"

ObjectSaveLoader::ObjectSaveLoader(QObject *parent) :
    QObject(parent)
{
}

Object* ObjectSaveLoader::load(QString strFilename)
{
    // ---- test before opening ----
    QScopedPointer<QFile> file(new QFile(strFilename));

    if (!file->open(QFile::ReadOnly))
    {
        return NULL;
    }

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

    QProgressDialog progress("Opening document...", "Abort", 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    //QSettings settings("Pencil","Pencil");
    //settings.setValue("lastFilePath", QVariant(object->strCurrentFilePath) );

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

    return NULL;
}

bool ObjectSaveLoader::save(Object* object, QString strFileName)
{
    Q_UNUSED(object);
    Q_UNUSED(strFileName);
    return true;
}