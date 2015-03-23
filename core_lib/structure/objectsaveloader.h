#ifndef OBJECTSAVELOADER_H
#define OBJECTSAVELOADER_H


#include <QObject>
#include <QString>
#include <QDomElement>
#include "log.h"
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

    Status error() { return mError; }

signals:
    void progressValueChanged( float );

private:
    bool loadObject( Object*, const QDomElement& root, const QString& strDataFolder );
    bool loadObjectOldWay( Object*, const QDomElement& root, const QString& strDataFolder );

    QString extractZipToTempFolder( QString strZipFile );
    QString createTempWorkingFolder( QString strFileName );
    void    cleanUpTempFolder();
    bool    isFileExists( QString strFilename );
    bool    loadDomElement( QDomElement element );

    Status mError;
    QString mstrLastTempFolder;

    QLoggingCategory mLog;
};

#endif // OBJECTSAVELOADER_H
