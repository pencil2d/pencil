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

#ifndef LAYER_MANAGER_H
#define LAYER_MANAGER_H

#include "basemanager.h"

class Layer;
class LayerBitmap;
class LayerVector;
class LayerCamera;
class LayerSound;


class LayerManager : public BaseManager
{
    Q_OBJECT

public:
    explicit LayerManager(Editor* editor);
    ~LayerManager();
    bool init() override;
    Status load(Object*) override;
    Status save(Object*) override;

    // Layer Management
    Layer* currentLayer();
    Layer* currentLayer(int offset);
    Layer* getLayer(int index);
    Layer* getLayerByName(QString sName);
    int    getLastCameraLayer();
    int    currentLayerIndex();
    void   setCurrentLayer(int nIndex);
    void   setCurrentLayer(Layer* layer);
    int    count();

    Status deleteLayer(int index);

    void gotoNextLayer();
    void gotoPreviouslayer();

    LayerBitmap* createBitmapLayer(const QString& strLayerName);
    LayerVector* createVectorLayer(const QString& strLayerName);
    LayerCamera* createCameraLayer(const QString& strLayerName);
    LayerSound*  createSoundLayer(const QString& strLayerName);

    // KeyFrame Management
    int LastFrameAtFrame(int frameIndex);
    int firstKeyFrameIndex();
    int lastKeyFrameIndex();

    int projectLength(bool includeSounds = true);

    void layerUpdated(int layerId);

Q_SIGNALS:
    void currentLayerChanged(int n);
    void layerCountChanged(int count);

private:
    int mLastCameraLayer = 0;
};

#endif
