#include "clipboardmanager.h"

#include <QClipboard>

#include <editor.h>

ClipboardManager::ClipboardManager(Editor* editor) : mEditor(editor)
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

    // Clipboard is up to date
    if (*mBitmapImage.image() == clipboardImage) {
        return false;
    }

    // Clipboard updated while app was not in focus, update
    // We intentionally do not update position here.
    mBitmapImage = BitmapImage(QPoint(), clipboardImage);
    return true;
}

bool ClipboardManager::canCopy(int keyPos, const Layer* layer) const
{
    KeyFrame* keyframe = layer->getLastKeyFrameAtPosition(keyPos);

    switch (layer->type())
    {
    case Layer::SOUND:
    case Layer::CAMERA:
        return canCopyFrames(layer);
    case Layer::BITMAP:
        return canCopyBitmapImage(static_cast<BitmapImage*>(keyframe)) || canCopyFrames(layer);
    case Layer::VECTOR:
        return canCopyVectorImage(static_cast<VectorImage*>(keyframe)) || canCopyFrames(layer);
    default:
        Q_UNREACHABLE();
    }
}

bool ClipboardManager::canPaste(const Layer* layer) const
{
    return (layer->type() == mFramesType && !mFrames.empty()) ||
           (layer->type() == Layer::BITMAP && mBitmapImage.isLoaded()) ||
           (layer->type() == Layer::VECTOR && mVectorImage.isValid());
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
    if (bitmapImage == nullptr || !bitmapImage->isLoaded()) { return; }

    else if (!selectionRect.isEmpty())
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

bool ClipboardManager::canCopyFrames(const Layer* layer) const
{
    Q_ASSERT(layer != nullptr);
    return layer->hasAnySelectedFrames();
}

bool ClipboardManager::canCopyBitmapImage(BitmapImage* bitmapImage) const
{
    return bitmapImage != nullptr && bitmapImage->isLoaded() && !bitmapImage->bounds().isEmpty();
}

bool ClipboardManager::canCopyVectorImage(const VectorImage* vectorImage) const
{
    return vectorImage != nullptr && vectorImage->isValid();
}

void ClipboardManager::resetStates()
{
    mFrames.clear();
    mBitmapImage = BitmapImage();
    mVectorImage = VectorImage();
    mLastBitmapPosition = QPoint();
    mFramesType = Layer::LAYER_TYPE::UNDEFINED;
}
