#ifndef OBJECTSAVELOADER_H
#define OBJECTSAVELOADER_H

#include <functional>

#include <QObject>
#include <QString>


class Object;


class ObjectSaveLoader : public QObject
{
    Q_OBJECT

public:
    explicit ObjectSaveLoader(QObject *parent = 0);

    Object* loadFromFile(QString strFilename);
    bool    saveToFile(Object* pObject, QString strFileName);

    void    cleanUpTempFolder();
    QString error() { return m_strLastErrorMessage; }

signals:
    void progressValueChanged(float);

private:
    QString extractZipToTempFolder( QString strZipFile );

    QString m_strLastErrorMessage;
    QString m_strLastTempWorkingFolder;
};

#endif // OBJECTSAVELOADER_H
