/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2014 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "editor.h"
#include <memory>
#include <iostream>
#include <QApplication>
#include <QClipboard>
#include <QBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QSvgGenerator>
#include <QMessageBox>
#include <QImageReader>
#include <QComboBox>
#include <QSlider>
#include <QFileDialog>
#include <QInputDialog>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QDragEnterEvent>
#include <QDropEvent>

#include "object.h"
#include "editorstate.h"
#include "vectorimage.h"
#include "bitmapimage.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layersound.h"
#include "layercamera.h"
#include "keyframefactory.h"

#include "colormanager.h"
#include "toolmanager.h"
#include "layermanager.h"
#include "playbackmanager.h"
#include "viewmanager.h"
#include "preferencemanager.h"
#include "soundmanager.h"

#include "scribblearea.h"
#include "timeline.h"
#include "util.h"

#define MIN(a,b) ((a)>(b)?(b):(a))


static BitmapImage g_clipboardBitmapImage;
static VectorImage g_clipboardVectorImage;


Editor::Editor( QObject* parent ) : QObject( parent )
{
	mBackupIndex = -1;
	clipboardBitmapOk = false;
	clipboardVectorOk = false;	
}

Editor::~Editor()
{
	// a lot more probably needs to be cleaned here...
	clearUndoStack();
}

bool Editor::init()
{
	// Initialize managers
	mColorManager = new ColorManager( this );
	mLayerManager = new LayerManager( this );
	mToolManager = new ToolManager( this );
	mPlaybackManager = new PlaybackManager( this );
	mViewManager = new ViewManager( this );
	mPreferenceManager = new PreferenceManager( this );
    mSoundManager = new SoundManager( this );

	mAllManagers =
	{
		mColorManager,
		mToolManager,
		mLayerManager,
		mPlaybackManager,
		mViewManager,
		mPreferenceManager,
        mSoundManager
	};

    for ( BaseManager* pManager : mAllManagers )
	{
		pManager->setEditor( this );
		pManager->init();
	}
    //setAcceptDrops( true ); // TODO: drop event

    makeConnections();

    mIsAutosave = mPreferenceManager->isOn(SETTING::AUTO_SAVE);
    autosaveNumber = mPreferenceManager->getInt(SETTING::AUTO_SAVE_NUMBER);

    //onionPrevFramesNum = mPreferenceManager->getInt(SETTING::ONION_PREV_FRAMES_NUM);
    //onionNextFramesNum = mPreferenceManager->getInt(SETTING::ONION_NEXT_FRAMES_NUM);

	return true;
}

int Editor::currentFrame()
{
	return mFrame;
}

int Editor::fps()
{
    return mPlaybackManager->fps();
}

void Editor::makeConnections()
{
    connect( mPreferenceManager, &PreferenceManager::optionChanged, this, &Editor::settingUpdated );
	connect( QApplication::clipboard(), &QClipboard::dataChanged, this, &Editor::clipboardChanged );
}

void Editor::dragEnterEvent( QDragEnterEvent* event )
{
	event->acceptProposedAction();
}

void Editor::dropEvent( QDropEvent* event )
{
	if ( event->mimeData()->hasUrls() )
	{
		for ( int i = 0; i < event->mimeData()->urls().size(); i++ )
		{
			if ( i > 0 ) scrubForward();
			QUrl url = event->mimeData()->urls()[ i ];
			QString filePath = url.toLocalFile();
			if ( filePath.endsWith( ".png" ) || filePath.endsWith( ".jpg" ) || filePath.endsWith( ".jpeg" ) )
				importImage( filePath );
			if ( filePath.endsWith( ".aif" ) || filePath.endsWith( ".mp3" ) || filePath.endsWith( ".wav" ) )
				//importSound( filePath );
				nullptr;
		}
	}
}

void Editor::settingUpdated(SETTING setting)
{
    switch (setting)
    {
    case SETTING::AUTO_SAVE:
        mIsAutosave = mPreferenceManager->isOn( SETTING::AUTO_SAVE );
        break;
    case SETTING::AUTO_SAVE_NUMBER:
        autosaveNumber = mPreferenceManager->getInt( SETTING::AUTO_SAVE_NUMBER );
        break;
    case SETTING::ONION_TYPE:
        mScribbleArea->updateAllFrames();
        emit updateTimeLine();
        break;
    default:
        break;
    }
}

