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
#ifndef OBJECT_H
#define OBJECT_H

#include <memory>
#include <QCoreApplication>
#include <QObject>
#include <QList>
#include <QColor>
#include "layer.h"
#include "colorref.h"
#include "pencilerror.h"
#include "pencildef.h"
#include "objectdata.h"

class QProgressDialog;
class QFile;
class LayerBitmap;
class LayerVector;
class LayerCamera;
class LayerSound;
class ObjectData;
class ActiveFramePool;


class Object final
{
    Q_DECLARE_TR_FUNCTIONS(Object)
public:
    explicit Object();
    ~Object();

	Object(Object const&) = delete;
	Object(Object&&) = delete;
	Object& operator=(Object const&) = delete;
	Object& operator=(Object&&) = delete;

    void init();
    void createWorkingDir();
    void deleteWorkingDir() const;
    void setWorkingDir(const QString& path); // used by crash recovery

    QString filePath() const { return mFilePath; }
    void    setFilePath(const QString& strFileName) { mFilePath = strFileName; }

    QString workingDir() const { return mWorkingDirPath; }

    QString dataDir() const { return mDataDirPath; }
    void    setDataDir(const QString& dirPath) { mDataDirPath = dirPath; }

    QString mainXMLFile() const { return mMainXMLFile; }
    void    setMainXMLFile(const QString& file) { mMainXMLFile = file; }

    QDomElement saveXML(QDomDocument& doc) const;
    bool loadXML(const QDomElement& element, ProgressCallback progressForward);

    void paintImage(QPainter& painter, int frameNumber, bool background, bool antialiasing) const;

    QString copyFileToDataFolder(const QString& strFilePath);

    // Color palette
    ColorRef getColor(int index) const;
    void setColor(int index, const QColor& newColor);
    void setColorRef(int index, const ColorRef& newColorRef);
    void movePaletteColor(int start, int end);
    void moveVectorColor(int start, int end);

    void addColor(const ColorRef& newColor) { mPalette.append(newColor); }
    void addColorAtIndex(int index, const ColorRef& newColor);
    void removeColor(int index);
    bool isColorInUse(int index) const;
    void renameColor(int i, const QString& text);
    int getColorCount() { return mPalette.size(); }
    bool importPalette(const QString& filePath);
    void importPaletteGPL(QFile& file);
    void importPalettePencil(QFile& file);
    void openPalette(const QString& filePath);

    bool exportPalette(const QString& filePath) const;
    void exportPaletteGPL(QFile& file) const;
    void exportPalettePencil(QFile& file) const;
    QString savePalette(const QString& filePath) const;

    void loadDefaultPalette();

    LayerBitmap* addNewBitmapLayer();
    LayerVector* addNewVectorLayer();
    LayerSound* addNewSoundLayer();
    LayerCamera* addNewCameraLayer();

    int  getLayerCount() const;
    Layer* getLayer(int i) const;
    Layer* findLayerByName(const QString& strName, Layer::LAYER_TYPE type = Layer::UNDEFINED) const;
    Layer* findLayerById(int layerId) const;
    Layer* takeLayer(int layerId); // Note: transfer ownership of the layer

    bool swapLayers(int i, int j);
    void deleteLayer(int i);
    void deleteLayer(Layer*);
    bool addLayer(Layer* layer);

    template<typename T>
    std::vector<T*> getLayersByType() const
    {
        std::vector<T*> result;
        for (Layer* layer : mLayers)
        {
            T* t = dynamic_cast<T*>(layer);
            if (t)
                result.push_back(t);
        }
        return result;
    }

    // these functions need to be moved to somewhere...
    bool exportFrames(int frameStart, int frameEnd, const LayerCamera* cameraLayer, QSize exportSize, QString filePath, QString format,
                      bool transparency, bool exportKeyframesOnly, const QString& layerName, bool antialiasing, QProgressDialog* progress, int progressMax) const;

    bool exportIm(int frameStart, const QTransform& view, QSize cameraSize, QSize exportSize, const QString& filePath, const QString& format, bool antialiasing, bool transparency) const;

    void modification() { modified = true; }
    bool isModified() const { return modified; }
    void setModified(bool b) { modified = b; }

    int getUniqueLayerID();

    ObjectData* data() { return &mData; }
    const ObjectData* data() const { return &mData; }
    void setData(const ObjectData&);

    int totalKeyFrameCount() const;
    void updateActiveFrames(int frame) const;
    void setActiveFramePoolSize(int sizeInMB);

private:
    int getMaxLayerID();

    QString mFilePath;       //< where this object come from. (empty if new project)
    QString mWorkingDirPath; //< the folder that pclx will uncompress to.
    QString mDataDirPath;    //< the folder which contains all bitmap & vector image & sound files.
    QString mMainXMLFile;    //< the location of main.xml

    QList<Layer*> mLayers;
    bool modified = false;

    QList<ColorRef> mPalette;

    ObjectData mData;
    mutable std::unique_ptr<ActiveFramePool> mActiveFramePool;
};


#endif
