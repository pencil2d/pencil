/*

Pencil - Traditional Animation Software
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

#include <memory>
#include <QObject>
#include <QList>
#include "pencilerror.h"
#include "pencildef.h"


class QDragEnterEvent;
class QDropEvent;
class QTemporaryDir;
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
class SoundManager;
class ScribbleArea;
class TimeLine;
class BackupElement;
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

    Object* object() const { return mObject.get(); }
    Status setObject(Object* object);
    void updateObject();
    void prepareSave();

    void setScribbleArea(ScribbleArea* pScirbbleArea) { mScribbleArea = pScirbbleArea; }
    ScribbleArea* getScribbleArea() { return mScribbleArea; }

    int currentFrame();
    int fps();
    void setFps(int fps);
    void retime(int newFps, qreal speed);

    int  currentLayerIndex() { return mCurrentLayerIndex; }
    void setCurrentLayerIndex(int i);

    void scrubTo(int frameNumber);


    /**
     * @brief The visiblity value should match any of the VISIBILITY enum values
     */
    void setLayerVisibility(LayerVisibility visibility);
    LayerVisibility layerVisibility();
    bool exportSeqCLI(QString filePath, LayerCamera* cameraLayer, QString format = "PNG", int width = -1, int height = -1, int startFrame = 1, int endFrame = -1, bool transparency = false, bool antialias = true);
    bool exportMovieCLI(QString filePath, LayerCamera* cameraLayer, int width = -1, int height = -1, int startFrame = 1, int endFrame = -1);

    qreal viewScaleInversed();
    void deselectAll();
    void selectAll();

    QString workingDir() const;

    // backup
    int mBackupIndex;
    BackupElement* currentBackup();
    QList<BackupElement*> mBackupList;

signals:
    void updateTimeLine();
    void updateLayerCount();
    void updateBackup();

    void objectLoaded();

    void changeThinLinesButton(bool);
    void currentFrameChanged(int n);

    void needSave();
    void needDisplayInfo(const QString& title, const QString& body);
    void needDisplayInfoNoTitle(const QString& body);

    void retimed(int newFps);

public: //slots
    void clearCurrentFrame();

    void cut();

    bool importImage(QString filePath);
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
    void removeKey();

    void notifyAnimationLengthChanged();
    void switchVisibilityOfLayer(int layerNumber);
    void swapLayers(int i, int j);
    Status pegBarAlignment(QStringList layers);

    void backup(QString undoText);
    void backup(int layerNumber, int frameNumber, QString undoText);
    void undo();
    void redo();
    void copy();

    void paste();
    void clipboardChanged();
    void increaseLayerVisibilityIndex();
    void decreaseLayerVisibilityIndex();
    void flipSelection(bool flipVertical);

    void toogleOnionSkinType();

    void clearTemporary();
    void addTemporaryDir(QTemporaryDir* const dir);

    void settingUpdated(SETTING);

    void dontAskAutoSave(bool b) { mAutosaveNeverAskAgain = b; }
    bool autoSaveNeverAskAgain() { return mAutosaveNeverAskAgain; }
    void resetAutoSaveCounter();

    void createNewBitmapLayer(const QString& name);
    void createNewVectorLayer(const QString& name);
    void createNewSoundLayer(const QString& name);
    void createNewCameraLayer(const QString& name);

protected:
    // Need to move to somewhere...
    void dragEnterEvent(QDragEnterEvent*);
    void dropEvent(QDropEvent*);

private:
    bool importBitmapImage(QString, int space = 0);
    bool importVectorImage(QString);

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

    std::vector< BaseManager* > mAllManagers;

    bool mIsAltPressed = false;

    bool mIsAutosave = true;
    int mAutosaveNumber = 12;
    int mAutosaveCounter = 0;
    bool mAutosaveNeverAskAgain = false;

    void makeConnections();
    KeyFrame* addKeyFrame(int layerNumber, int frameNumber);

    QList<QTemporaryDir*> mTemporaryDirs;

    // backup
    void clearUndoStack();
    void updateAutoSaveCounter();
    int mLastModifiedFrame = -1;
    int mLastModifiedLayer = -1;

    // clipboard
    bool clipboardBitmapOk = true;
    bool clipboardVectorOk = true;
    bool clipboardSoundClipOk = true;
};

#endif
