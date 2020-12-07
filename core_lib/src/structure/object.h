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


struct ExportMovieParameters
{
    int startFrame;
    int endFrame;
    QTransform view;
    Layer* currentLayer;
    QSize exportSize;
    QString filePath;
    int fps;
    int exportFps;
    QString exportFormat;
};


class Object : public QObject
{
    Q_OBJECT

public:
    Object(QObject* parent = nullptr);
    virtual ~Object();

    void init();
    void createWorkingDir();
    void deleteWorkingDir() const;
    void setWorkingDir(const QString& path); // used by crash recovery
    void createDefaultLayers();

    QString filePath() const { return mFilePath; }
    void    setFilePath(QString strFileName) { mFilePath = strFileName; }

    QString workingDir() const { return mWorkingDirPath; }

    QString dataDir() const { return mDataDirPath; }
    void    setDataDir(QString dirPath) { mDataDirPath = dirPath; }

    QString mainXMLFile() const { return mMainXMLFile; }
    void    setMainXMLFile(QString file) { mMainXMLFile = file; }

    QDomElement saveXML(QDomDocument& doc) const;
    bool loadXML(QDomElement element, ProgressCallback progressForward);

    void paintImage(QPainter& painter, int frameNumber, bool background, bool antialiasing) const;

    QString copyFileToDataFolder(QString strFilePath);

    // Color palette
    ColorRef getColor(int index) const;
    void setColor(int index, QColor newColor);
    void setColorRef(int index, ColorRef newColorRef);
    void addColor(QColor);
    void movePaletteColor(int start, int end);
    void moveVectorColor(int start, int end);

    void addColor(ColorRef newColor) { mPalette.append(newColor); }
    void addColorAtIndex(int index, ColorRef newColor);
    void removeColor(int index);
    bool isColorInUse(int index);
    void renameColor(int i, QString text);
    int getColorCount() { return mPalette.size(); }
    bool importPalette(QString filePath);
    void importPaletteGPL(QFile& file);
    void importPalettePencil(QFile& file);
    void openPalette(QString filePath);

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
    Layer* findLayerByName(QString strName, Layer::LAYER_TYPE type = Layer::UNDEFINED) const;
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
    bool exportX(int frameStart, int frameEnd, QTransform view, QSize exportSize, QString filePath, bool antialiasing);
    bool exportIm(int frameStart, QTransform view, QSize cameraSize, QSize exportSize, QString filePath, QString format, bool antialiasing, bool transparency) const;

    void modification() { modified = true; }
    bool isModified() { return modified; }
    void setModified(bool b) { modified = b; }

    int getUniqueLayerID();

    ObjectData* data() const;
    void setData(ObjectData*);

    int totalKeyFrameCount() const;
    void updateActiveFrames(int frame) const;
    void setActiveFramePoolSize(int sizeInMB);

signals:
    void layerViewChanged();
    void paletteImported();

private:
    int getMaxLayerID();

    QString mFilePath;       //< where this object come from. (empty if new project)
    QString mWorkingDirPath; //< the folder that pclx will uncompress to.
    QString mDataDirPath;    //< the folder which contains all bitmap & vector image & sound files.
    QString mMainXMLFile;    //< the location of main.xml

    QList<Layer*> mLayers;
    bool modified = false;

    QList<ColorRef> mPalette;

    std::unique_ptr<ObjectData> mData;
    mutable std::unique_ptr<ActiveFramePool> mActiveFramePool;
};


#endif
