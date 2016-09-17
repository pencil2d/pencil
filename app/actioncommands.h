#ifndef COMMANDCENTER_H
#define COMMANDCENTER_H

#include <QObject>
#include "pencilerror.h"

class Editor;


class ActionCommands : public QObject
{
    Q_OBJECT

public:
    explicit ActionCommands(QObject* parent = 0);
    virtual ~ActionCommands();

    void setCore( Editor* e ) { mEditor = e; }
    
    // file 
    Status importSound();

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
    Editor* mEditor = nullptr;
};

#endif // COMMANDCENTER_H
