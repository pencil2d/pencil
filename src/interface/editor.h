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
#include "toolset.h"
#include "timecontrols.h"
#include "object.h"
#include "vectorimage.h"
#include "bitmapimage.h"
#include "backupelement.h"
#include "colorbox.h"


//class QPrinter;
class QComboBox;
class QSlider;

class MainWindow2;
class ColorManager;
class ToolManager;
class LayerManager;


class Editor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY( ColorManager* colorManager READ colorManager )
    Q_PROPERTY( ToolManager*  toolManager  READ toolManager )
    Q_PROPERTY( LayerManager* layerManager READ layerManager )

public:
    Editor( MainWindow2* parent );
    virtual ~Editor();

    ColorManager* colorManager() const { return m_colorManager; }
    ToolManager* toolManager() const { return m_pToolManager; }
    LayerManager* layerManager() const { return m_pLayerManager; }

    Object* m_pObject;  // the object to be edited by the editor
    Object* object() const { return m_pObject; }
    void setObject( Object* object );

    int maxFrame; // the number of the last frame for the current object

    int fps; // the number of frames per second used by the editor
    QTimer* timer; // the timer used for animation in the editor
    bool playing;
    bool looping;
    bool loopControl;
    int loopStart;
    int loopStarts;
    int loopEnd;
    int loopEnds;
    bool sound;
    ToolSetWidget* toolSet;

    TimeLine* getTimeLine();

    Layer* getCurrentLayer( int incr );
    Layer* getCurrentLayer() { return getCurrentLayer( 0 ); }
    Layer* getLayer( int i );
    bool isModified() { return modified; }
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
    void toggleMultiLayerOnionSkin(bool);
    void toggleOnionNext( bool );
    void toggleOnionPrev( bool );
    void multiLayerOnionSkinChanged(bool);
    void onionPrevChanged( bool );
    void onionNextChanged( bool );
    void changeThinLinesButton( bool );
    void changeOutlinesButton( bool );

    // Tool Option
    void changeTool( ToolType );

    void penWidthValueChange( qreal );
    void penFeatherValueChange( qreal );
    void penInvisiblityValueChange( int );
    void penPreserveAlphaValueChange( int );
    void penPressureValueChange( int );
    void penFollowContourValueChange( int );
    void penColorValueChange( QColor );

    // save
    void needSave();

public slots:

    void setTool( ToolType );
    void clearCurrentFrame();

    void importImageSequence();
    void cut();
    void crop();
    void croptoselect();
    void deselectAll();
    void setzoom();
    void setzoom1();
    void rotatecw();
    void rotateacw();
    void gridview();
    void resetView();

    void importImage();
    void importImage( QString filePath );
    void importSound( QString filePath = "" );
    bool importMov();
    void updateFrame( int frameNumber );
    void updateFrameAndVector( int frameNumber );

    void scrubTo( int frameNumber );
    void scrubNextKeyframe();
    void scrubPreviousKeyframe();
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

    void addKey();
    void duplicateKey();
    void removeKey();

    //void printAndPreview( QPrinter* printer );
    void resetUI();

    void updateObject();

    void setCurrentLayer( int layerNumber );
    void switchVisibilityOfLayer( int layerNumber );
    void moveLayer( int i, int j );
    void updateMaxFrame();

    void setToolProperties( const Properties& p );
    void setWidth( qreal );
    void applyWidth( qreal );
    void setFeather( qreal );
    void applyFeather( qreal );
    void setInvisibility( int );
    void applyInvisibility( bool );
    void setPressure( int );
    void applyPressure( bool );
    void setPreserveAlpha( int );
    void applyPreserveAlpha( bool );
    void setFollowContour( int );
    void applyFollowContour( bool );
    void selectAndApplyColour( int );
    void setFrontColour( int, QColor );

    void changeAutosave( int );
    void changeAutosaveNumber( int );

    void onionLayer1OpacityChangeSlot( int );
    void onionLayer2OpacityChangeSlot( int );
    void onionLayer3OpacityChangeSlot( int );

    void modification();
    void modification( int );
    void backup( QString undoText );
    void backup( int layerNumber, int frameNumber, QString undoText );
    void undo();
    void redo();
    void copy();
    //void copyFrames();
    //void pasteFrames();

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

    //void print();
    void restorePalettesSettings( bool, bool, bool );
    void saveSvg();

private slots:
    bool exportX();
    bool exportImage();
    bool exportSeq();
    bool exportMov();
    bool exportFlash();

    void saveLength( QString );
    void getCameraLayer();

private:
    ScribbleArea* m_pScribbleArea;
    MainWindow2* mainWindow;

    ColorManager* m_colorManager;
    ToolManager* m_pToolManager;
    LayerManager* m_pLayerManager;

    QString path;
    bool altpress;
    bool modified;
    int numberOfModifications;

    bool autosave;
    int autosaveNumber;

    int onionLayer1Opacity;
    int onionLayer2Opacity;
    int onionLayer3Opacity;

    void makeConnections();
    void addKey( int layerNumber, int frameNumber );

    // backup
    void clearBackup();
    int lastModifiedFrame, lastModifiedLayer;

    // clipboard
    bool clipboardBitmapOk, clipboardVectorOk;
    BitmapImage clipboardBitmapImage;
    VectorImage clipboardVectorImage;

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
