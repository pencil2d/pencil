/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon

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
#include <QMainWindow>
#include <QLabel>
#include <QToolButton>
#include "timeline.h"
#include "scribblearea.h"
#include "timecontrols.h"
#include "object.h"
#include "vectorimage.h"
#include "bitmapimage.h"
#include "backupelement.h"
#include "colorbox.h"


class QComboBox;
class QSlider;
class MainWindow2;
class ColorManager;
class ToolManager;
class LayerManager;
class ScribbleArea;


class Editor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY( ColorManager* colorManager READ colorManager )
    Q_PROPERTY( ToolManager*  toolManager  READ toolManager )
    Q_PROPERTY( LayerManager* layerManager READ layerManager )

public:
    Editor( MainWindow2* parent );
    virtual ~Editor();

    bool initialize();

    ColorManager* colorManager() const { return m_colorManager; }
    ToolManager* toolManager() const { return m_pToolManager; }
    LayerManager* layerManager() const { return m_pLayerManager; }

    Object* object() const { return m_pObject; }
    void setObject( Object* object );

    void setScribbleArea( ScribbleArea* pScirbbleArea ) { m_pScribbleArea = pScirbbleArea; }

    int maxFrame; // the number of the last frame for the current object

    int fps; // the number of frames per second used by the editor
    QTimer* timer; // the timer used for animation in the editor
    bool playing;
    bool looping;
    bool loopControl;
    int loopStart;
    int loopEnd;
    bool sound;

    TimeLine* getTimeLine();

    Layer* getCurrentLayer( int incr );
    Layer* getCurrentLayer() { return getCurrentLayer( 0 ); }
    Layer* getLayer( int i );
    int allLayers() { return m_pScribbleArea->showAllLayers(); }
    static QMatrix map( QRectF, QRectF );
    bool exportSeqCLI( QString, QString );

    int getOnionLayer1Opacity() { return onionLayer1Opacity; }
    int getOnionLayer2Opacity() { return onionLayer2Opacity; }
    int getOnionLayer3Opacity() { return onionLayer3Opacity; }

    void importMovie( QString filePath, int fps );

    // backup
    int backupIndex;
    QList<BackupElement*> backupList;

    ScribbleArea* getScribbleArea() { return m_pScribbleArea; }

protected:
    void keyPressEvent( QKeyEvent *event );
    void dragEnterEvent( QDragEnterEvent* event );
    void dropEvent( QDropEvent* event );
    QRect viewRect;
signals:
    void selectAll();
    void toggleLoop( bool );
    void toggleLoopControl( bool ) ;
    void loopToggled( bool );
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

public slots:
    void clearCurrentFrame();

    void cut();
    void flipX();
    void flipY();
    void deselectAll();
    void setzoom();
    void setzoom1();
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

    void play();
    void startOrStop();
    void playNextFrame();
    void playPrevFrame();

    void changeFps( int );
    int getFps();
    void setLoop( bool checked );
    void setLoopControl( bool checked );
    void changeLoopStart (int);
    void changeLoopEnd (int);
    void setSound();

    void previousLayer();
    void nextLayer();
    void endPlay();
    void startPlay();

    void addNewKey();
    void duplicateKey();
    void removeKey();

    //void printAndPreview( QPrinter* printer );
    void resetUI();

    void updateObject();

    void setCurrentLayer( int layerNumber );
    void switchVisibilityOfLayer( int layerNumber );
    void moveLayer( int i, int j );
    void updateMaxFrame();

    void selectAndApplyColour( int );
    void setFrontColour( int, QColor );

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
    bool exportSeq();
    bool exportMov();
    bool exportFlash();

private slots:
    void saveLength( QString );
    void getCameraLayer();

private:
    Object* m_pObject;  // the object to be edited by the editor

    ScribbleArea* m_pScribbleArea;
    MainWindow2* m_pMainWindow;

    ColorManager* m_colorManager;
    ToolManager* m_pToolManager;
    LayerManager* m_pLayerManager;

    bool m_isAltPressed;
    int numberOfModifications;

    bool m_isAutosave;
    int autosaveNumber;

    int onionLayer1Opacity;
    int onionLayer2Opacity;
    int onionLayer3Opacity;

    void makeConnections();
    void addKeyFame( int layerNumber, int frameNumber );

    // backup
    void clearBackup();
    int lastModifiedFrame;
    int lastModifiedLayer;

    // clipboard
    bool clipboardBitmapOk, clipboardVectorOk;
    BitmapImage m_clipboardBitmapImage;
    VectorImage m_clipboardVectorImage;

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
