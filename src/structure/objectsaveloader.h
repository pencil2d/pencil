#ifndef OBJECTSAVELOADER_H
#define OBJECTSAVELOADER_H


#include <QObject>
#include <QString>
#include "pencildef.h"

class Object;



class ObjectSaveLoader : public QObject
{
    Q_OBJECT

public:
    explicit ObjectSaveLoader(QObject *parent = 0);

    Object* loadFromFile(QString strFilename);
    bool    saveToFile(Object* pObject, QString strFileName);

    void    cleanUpTempFolder();
    QString errorMessage() { return m_strLastErrorMessage; }
    PencilError error() { return m_error; }

signals:
    void progressValueChanged(float);

private:
    QString extractZipToTempFolder( QString strZipFile );

    PencilError m_error;
    QString m_strLastErrorMessage;
    QString m_strLastTempWorkingFolder;
};

#endif // OBJECTSAVELOADER_H
