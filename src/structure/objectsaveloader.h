#ifndef OBJECTSAVELOADER_H
#define OBJECTSAVELOADER_H


#include <QObject>
#include <QString>
#include "pencildef.h"
#include "colourref.h"

class Object;



class ObjectSaveLoader : public QObject
{
    Q_OBJECT

public:
    explicit ObjectSaveLoader(QObject *parent = 0);

    Object* loadFromFile(QString strFilename);
    bool    saveToFile(Object* pObject, QString strFileName);
    QList<ColourRef> loadPaletteFile( QString strFilename );


    PencilError error() { return m_error; }

signals:
    void progressValueChanged(float);

private:
    QString extractZipToTempFolder( QString strZipFile );
    void    cleanUpTempFolder();


    PencilError m_error;
    QString m_strLastTempWorkingFolder;
};

#endif // OBJECTSAVELOADER_H
