#include "actioncommands.h"

#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

#include "pencildef.h"
#include "editor.h"
#include "viewmanager.h"
#include "layermanager.h"
#include "soundmanager.h"
#include "playbackmanager.h"
#include "preferencemanager.h"
#include "util.h"

//#include "layerbitmap.h"
//#include "layervector.h"
#include "layersound.h"
#include "bitmapimage.h"
#include "vectorimage.h"

#include "filedialogex.h"


ActionCommands::ActionCommands( QObject* parent ) : QObject( parent ) {}
ActionCommands::~ActionCommands() {}

Status ActionCommands::importSound()
{
    Layer* layer = mEditor->layers()->currentLayer();
    Q_ASSERT( layer );
    NULLReturn( layer, Status::FAIL );

    if ( layer->type() != Layer::SOUND )
    {
        QMessageBox msg;
        msg.setText( tr( "No sound layer exists as a destination for your import. Create a new sound layer?" ) );
        msg.addButton( tr( "Create sound layer" ), QMessageBox::AcceptRole );
        msg.addButton( tr( "Don't create layer" ), QMessageBox::RejectRole );
        
        int buttonClicked = msg.exec();
        if ( buttonClicked != QMessageBox::AcceptRole )
        {
            return Status::SAFE;
        }

        // Create new sound layer.
        Status s = addNewSoundLayer();
        if ( !s.ok() )
        {
            Q_ASSERT( false );
            return s;
        }

        layer = mEditor->layers()->currentLayer();
    }

    if ( layer->keyExists( mEditor->currentFrame() ) )
    {
        //layer->getKeyFrameAt()
        //QMessageBox msg;
        //msg.setText( tr( "The selected sound layer already contains a sound item. Please select another." ) );
        //msg.exec();
        //return Status::SAFE;
    }

    FileDialog fileDialog( this );
    QString strSoundFile = fileDialog.openFile( EFile::SOUND );

    Status st = mEditor->sound()->loadSound( layer, mEditor->currentFrame(), strSoundFile );
    
    //mTimeLine->updateContent();

    return st;
}

void ActionCommands::ZoomIn()
{
    float newScaleValue = mEditor->view()->scaling() * 1.2;
    mEditor->view()->scale( newScaleValue );
}

void ActionCommands::ZoomOut()
{
    float newScaleValue = mEditor->view()->scaling() * 0.8333;
    mEditor->view()->scale( newScaleValue );
}

void ActionCommands::flipX()
{
    auto view = mEditor->view();

    bool b = view->isFlipHorizontal();
    view->flipHorizontal( !b );
}

void ActionCommands::flipY()
{
    auto view = mEditor->view();

    bool b = view->isFlipVertical();
    view->flipVertical( !b );
}

void ActionCommands::rotateClockwise()
{
    mEditor->view()->rotate( 15 );
}

void ActionCommands::rotateCounterClockwise()
{
    mEditor->view()->rotate( -15 );
}


void ActionCommands::showGrid( bool bShow )
{
    auto prefs = mEditor->preference();
    if ( bShow )
        prefs->turnOn( SETTING::GRID );
    else
        prefs->turnOff( SETTING::GRID );
}

void ActionCommands::PlayStop()
{
    PlaybackManager* playback = mEditor->playback();
    if ( playback->isPlaying() )
    {
        playback->stop();
    }
    else
    {
        playback->play();
    }
}

void ActionCommands::GotoNextFrame()
{
}

void ActionCommands::GotoPrevFrame()
{
}

void ActionCommands::GotoNextKeyFrame()
{
}

void ActionCommands::GotoPrevKeyFrame()
{
}

Status ActionCommands::addNewBitmapLayer()
{
    bool ok;
    QString text = QInputDialog::getText( nullptr, tr( "Layer Properties" ),
                                          tr( "Layer name:" ), QLineEdit::Normal,
                                          tr( "Bitmap Layer" ), &ok );
    if ( ok && !text.isEmpty() )
    {
        mEditor->layers()->createBitmapLayer( text );
    }
    return Status::OK;
}

Status ActionCommands::addNewVectorLayer()
{
    bool ok;
    QString text = QInputDialog::getText( nullptr, tr( "Layer Properties" ),
                                         tr( "Layer name:" ), QLineEdit::Normal,
                                         tr( "Vector Layer" ), &ok );
    if ( ok && !text.isEmpty() )
    {
        mEditor->layers()->createVectorLayer( text );
    }

    return Status::OK;
}

Status ActionCommands::addNewCameraLayer()
{
    bool ok;
    QString text = QInputDialog::getText( nullptr, tr( "Layer Properties" ),
                                         tr( "Layer name:" ), QLineEdit::Normal,
                                         tr( "Camera Layer" ), &ok );
    if ( ok && !text.isEmpty() )
    {
        mEditor->layers()->createCameraLayer( text );
    }
    
    return Status::OK;

}

Status ActionCommands::addNewSoundLayer()
{
    bool ok = false;
    QString strLayerName = QInputDialog::getText( nullptr, tr( "Layer Properties" ),
                                          tr( "Layer name:" ), QLineEdit::Normal,
                                          tr( "Sound Layer" ), &ok );
    if ( ok && !strLayerName.isEmpty() )
    {
        Layer* layer = mEditor->layers()->createSoundLayer( strLayerName );
        mEditor->layers()->setCurrentLayer( layer );

        return Status::OK;
    }
    return Status::FAIL;
}
