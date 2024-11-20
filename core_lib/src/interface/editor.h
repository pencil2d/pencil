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

#ifndef EDITOR_H
#define EDITOR_H

#include <functional>
#include <memory>
#include <QObject>
#include "pencilerror.h"
#include "pencildef.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
Q_MOC_INCLUDE("colormanager.h")
Q_MOC_INCLUDE("toolmanager.h")
Q_MOC_INCLUDE("layermanager.h")
Q_MOC_INCLUDE("playbackmanager.h")
Q_MOC_INCLUDE("viewmanager.h")
Q_MOC_INCLUDE("preferencemanager.h")
Q_MOC_INCLUDE("selectionmanager.h")
Q_MOC_INCLUDE("soundmanager.h")
Q_MOC_INCLUDE("overlaymanager.h")
Q_MOC_INCLUDE("clipboardmanager.h")
#endif

class QClipboard;
class QTemporaryDir;
class Object;
class KeyFrame;
class BitmapImage;
class VectorImage;
class LayerCamera;
class MainWindow2;
class BaseManager;
class ColorManager;
class ToolManager;
class LayerManager;
class PlaybackManager;
class ViewManager;
class PreferenceManager;
class SelectionManager;
class SoundManager;
class OverlayManager;
class ClipboardManager;
class ScribbleArea;
class TimeLine;
class BackupElement;
class ActiveFramePool;
class Layer;

enum class SETTING;

class Editor : public QObject
{
    Q_OBJECT

        Q_PROPERTY(ColorManager*    color    READ color)
        Q_PROPERTY(ToolManager*     tools    READ tools)
        Q_PROPERTY(LayerManager*    layers   READ layers)
        Q_PROPERTY(PlaybackManager* playback READ playback)
        Q_PROPERTY(ViewManager*     view     READ view)
        Q_PROPERTY(PreferenceManager* preference READ preference)
        Q_PROPERTY(SoundManager*    sound    READ sound)
        Q_PROPERTY(SelectionManager* select  READ select)
        Q_PROPERTY(OverlayManager*  overlays READ overlays)
        Q_PROPERTY(ClipboardManager* clipboards READ clipboards)

public:
    explicit Editor(QObject* parent = nullptr);
    ~Editor() override;

    bool init();

    /************************************************************************/
    /* Managers                                                             */
    /************************************************************************/
    ColorManager*      color() const { return mColorManager; }
    ToolManager*       tools() const { return mToolManager; }
    LayerManager*      layers() const { return mLayerManager; }
    PlaybackManager*   playback() const { return mPlaybackManager; }
    ViewManager*       view() const { return mViewManager; }
    PreferenceManager* preference() const { return mPreferenceManager; }
    SoundManager*      sound() const { return mSoundManager; }
    SelectionManager*  select() const { return mSelectionManager; }
    OverlayManager*    overlays() const { return mOverlayManager; }
    ClipboardManager*  clipboards() const { return mClipboardManager; }

    Object* object() const { return mObject.get(); }
    Status openObject(const QString& strFilePath, const std::function<void(int)>& progressChanged, const std::function<void(int)>& progressRangeChanged);
    Status setObject(Object* object);
    void updateObject();
    void prepareSave();

    void setScribbleArea(ScribbleArea* pScirbbleArea) { mScribbleArea = pScirbbleArea; }
    ScribbleArea* getScribbleArea() { return mScribbleArea; }

    int currentFrame() const;
    int fps();
    void setFps(int fps);

    int  currentLayerIndex() const { return mCurrentLayerIndex; }
    void setCurrentLayerIndex(int i);

    void scrubTo(int frameNumber);

    /**
     * @brief The visibility value should match any of the VISIBILITY enum values
     */
    void setLayerVisibility(LayerVisibility visibility);
    LayerVisibility layerVisibility();

    qreal viewScaleInversed();
    void deselectAll() const;
    void selectAll() const;

    void clipboardChanged();

    // backup
    int mBackupIndex;
    BackupElement* currentBackup();
    QList<BackupElement*> mBackupList;

signals:

    /** This should be emitted after scrubbing */
    void scrubbed(int frameNumber);

    /** This should be emitted after modifying the frame content */
    void frameModified(int frameNumber);

    /** This should be emitted after modifying multiple frames */
    void framesModified();
    void selectedFramesChanged();

    void updateTimeLine() const;
    void updateTimeLineCached();
    void updateLayerCount();
    void updateBackup();

    void objectLoaded();

    void fpsChanged(int fps);

    void needSave();
    void needDisplayInfo(const QString& title, const QString& body);
    void needDisplayInfoNoTitle(const QString& body);

    void canCopyChanged(bool enabled);
    void canPasteChanged(bool enabled);

public: //slots

