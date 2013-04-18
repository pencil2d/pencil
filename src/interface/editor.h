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
#include "preferences.h"
#include "palette.h"
#include "object.h"
#include "vectorimage.h"
#include "bitmapimage.h"

class Editor;

class BackupElement : public QObject
{
    Q_OBJECT
public:
    enum types { UNDEFINED, BITMAP_MODIF, VECTOR_MODIF };
    //int type;
    QString undoText;
    bool somethingSelected;
    QRectF mySelection, myTransformedSelection, myTempTransformedSelection;
    //BackupElement() { type = UNDEFINED; }
    virtual int type() { return UNDEFINED; }
    virtual void restore(Editor*) { qDebug() << "Wrong"; };
};

class BackupBitmapElement : public BackupElement
{
    Q_OBJECT
public:
    int layer, frame;
    BitmapImage bitmapImage;
    //BackupBitmapElement() { type = BackupElement::BITMAP_MODIF; }
    int type() { return BackupElement::BITMAP_MODIF; }
    void restore(Editor*);
};

class BackupVectorElement : public BackupElement
{
    Q_OBJECT
public:
    int layer, frame;
    VectorImage vectorImage;
    //BackupVectorElement() { type = BackupElement::VECTOR_MODIF; }
    int type() { return BackupElement::VECTOR_MODIF; }
    void restore(Editor*);
};


class Editor : public QWidget
{
    Q_OBJECT

public:
    Editor(QMainWindow* parent);
    virtual ~Editor();

    //Object* getObject() { return object; }
    //bool somethingSelected();
    Object* object;  // the object to be edited by the editor
    //int currentPenColour, currentFillColour;
    int currentLayer; // the current layer to be edited/displayed by the editor
    int currentFrame; // the current frame to be edited/displayed by the editor
    int maxFrame; // the number of the last frame for the current object
    QList<int> frameList; // the frames that are to be cached -- should we use a QMap, or a QHash?

    int fps; // the number of frames per second used by the editor
    QTimer* timer; // the timer used for animation in the editor
    bool playing;
    bool looping;
    bool sound;

    Palette* getPalette() { return palette; }
    TimeLine* getTimeLine() { return timeLine; }
    ToolSet* getToolSet() { return toolSet; }
    Layer* getCurrentLayer(int incr) { if(object != NULL) { return object->getLayer(currentLayer+incr); } else { return NULL; } }
    Layer* getCurrentLayer() { return getCurrentLayer(0); }
    Layer* getLayer(int i);
    bool isModified() { return modified; }
    int allLayers() { return scribbleArea->allLayers(); }
    static QMatrix map(QRectF, QRectF);
    bool openObject(QString);
    bool exportSeqCLI(QString, QString);

    int getOnionLayer1Opacity() {return onionLayer1Opacity;}
    int getOnionLayer2Opacity() {return onionLayer2Opacity;}
    int getOnionLayer3Opacity() {return onionLayer3Opacity;}

    void importMovie (QString filePath, int fps);

    // backup
    int backupIndex;
    QList<BackupElement*> backupList;

    ScribbleArea* getScribbleArea() {return scribbleArea;};

protected:
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

public slots:
    //tools menu
    void move_clicked();
    void pencil_clicked();
    void select_clicked();
    void hand_clicked();
    void eyedropper_clicked();

    void clear_clicked();
    void eraser_clicked();
    void pen_clicked();
    void polyline_clicked();
    void bucket_clicked();
    void color_clicked();
    void smudge_clicked();
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

    void showCounter(int);
    bool maybeSave();
    void importImage();
    void importImage(QString filePath);
    //void importSound();
    void importSound(QString filePath = "");
    bool importMov();
    void importPalette();
    void exportPalette();
    void updateFrame(int frameNumber);
    void updateFrameAndVector(int frameNumber);
    void scrubTo(int frameNumber);
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

    //void scrubKF();
    //void scrubKB();
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

    //void altPress();
    //void altRelease();
    void showPreferences();

    void newObject();
    void setObject(Object* object);
    void updateObject();

    void setCurrentLayer(int layerNumber);
    void switchVisibilityOfLayer(int layerNumber);
    void moveLayer(int i, int j);
    void updateMaxFrame();

    void setWidth(qreal);
    void applyWidth(qreal);
    void setFeather(qreal);
    void applyFeather(qreal);
    void setOpacity(qreal);
    void applyOpacity(qreal);
    void setInvisibility(int);
    void applyInvisibility(int);
    void setPressure(int);
    void applyPressure(int);
    void setPreserveAlpha(int);
    void applyPreserveAlpha(int);
    void setFollowContour(int);
    void applyFollowContour(int);
    void selectColour(int);
    void selectAndApplyColour(int);
    void setColour(QColor);
    void changeColour(int);
    void changeColourName(int);
    void changeColour(int,QColor);
    void updateColour(int, QColor);
    void addColour();
    void removeColour(int);

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

    void inbetween();
    void inbetweenV();

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

    void openRecent();
    void print();
    void saveForce();
    void dockAllPalettes();
    //void detachAllPalettes();
    void restorePalettesSettings(bool, bool, bool);
    void saveSvg();
    void addcolorbutton();

private slots:
    void newDocument();
    void openDocument();
    bool saveDocument();

    bool exportX();
    bool exportImage();
    bool exportSeq();
    bool exportMov();
    bool exportFlash();

    void showPalette();
    void about();
    void helpBox();
    void saveLength(QString);
    void getCameraLayer();

    void printAndPreview(QPrinter*);


private:
    bool saveObject(QString);

    ScribbleArea* scribbleArea;
    TimeLine* timeLine;
    TimeControls* timeControl;
    ToolSet* toolSet;
    Palette* palette;
    Preferences* preferences;
    QMainWindow* mainWindow;

    QString savedName;
    QString path;
    bool altpress;
    bool modified;
    int numberOfModifications;

    bool autosave;
    int autosaveNumber;

    int onionLayer1Opacity;
    int onionLayer2Opacity;
    int onionLayer3Opacity;



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
    void createNewDocumentDialog();
    QDialog* newDocumentDialog;
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

    // saving (XML)
    QDomElement createDomElement(QDomDocument& doc);
    bool loadDomElement(QDomElement element,  QString filePath);
};

#endif
