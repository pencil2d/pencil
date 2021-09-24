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
#include "clipboardmanager.h"

#include <QClipboard>

#include <editor.h>

ClipboardManager::ClipboardManager(Editor* editor) : BaseManager(editor, "ClipboardManager")
{
    resetStates();
}

ClipboardManager::~ClipboardManager()
{

}

void ClipboardManager::setFromSystemClipboard(const QPointF& pos, const Layer* layer)
{
    // We intentially do not call resetStates here because we can only store image changes to the clipboard
    // otherwise we break pasting for vector.

    QImage image = mClipboard->image(QClipboard::Clipboard);
    // Only bitmap is supported currently...
    if (layer->type() == Layer::BITMAP && !image.isNull()) {
        mBitmapImage = BitmapImage(pos.toPoint()-QPoint(image.size().width()/2, image.size().height()/2), image);
    }
}

void ClipboardManager::copyBitmapImage(BitmapImage* bitmapImage, QRectF selectionRect)
{
    resetStates();
    Q_ASSERT(bitmapImage != nullptr && bitmapImage->isLoaded());

    if (!selectionRect.isEmpty())
    {
        mBitmapImage = bitmapImage->copy(selectionRect.toRect());
    }
    else
    {
        mBitmapImage = bitmapImage->copy();
    }

    mClipboard->setImage(*mBitmapImage.image());
}

void ClipboardManager::copyVectorImage(const VectorImage* vectorImage)
{
    resetStates();
    if (vectorImage == nullptr || vectorImage->isEmpty()) { return; }

    // FIXME: handle vector selections, ie. independent strokes...
    mVectorImage = *vectorImage->clone();
}

void ClipboardManager::copySelectedFrames(const Layer* currentLayer) {
    resetStates();

    for (int pos : currentLayer->selectedKeyFramesPositions()) {
        KeyFrame* keyframe = currentLayer->getKeyFrameAt(pos);

        Q_ASSERT(keyframe != nullptr);

        keyframe->loadFile();

        mFrames.insert(std::make_pair(keyframe->pos(), keyframe->clone()));
    }
    mFramesType = currentLayer->type();
}

void ClipboardManager::resetStates()
{
    mFrames.clear();
    mBitmapImage = BitmapImage();
    mVectorImage = VectorImage();
    mFramesType = Layer::LAYER_TYPE::UNDEFINED;
}
