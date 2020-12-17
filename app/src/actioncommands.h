/*

Pencil2D - Traditional Animation Software
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef COMMANDCENTER_H
#define COMMANDCENTER_H

#include <QObject>
#include "pencilerror.h"

class Editor;
class QWidget;
class ExportMovieDialog;


class ActionCommands : public QObject
{
    Q_OBJECT

public:
    explicit ActionCommands(QWidget* parent = nullptr);
    virtual ~ActionCommands();

    void setCore(Editor* e) { mEditor = e; }

    // file
    Status importMovieVideo();
    Status importMovieAudio();
    Status importSound();
    Status exportMovie(bool isGif = false);
    Status exportImageSequence();
    Status exportImage();
    Status exportGif();

    // edit
    void flipSelectionX();
    void flipSelectionY();
    void selectAll();
    void deselectAll();

    // view
    void ZoomIn();
    void ZoomOut();
    void rotateClockwise();
    void rotateCounterClockwise();

    // Animation
    void PlayStop();
    void GotoNextFrame();
    void GotoPrevFrame();
    void GotoNextKeyFrame();
    void GotoPrevKeyFrame();
    Status addNewKey();
    Status insertNewKey();
    void removeKey();
    void duplicateKey();
    void moveFrameForward();
    void moveFrameBackward();
    void removeSelectedFrames();
    void reverseSelectedFrames();
    void increaseFrameExposure();
    void decreaseFrameExposure();

    // Layer
    Status addNewBitmapLayer();
    Status addNewVectorLayer();
    Status addNewCameraLayer();
    Status addNewSoundLayer();
    Status deleteCurrentLayer();
    void changeKeyframeLineColor();
    void changeallKeyframeLineColor();

    void setLayerVisibilityIndex(int index);

    // Help
    void help();
    void quickGuide();
    void website();
    void forum();
    void discord();
    void reportbug();
    void checkForUpdates();
    void openTemporaryDirectory();
    void about();

private:

    Status convertSoundToWav(const QString& filePath);

    Editor* mEditor = nullptr;
    QWidget* mParent = nullptr;
};

#endif // COMMANDCENTER_H
