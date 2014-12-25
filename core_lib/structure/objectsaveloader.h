/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2014 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

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

    Error error() { return mError; }

signals:
    void progressValueChanged( float );

private:
    bool loadObject( Object*, const QDomElement& root, const QString& strDataFolder );
    bool loadObjectOladWay( Object*, const QDomElement& root, const QString& strDataFolder );

    QString extractZipToTempFolder( QString strZipFile );
    QString createTempWorkingFolder( QString strFileName );
    void    cleanUpTempFolder();
    bool    isFileExists( QString strFilename );
    bool    loadDomElement( QDomElement element );

    Error mError;
    QString mstrLastTempFolder;

    QLoggingCategory mLog;
};

#endif // OBJECTSAVELOADER_H
