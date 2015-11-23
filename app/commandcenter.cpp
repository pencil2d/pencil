#include "commandcenter.h"

#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

#include "pencildef.h"
#include "editor.h"
#include "viewmanager.h"
#include "layermanager.h"
#include "soundmanager.h"
#include "util.h"

#include "layerbitmap.h"
#include "layervector.h"
#include "layersound.h"
#include "bitmapimage.h"
#include "vectorimage.h"

#include "filedialogex.h"


CommandCenter::CommandCenter( QObject* parent ) : QObject( parent ) {}
CommandCenter::~CommandCenter() {}

Status CommandCenter::importSound()
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
        
        //qDebug() << "Button clicked: 0" << ret;

        if ( buttonClicked != QMessageBox::AcceptRole )
        {
            return Status::SAFE;
        }

        // Create new sound layer.
        if ( addNewSoundLayer() == false )
        {
            Q_ASSERT( false );
            return Status::FAIL;
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

    FileDialogEx fileDialog( this );
    QString strSoundFile = fileDialog.openFile( EFile::SOUND );

    Status st = mEditor->sound()->loadSound( layer, mEditor->currentFrame(), strSoundFile );
    
    //mTimeLine->updateContent();

    return st;
}

void CommandCenter::ZoomIn()
{
    float newScaleValue = mEditor->view()->scaling() * 1.2;
    mEditor->view()->scale( newScaleValue );
}

void CommandCenter::ZoomOut()
{
    float newScaleValue = mEditor->view()->scaling() * 0.8333;
    mEditor->view()->scale( newScaleValue );
}

void CommandCenter::flipX()
{
    auto view = mEditor->view();

    bool b = view->isFlipHorizontal();
    view->flipHorizontal( !b );
}

void CommandCenter::flipY()
{
    auto view = mEditor->view();

    bool b = view->isFlipVertical();
    view->flipVertical( !b );
}

void CommandCenter::GotoNextFrame()
{
}

void CommandCenter::GotoPrevFrame()
{
}

void CommandCenter::GotoNextKeyFrame()
{
}

void CommandCenter::GotoPrevKeyFrame()
{
}

bool CommandCenter::addNewSoundLayer()
{
    bool ok = false;
    QString strLayerName = QInputDialog::getText( nullptr, tr( "Layer Properties" ),
                                          tr( "Layer name:" ), QLineEdit::Normal,
                                          tr( "Sound Layer" ), &ok );
    if ( ok && !strLayerName.isEmpty() )
    {
        Layer* layer = mEditor->layers()->createSoundLayer( strLayerName );
        mEditor->layers()->setCurrentLayer( layer );

        return true;
    }
    return false;
}