void Editor::backup( QString undoText )
{
	if ( lastModifiedLayer > -1 && lastModifiedFrame > 0 )
	{
		backup( lastModifiedLayer, lastModifiedFrame, undoText );
	}
	if ( lastModifiedLayer != layers()->currentLayerIndex() || lastModifiedFrame != currentFrame() )
	{
		backup( layers()->currentLayerIndex(), currentFrame(), undoText );
	}
}

void Editor::backup( int backupLayer, int backupFrame, QString undoText )
{
	while ( mBackupList.size() - 1 > mBackupIndex && mBackupList.size() > 0 )
	{
		delete mBackupList.takeLast();
	}
	while ( mBackupList.size() > 19 )   // we authorize only 20 levels of cancellation
	{
		delete mBackupList.takeFirst();
		mBackupIndex--;
	}
	Layer* layer = mObject->getLayer( backupLayer );
	if ( layer != NULL )
	{
		if ( layer->type() == Layer::BITMAP )
		{
			BackupBitmapElement* element = new BackupBitmapElement();
			element->layer = backupLayer;
			element->frame = backupFrame;
			element->undoText = undoText;
			element->somethingSelected = this->getScribbleArea()->somethingSelected;
			element->mySelection = this->getScribbleArea()->mySelection;
			element->myTransformedSelection = this->getScribbleArea()->myTransformedSelection;
			element->myTempTransformedSelection = this->getScribbleArea()->myTempTransformedSelection;
			BitmapImage* bitmapImage = ( (LayerBitmap*)layer )->getLastBitmapImageAtFrame( backupFrame, 0 );
			if ( bitmapImage != NULL )
			{
				element->bitmapImage = bitmapImage->copy();  // copy the image
				mBackupList.append( element );
				mBackupIndex++;
			}
		}
		if ( layer->type() == Layer::VECTOR )
		{
			BackupVectorElement* element = new BackupVectorElement();
			element->layer = backupLayer;
			element->frame = backupFrame;
			element->undoText = undoText;
			element->somethingSelected = this->getScribbleArea()->somethingSelected;
			element->mySelection = this->getScribbleArea()->mySelection;
			element->myTransformedSelection = this->getScribbleArea()->myTransformedSelection;
			element->myTempTransformedSelection = this->getScribbleArea()->myTempTransformedSelection;
			VectorImage* vectorImage = ( (LayerVector*)layer )->getLastVectorImageAtFrame( backupFrame, 0 );
			if ( vectorImage != NULL )
			{
				element->vectorImage = *vectorImage;  // copy the image (that works but I should also provide a copy() method)
				mBackupList.append( element );
				mBackupIndex++;
			}
		}
	}
}

void BackupBitmapElement::restore( Editor* editor )
{
	Layer* layer = editor->object()->getLayer( this->layer );
	if ( layer != NULL )
	{
		if ( layer->type() == Layer::BITMAP )
		{
			*( ( (LayerBitmap*)layer )->getLastBitmapImageAtFrame( this->frame, 0 ) ) = this->bitmapImage;  // restore the image
		}
	}
	editor->getScribbleArea()->somethingSelected = this->somethingSelected;
	editor->getScribbleArea()->mySelection = this->mySelection;
	editor->getScribbleArea()->myTransformedSelection = this->myTransformedSelection;
	editor->getScribbleArea()->myTempTransformedSelection = this->myTempTransformedSelection;

	editor->updateFrame( this->frame );
	editor->scrubTo( this->frame );
}

