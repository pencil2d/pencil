/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

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
#include <QList>
#include "backupelement.h"
#include "pencilerror.h"


class QDragEnterEvent;
class QDropEvent;
class Object;
class MainWindow2;
class BaseManager;
class ColorManager;
class ToolManager;
class LayerManager;
class PlaybackManager;
class ViewManager;
class PreferenceManager;
class SoundManager;
class ScribbleArea;
class TimeLine;

enum class SETTING;


class Editor : public QObject
{
    Q_OBJECT

    Q_PROPERTY( ColorManager*    color    READ color )
    Q_PROPERTY( ToolManager*     tools    READ tools )
    Q_PROPERTY( LayerManager*    layers   READ layers )
    Q_PROPERTY( PlaybackManager* playback READ playback )
    Q_PROPERTY( ViewManager*     view     READ view )
    Q_PROPERTY( PreferenceManager* preference READ preference )
    Q_PROPERTY( SoundManager*    sound    READ sound )

public:
    explicit Editor( QObject* parent = nullptr );
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

    Object* object() const { return mObject.get(); }
    Status setObject( Object* object );
    void updateObject();
    void prepareSave();

    void setScribbleArea( ScribbleArea* pScirbbleArea ) { mScribbleArea = pScirbbleArea; }
    ScribbleArea* getScribbleArea() { return mScribbleArea; }

    int  currentFrame();
    int  fps();

    void scrubTo( int frameNumber );

    int  allLayers();
    bool exportSeqCLI( QString filePath, QString format = "PNG", int width = -1, int height = -1, bool transparency = false, bool antialias = true );
    
    QString workingDir() const;

    void importMovie( QString filePath, int fps );

    // backup
    int mBackupIndex;
    BackupElement* currentBackup();
    QList<BackupElement*> mBackupList;

Q_SIGNALS:
    void updateTimeLine();
    void updateLayerCount();
    void updateBackup();

    void objectLoaded();

    void selectAll();
    void changeThinLinesButton( bool );
    void currentFrameChanged( int n );

    void needSave();

public: //slots
    void clearCurrentFrame();

    void cut();
    
    void deselectAll();

    bool importImage( QString filePath );
    void updateFrame( int frameNumber );
    void restoreKey();

    void updateFrameAndVector( int frameNumber );
    void updateCurrentFrame();

    void scrubNextKeyFrame();
    void scrubPreviousKeyFrame();
    void scrubForward();
    void scrubBackward();

    KeyFrame* addNewKey();
    void duplicateKey();
    void removeKey();

    void moveFrameForward();
    void moveFrameBackward();

    void setCurrentLayer( int layerNumber );
    void switchVisibilityOfLayer( int layerNumber );
    void moveLayer( int i, int j );

    void backup( QString undoText );
    void backup( int layerNumber, int frameNumber, QString undoText );
    void undo();
    void redo();
    void copy();

    void paste();
    void clipboardChanged();

    void toggleMirror();
    void toggleMirrorV();
    void toggleShowAllLayers();
    void flipSelection(bool flipVertical);

    void toogleOnionSkinType();

    void settingUpdated(SETTING);

protected:
    // Need to move to somewhere...
    void dragEnterEvent( QDragEnterEvent* event );
    void dropEvent( QDropEvent* event );

private:
    bool importBitmapImage( QString );
    bool importVectorImage( QString );

    // the object to be edited by the editor
    std::shared_ptr<Object> mObject = nullptr;

    int mFrame = 1; // current frame number.

    ScribbleArea* mScribbleArea = nullptr;

    ColorManager*      mColorManager      = nullptr;
    ToolManager*       mToolManager       = nullptr;
    LayerManager*      mLayerManager      = nullptr;
    PlaybackManager*   mPlaybackManager   = nullptr;
    ViewManager*       mViewManager       = nullptr;
    PreferenceManager* mPreferenceManager = nullptr;
    SoundManager*      mSoundManager      = nullptr;

    std::vector< BaseManager* > mAllManagers;

    bool m_isAltPressed = false;
    int numberOfModifications = 0;

    bool mIsAutosave   = true;
    int autosaveNumber = 12;

    void makeConnections();
    KeyFrame* addKeyFrame( int layerNumber, int frameNumber );

    // backup
    void clearUndoStack();
    int lastModifiedFrame;
    int lastModifiedLayer;

    // clipboard
    bool clipboardBitmapOk, clipboardVectorOk, clipboardSoundClipOk;
};

#endif
