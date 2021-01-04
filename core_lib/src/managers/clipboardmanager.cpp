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

ClipboardManager::ClipboardManager(Editor* editor) : BaseManager(editor)
{
    resetStates();
}

ClipboardManager::~ClipboardManager()
{

}

bool ClipboardManager::updateIfNeeded(const Layer* layer)
{
    if (layer->type() != Layer::BITMAP) {
        return false;
    }

    QImage clipboardImage = mClipboard->image(QClipboard::Mode::Clipboard);
    if (clipboardImage.isNull()) { return false; }

    // Clipboard updated while app was not in focus, update
    // We intentionally do not update position here.
    mBitmapImage = BitmapImage(QPoint(), clipboardImage);
    return true;
}

void ClipboardManager::setFromSystemClipboard(const QClipboard *clipboard, const Layer* layer)
{
    QImage image = clipboard->image(QClipboard::Clipboard);
    // Only bitmap is supported currently...
    if (layer->type() == Layer::BITMAP && !image.isNull()) {
        mBitmapImage = BitmapImage(mLastBitmapPosition, image);
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

    mLastBitmapPosition = mBitmapImage.topLeft();
    mClipboard->setImage(*mBitmapImage.image());
}

void ClipboardManager::copyVectorImage(const VectorImage* vectorImage)
{
    resetStates();
    if (vectorImage == nullptr || !vectorImage->isValid()) { return; }

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
    mLastBitmapPosition = QPoint();
    mFramesType = Layer::LAYER_TYPE::UNDEFINED;
}
