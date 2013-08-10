#ifndef OBJECTSAVELOADER_H
#define OBJECTSAVELOADER_H

#include <functional>

#include <QObject>
#include <QString>


class Object;

class PencilError
{
public:
    QString message;
};

class ObjectSaveLoader : public QObject
{
    Q_OBJECT

public:
    explicit ObjectSaveLoader(QObject *parent = 0);
    Object* loadFile(QString strFilename, PencilError* error);
    bool    saveFile(Object* object, QString strFileName, PencilError* error);

signals:
    void loadingProgressUpdated(float);
    void savingProgressUpdated(float);
public slots:

};

#endif // OBJECTSAVELOADER_H