void BackupVectorElement::restore( Editor* editor )
{
	Layer* layer = editor->object()->getLayer( this->layer );
	if ( layer != NULL )
	{
		if ( layer->type() == Layer::VECTOR )
		{
			*( ( (LayerVector*)layer )->getLastVectorImageAtFrame( this->frame, 0 ) ) = this->vectorImage;  // restore the image
			//((LayerVector*)layer)->getLastVectorImageAtFrame(this->frame, 0)->setModified(true); // why?
			//editor->scribbleArea->setModified(layer, this->frame);
		}
	}
	editor->getScribbleArea()->somethingSelected = this->somethingSelected;
	editor->getScribbleArea()->mySelection = this->mySelection;
	editor->getScribbleArea()->myTransformedSelection = this->myTransformedSelection;
	editor->getScribbleArea()->myTempTransformedSelection = this->myTempTransformedSelection;

	editor->updateFrameAndVector( this->frame );
	editor->scrubTo( this->frame );
}

void Editor::undo()
{
	if ( mBackupList.size() > 0 && mBackupIndex > -1 )
	{
		if ( mBackupIndex == mBackupList.size() - 1 )
		{
			BackupElement* lastBackupElement = mBackupList[ mBackupIndex ];
			if ( lastBackupElement->type() == BackupElement::BITMAP_MODIF )
			{
				BackupBitmapElement* lastBackupBitmapElement = (BackupBitmapElement*)lastBackupElement;
				backup( lastBackupBitmapElement->layer, lastBackupBitmapElement->frame, "NoOp" );
				mBackupIndex--;
			}
			if ( lastBackupElement->type() == BackupElement::VECTOR_MODIF )
			{
				BackupVectorElement* lastBackupVectorElement = (BackupVectorElement*)lastBackupElement;
				backup( lastBackupVectorElement->layer, lastBackupVectorElement->frame, "NoOp" );
				mBackupIndex--;
			}
		}
		//
		mBackupList[ mBackupIndex ]->restore( this );
		mBackupIndex--;
        mScribbleArea->cancelTransformedSelection();
        mScribbleArea->calculateSelectionRect(); // really ugly -- to improve
	}
}

void Editor::redo()
{
	if ( mBackupList.size() > 0 && mBackupIndex < mBackupList.size() - 2 )
	{
		mBackupIndex++;
		mBackupList[ mBackupIndex + 1 ]->restore( this );
	}
}

void Editor::clearUndoStack()
{
	mBackupIndex = -1;
	while ( !mBackupList.isEmpty() )
	{
		delete mBackupList.takeLast();
	}
	lastModifiedLayer = -1;
	lastModifiedFrame = -1;
}

void Editor::cut()
{
	copy();
	mScribbleArea->deleteSelection();
	mScribbleArea->deselectAll();
}

void Editor::copy()
{
	Layer* layer = mObject->getLayer( layers()->currentLayerIndex() );
	if ( layer != NULL )
	{
		if ( layer->type() == Layer::BITMAP )
		{
			if ( mScribbleArea->somethingSelected )
			{
				g_clipboardBitmapImage = ( (LayerBitmap*)layer )->getLastBitmapImageAtFrame( currentFrame(), 0 )->copy( mScribbleArea->getSelection().toRect() );  // copy part of the image
			}
			else
			{
				g_clipboardBitmapImage = ( (LayerBitmap*)layer )->getLastBitmapImageAtFrame( currentFrame(), 0 )->copy();  // copy the whole image
			}
			clipboardBitmapOk = true;
			if ( g_clipboardBitmapImage.image() != NULL ) QApplication::clipboard()->setImage( *g_clipboardBitmapImage.image() );
		}
		if ( layer->type() == Layer::VECTOR )
		{
			clipboardVectorOk = true;
			g_clipboardVectorImage = *( ( (LayerVector*)layer )->getLastVectorImageAtFrame( currentFrame(), 0 ) );  // copy the image
		}
	}
}

