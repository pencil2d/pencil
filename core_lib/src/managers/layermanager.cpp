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

#include "layermanager.h"

#include "object.h"
#include "editor.h"

#include "layersound.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layercamera.h"

#include <QDebug>

LayerManager::LayerManager(Editor* editor) : BaseManager(editor, __FUNCTION__)
{
}

LayerManager::~LayerManager()
{
}

bool LayerManager::init()
{
    return true;
}

Status LayerManager::load(Object*)
{
    mLastCameraLayerIdx = 0;
    // Do not emit layerCountChanged here because the editor has not updated to this object yet
    // Leave that to the caller of this function
    return Status::OK;
}

Status LayerManager::save(Object* o)
{
    o->data()->setCurrentLayer(editor()->currentLayerIndex());
    return Status::OK;
}

LayerCamera* LayerManager::getCameraLayerBelow(int layerIndex) const
{
    return static_cast<LayerCamera*>(object()->getLayerBelow(layerIndex, Layer::CAMERA));
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
    Q_ASSERT(object() != nullptr);
    return object()->getLayer(editor()->currentLayerIndex() + incr);
}

Layer* LayerManager::getLayer(int index)
{
    Q_ASSERT(object() != nullptr);
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
    Q_ASSERT(layerIndex < object()->getLayerCount());

    // Deselect frames of previous layer.
    Layer* previousLayer = currentLayer();
    previousLayer->deselectAll();

    emit currentLayerWillChange(layerIndex);

    // Do not check if layer index has changed
    // because the current layer may have changed either way

    editor()->setCurrentLayerIndex(layerIndex);
    emit currentLayerChanged(layerIndex);

    if (object()->getLayer(layerIndex)->type() == Layer::CAMERA)
    {
        mLastCameraLayerIdx = layerIndex;
    }

    if (currentLayer()->type() == Layer::BITMAP)
    {
        LayerBitmap* layerBit = static_cast<LayerBitmap*>(editor()->layers()->currentLayer());
        LayerCamera* layerCam = static_cast<LayerCamera*>(editor()->layers()->getLastCameraLayer());
        int w = layerCam->getViewRect().width();
        qDebug() << "BLUR: " << layerBit->getBlur(layerCam->getCameraDistance(editor()->currentFrame()),
                                      w,
                                      w / layerCam->getViewAtFrame(editor()->currentFrame()).m11(),
                                      layerCam->getAperture());
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
        currentLayer()->deselectAll();
        editor()->setCurrentLayerIndex(editor()->currentLayerIndex() + 1);
        emit currentLayerChanged(editor()->currentLayerIndex());
    }
}

void LayerManager::gotoPreviouslayer()
{
    if (editor()->currentLayerIndex() > 0)
    {
        currentLayer()->deselectAll();
        editor()->setCurrentLayerIndex(editor()->currentLayerIndex() - 1);
        emit currentLayerChanged(editor()->currentLayerIndex());
    }
}

QString LayerManager::nameSuggestLayer(const QString& name)
{
    // if no layers: return name
    if (count() == 0)
    {
        return name;
    }
    QVector<QString> sLayers;
    // fill Vector with layer names
    for (int i = 0; i < count(); i++)
    {
        sLayers.append(getLayer(i)->name());
    }
    // if name is not in list, return name
    if (!sLayers.contains(name))
    {
        return name;
    }
    int newIndex = 2;
    QString newName = name;
    do {
        newName = QStringLiteral("%1 %2")
            .arg(name, QString::number(newIndex++));
    } while (sLayers.contains(newName));
    return newName;
}

void LayerManager::sortLayersByDistance(int id)
{
    int swaps;
    do
    {
        swaps = 0;
        for (int i = 0; i < count(); i++)
        {
            Layer* layer = editor()->object()->getLayer(i);
            if (layer->type() != Layer::BITMAP && layer->type() != Layer::VECTOR)
                continue;
            int next = i + 1;
            Layer* layer2 = editor()->object()->getLayer(next);
            if ((layer2->type() == Layer::BITMAP || layer2->type() == Layer::VECTOR)
                    && (layer->getDistance() < layer2->getDistance()))
            {
                if (editor()->canSwapLayers(i, next))
                {
                    editor()->swapLayers(i, next);
                    swaps++;
                }
            }
        }
    } while (swaps > 0);

    setCurrentLayer(editor()->object()->findLayerById(id));
}

Layer* LayerManager::createLayer(Layer::LAYER_TYPE type, const QString& strLayerName)
{
    Layer* layer = nullptr;
    switch (type) {
    case Layer::BITMAP:
        layer = object()->addNewBitmapLayer();
        break;
    case Layer::VECTOR:
        layer = object()->addNewVectorLayer();
        break;
    case Layer::SOUND:
        layer = object()->addNewSoundLayer();
        break;
    case Layer::CAMERA:
        layer = object()->addNewCameraLayer();
        break;
    default:
        Q_ASSERT(true);
        return nullptr;
    }

    layer->setName(strLayerName);
    emit layerCountChanged(count());
    setCurrentLayer(getLastLayerIndex());

    return layer;
}

LayerBitmap* LayerManager::createBitmapLayer(const QString& strLayerName)
{
    LayerBitmap* layer = object()->addNewBitmapLayer();
    layer->setName(strLayerName);

    emit layerCountChanged(count());
    setCurrentLayer(getLastLayerIndex());

    return layer;
}

LayerVector* LayerManager::createVectorLayer(const QString& strLayerName)
{
    LayerVector* layer = object()->addNewVectorLayer();
    layer->setName(strLayerName);

    emit layerCountChanged(count());
    setCurrentLayer(getLastLayerIndex());

    return layer;
}

LayerCamera* LayerManager::createCameraLayer(const QString& strLayerName)
{
    LayerCamera* layer = object()->addNewCameraLayer();
    layer->setName(strLayerName);

    emit layerCountChanged(count());
    setCurrentLayer(getLastLayerIndex());

    return layer;
}

LayerSound* LayerManager::createSoundLayer(const QString& strLayerName)
{
    LayerSound* layer = object()->addNewSoundLayer();
    layer->setName(strLayerName);

    emit layerCountChanged(count());
    setCurrentLayer(getLastLayerIndex());

    return layer;
}

int LayerManager::lastFrameAtFrame(int frameIndex)
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

bool LayerManager::canDeleteLayer(int index) const
{
    return object()->canDeleteLayer(index);
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
    Q_ASSERT(object()->getLayerCount() >= 2);

    // current layer is the last layer && we are deleting it
    if (index == object()->getLayerCount() - 1 &&
        index == currentLayerIndex())
    {
        setCurrentLayer(currentLayerIndex() - 1);
    }
    object()->deleteLayer(layer);
    if (index >= currentLayerIndex())
    {
        // current layer has changed, so trigger updates
        setCurrentLayer(currentLayerIndex());
    }

    emit layerDeleted(index);
    emit layerCountChanged(count());

    return Status::OK;
}

Status LayerManager::renameLayer(Layer* layer, const QString& newName)
{
    if (newName.isEmpty()) return Status::FAIL;

    layer->setName(newName);
    emit currentLayerChanged(getIndex(layer));
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
    emit animationLengthChanged(animationLength(true));
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
