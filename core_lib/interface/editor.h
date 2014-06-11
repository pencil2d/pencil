/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2014 Matt Chang

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

#include <QList>
#include <QLabel>
#include <QToolButton>
#include <QSpinBox>
#include "object.h"
#include "backupelement.h"

class QComboBox;
class QSlider;
class MainWindow2;
class ColorManager;
class ToolManager;
class LayerManager;
class PlaybackManager;
class ScribbleArea;
class TimeLine;


class Editor : public QObject
{
    Q_OBJECT
    Q_PROPERTY( ColorManager* color READ color )
    Q_PROPERTY( ToolManager*  tools  READ tools )
    Q_PROPERTY( LayerManager* layers READ layers )
    Q_PROPERTY( PlaybackManager* playback READ playback )

public:
    Editor( MainWindow2* parent );
    virtual ~Editor();

    bool initialize( ScribbleArea* pScribbleArea );

    ColorManager* color() const { return mColorManager; }
    ToolManager* tools() const { return mToolManager; }
    LayerManager* layers() const { return mLayerManager; }
    PlaybackManager* playback() const { return mPlaybackManager; }

    Object* object() const { return mObject; }
    void setObject( Object* object );

    void setScribbleArea( ScribbleArea* pScirbbleArea ) { mScribbleArea = pScirbbleArea; }

    int frame();

    Layer* getCurrentLayer( int incr );
    Layer* getCurrentLayer() { return getCurrentLayer( 0 ); }
    Layer* getLayer( int i );
    int allLayers();
    static QMatrix map( QRectF, QRectF );
    bool exportSeqCLI( QString, QString );

    int getOnionLayer1Opacity() { return onionLayer1Opacity; }
    int getOnionLayer2Opacity() { return onionLayer2Opacity; }
    int getOnionLayer3Opacity() { return onionLayer3Opacity; }

    void importMovie( QString filePath, int fps );

    // backup
    int mBackupIndex;
    QList<BackupElement*> mBackupList;
    ScribbleArea* getScribbleArea() { return mScribbleArea; }

protected:
    // Need to move to somewhere...
    void dragEnterEvent( QDragEnterEvent* event );
    void dropEvent( QDropEvent* event );

signals:
    void selectAll();
    void toggleMultiLayerOnionSkin( bool );
    void toggleOnionNext( bool );
    void toggleOnionPrev( bool );
    void multiLayerOnionSkinChanged( bool );
    void onionPrevChanged( bool );
    void onionNextChanged( bool );
    void changeThinLinesButton( bool );
    void changeOutlinesButton( bool );

    // save
    void needSave();
    void fileLoaded();

public slots:
    void clearCurrentFrame();

    void cut();
    void flipX();
    void flipY();
    void deselectAll();
    void zoomIn();
    void zoomOut();
    void rotatecw();
    void rotateacw();
    void gridview();
    void resetView();

    void importImageFromDialog();
    void importImage( QString filePath );
    void importImageSequence();
    void importSound( QString filePath = "" );
    bool importMov();
    void updateFrame( int frameNumber );
    void updateFrameAndVector( int frameNumber );

    void scrubTo( int frameNumber );
    void scrubNextKeyFrame();
    void scrubPreviousKeyFrame();
    void scrubForward();
    void scrubBackward();

    void previousLayer();
    void nextLayer();

    void addNewKey();
    void duplicateKey();
    void removeKey();

    void resetUI();

    void updateObject();

    void setCurrentLayer( int layerNumber );
    void switchVisibilityOfLayer( int layerNumber );
    void moveLayer( int i, int j );

    void changeAutosave( int );
    void changeAutosaveNumber( int );

    void onionLayer1OpacityChangeSlot( int );
    void onionLayer2OpacityChangeSlot( int );
    void onionLayer3OpacityChangeSlot( int );

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
    void deleteCurrentLayer();

    void toggleMirror();
    void toggleMirrorV();
    void toggleShowAllLayers();
    void resetMirror();

    bool exportX();
    bool exportImage();
    bool exportImageSequence();
    bool exportMov();
    bool exportFlash();

private slots:
    void saveLength( QString );
    void getCameraLayer();

private:
    TimeLine* getTimeLine();

    Object* mObject = nullptr;  // the object to be edited by the editor

    ScribbleArea* mScribbleArea = nullptr;
    MainWindow2*  mMainWindow = nullptr;

    ColorManager*    mColorManager = nullptr;
    ToolManager*     mToolManager = nullptr;
    LayerManager*    mLayerManager = nullptr;
    PlaybackManager* mPlaybackManager = nullptr;

    bool m_isAltPressed;
    int numberOfModifications;

    bool mIsAutosave;
    int autosaveNumber;

    int onionLayer1Opacity;
    int onionLayer2Opacity;
    int onionLayer3Opacity;

    void makeConnections();
    void addKeyFame( int layerNumber, int frameNumber );

    // backup
    void clearUndoStack();
    int lastModifiedFrame;
    int lastModifiedLayer;

    // clipboard
    bool clipboardBitmapOk, clipboardVectorOk;

    // dialogs
    void createExportFramesSizeBox();
    void createExportMovieSizeBox();
    void createExportFramesDialog();
    void createExportMovieDialog();
    void createExportFlashDialog();
    QDialog* exportFramesDialog;
    QDialog* exportMovieDialog;
    QDialog* exportFlashDialog;
    QSpinBox* exportFramesDialog_hBox;
    QSpinBox* exportFramesDialog_vBox;
    QSpinBox* exportMovieDialog_hBox;
    QSpinBox* exportMovieDialog_vBox;
    QComboBox* exportFramesDialog_format;
    QSpinBox* exportMovieDialog_fpsBox;
    QComboBox* exportMovieDialog_format;
    QSlider* exportFlashDialog_compression;
};

#endif
