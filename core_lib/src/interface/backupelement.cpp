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

#include "backupelement.h"

#include "editor.h"
#include "layer.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "object.h"
#include "selectionmanager.h"
#include "layermanager.h"

void BackupBitmapElement::restore(Editor* editor)
{
    Layer* layer = editor->object()->findLayerById(this->layerId);

    if (editor->currentFrame() != this->frame) {
        editor->scrubTo(this->frame);
    }

    editor->layers()->setCurrentLayer(layer);

    if (this->frame > 0 && layer->getKeyFrameAt(this->frame) == nullptr)
    {
        editor->restoreKey();
    }
    else
    {
        if (layer != nullptr)
        {
            if (layer->type() == Layer::BITMAP)
            {
                auto bitmapLayer = static_cast<LayerBitmap*>(layer);
                *bitmapLayer->getLastBitmapImageAtFrame(this->frame, 0) = bitmapImage;  // restore the image
            }
        }
    }

    auto selectMan = editor->select();
    selectMan->setSelection(mySelection, true);
    selectMan->setTransformAnchor(selectionAnchor);
    selectMan->setRotation(rotationAngle);
    selectMan->setScale(scaleX, scaleY);
    selectMan->setTranslation(translation);

    selectMan->calculateSelectionTransformation();

    editor->frameModified(this->frame);
}

void BackupVectorElement::restore(Editor* editor)
{
    Layer* layer = editor->object()->findLayerById(this->layerId);
    for (int i = 0; i < editor->object()->getLayerCount(); i++)
    {
        Layer* layer = editor->object()->getLayer(i);
        if (layer->type() == Layer::VECTOR)
        {
            VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getVectorImageAtFrame(this->frame);
            if (vectorImage != nullptr)
            {
                vectorImage->modification();
            }
        }
    }

    if (editor->currentFrame() != this->frame) {
        editor->scrubTo(this->frame);
    }

    editor->layers()->setCurrentLayer(layer);

    if (this->frame > 0 && layer->getKeyFrameAt(this->frame) == nullptr)
    {
        editor->restoreKey();
    }
    else
    {
        if (layer != nullptr)
        {
            if (layer->type() == Layer::VECTOR)
            {
                auto pVectorImage = static_cast<LayerVector*>(layer);
                *pVectorImage->getLastVectorImageAtFrame(this->frame, 0) = this->vectorImage;  // restore the image
            }
        }
    }

    auto selectMan = editor->select();
    selectMan->setSelection(mySelection, false);
    selectMan->setTransformAnchor(selectionAnchor);
    selectMan->setRotation(rotationAngle);
    selectMan->setScale(scaleX, scaleY);
    selectMan->setTranslation(translation);
    selectMan->calculateSelectionTransformation();

    editor->frameModified(this->frame);

}

void BackupSoundElement::restore(Editor* editor)
{
    Layer* layer = editor->object()->findLayerById(this->layerId);

    editor->layers()->setCurrentLayer(layer);

    if (editor->currentFrame() != this->frame) {
        editor->scrubTo(this->frame);
    }
    editor->frameModified(this->frame);

    // TODO: soundclip won't restore if overlapping on first frame
    if (this->frame > 0 && layer->getKeyFrameAt(this->frame) == nullptr)
    {
        editor->restoreKey();
    }
}
