/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

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
#include "colorref.h"
#include "layer.h"

class Object;
class ObjectData;


class FileManager : public QObject
{
    Q_OBJECT

public:
    FileManager(QObject* parent = 0);

    Object* load(const QString& sFilenNme);
    Status  save(const Object*, const QString& sFileName);
    Status  writeToWorkingFolder(const Object*);

    QList<ColorRef> loadPaletteFile(QString strFilename);
    Status error() const { return mError; }
    Status verifyObject(Object* obj);

    QStringList searchForUnsavedProjects();
    Object* recoverUnsavedProject(QString projectIntermediatePath);

signals:
    void progressChanged(int progress);
    void progressRangeChanged(int maxValue);

private:
    Status unzip(const QString& strZipFile, const QString& strUnzipTarget);

    bool loadObject(Object*, const QDomElement& root);
    bool loadObjectOldWay(Object*, const QDomElement& root);
    bool isArchiveFormat(const QString& fileName) const;
    bool loadPalette(Object*);
    Status writeKeyFrameFiles(const Object* obj, const QString& dataFolder, QStringList& filesWritten);
    Status writeMainXml(const Object* obj, const QString& mainXmlPath, QStringList& filesWritten);
    Status writePalette(const Object* obj, const QString& dataFolder, QStringList& filesWritten);

    ObjectData loadProjectData(const QDomElement& element);
    QDomElement saveProjectData(const ObjectData*, QDomDocument& xmlDoc);

    void extractProjectData(const QDomElement& element, ObjectData& data);
    void handleOpenProjectError(Status::ErrorCode, const DebugDetails&);

    QString backupPreviousFile(const QString& fileName);
    void deleteBackupFile(const QString& fileName);
    int countExistingBackups(const QString& fileName) const;

    void progressForward();

private: // Project recovery
    bool isProjectRecoverable(const QString& projectFolder);
    Status recoverObject(Object* object);
    Status rebuildMainXML(Object* object);
    Status rebuildLayerXmlTag(QDomDocument& doc, QDomElement& elemObject,
                              const int layerIndex, const QStringList& frames);
    QString recoverLayerName(Layer::LAYER_TYPE, int index);
    int layerIndexFromFilename(const QString& filename);
    int framePosFromFilename(const QString& filename);

private:
    Status mError = Status::OK;
    QString mstrLastTempFolder;

    int mCurrentProgress = 0;
    int mMaxProgressValue = 100;
};

#endif // OBJECTSAVELOADER_H
