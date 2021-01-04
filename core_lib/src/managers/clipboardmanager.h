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
#ifndef CLIPBOARDMANAGER_H
#define CLIPBOARDMANAGER_H

#include "basemanager.h"

#include "bitmapimage.h"
#include "vectorimage.h"
#include "layer.h"

class Editor;
class QClipboard;
class KeyFrame;

class ClipboardManager: public BaseManager
{
    Q_OBJECT
public:
    explicit ClipboardManager(Editor* editor);
    ~ClipboardManager() override;

    bool init() override { return true; }
    Status load(Object*) override { return Status::OK; }
    Status save(Object*) override { return Status::OK; }
    void workingLayerChanged(Layer*) override { }

    /** Update latest locally stored clipboard if needed
     * @param layer
     * @return true if the clipboard has been updated, otherwise false
     */
    bool updateIfNeeded(const Layer* layer);

    void setFromSystemClipboard(const QClipboard* clipboard, const Layer* layer);

    /** Copy bitmap image to clipboard and save its latest position
     *  Additionally only a part of the image will be copied if a non-empty rect is given
     * @param image
     * @param selectionRect
     */
    void copyBitmapImage(BitmapImage* image, QRectF selectionRect);

    /** Copy the entire vector image to clipboard,
     *  this operation does not yet support partial selections
     * @param vectorImage
     */
    void copyVectorImage(const VectorImage* vectorImage);

    /** Copy selected keyframes of any given layer and remember its type.
     * @param currentLayer
     */
    void copySelectedFrames(const Layer* currentLayer);

    BitmapImage getBitmapClipboard() const { return mBitmapImage; }
    VectorImage getVectorClipboard() const { return mVectorImage; }
    std::map<int, KeyFrame*> getClipboardFrames() { return mFrames; }

    bool framesTypeChanged(const Layer* layer) const { return layer->type() != mFramesType; }
    bool anyFramesInClipbord() const { return !mFrames.empty(); }

private:

    /** This should be called before copying and updating the clipboard to ensure no previous state is saved */
    void resetStates();

    QPoint mLastBitmapPosition;
    BitmapImage mBitmapImage;
    VectorImage mVectorImage;
    std::map<int, KeyFrame*> mFrames;
    Layer::LAYER_TYPE mFramesType = Layer::LAYER_TYPE::UNDEFINED;

    QClipboard* mClipboard = nullptr;
};

#endif // CLIPBOARDMANAGER_H
