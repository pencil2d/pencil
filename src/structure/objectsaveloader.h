#ifndef OBJECTSAVELOADER_H
#define OBJECTSAVELOADER_H

#include <functional>

#include <QObject>
#include <QString>
#include <functional>

class Object;

class ProgressInterface
{
};

class ObjectSaveLoader : public QObject
{
    Q_OBJECT

public:
    explicit ObjectSaveLoader(QObject *parent = 0);
    Object* load(QString strFilename);
    bool    save(Object* object, QString strFileName);

signals:

    public slots:
};

#endif // OBJECTSAVELOADER_H
