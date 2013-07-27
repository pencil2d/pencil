
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
        return NULL; // this is not a XML file
    }

    QDomDocumentType type = doc.doctype();
    if (type.name() != "PencilDocument" && type.name() != "MyObject")
    {
        return NULL; // this is not a Pencil document
    }

    return NULL;
}

bool ObjectSaveLoader::save(Object* object, QString strFileName)
{
    Q_UNUSED(object);
    Q_UNUSED(strFileName);
    return true;
}
