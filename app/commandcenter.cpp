#include "commandcenter.h"

#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

#include "pencildef.h"
#include "editor.h"
#include "viewmanager.h"
#include "layermanager.h"
#include "util.h"

#include "layerbitmap.h"
#include "layervector.h"
#include "layersound.h"
#include "bitmapimage.h"
#include "vectorimage.h"

#include "filedialogex.h"


CommandCenter::CommandCenter( QObject* parent ) : QObject( parent ) {}
CommandCenter::~CommandCenter() {}

void CommandCenter::importSound()
{
	Layer* layer = mEditor->layers()->currentLayer();
	Q_ASSERT( layer );

	if ( layer->type() != Layer::SOUND )
	{
		QMessageBox msg;
		msg.setText( tr( "No sound layer exists as a destination for your import. Create a new sound layer?" ) );
		msg.addButton( tr( "Create sound layer" ), QMessageBox::AcceptRole );
		msg.addButton( tr( "Don't create layer" ), QMessageBox::RejectRole );
		int ret = msg.exec();

		if ( ret != QMessageBox::Ok )
		{
			return;
		}

		// Create new sound layer.
		addNewSoundLayer();
		layer = mEditor->layers()->currentLayer();
	}

	LayerSound* layerSound = static_cast<LayerSound*>( layer );

	if ( layerSound->keyFrameCount() > 0 )
	{
		QMessageBox msg;
		msg.setText( "The sound layer you have selected already contains a sound item. Please select another." );
		msg.exec();
		return;
	}

	FileDialogEx fileDialog( this );
	QString strSoundFile = fileDialog.openFile( EFile::SOUND );

	//layerSound->loadSoundAtFrame( filePath, currentFrame() );

	//mTimeLine->updateContent();
}

void CommandCenter::ZoomIn()
{
	float newScaleValue = mEditor->view()->scaling() * 1.2;
	mEditor->view()->scale( newScaleValue );
}

void CommandCenter::ZoomOut()
{
	float newScaleValue = mEditor->view()->scaling() * 0.8;
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

void CommandCenter::addNewSoundLayer()
{
	bool ok = false;
	QString text = QInputDialog::getText( nullptr, tr( "Layer Properties" ),
										  tr( "Layer name:" ), QLineEdit::Normal,
										  tr( "Sound Layer" ), &ok );
	if ( ok && !text.isEmpty() )
	{
		Layer* layer = mEditor->layers()->newSoundLayer( text );
		mEditor->layers()->setCurrentLayer( layer );
	}
}