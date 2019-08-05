/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "layermanager.h"

#include "object.h"
#include "editor.h"

#include "layersound.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layercamera.h"


LayerManager::LayerManager(Editor* editor) : BaseManager(editor)
{
}

LayerManager::~LayerManager()
{
}

bool LayerManager::init()
{
    return true;
}

Status LayerManager::load(Object* o)
{
    mLastCameraLayerIdx = 0;
    emit layerCountChanged(o->getLayerCount());
    return Status::OK;
}

Status LayerManager::save(Object* o)
{
    o->data()->setCurrentLayer(editor()->currentLayerIndex());
    return Status::OK;
}

Layer* LayerManager::getLastCameraLayer()
{
    Layer* layer = object()->getLayer(mLastCameraLayerIdx);
    if (layer->type() == Layer::CAMERA)
    {
        return layer;
    }

    // it's not a camera layer
    std::vector<LayerCamera*> camLayers = object()->getLayersByType<LayerCamera>();
    if (camLayers.size() > 0)
    {
        return camLayers[0];
    }
    return nullptr;
}

Layer* LayerManager::currentLayer()
{
    Layer* layer = currentLayer(0);
    Q_ASSERT(layer != nullptr);
    return layer;
}

Layer* LayerManager::currentLayer(int incr)
{
    Q_ASSERT(object() != NULL);
    return object()->getLayer(editor()->currentLayerIndex() + incr);
}

Layer* LayerManager::getLayer(int index)
{
    Q_ASSERT(object() != NULL);
    return object()->getLayer(index);
}

Layer* LayerManager::findLayerByName(QString sName, Layer::LAYER_TYPE type)
{
    return object()->findLayerByName(sName, type);
}

int LayerManager::currentLayerIndex()
{
    return editor()->currentLayerIndex();
}

void LayerManager::setCurrentLayer(int layerIndex)
{
    Q_ASSERT(layerIndex >= 0);

    Object* o = object();
    if (layerIndex >= o->getLayerCount())
    {
        Q_ASSERT(false);
        return;
    }

    // Do not check if layer index has changed
    // because the current layer may have changed either way
    editor()->setCurrentLayerIndex(layerIndex);
    Q_EMIT currentLayerChanged(layerIndex);

    if (object())
    {
        if (object()->getLayer(layerIndex)->type() == Layer::CAMERA)
        {
            mLastCameraLayerIdx = layerIndex;
        }
    }
}

void LayerManager::setCurrentLayer(Layer* layer)
{
    setCurrentLayer(getIndex(layer));
}

void LayerManager::gotoNextLayer()
{
    if (editor()->currentLayerIndex() < object()->getLayerCount() - 1)
    {
        editor()->setCurrentLayerIndex(editor()->currentLayerIndex() + 1);
        Q_EMIT currentLayerChanged(editor()->currentLayerIndex());
    }
}

void LayerManager::gotoPreviouslayer()
{
    if (editor()->currentLayerIndex() > 0)
    {
        editor()->setCurrentLayerIndex(editor()->currentLayerIndex() - 1);
        Q_EMIT currentLayerChanged(editor()->currentLayerIndex());
    }
}

LayerBitmap* LayerManager::createBitmapLayer(const QString& strLayerName)
{
    LayerBitmap* layer = object()->addNewBitmapLayer();
    layer->setName(strLayerName);

    Q_EMIT layerCountChanged(count());

    return layer;
}

LayerVector* LayerManager::createVectorLayer(const QString& strLayerName)
{
    LayerVector* layer = object()->addNewVectorLayer();
    layer->setName(strLayerName);

    Q_EMIT layerCountChanged(count());

    return layer;
}

LayerCamera* LayerManager::createCameraLayer(const QString& strLayerName)
{
    LayerCamera* layer = object()->addNewCameraLayer();
    layer->setName(strLayerName);

    Q_EMIT layerCountChanged(count());

    return layer;
}

