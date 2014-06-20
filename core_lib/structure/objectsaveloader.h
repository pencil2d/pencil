#ifndef OBJECTSAVELOADER_H
#define OBJECTSAVELOADER_H


#include <QObject>
#include <QString>
#include <QDomElement>
#include <QLoggingCategory>
#include "pencildef.h"
#include "pencilerror.h"
#include "colourref.h"

class Object;

class ObjectSaveLoader : public QObject
{
    Q_OBJECT

public:
    ObjectSaveLoader( QObject *parent = 0 );

    Object* load( QString strFilenNme );
    bool    save( Object* pObject, QString strFileName );

    QList<ColourRef> loadPaletteFile( QString strFilename );

    PencilError error() { return mError; }

signals:
    void progressValueChanged( float );

private:
    bool loadObject( Object*, const QDomElement& root, const QString& strDataFolder );
    bool loadObjectOladWay( Object*, const QDomElement& root, const QString& strDataFolder );

    QString extractZipToTempFolder( QString strZipFile );
    void    cleanUpTempFolder();
    bool    isFileExists( QString strFilename );
    bool    loadDomElement( QDomElement element );

    PencilError mError;
    QString mstrLastTempFolder;

    QLoggingCategory mLog;
};

#endif // OBJECTSAVELOADER_H
