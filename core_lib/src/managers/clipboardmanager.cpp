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
#include <QGuiApplication>

#include <editor.h>

ClipboardManager::ClipboardManager(Editor* editor) : BaseManager(editor, "ClipboardManager")
{
    resetStates();
}

ClipboardManager::~ClipboardManager()
{
    for (auto it : mFrames)
    {
        KeyFrame* frame = it.second;
        delete frame;
    }
}

void ClipboardManager::setFromSystemClipboard(const QPointF& pos, const Layer* layer)
{
    const QClipboard *clipboard = QGuiApplication::clipboard();

    // We intentially do not call resetStates here because we can only store image changes to the clipboard
    // otherwise we break pasting for vector.
    // Only bitmap is supported currently...
    // Only update clipboard data if it was stored by other applications
    if (layer->type() != Layer::BITMAP || clipboard->ownsClipboard()) {
        return;
    }

    QImage image = clipboard->image(QClipboard::Clipboard);
    if (!image.isNull()) {
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

    QGuiApplication::clipboard()->setImage(*mBitmapImage.image());
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

std::map<int, KeyFrame*> ClipboardManager::getClipboardFrames()
{
    std::map<int, KeyFrame*> resultMap;
    for (auto it : mFrames)
    {
        resultMap.insert(std::make_pair(it.first, it.second->clone()));
    }
    return resultMap;
}

void ClipboardManager::resetStates()
{
    for (auto it : mFrames)
    {
        KeyFrame* frame = it.second;
        delete frame;
    }
    mFrames.clear();

    mBitmapImage = BitmapImage();
    mVectorImage = VectorImage();
    mFramesType = Layer::LAYER_TYPE::UNDEFINED;
}
