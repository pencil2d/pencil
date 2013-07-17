
#include "object.h"
#include "objectsaveloader.h"

ObjectSaveLoader::ObjectSaveLoader(QObject *parent) :
    QObject(parent)
{
}

Object* ObjectSaveLoader::load(QString strFilename, std::function<void(float)> progress)
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


}

bool ObjectSaveLoader::save(Object* object, QString strFileName, std::function<void(float)> progress)
{
    return true;
}