void Editor::paste()
{
	Layer* layer = mObject->getLayer( layers()->currentLayerIndex() );
	if ( layer != NULL )
	{
		if ( layer->type() == Layer::BITMAP && g_clipboardBitmapImage.image() != NULL )
		{
			backup( tr( "Paste" ) );
			BitmapImage tobePasted = g_clipboardBitmapImage.copy();
			qDebug() << "to be pasted --->" << tobePasted.image()->size();
			if ( mScribbleArea->somethingSelected )
			{
				QRectF selection = mScribbleArea->getSelection();
				if ( g_clipboardBitmapImage.width() <= selection.width() && g_clipboardBitmapImage.height() <= selection.height() )
				{
					tobePasted.moveTopLeft( selection.topLeft() );
				}
				else
				{
					tobePasted.transform( selection, true );
				}
			}
			auto pLayerBitmap = static_cast<LayerBitmap*>( layer );
			pLayerBitmap->getLastBitmapImageAtFrame( currentFrame(), 0 )->paste( &tobePasted ); // paste the clipboard
		}
		else if ( layer->type() == Layer::VECTOR && clipboardVectorOk )
		{
			backup( tr( "Paste" ) );
			mScribbleArea->deselectAll();
			VectorImage* vectorImage = ( (LayerVector*)layer )->getLastVectorImageAtFrame( currentFrame(), 0 );
			vectorImage->paste( g_clipboardVectorImage );  // paste the clipboard
			mScribbleArea->setSelection( vectorImage->getSelectionRect(), true );
			//((LayerVector*)layer)->getLastVectorImageAtFrame(backupFrame, 0)->modification(); ????
		}
	}
	mScribbleArea->updateCurrentFrame();
}

void Editor::deselectAll()
{
	mScribbleArea->deselectAll();
}

void Editor::clipboardChanged()
{
	if ( clipboardBitmapOk == false )
	{
		g_clipboardBitmapImage.setImage( new QImage( QApplication::clipboard()->image() ) );
		g_clipboardBitmapImage.bounds() = QRect( g_clipboardBitmapImage.topLeft(), g_clipboardBitmapImage.image()->size() );
		qDebug() << "New clipboard image" << g_clipboardBitmapImage.image()->size();
	}
	else
	{
		clipboardBitmapOk = false;
		qDebug() << "The image has been saved in the clipboard";
	}
}

int Editor::allLayers()
{
	return mScribbleArea->showAllLayers();
}

void Editor::toggleMirror()
{
	bool flipX = view()->isFlipHorizontal();
	view()->flipHorizontal( !flipX );
}

void Editor::toggleMirrorV()
{
	bool flipY = view()->isFlipVertical();
	view()->flipVertical( !flipY );
}

void Editor::toggleShowAllLayers()
{
	mScribbleArea->toggleShowAllLayers();
    emit updateTimeLine();
}

void Editor::toogleOnionSkinType()
{
    QString onionSkinState = mPreferenceManager->getString(SETTING::ONION_TYPE);
    QString newState;
    if (onionSkinState == "relative")
    {
        newState = "absolute";
    }
    else
    {
        newState = "relative";
    }

    mPreferenceManager->set(SETTING::ONION_TYPE, newState);
}

Status Editor::setObject( Object* newObject )
{
    if ( newObject == nullptr )
    {
        Q_ASSERT( false );
        return Status::INVALID_ARGUMENT;
    }

    if ( newObject == mObject.get() )
    {
        return Status::SAFE;
    }

    mObject.reset( newObject );


    for ( BaseManager* m : mAllManagers )
    {
        m->onObjectLoaded( mObject.get() );
    }

	g_clipboardVectorImage.setObject( newObject );
    
    updateObject();

    return Status::OK;
}

void Editor::updateObject()
{
    scrubTo( mObject->editorState()->mCurrentFrame );
    if (layers() != NULL)
    {
      layers()->setCurrentLayer( mObject->editorState()->mCurrentLayer );
    }

	clearUndoStack();

	if ( mScribbleArea )
	{
		mScribbleArea->updateAllFrames();
	}

    emit updateLayerCount();
}

void Editor::createExportMovieSizeBox()
{
	/*
	exportMovieDialog_format = new QComboBox();
	exportMovieDialog_format->addItem( "AUTO" );
	exportMovieDialog_format->addItem( "MOV" );
	exportMovieDialog_format->addItem( "MPEG2/AVI" );
	exportMovieDialog_format->addItem( "MPEG4/AVI" );
	exportMovieDialog_format->addItem( "MPEG4/MP4" );
	exportMovieDialog_fpsBox = new QSpinBox( mMainWindow );
	exportMovieDialog_fpsBox->setMinimum( 1 );
	exportMovieDialog_fpsBox->setMaximum( 60 );
	exportMovieDialog_fpsBox->setValue( defaultFps );
	exportMovieDialog_fpsBox->setFixedWidth( 40 );
	*/
}