    /** Will call update() and update the canvas
     * Only call this directly If you need the cache to be intact and require the frame to be repainted
    */
    void updateFrame();

    void setModified(int layerNumber, int frameNumber);

    void clearCurrentFrame();

    Status importImage(const QString& filePath);
    Status importAnimatedImage(const QString& filePath, int frameSpacing, const std::function<void (int)>& progressChanged, const std::function<bool ()>& wasCanceled);
    void restoreKey();

    void scrubNextKeyFrame();
    void scrubPreviousKeyFrame();
    void scrubForward();
    void scrubBackward();

    /**
     * Attempts to create a new keyframe at the current frame and layer. If the current frame already holds a keyframe,
     * the new one will instead be created on the first empty frame that follows. If the current layer is not visible, a
     * warning dialog will be shown to the user and no new keyframe is created.
     * @return The newly created keyframe or `nullptr` if the layer is not visible
     */
    KeyFrame* addNewKey();
    void removeKey();

    void switchVisibilityOfLayer(int layerNumber);
    void swapLayers(int i, int j);
    bool canSwapLayers(int layerIndexLeft, int layerIndexRight) const;

    void backup(const QString& undoText);
    bool backup(int layerNumber, int frameNumber, const QString& undoText);
    /**
     * Restores integrity of the backup elements after a layer has been deleted.
     * Removes backup elements affecting the deleted layer and adjusts the layer
     * index on other backup elements as necessary.
     *
     * @param layerIndex The index of the layer that was deleted
     *
     * @warning This serves as a temporary hack to prevent crashes until #864 is done
     *          (see #1412).
     */
    void sanitizeBackupElementsAfterLayerDeletion(int layerIndex);

    void onCurrentLayerWillChange(int index);
    void undo();
    void redo();

    void copy();
    void copyAndCut();
    void pasteFromPreviousFrame();
    void paste();

    bool canCopy() const;
    bool canPaste() const;

    void increaseLayerVisibilityIndex();
    void decreaseLayerVisibilityIndex();
    void flipSelection(bool flipVertical);
    void repositionImage(QPoint transform, int frame);

    void clearTemporary();
    void addTemporaryDir(QTemporaryDir* dir);

    void settingUpdated(SETTING);

    void dontAskAutoSave(bool b) { mAutosaveNeverAskAgain = b; }
    bool autoSaveNeverAskAgain() const { return mAutosaveNeverAskAgain; }
    void resetAutoSaveCounter();

private:
    Status importBitmapImage(const QString&);
    Status importVectorImage(const QString&);

    void pasteToCanvas(BitmapImage* bitmapImage, int frameNumber);
    void pasteToCanvas(VectorImage* vectorImage, int frameNumber);
    void pasteToFrames();

    bool canCopyBitmapImage(BitmapImage* bitmapImage) const;
    bool canCopyFrames(const Layer* layer) const;
    bool canCopyVectorImage(const VectorImage* vectorImage) const;

    // the object to be edited by the editor
    std::unique_ptr<Object> mObject;

    int mFrame = 1; // current frame number.
    int mCurrentLayerIndex = 0; // the current layer to be edited/displayed

    ScribbleArea* mScribbleArea = nullptr;

    ColorManager*      mColorManager = nullptr;
    ToolManager*       mToolManager = nullptr;
    LayerManager*      mLayerManager = nullptr;
    PlaybackManager*   mPlaybackManager = nullptr;
    ViewManager*       mViewManager = nullptr;
    PreferenceManager* mPreferenceManager = nullptr;
    SoundManager*      mSoundManager = nullptr;
    SelectionManager*  mSelectionManager = nullptr;
    OverlayManager*    mOverlayManager = nullptr;
    ClipboardManager*  mClipboardManager = nullptr;

    std::vector< BaseManager* > mAllManagers;

    bool mIsAutosave = true;
    int mAutosaveNumber = 12;
    int mAutosaveCounter = 0;
    bool mAutosaveNeverAskAgain = false;

    void makeConnections();
    /**
     * Attempts to create a new keyframe at the given position and layer. If a keyframe already exists at the position,
     * the new one will instead be created on the first empty frame that follows. If the layer is not visible, a warning
     * dialog will be shown to the user and no new keyframe is created.
     * @param layerNumber The number of an existing layer on which the keyframe is to be created
     * @param frameIndex The desired position of the new keyframe
     * @return The newly created keyframe or `nullptr` if the layer is not visible
     * @see Editor::addNewKey()
     */
    KeyFrame* addKeyFrame(int layerNumber, int frameIndex);

    QList<QTemporaryDir*> mTemporaryDirs;

    // backup
    void clearUndoStack();
    void updateAutoSaveCounter();
    int mLastModifiedFrame = -1;
    int mLastModifiedLayer = -1;
};

#endif
