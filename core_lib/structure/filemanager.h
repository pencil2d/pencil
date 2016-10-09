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
class ObjectData;


class FileManager : public QObject
{
    Q_OBJECT

public:
    FileManager( QObject* parent = 0 );

    Object* load( QString strFilenNme );
    Status  save( Object* pObject, QString strFileName );

    QList<ColourRef> loadPaletteFile( QString strFilename );

    Status error() { return mError; }

Q_SIGNALS:
    void progressUpdated( float );

private:
    bool loadObject( Object*, const QDomElement& root );
    bool loadObjectOldWay( Object*, const QDomElement& root );

    bool isOldForamt( const QString& fileName );

    void unzip( const QString& strZipFile, const QString& strUnzipTarget );
    
    Object* cleanUpWithErrorCode( Status );
    
    bool loadPalette( Object* );
    ObjectData* loadEditorState( QDomElement element );

    void extractEditorStateData( const QDomElement& element, ObjectData* data );

    Status mError = Status::OK;
    QString mstrLastTempFolder;

    QLoggingCategory mLog;
};

#endif // OBJECTSAVELOADER_H
