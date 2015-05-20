/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2014 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

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
class ColorManager;
class ToolManager;
class LayerManager;
class PlaybackManager;
class ViewManager;
class PreferenceManager;
class ScribbleArea;
class TimeLine;


class Editor : public QObject
{
    Q_OBJECT

    Q_PROPERTY( ColorManager*    color    READ color )
    Q_PROPERTY( ToolManager*     tools    READ tools )
    Q_PROPERTY( LayerManager*    layers   READ layers )
    Q_PROPERTY( PlaybackManager* playback READ playback )
    Q_PROPERTY( ViewManager*     view     READ view )
    Q_PROPERTY( PreferenceManager* preference READ preference )

public:
    explicit Editor( QObject* parent );
    virtual ~Editor();

    bool initialize( ScribbleArea* pScribbleArea );

    /************************************************************************/
    /* Managers                                                             */
    /************************************************************************/ 
    ColorManager*      color() const { return mColorManager; }
    ToolManager*       tools() const { return mToolManager; }
    LayerManager*      layers() const { return mLayerManager; }
    PlaybackManager*   playback() const { return mPlaybackManager; }
    ViewManager*       view() const { return mViewManager; }
    PreferenceManager* preference() const { return mPreferenceManager; }

    Object* object() const { return mObject.get(); }
    void setObject( Object* object );

    Status getError() { return mLastError; }

    void setScribbleArea( ScribbleArea* pScirbbleArea ) { mScribbleArea = pScirbbleArea; }

    int  currentFrame();
    void scrubTo( int frameNumber );

    int  allLayers();
    bool exportSeqCLI( QString, QString );

    int getOnionMaxOpacity() { return onionMaxOpacity; }
    int getOnionMinOpacity() { return onionMinOpacity; }
    int getOnionPrevFramesNum() { return onionPrevFramesNum; }
    int getOnionNextFramesNum() { return onionNextFramesNum; }

    void importMovie( QString filePath, int fps );

    // backup
    int mBackupIndex;
    QList<BackupElement*> mBackupList;
    ScribbleArea* getScribbleArea() { return mScribbleArea; }


Q_SIGNALS:
    void updateTimeLine();
    void updateLayerCount();

    void selectAll();
    void toggleMultiLayerOnionSkin( bool );
    void toggleOnionNext( bool );
    void toggleOnionPrev( bool );
    void multiLayerOnionSkinChanged( bool );
    void onionPrevChanged( bool );
    void onionNextChanged( bool );
    void changeThinLinesButton( bool );
    void changeOutlinesButton( bool );

    void currentFrameChanged( int n );

    // save
    void needSave();
    void fileLoaded();
    
public slots:
    void onionMaxOpacityChangeSlot( int );
    void onionMinOpacityChangeSlot( int );
    void onionPrevFramesNumChangeSlot( int );
    void onionNextFramesNumChangeSlot( int );

public: //slots
    void clearCurrentFrame();

    void cut();
    void flipX();
    void flipY();
    void deselectAll();
    void rotatecw();
    void rotateacw();
    void resetView();

    bool importImage( QString filePath );
    void importSound( QString filePath );
    void updateFrame( int frameNumber );
    void updateFrameAndVector( int frameNumber );

    void scrubNextKeyFrame();
    void scrubPreviousKeyFrame();
    void scrubForward();
    void scrubBackward();

    void addNewKey();
    void duplicateKey();
    void removeKey();

    void moveFrameForward();
    void moveFrameBackward();

    void resetUI();

    void updateObject();

    void setCurrentLayer( int layerNumber );
    void switchVisibilityOfLayer( int layerNumber );
    void moveLayer( int i, int j );

    void changeAutosave( int );
    void changeAutosaveNumber( int );

    void currentKeyFrameModification();
    void modification( int );
    void backup( QString undoText );
    void backup( int layerNumber, int frameNumber, QString undoText );
    void undo();
    void redo();
    void copy();

    void paste();
    void clipboardChanged();

    void newBitmapLayer();
    void newVectorLayer();
    void newSoundLayer();
    void newCameraLayer();

    void toggleMirror();
    void toggleMirrorV();
    void toggleShowAllLayers();
    void resetMirror();

protected:
    // Need to move to somewhere...
    void dragEnterEvent( QDragEnterEvent* event );
    void dropEvent( QDropEvent* event );

private:
    bool importBitmapImage( QString );
    bool importVectorImage( QString );
    void saveLength( QString );

    // the object to be edited by the editor
    std::shared_ptr<Object> mObject = nullptr;

    int mFrame; // current frame number.

    ScribbleArea* mScribbleArea = nullptr;

    ColorManager*      mColorManager      = nullptr;
    ToolManager*       mToolManager       = nullptr;
    LayerManager*      mLayerManager      = nullptr;
    PlaybackManager*   mPlaybackManager   = nullptr;
    ViewManager*       mViewManager       = nullptr;
    PreferenceManager* mPreferenceManager = nullptr;

    bool m_isAltPressed;
    int numberOfModifications;

    bool mIsAutosave;
    int autosaveNumber;

    int onionMaxOpacity;
    int onionMinOpacity;
    int onionNextFramesNum;
    int onionPrevFramesNum;

    void makeConnections();
    void addKeyFame( int layerNumber, int frameNumber );

    // backup
    void clearUndoStack();
    int lastModifiedFrame;
    int lastModifiedLayer;

    // clipboard
    bool clipboardBitmapOk, clipboardVectorOk;

    // dialogs
    void createExportMovieSizeBox();
    void createExportMovieDialog();

    Status mLastError;
};

#endif