LayerSound* LayerManager::createSoundLayer(const QString& strLayerName)
{
    LayerSound* layer = object()->addNewSoundLayer();
    layer->setName(strLayerName);

    Q_EMIT layerCountChanged(count());

    return layer;
}

int LayerManager::LastFrameAtFrame(int frameIndex)
{
    Object* o = object();
    for (int i = frameIndex; i >= 0; i -= 1)
    {
        for (int layerIndex = 0; layerIndex < o->getLayerCount(); ++layerIndex)
        {
            auto pLayer = o->getLayer(layerIndex);
            if (pLayer->keyExists(i))
            {
                return i;
            }
        }
    }
    return -1;
}

int LayerManager::firstKeyFrameIndex()
{
    int minPosition = INT_MAX;

    Object* o = object();
    for (int i = 0; i < o->getLayerCount(); ++i)
    {
        Layer* pLayer = o->getLayer(i);

        int position = pLayer->firstKeyFramePosition();
        if (position < minPosition)
        {
            minPosition = position;
        }
    }
    return minPosition;
}

int LayerManager::lastKeyFrameIndex()
{
    int maxPosition = 0;

    for (int i = 0; i < object()->getLayerCount(); ++i)
    {
        Layer* pLayer = object()->getLayer(i);

        int position = pLayer->getMaxKeyFramePosition();
        if (position > maxPosition)
        {
            maxPosition = position;
        }
    }
    return maxPosition;
}

int LayerManager::count()
{
    return object()->getLayerCount();
}

Status LayerManager::deleteLayer(int index)
{
    Layer* layer = object()->getLayer(index);
    if (layer->type() == Layer::CAMERA)
    {
        std::vector<LayerCamera*> camLayers = object()->getLayersByType<LayerCamera>();
        if (camLayers.size() == 1)
            return Status::ERROR_NEED_AT_LEAST_ONE_CAMERA_LAYER;
    }

    object()->deleteLayer(layer);

    // current layer is the last layer && we are deleting it
    if (index == object()->getLayerCount() &&
        index == currentLayerIndex())
    {
        setCurrentLayer(currentLayerIndex() - 1);
    }
    if (index >= currentLayerIndex())
    {
        // current layer has changed, so trigger updates
        setCurrentLayer(currentLayerIndex());
    }

    Q_EMIT layerDeleted(index);
    Q_EMIT layerCountChanged(count());

    return Status::OK;
}

Status LayerManager::renameLayer(Layer* layer, const QString& newName)
{
    if (newName.isEmpty()) return Status::FAIL;

    layer->setName(newName);
    currentLayerChanged(getIndex(layer));
    return Status::OK;
}

void LayerManager::notifyLayerChanged(Layer* layer)
{
    emit currentLayerChanged(getIndex(layer));
}

/**
 * @brief Get the length of current project
 * @return int: the position of the last key frame in the timeline + its length
 */
int LayerManager::animationLength(bool includeSounds)
{
    int maxFrame = -1;

    Object* o = object();
    for (int i = 0; i < o->getLayerCount(); i++)
    {
        if (o->getLayer(i)->type() == Layer::SOUND)
        {
            if (!includeSounds)
                continue;

            Layer* soundLayer = o->getLayer(i);
            soundLayer->foreachKeyFrame([&maxFrame](KeyFrame* keyFrame)
            {
                int endPosition = keyFrame->pos() + (keyFrame->length() - 1);
                if (endPosition > maxFrame)
                {
                    maxFrame = endPosition;
                }
            });
        }
        else
        {
            int lastFramePos = o->getLayer(i)->getMaxKeyFramePosition();
            if (lastFramePos > maxFrame)
            {
                maxFrame = lastFramePos;
            }
        }
    }
    return maxFrame;
}

void LayerManager::notifyAnimationLengthChanged()
{
    emit animationLengthChanged(animationLength(false));
}

int LayerManager::getIndex(Layer* layer) const
{
    const Object* o = object();
    for (int i = 0; i < o->getLayerCount(); ++i)
    {
        if (layer == o->getLayer(i))
            return i;
    }
    return -1;
}