void Editor::createExportMovieDialog()
{
	/*
	exportMovieDialog = new QDialog( mMainWindow, Qt::Dialog );
	QGridLayout* mainLayout = new QGridLayout;

	QGroupBox* resolutionBox = new QGroupBox( tr( "Resolution" ) );
	if ( !exportMovieDialog_hBox || !exportMovieDialog_vBox )
	{
	createExportMovieSizeBox();
	}
	QGridLayout* resolutionLayout = new QGridLayout;
	resolutionLayout->addWidget( exportMovieDialog_hBox, 0, 0 );
	resolutionLayout->addWidget( exportMovieDialog_vBox, 0, 1 );
	resolutionBox->setLayout( resolutionLayout );

	QGroupBox* formatBox = new QGroupBox( tr( "Format" ) );
	QGridLayout* formatLayout = new QGridLayout;
	QLabel* label1 = new QLabel( "Save as" );
	formatLayout->addWidget( label1, 0, 0 );
	formatLayout->addWidget( exportMovieDialog_format, 0, 1 );
	QLabel* label2 = new QLabel( "Fps" );
	formatLayout->addWidget( label2, 0, 2 );
	formatLayout->addWidget( exportMovieDialog_fpsBox, 0, 3 );
	formatBox->setLayout( formatLayout );

	QDialogButtonBox* buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
	connect( buttonBox, SIGNAL( accepted() ), exportMovieDialog, SLOT( accept() ) );
	connect( buttonBox, SIGNAL( rejected() ), exportMovieDialog, SLOT( reject() ) );

	mainLayout->addWidget( resolutionBox, 0, 0 );
	mainLayout->addWidget( formatBox, 1, 0 );
	mainLayout->addWidget( buttonBox, 2, 0 );
	exportMovieDialog->setLayout( mainLayout );
	exportMovieDialog->setWindowTitle( tr( "Options" ) );
	exportMovieDialog->setModal( true );
	*/
}



bool Editor::exportSeqCLI( QString filePath = "", QString format = "PNG" )
{
	int width = mScribbleArea->getViewRect().toRect().width();
	int height = mScribbleArea->getViewRect().toRect().height();

	QSize exportSize = QSize( width, height );
	QByteArray exportFormat( format.toLatin1() );

	QTransform view = RectMapTransform( mScribbleArea->getViewRect(), QRectF( QPointF( 0, 0 ), exportSize ) );
	view = mScribbleArea->getView() * view;

	int projectLength = layers()->projectLength();

	mObject->exportFrames( 1, projectLength, layers()->currentLayer(),
						   exportSize,
						   filePath,
						   exportFormat, -1, false, true, NULL, 0 );
	return true;
}

QString Editor::workingDir() const
{
    return mObject->workingDir();
}

/*
bool Editor::exportMov()
{
QSettings settings( "Pencil", "Pencil" );
QString initialPath = settings.value( "lastExportPath", QVariant( QDir::homePath() ) ).toString();
if ( initialPath.isEmpty() ) initialPath = QDir::homePath() + "/untitled.avi";
//  QString filePath = QFileDialog::getSaveFileName(this, tr("Export As"),initialPath);
QString filePath = QFileDialog::getSaveFileName( mMainWindow, tr( "Export Movie As..." ), initialPath, tr( "AVI (*.avi);;MOV(*.mov);;WMV(*.wmv)" ) );
if ( filePath.isEmpty() )
{
return false;
}
else
{
settings.setValue( "lastExportPath", QVariant( filePath ) );
if ( !exportMovieDialog ) createExportMovieDialog();
exportMovieDialog_hBox->setValue( mScribbleArea->getViewRect().toRect().width() );
exportMovieDialog_vBox->setValue( mScribbleArea->getViewRect().toRect().height() );
exportMovieDialog->exec();
if ( exportMovieDialog->result() == QDialog::Rejected ) return false;

QSize exportSize = QSize( exportMovieDialog_hBox->value(), exportMovieDialog_vBox->value() );
QTransform view = map( mScribbleArea->getViewRect(), QRectF( QPointF( 0, 0 ), exportSize ) );
view = mScribbleArea->getView() * view;

int projectLength = layers()->projectLength();
int fps = playback()->fps();

ExportMovieParameters par;
par.startFrame = 1;
par.endFrame = projectLength;
par.view = view;
par.currentLayer = layers()->currentLayer();
par.exportSize = exportSize;
par.filePath = filePath;
par.fps = fps;
par.exportFps = exportMovieDialog_fpsBox->value();
par.exportFormat = exportMovieDialog_format->currentText();
mObject->exportMovie( par );

return true;
}
}
*/

