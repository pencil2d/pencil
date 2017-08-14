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

#include "actioncommands.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QApplication>
#include <QDesktopServices>

#include "pencildef.h"
#include "editor.h"
#include "object.h"
#include "viewmanager.h"
#include "layermanager.h"
#include "soundmanager.h"
#include "playbackmanager.h"
#include "preferencemanager.h"
#include "util.h"

//#include "layerbitmap.h"
//#include "layervector.h"
#include "layercamera.h"
#include "layersound.h"
#include "bitmapimage.h"
#include "vectorimage.h"
#include "soundclip.h"

#include "movieexporter.h"
#include "filedialogex.h"
#include "exportmoviedialog.h"



ActionCommands::ActionCommands( QWidget* parent ) : QObject( parent )
{
    mParent = parent;
}

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
        bool ok = false;
        QString strLayerName = QInputDialog::getText( mParent, tr( "Layer Properties" ),
                                                      tr( "Layer name:" ), QLineEdit::Normal,
                                                      tr( "Sound Layer" ), &ok );
        if ( ok && !strLayerName.isEmpty() )
        {
            Layer* newLayer = mEditor->layers()->createSoundLayer( strLayerName );
            mEditor->layers()->setCurrentLayer( newLayer );
        }
        else
        {
            Q_ASSERT( false );
            return Status::FAIL;
        }
    }

    layer = mEditor->layers()->currentLayer();

    if ( layer->keyExists( mEditor->currentFrame() ) )
    {
        QMessageBox::warning( nullptr,
                              "",
                              tr( "A sound clip already exists on this frame! Please select another frame or layer." ) );
        return Status::SAFE;
    }

    FileDialog fileDialog( mParent );
    QString strSoundFile = fileDialog.openFile( FileType::SOUND );

    Status st = mEditor->sound()->loadSound( layer, mEditor->currentFrame(), strSoundFile );

    return st;
}

Status ActionCommands::exportMovie()
{
    ExportMovieDialog exportDialog( mParent );

    std::vector< std::pair<QString, QSize > > camerasInfo;
    auto cameraLayers = mEditor->object()->getLayersByType< LayerCamera >();
    for ( LayerCamera* i : cameraLayers )
    {
        camerasInfo.push_back( std::make_pair( i->name(), i->getViewSize() ) );
    }

    auto currLayer = mEditor->layers()->currentLayer();
    if ( currLayer->type() == Layer::CAMERA )
    {
        QString strName = currLayer->name();
        auto it = std::find_if( camerasInfo.begin(), camerasInfo.end(),
            [strName] ( std::pair<QString, QSize> p )
        {
            return p.first == strName;
        } );

        Q_ASSERT(it != camerasInfo.end());

        std::swap( camerasInfo[ 0 ], *it );
    }

    exportDialog.setCamerasInfo( camerasInfo );
    exportDialog.setDefaultRange( 1, mEditor->layers()->projectLength() );
    exportDialog.exec();
    if ( exportDialog.result() == QDialog::Rejected )
    {
        return Status::SAFE;
    }
	QString strMoviePath = exportDialog.getFilePath();

    ExportMovieDesc desc;
    desc.strFileName   = strMoviePath;
    desc.startFrame    = exportDialog.getStartFrame();
    desc.endFrame      = exportDialog.getEndFrame();
    desc.fps           = mEditor->playback()->fps();
    desc.exportSize    = exportDialog.getExportSize();
    desc.strCameraName = exportDialog.getSelectedCameraName();

    QProgressDialog progressDlg;
    progressDlg.setWindowModality( Qt::WindowModal );
    progressDlg.setLabelText( tr("Exporting movie...") );
    Qt::WindowFlags eFlags = Qt::Dialog | Qt::WindowTitleHint;
    progressDlg.setWindowFlags( eFlags );
    progressDlg.show();

    MovieExporter ex;

    connect( &progressDlg, &QProgressDialog::canceled, [&ex]
    {
        ex.cancel();
    } );

    Status st = ex.run( mEditor->object(), desc, [ &progressDlg ]( float f )
    {
        progressDlg.setValue( (int)(f * 100.f) );
        QApplication::processEvents( QEventLoop::ExcludeUserInputEvents );
    } );

    if ( st.ok() && QFile::exists( strMoviePath ) )
    {
        auto btn = QMessageBox::question( mParent,
                                          "Pencil2D",
                                          tr( "Finished. Open movie now?" ) );
        if ( btn == QMessageBox::Yes )
        {
            QDesktopServices::openUrl( QUrl::fromLocalFile( strMoviePath ) );
        }
    }

    return Status::OK;
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

void ActionCommands::flipSelectionX()
{
   bool flipVertical = false;
   mEditor->flipSelection(flipVertical);
}

void ActionCommands::flipSelectionY()
{
    bool flipVertical = true;
    mEditor->flipSelection(flipVertical);
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
    mEditor->scrubForward();
}

void ActionCommands::GotoPrevFrame()
{
    mEditor->scrubBackward();
}

void ActionCommands::GotoNextKeyFrame()
{
    mEditor->scrubNextKeyFrame();
}

void ActionCommands::GotoPrevKeyFrame()
{
    mEditor->scrubPreviousKeyFrame();
}

void ActionCommands::addNewKey()
{
    KeyFrame* key = mEditor->addNewKey();

    SoundClip* clip = dynamic_cast< SoundClip* >( key );
    if ( clip )
    {
        FileDialog fileDialog( mParent );
        QString strSoundFile = fileDialog.openFile( FileType::SOUND );

        if ( strSoundFile.isEmpty() )
        {
            mEditor->removeKey();
            return;
        }
        Status st = mEditor->sound()->loadSound( clip, strSoundFile );
        Q_ASSERT( st.ok() );
    }
}

void ActionCommands::removeKey()
{
    mEditor->removeKey();

    Layer* layer = mEditor->layers()->currentLayer();
    if ( layer->keyFrameCount() == 0 )
    {
        switch ( layer->type() )
        {
            case Layer::BITMAP:
            case Layer::VECTOR:
            case Layer::CAMERA:
                layer->addNewEmptyKeyAt( 1 );
                break;
            default:
                break;
        }
    }
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
