/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

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

#include <memory>
#include <QObject>
#include <QList>
#include "pencilerror.h"


class QDragEnterEvent;
class QDropEvent;
class Object;
class KeyFrame;
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
class KeyFrameManager;
class CanvasManager;
class SoundManager;
class ScribbleArea;
class TimeLine;
class BackupElement;

class BackupManager;
class ActiveFramePool;


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
        Q_PROPERTY(SelectionManager* select READ select)
        Q_PROPERTY(BackupManager*   backups  READ backups)
        Q_PROPERTY(KeyFrameManager*   keyframes  READ keyframes)
        Q_PROPERTY(CanvasManager*   canvas  READ canvas)

public:
    explicit Editor(QObject* parent = nullptr);
    virtual ~Editor();

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
    BackupManager*     backups() const { return mBackupManager; }
    KeyFrameManager*   keyframes() const { return mKeyFrameManager; }
    CanvasManager*     canvas() const { return mCanvasManager; }

    Object* object() const { return mObject.get(); }
    Status setObject(Object* object);
    void updateObject();
    void prepareSave();

    void setScribbleArea(ScribbleArea* pScirbbleArea) { mScribbleArea = pScirbbleArea; }
    ScribbleArea* getScribbleArea() { return mScribbleArea; }

    int currentFrame();
    int fps();

    int  currentLayerIndex() { return mCurrentLayerIndex; }
    void setCurrentLayerIndex(int i);

    void scrubTo(int frameNumber);

    int  allLayers();
    bool exportSeqCLI(QString filePath, LayerCamera* cameraLayer, QString format = "PNG", int width = -1, int height = -1, int startFrame = 1, int endFrame = -1, bool transparency = false, bool antialias = true);
    bool exportMovieCLI(QString filePath, LayerCamera* cameraLayer, int width = -1, int height = -1, int startFrame = 1, int endFrame = -1);

    qreal viewScaleInversed();
    void deselectAll();
    void selectAll();

    QString workingDir() const;

    void importMovie(QString filePath, int fps);

Q_SIGNALS:
    void updateTimeLine();
    void updateLayerCount();

    void objectLoaded();

    void changeThinLinesButton(bool);
    void currentFrameChanged(int n);

    void needSave();
    void needDisplayInfo(const QString& title, const QString& body);
    void needDisplayInfoNoTitle(const QString& body);

public: //slots
    void clearCurrentFrame();

    void cut();

    void deselectAllSelections();
    void deselectAllAndCancelTransform();
    bool importImage(QString filePath, bool isSequence);
    bool importGIF(QString filePath, int numOfImages = 0);
    void updateFrame(int frameNumber);
    void restoreKey();

    void updateFrameAndVector(int frameNumber);
    void updateCurrentFrame();

    void scrubNextKeyFrame();
    void scrubPreviousKeyFrame();
    void scrubForward();
    void scrubBackward();

    KeyFrame* addNewKey();
    KeyFrame* addKeyFrame(int layerIndex, int frameIndex);
    KeyFrame* addKeyFrame(int layerIndex, int frameIndex, bool ignoreKeyExists);
    KeyFrame* addKeyFrameToLayerId(int layerId, int frameIndex, bool ignoreKeyExists);
    KeyFrame* addKeyFrameToLayerId(int layerId, int frameIndex);
    void addKeyContaining(int layerId, int frameIndex, KeyFrame* key);


    void removeKey();
    void removeCurrentKey();
    void removeKeyAt(int layerIndex, int frameIndex);
    void removeKeyAtLayerId(int layerId, int frameIndex);

    void switchVisibilityOfLayer(int layerNumber);
    void showLayerNotVisibleWarning();
    void swapLayers(int i, int j);
    Status::StatusInt pegBarAlignment(QStringList layers);

    void copy();
    void paste();

    void clipboardChanged();
    void toggleShowAllLayers();
    void flipSelection(bool flipVertical);

    void toogleOnionSkinType();

    void settingUpdated(SETTING);

    void dontAskAutoSave(bool b) { mAutosaveNerverAskAgain = b; }
    bool autoSaveNeverAskAgain() { return mAutosaveNerverAskAgain; }

signals:
    void needPaint();
    void needPaintAtFrame(int frameIndex);

protected:
    // Need to move to somewhere...
    void dragEnterEvent(QDragEnterEvent*);
    void dropEvent(QDropEvent*);

private:
    bool importBitmapImage(QString, int space);
    bool importVectorImage(QString, bool);

    // the object to be edited by the editor
    std::shared_ptr<Object> mObject = nullptr;

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
    SelectionManager* mSelectionManager = nullptr;
    BackupManager*     mBackupManager = nullptr;
    KeyFrameManager*   mKeyFrameManager = nullptr;
    CanvasManager* mCanvasManager = nullptr;

    std::vector< BaseManager* > mAllManagers;

    bool mIsAltPressed = false;

    bool mIsAutosave = true;
    int mAutosaveNumber = 12;
    int mAutosaveCounter = 0;
    bool mAutosaveNerverAskAgain = false;

    void makeConnections();

    // clipboard
    bool clipboardBitmapOk = true;
    bool clipboardVectorOk = true;
    bool clipboardSoundClipOk = true;
};

#endif