bool Editor::importBitmapImage( QString filePath )
{
	backup( tr( "ImportImg" ) );

	QImageReader reader( filePath );

	Q_ASSERT( layers()->currentLayer()->type() == Layer::BITMAP );
	auto layer = static_cast<LayerBitmap*>( layers()->currentLayer() );

	QImage img( reader.size(), QImage::Format_ARGB32_Premultiplied );
  if ( img.isNull() )
  {
    return false;
  }

	while ( reader.read( &img ) )
	{
		if ( !layer->keyExists( currentFrame() ) )
		{
			addNewKey();
		}
		BitmapImage* bitmapImage = layer->getBitmapImageAtFrame( currentFrame() );

		QRect boundaries = img.rect();
		boundaries.moveTopLeft( mScribbleArea->getCentralPoint().toPoint() - QPoint( boundaries.width() / 2, boundaries.height() / 2 ) );

		BitmapImage* importedBitmapImage = new BitmapImage( boundaries, img );
		bitmapImage->paste( importedBitmapImage );

		scrubTo( currentFrame() + 1 );
	}

	return true;
}

bool Editor::importVectorImage( QString filePath )
{
	Q_ASSERT( layers()->currentLayer()->type() == Layer::VECTOR );

	backup( tr( "ImportImg" ) );

	auto layer = static_cast<LayerVector*>( layers()->currentLayer() );

	VectorImage* vectorImage = ( (LayerVector*)layer )->getVectorImageAtFrame( currentFrame() );
	if ( vectorImage == NULL )
	{
		addNewKey();
		vectorImage = ( (LayerVector*)layer )->getVectorImageAtFrame( currentFrame() );
	}
	VectorImage* importedVectorImage = new VectorImage;
	bool ok = importedVectorImage->read( filePath );
	if ( ok )
	{
		importedVectorImage->selectAll();
		vectorImage->paste( *importedVectorImage );
	}
	/*
	else
	{
	QMessageBox::warning( mMainWindow,
	tr( "Warning" ),
	tr( "Unable to load vector image.<br><b>TIP:</b> Use Vector layer to import vectors." ),
	QMessageBox::Ok,
	QMessageBox::Ok );
	}
	*/
	return ok;
}

bool Editor::importImage( QString filePath )
{
	Layer* layer = layers()->currentLayer();

	switch ( layer->type() )
	{
	case Layer::BITMAP:
		return importBitmapImage( filePath );

	case Layer::VECTOR:
		return importVectorImage( filePath );

	default:
	{
		//mLastError = Status::ERROR_INVALID_LAYER_TYPE;
		return false;
	}
	}
}

void Editor::updateFrame( int frameNumber )
{
	mScribbleArea->updateFrame( frameNumber );
}

void Editor::updateFrameAndVector( int frameNumber )
{
    mScribbleArea->updateAllVectorLayersAt( frameNumber );
}

void Editor::updateCurrentFrame()
{
    mScribbleArea->updateCurrentFrame();
}

void Editor::scrubTo( int frame )
{
	if ( frame < 1 )
	{
		frame = 1;
	}
	int oldFrame = mFrame;
	mFrame = frame;

	Q_EMIT currentFrameChanged( frame );
	Q_EMIT currentFrameChanged( oldFrame );
    
    // FIXME: should not emit Timeline update here.
    // Editor must be an individual class.
    // Will remove all Timeline related code in Editor class.
    if ( mPlaybackManager && !mPlaybackManager->isPlaying() )
    {
        emit updateTimeLine(); // needs to update the timeline to update onion skin positions
    }
}

