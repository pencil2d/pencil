#ifndef CLIPBOARDMANAGER_H
#define CLIPBOARDMANAGER_H

#include "bitmapimage.h"
#include "vectorimage.h"
#include "layer.h"

class Editor;
class QClipboard;
class KeyFrame;

class ClipboardManager: QObject
{
    Q_OBJECT
public:
    ClipboardManager(Editor* editor);
    ~ClipboardManager();

    /** Update latest locally stored clipboard if needed
     * @param layer
     * @return true the clipboard has been updated otherwhise false
     */
    bool updateIfNeeded(const Layer* layer);

    bool canCopy(int keyPos, const Layer* layer) const;
    bool canPaste(const Layer* layer);

    void setFromSystemClipboard(const QClipboard* clipboard, Layer* layer);

    /** Copy bitmap image to clipboard and save its latest position
     *  Additionally only a part of the image will be copied if a rect is given
     * @param image
     * @param selectionRect
     */
    void copyBitmapImage(BitmapImage* image, QRectF selectionRect);

    /** Copy the entire vector image to clipboard,
     *  this operation does not yet support partial selections
     * @param vectorImage
     */
    void copyVectorImage(VectorImage* vectorImage);

    /** Copy selected keyframes of any given layer and remember its type.
     * @param currentLayer
     */
    void copySelectedFrames(const Layer* currentLayer);

    BitmapImage getBitmapClipboard() const { return mBitmapImage; }
    VectorImage getVectorClipboard() const { return mVectorImage; }
    std::map<int, KeyFrame*> getClipboardFrames() { return mFrames; }

private:

    bool canCopyBitmapImage(BitmapImage* image) const;
    bool canCopyFrames(const Layer* layer) const;
    bool canCopyVectorImage(const VectorImage* image) const;

    /** This should be called before copying and updating the clipboard to ensure no previous state is saved */
    void resetStates();

    Editor* mEditor;

    QPoint mLastBitmapPosition;
    BitmapImage mBitmapImage;
    VectorImage mVectorImage;
    std::map<int, KeyFrame*> mFrames;
    Layer::LAYER_TYPE mFramesType = Layer::LAYER_TYPE::UNDEFINED;

    QClipboard* mClipboard;
};

#endif // CLIPBOARDMANAGER_H
