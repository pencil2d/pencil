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
    if (layer->type() == Layer::BITMAP) {

        QImage clipboardImage = mClipboard->image(QClipboard::Mode::Clipboard);
        if (clipboardImage.isNull()) { return false; }

        // Clipboard is upt to date
        if (*mBitmapImage.image() == clipboardImage) {
            return false;
        } else {
            // Clipboard updated while app was not in focus, update
            // We intentially do not update position here.
            mBitmapImage = BitmapImage(QPoint(), clipboardImage);
            return true;
        }
    }
    return false;
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
        break;
    }
    return false;
}

bool ClipboardManager::canPaste(const Layer* layer)
{
    if (!mFrames.empty() && mFramesType == layer->type()) {
        return true;
    } else if (mBitmapImage.isLoaded() && layer->type() == Layer::BITMAP) {
        return true;
    } else if (mVectorImage.isValid() && layer->type() == Layer::VECTOR) {
        return true;
    }
    return false;
}

void ClipboardManager::setFromSystemClipboard(const QClipboard *clipboard, Layer* layer)
{
    QImage image = clipboard->image(QClipboard::Clipboard);
    // Only bitmap is supported currently...
    if (layer->type() == Layer::BITMAP && !image.isNull()) {
        mBitmapImage = BitmapImage(image.rect().topLeft(), image);
        mBitmapImage.bounds() = QRect(mLastBitmapPosition, image.size());
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

void ClipboardManager::copyVectorImage(VectorImage* vectorImage)
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

        if (!keyframe->isLoaded()) {
            keyframe->loadFile();
        }

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
    if (!mFrames.empty()) {
        mFrames.clear();
    }
    mFrames = std::map<int, KeyFrame*>();
    mBitmapImage = BitmapImage();
    mVectorImage = VectorImage();
    mLastBitmapPosition = QPoint();
    mFramesType = Layer::LAYER_TYPE::UNDEFINED;
}