void Editor::scrubForward()
{
	scrubTo( currentFrame() + 1 );
}

void Editor::scrubBackward()
{
	if ( currentFrame() > 1 )
	{
		scrubTo( currentFrame() - 1 );
	}
}

void Editor::moveFrameForward()
{
	Layer* layer = layers()->currentLayer();
	if ( layer != NULL )
	{
		if ( layer->moveKeyFrameForward( currentFrame() ) )
		{
			mScribbleArea->updateAllFrames();
			scrubForward();
		}
	}
}

void Editor::moveFrameBackward()
{
	Layer* layer = layers()->currentLayer();
	if ( layer != NULL )
	{
		if ( layer->moveKeyFrameBackward( currentFrame() ) )
		{
			mScribbleArea->updateAllFrames();
			scrubBackward();
		}
	}
}

KeyFrame* Editor::addNewKey()
{
	return addKeyFame( layers()->currentLayerIndex(), currentFrame() );
}

void Editor::duplicateKey()
{
	Layer* layer = mObject->getLayer( layers()->currentLayerIndex() );
	if ( layer != NULL )
	{
        if ( layer->type() == Layer::VECTOR || layer->type() == Layer::BITMAP )
		{
            // Will copy the selection if any or the entire image if there is none
            //
            if(!mScribbleArea->somethingSelected) {
                mScribbleArea->selectAll();
            }

            copy();
            addNewKey();
            paste();

            mScribbleArea->setModified( layers()->currentLayerIndex(), currentFrame() );
            mScribbleArea->update();
		}
	}
}

KeyFrame* Editor::addKeyFame( int layerNumber, int frameIndex )
{
	Layer* layer = mObject->getLayer( layerNumber );
	if ( layer == NULL )
	{
        Q_ASSERT( false );
		return nullptr;
	}

	bool isOK = false;

    while ( layer->keyExists( frameIndex ) )
    {
        frameIndex += 1;
    }
    
    KeyFrame* keyFrame = KeyFrameFactory::create( layer->type(), mObject.get() );
    if ( keyFrame != nullptr )
    {
        isOK = layer->addKeyFrame( frameIndex, keyFrame );
    }
    else
    {
        Q_ASSERT( false );
    }

	if ( isOK )
	{
        scrubTo( frameIndex ); // currentFrameChanged() emit inside.
        //getScribbleArea()->updateCurrentFrame();
	}

    return keyFrame;
}

void Editor::removeKey()
{
	Layer* layer = layers()->currentLayer();
	if ( layer != NULL )
    {
		layer->removeKeyFrame( currentFrame() );
		
		scrubBackward();
        mScribbleArea->updateCurrentFrame();
	}
    Q_EMIT layers()->currentLayerChanged( layers()->currentLayerIndex() ); // trigger timeline repaint.
}

void Editor::scrubNextKeyFrame()
{
	Layer* layer = layers()->currentLayer();
	Q_ASSERT( layer );

	int nextPosition = layer->getNextKeyFramePosition( currentFrame() );
	scrubTo( nextPosition );
}

void Editor::scrubPreviousKeyFrame()
{
	Layer* layer = mObject->getLayer( layers()->currentLayerIndex() );
	Q_ASSERT( layer );

	int prevPosition = layer->getPreviousKeyFramePosition( currentFrame() );
	scrubTo( prevPosition );
}

void Editor::setCurrentLayer( int layerNumber )
{
	layers()->setCurrentLayer( layerNumber );
	mScribbleArea->updateAllFrames();
}

void Editor::switchVisibilityOfLayer( int layerNumber )
{
	Layer* layer = mObject->getLayer( layerNumber );
	if ( layer != NULL ) layer->switchVisibility();
	mScribbleArea->updateAllFrames();

	emit updateTimeLine();
}

void Editor::moveLayer( int i, int j )
{
	mObject->moveLayer( i, j );
	if ( j < i )
	{
		layers()->setCurrentLayer( j );
	}
	else
	{
		layers()->setCurrentLayer( j - 1 );
	}
	emit updateTimeLine();
	mScribbleArea->updateAllFrames();
}

void Editor::clearCurrentFrame()
{
	mScribbleArea->clearImage();
}
