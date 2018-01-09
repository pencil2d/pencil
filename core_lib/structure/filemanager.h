/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

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
    FileManager(QObject* parent = 0);

    Object* load(QString strFilenNme);
    Status  save(Object*, QString strFileName);

    QList<ColourRef> loadPaletteFile(QString strFilename);
    Status error() { return mError; }
    Status verifyObject(Object* obj);

Q_SIGNALS:
    void progressUpdated(float);

private:
    void unzip(const QString& strZipFile, const QString& strUnzipTarget);

    bool loadObject(Object*, const QDomElement& root);
    bool loadObjectOldWay(Object*, const QDomElement& root);
    bool isOldForamt(const QString& fileName);
    bool loadPalette(Object*);

    ObjectData* loadProjectData(const QDomElement& element);
    QDomElement saveProjectData(ObjectData*, QDomDocument& xmlDoc);

    void extractProjectData(const QDomElement& element, ObjectData* data);

    Object* cleanUpWithErrorCode(Status);

private:
    Status mError = Status::OK;
    QString mstrLastTempFolder;

    QLoggingCategory mLog;
};

#endif // OBJECTSAVELOADER_H
