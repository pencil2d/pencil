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

void BackupBitmapElement::restore(Editor* editor)
{
    Layer* layer = editor->object()->getLayer(this->layer);
    auto selectMan = editor->select();
    selectMan->setSelection(mySelection, true);
    selectMan->setTransformedSelectionRect(myTransformedSelection);
    selectMan->setTempTransformedSelectionRect(myTempTransformedSelection);
    selectMan->setRotation(rotationAngle);
    selectMan->setSomethingSelected(somethingSelected);

    editor->updateFrame(this->frame);
    editor->scrubTo(this->frame);

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
                auto pLayerBitmap = static_cast<LayerBitmap*>(layer);
                *pLayerBitmap->getLastBitmapImageAtFrame(this->frame, 0) = this->bitmapImage;  // restore the image
            }
        }
    }
}

void BackupVectorElement::restore(Editor* editor)
{
    Layer* layer = editor->object()->getLayer(this->layer);
    auto selectMan = editor->select();
    selectMan->setSelection(mySelection, false);
    selectMan->setTransformedSelectionRect(myTransformedSelection);
    selectMan->setTempTransformedSelectionRect(myTempTransformedSelection);
    selectMan->setRotation(rotationAngle);
    selectMan->setSomethingSelected(somethingSelected);

    editor->updateFrameAndVector(this->frame);
    editor->scrubTo(this->frame);
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
}

void BackupSoundElement::restore(Editor* editor)
{
    Layer* layer = editor->object()->getLayer(this->layer);
    editor->updateFrame(this->frame);
    editor->scrubTo(this->frame);

    // TODO: soundclip won't restore if overlapping on first frame
    if (this->frame > 0 && layer->getKeyFrameAt(this->frame) == nullptr)
    {
        editor->restoreKey();
    }
}
