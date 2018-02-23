/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2018 Matthew Chiawen Chang

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
    explicit ActionCommands(QWidget* parent = 0);
    virtual ~ActionCommands();

    void setCore(Editor* e) { mEditor = e; }

    // file 
    Status importSound();
    Status exportMovie();
    Status exportImageSequence();
    Status exportImage();

    // edit
    void flipSelectionX();
    void flipSelectionY();

    // view
    void ZoomIn();
    void ZoomOut();
    void rotateClockwise();
    void rotateCounterClockwise();
    void toggleMirror();
    void toggleMirrorV();
    void showGrid(bool bShow);

    // Animation
    void PlayStop();
    void GotoNextFrame();
    void GotoPrevFrame();
    void GotoNextKeyFrame();
    void GotoPrevKeyFrame();
    Status addNewKey();
    void removeKey();
    void duplicateKey();
    void moveFrameForward();
    void moveFrameBackward();

    // Layer
    Status addNewBitmapLayer();
    Status addNewVectorLayer();
    Status addNewCameraLayer();
    Status addNewSoundLayer();
    Status deleteCurrentLayer();

    // Help
    void about();
    void help();
    void website();
    void reportbug();

private:
    Editor* mEditor = nullptr;
    QWidget* mParent = nullptr;
};

#endif // COMMANDCENTER_H
