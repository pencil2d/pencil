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

class MainWindow2;
class ColorManager;

class Editor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY( ColorManager* colorManager READ colorManager );
public:
    Editor(MainWindow2* parent);
    virtual ~Editor();
    
    ColorManager* colorManager() const { return m_colorManager; }

    Object* object;  // the object to be edited by the editor
    Object* getObject() const { return object; } 
    void setObject(Object* object);

    int m_nCurrentLayerIndex; // the current layer to be edited/displayed by the editor
    int m_nCurrentFrameIndex; // the current frame to be edited/displayed by the editor
    int maxFrame; // the number of the last frame for the current object
    QList<int> frameList; // the frames that are to be cached -- should we use a QMap, or a QHash?

    int fps; // the number of frames per second used by the editor
    QTimer* timer; // the timer used for animation in the editor
    bool playing;
    bool looping;
    bool sound;
    ToolSetWidget* toolSet;

    TimeLine* getTimeLine();

    Layer* getCurrentLayer(int incr) 
    { 
        if (object != NULL) 
        { 
            return object->getLayer(m_nCurrentLayerIndex + incr); 
        } 
        else 
        { 
            return NULL; 
        } 
    }

    Layer* getCurrentLayer() { return getCurrentLayer(0); }
    Layer* getLayer(int i);
    bool isModified() { return modified; }
    int allLayers() { return scribbleArea->showAllLayers(); }
    static QMatrix map(QRectF, QRectF);    
    bool exportSeqCLI(QString, QString);

    int getOnionLayer1Opacity() {return onionLayer1Opacity;}
    int getOnionLayer2Opacity() {return onionLayer2Opacity;}
    int getOnionLayer3Opacity() {return onionLayer3Opacity;}

    void importMovie (QString filePath, int fps);

    // backup
    int backupIndex;
    QList<BackupElement*> backupList;

    ScribbleArea* getScribbleArea() { return scribbleArea; }
    void setColor(QColor argColor);

protected:
    void keyPressEvent(QKeyEvent *event);
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
    QRect viewRect;
signals:
    void selectAll();
    void toggleLoop(bool);
    void loopToggled(bool);
    void toggleOnionNext(bool);
    void toggleOnionPrev(bool);
    void onionPrevChanged(bool);
    void onionNextChanged(bool);
    void changeThinLinesButton(bool);
    void changeOutlinesButton(bool);

    // Tool Option
    void changeTool(ToolType);

    void penWidthValueChange(qreal);
    void penFeatherValueChange(qreal);
    void penInvisiblityValueChange(int);
    void penPreserveAlphaValueChange(int);
    void penPressureValueChange(int);
    void penFollowContourValueChange(int);
    void penColorValueChange(QColor);

    // save
    void needSave();

public slots:
    
    void setTool(ToolType);     
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
    void importImage(QString filePath);
    void importSound(QString filePath = "");
    bool importMov();
    void updateFrame(int frameNumber);
    void updateFrameAndVector(int frameNumber);

    void scrubTo(int frameNumber);
    void scrubNextKeyframe();
    void scrubPreviousKeyframe();
    void scrubForward();
    void scrubBackward();

    void play();
    void startOrStop();
    void playNextFrame();
    void playPrevFrame();

    void changeFps(int);
    int getFps();
    void setLoop(bool checked);
    void setSound();

    void previousLayer();
    void nextLayer();
    void endPlay();
    void startPlay();

    void addKey();
    void duplicateKey();
    void addKey(int layerNumber, int& frameNumber);
    void removeKey();

    void addFrame(int frameNumber);
    void addFrame(int frameNumber1, int frameNumber2);
    void removeFrame(int frameNumber);
    int getLastIndexAtFrame(int frameNumber);
    int getLastFrameAtFrame(int frameNumber);

    void resetUI();
    

    void updateObject();

    void setCurrentLayer(int layerNumber);
    void switchVisibilityOfLayer(int layerNumber);
    void moveLayer(int i, int j);
    void updateMaxFrame();

    void setToolProperties(const Properties& p);
    void setWidth(qreal);
    void applyWidth(qreal);
    void setFeather(qreal);
    void applyFeather(qreal);
    void setInvisibility(int);
    void applyInvisibility(bool);
    void setPressure(int);
    void applyPressure(bool);
    void setPreserveAlpha(int);
    void applyPreserveAlpha(bool);
    void setFollowContour(int);
    void applyFollowContour(bool);
    void selectAndApplyColour(int);    
    void setFrontColour(int, QColor);

    void changeAutosave(int);
    void changeAutosaveNumber(int);

    void onionLayer1OpacityChangeSlot(int);
    void onionLayer2OpacityChangeSlot(int);
    void onionLayer3OpacityChangeSlot(int);

    void modification();
    void modification(int);
    void backup(QString undoText);
    void backup(int layerNumber, int frameNumber, QString undoText);
    void undo();
    void redo();
    void copy();
    void copyFrames();
    void pasteFrames();

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

    void print();
    //void detachAllPalettes();
    void restorePalettesSettings(bool, bool, bool);
    void saveSvg();

private slots:
    bool exportX();
    bool exportImage();
    bool exportSeq();
    bool exportMov();
    bool exportFlash();

    void saveLength(QString);
    void getCameraLayer();

    void printAndPreview(QPrinter*);


private:
    ScribbleArea* scribbleArea;    
    MainWindow2* mainWindow;

    ColorManager* m_colorManager;

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
