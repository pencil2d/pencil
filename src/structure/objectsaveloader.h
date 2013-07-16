#ifndef OBJECTSAVELOADER_H
#define OBJECTSAVELOADER_H

#include <QObject>
#include <QString>
#include <functional>

class Object;


class ObjectSaveLoader : public QObject
{
    Q_OBJECT
public:
    explicit ObjectSaveLoader(QObject *parent = 0);
    Object* load(QString strFilename, std::function<void(float)> progress);
    bool    save(Object* object, QString strFileName, std::function<void(float)> progress);

signals:
    
public slots:
    
};

#endif // OBJECTSAVELOADER_H
