/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2017 Matthew Chiawen Chang

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

class ActionCommands : public QObject
{
    Q_OBJECT

public:
    explicit ActionCommands( QWidget* parent = 0 );
    virtual ~ActionCommands();

    void setCore( Editor* e ) { mEditor = e; }
    
    // file 
    Status importSound();
	Status exportMovie();

    // edit


    // view
    void ZoomIn();
    void ZoomOut();
    void flipX();
    void flipY();
    void rotateClockwise();
    void rotateCounterClockwise();

    void showGrid( bool bShow );

    // Animation
    void PlayStop();
    void GotoNextFrame();
    void GotoPrevFrame();
    void GotoNextKeyFrame();
    void GotoPrevKeyFrame();
    void addNewKey();
    void removeKey();

    // Layer
    Status addNewBitmapLayer();
    Status addNewVectorLayer();
    Status addNewCameraLayer();
    Status addNewSoundLayer();

private:
    Editor* mEditor  = nullptr;
	QWidget* mParent = nullptr;
};

#endif // COMMANDCENTER_H
