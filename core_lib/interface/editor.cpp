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
#include "vectorimage.h"
#include "bitmapimage.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layersound.h"
#include "layercamera.h"
#include "layerimage.h"

#include "colormanager.h"
#include "toolmanager.h"
#include "layermanager.h"
#include "playbackmanager.h"
#include "viewmanager.h"
#include "scribblearea.h"
#include "timeline.h"
#include "util.h"

#define MIN(a,b) ((a)>(b)?(b):(a))


static BitmapImage g_clipboardBitmapImage;
static VectorImage g_clipboardVectorImage;


Editor::Editor( QObject* parent ) : QObject( parent )
{
    m_isAltPressed = false;
    numberOfModifications = 0;

    QSettings settings( "Pencil", "Pencil" );
    mIsAutosave = settings.value( "autosave" ).toBool();
    autosaveNumber = settings.value( "autosaveNumber" ).toInt();
    if ( autosaveNumber == 0 )
    {
        autosaveNumber = 20;
        settings.setValue( "autosaveNumber", 20 );
    }
    mBackupIndex = -1;
    clipboardBitmapOk = false;
    clipboardVectorOk = false;

    if ( settings.value( "onionLayer1Opacity" ).isNull() ) settings.setValue( "onionLayer1Opacity", 50 );
    if ( settings.value( "onionLayer2Opacity" ).isNull() ) settings.setValue( "onionLayer2Opacity", 0 );
    if ( settings.value( "onionLayer3Opacity" ).isNull() ) settings.setValue( "onionLayer3Opacity", 0 );
    onionLayer1Opacity = settings.value( "onionLayer1Opacity" ).toInt();
    onionLayer2Opacity = settings.value( "onionLayer2Opacity" ).toInt();
    onionLayer3Opacity = settings.value( "onionLayer3Opacity" ).toInt();

    //qDebug() << QLibraryInfo::location( QLibraryInfo::PluginsPath );
    //qDebug() << QLibraryInfo::location( QLibraryInfo::BinariesPath );
    //qDebug() << QLibraryInfo::location( QLibraryInfo::LibrariesPath );
}

Editor::~Editor()
{
    // a lot more probably needs to be cleaned here...
    clearUndoStack();
}

bool Editor::initialize( ScribbleArea* pScribbleArea )
{
    mScribbleArea = pScribbleArea;

    // Initialize managers
    mColorManager = new ColorManager( this );
    mLayerManager = new LayerManager( this );
    mToolManager = new ToolManager( this );
    mPlaybackManager = new PlaybackManager( this );
    mViewManager = new ViewManager( this );
    BaseManager* allManagers[] =
    {
        mColorManager,
        mToolManager,
        mLayerManager,
        mPlaybackManager,
        mViewManager
    };

    for ( BaseManager* pManager : allManagers )
    {
        pManager->setEditor( this );
        pManager->init();
    }

    mFrame = 1;
    layers()->setCurrentLayer( 0 );

    tools()->setCurrentTool( PENCIL );

    //setAcceptDrops( true ); // TODO: drop event

    // CONNECTIONS
    makeConnections();

    return true;
}

int Editor::currentFrame()
{
    return mFrame;
}

void Editor::makeConnections()
{
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
                importSound( filePath );
        }
    }
}

void Editor::changeAutosave( int x )
{
    QSettings settings( "Pencil", "Pencil" );
    if ( x == 0 )
    {
        mIsAutosave = false;
        settings.setValue( "autosave", "false" );
    }
    else
    {
        mIsAutosave = true;
        settings.setValue( "autosave", "true" );
    }
}

void Editor::changeAutosaveNumber( int number )
{
    autosaveNumber = number;
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue( "autosaveNumber", number );
}

void Editor::onionLayer1OpacityChangeSlot( int number )
{
    onionLayer1Opacity = number;
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue( "onionLayer1Opacity", number );
}

void Editor::onionLayer2OpacityChangeSlot( int number )
{
    onionLayer2Opacity = number;
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue( "onionLayer2Opacity", number );
}

void Editor::onionLayer3OpacityChangeSlot( int number )
{
    onionLayer3Opacity = number;
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue( "onionLayer3Opacity", number );
}

void Editor::currentKeyFrameModification()
{
    modification( layers()->currentLayerIndex() );
}

void Editor::modification( int layerNumber )
{
    if ( mObject != NULL )
    {
        mObject->modification();
    }
    lastModifiedFrame = currentFrame();
    lastModifiedLayer = layerNumber;

    mScribbleArea->update();

    emit updateTimeLine();

    numberOfModifications++;
    if ( mIsAutosave && numberOfModifications > autosaveNumber )
    {
        numberOfModifications = 0;
        emit needSave();
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
            BitmapImage* bitmapImage = ( ( LayerBitmap* )layer )->getLastBitmapImageAtFrame( backupFrame, 0 );
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
            VectorImage* vectorImage = ( ( LayerVector* )layer )->getLastVectorImageAtFrame( backupFrame, 0 );
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
            *( ( ( LayerBitmap* )layer )->getLastBitmapImageAtFrame( this->frame, 0 ) ) = this->bitmapImage;  // restore the image
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
            *( ( ( LayerVector* )layer )->getLastVectorImageAtFrame( this->frame, 0 ) ) = this->vectorImage;  // restore the image
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
                BackupBitmapElement* lastBackupBitmapElement = ( BackupBitmapElement* )lastBackupElement;
                backup( lastBackupBitmapElement->layer, lastBackupBitmapElement->frame, "NoOp" );
                mBackupIndex--;
            }
            if ( lastBackupElement->type() == BackupElement::VECTOR_MODIF )
            {
                BackupVectorElement* lastBackupVectorElement = ( BackupVectorElement* )lastBackupElement;
                backup( lastBackupVectorElement->layer, lastBackupVectorElement->frame, "NoOp" );
                mBackupIndex--;
            }
        }
        //
        mBackupList[ mBackupIndex ]->restore( this );
        mBackupIndex--;
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

void Editor::flipX()
{
    tools()->setCurrentTool( MOVE );
    mScribbleArea->myFlipX = -mScribbleArea->myFlipX;
}

void Editor::flipY()
{
    tools()->setCurrentTool( MOVE );
    mScribbleArea->myFlipY = -mScribbleArea->myFlipY;
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
                g_clipboardBitmapImage = ( ( LayerBitmap* )layer )->getLastBitmapImageAtFrame( currentFrame(), 0 )->copy( mScribbleArea->getSelection().toRect() );  // copy part of the image
            }
            else
            {
                g_clipboardBitmapImage = ( ( LayerBitmap* )layer )->getLastBitmapImageAtFrame( currentFrame(), 0 )->copy();  // copy the whole image
            }
            clipboardBitmapOk = true;
            if ( g_clipboardBitmapImage.image() != NULL ) QApplication::clipboard()->setImage( *g_clipboardBitmapImage.image() );
        }
        if ( layer->type() == Layer::VECTOR )
        {
            clipboardVectorOk = true;
            g_clipboardVectorImage = *( ( ( LayerVector* )layer )->getLastVectorImageAtFrame( currentFrame(), 0 ) );  // copy the image
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
            auto pLayerBitmap = static_cast< LayerBitmap* >( layer );
            pLayerBitmap->getLastBitmapImageAtFrame( currentFrame(), 0 )->paste( &tobePasted ); // paste the clipboard
        }
        else if ( layer->type() == Layer::VECTOR && clipboardVectorOk )
        {
            backup( tr( "Paste" ) );
            mScribbleArea->deselectAll();
            VectorImage* vectorImage = ( ( LayerVector* )layer )->getLastVectorImageAtFrame( currentFrame(), 0 );
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

void Editor::newBitmapLayer()
{
    if ( mObject != NULL )
    {
        bool ok;
        QString text = QInputDialog::getText( NULL, tr( "Layer Properties" ),
                                              tr( "Layer name:" ), QLineEdit::Normal,
                                              tr( "Bitmap Layer" ), &ok );
        if ( ok && !text.isEmpty() )
        {
            Layer *layer = mObject->addNewBitmapLayer();
            layer->mName = text;

            emit updateLayerCount();

            setCurrentLayer( mObject->getLayerCount() - 1 );
        }
    }
}

void Editor::newVectorLayer()
{
    if ( mObject != NULL )
    {
        bool ok;
        QString text = QInputDialog::getText( NULL, tr( "Layer Properties" ),
                                              tr( "Layer name:" ), QLineEdit::Normal,
                                              tr( "Bitmap Layer" ), &ok );
        if ( ok && !text.isEmpty() )
        {
            Layer *layer = mObject->addNewVectorLayer();
            layer->mName = text;
            emit updateLayerCount();
            setCurrentLayer( mObject->getLayerCount() - 1 );
        }
    }
}

void Editor::newSoundLayer()
{
    if ( mObject != NULL )
    {
        bool ok;
        QString text = QInputDialog::getText( NULL, tr( "Layer Properties" ),
                                              tr( "Layer name:" ), QLineEdit::Normal,
                                              tr( "Bitmap Layer" ), &ok );
        if ( ok && !text.isEmpty() )
        {
            Layer *layer = mObject->addNewSoundLayer();
            layer->mName = text;
            emit updateLayerCount();
            setCurrentLayer( mObject->getLayerCount() - 1 );
        }
    }
}

void Editor::newCameraLayer()
{
    if ( mObject != NULL )
    {
        bool ok;
        QString text = QInputDialog::getText( NULL, tr( "Layer Properties" ),
                                              tr( "Layer name:" ), QLineEdit::Normal,
                                              tr( "Bitmap Layer" ), &ok );
        if ( ok && !text.isEmpty() )
        {
            Layer *layer = mObject->addNewCameraLayer();
            layer->mName = text;
            emit updateLayerCount();
            setCurrentLayer( mObject->getLayerCount() - 1 );
        }
    }
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

void Editor::resetMirror()
{
    view()->flipHorizontal( false );
    view()->flipVertical( false );
}

void Editor::saveLength( QString x )
{
    bool ok;
    int dec = x.toInt( &ok, 10 );
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue( "length", dec );
}

void Editor::resetUI()
{
    updateObject();
    scrubTo( 0 );
}

void Editor::setObject( Object* newObject )
{
    if ( newObject == NULL ) { return; }
    if ( newObject == mObject.get() ) { return; }

    mObject.reset( newObject );

    //qDebug( "New object loaded." );

    // the default selected layer is the last one
    layers()->setCurrentLayer( mObject->getLayerCount() - 1 );

    g_clipboardVectorImage.setObject( newObject );
}

void Editor::updateObject()
{
    color()->setColorNumber( 0 );

    emit updateLayerCount();

    clearUndoStack();

    if ( mScribbleArea )
    {
        mScribbleArea->updateAllFrames();
    }
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

    while ( reader.read( &img ) )
    {
        if ( img.isNull() || reader.nextImageDelay() <= 0 )
        {
            break;
        }

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

    VectorImage* vectorImage = ( ( LayerVector* )layer )->getVectorImageAtFrame( currentFrame() );
    if ( vectorImage == NULL )
    {
        addNewKey();
        vectorImage = ( ( LayerVector* )layer )->getVectorImageAtFrame( currentFrame() );
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
        mLastError = Error( ERROR_INVALID_LAYER_TYPE );
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

void Editor::scrubTo( int frame )
{
    if ( frame < 1 )
    {
        frame = 1;
    }
    int oldFrame = mFrame;
    mFrame = frame;

    if ( mScribbleArea->shouldUpdateAll() )
    {
        mScribbleArea->updateAllFrames();
    }

    Q_EMIT currentFrameChanged( frame );

    mScribbleArea->update();
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


void Editor::previousLayer()
{
    layers()->gotoPreviouslayer();
    mScribbleArea->updateAllFrames();
}

void Editor::nextLayer()
{
    layers()->gotoNextLayer();
    mScribbleArea->updateAllFrames();
}

void Editor::addNewKey()
{
    addKeyFame( layers()->currentLayerIndex(), currentFrame() );
}

void Editor::duplicateKey()
{
    Layer* layer = mObject->getLayer( layers()->currentLayerIndex() );
    if ( layer != NULL )
    {
        if ( layer->type() == Layer::VECTOR )
        {
            mScribbleArea->selectAll();
            clipboardVectorOk = true;
            g_clipboardVectorImage = *( ( ( LayerVector* )layer )->getLastVectorImageAtFrame( currentFrame(), 0 ) );  // copy the image (that works but I should also provide a copy() method)
            addNewKey();
            VectorImage* vectorImage = ( ( LayerVector* )layer )->getLastVectorImageAtFrame( currentFrame(), 0 );
            vectorImage->paste( g_clipboardVectorImage ); // paste the clipboard
            mScribbleArea->setModified( layers()->currentLayerIndex(), currentFrame() );
            mScribbleArea->update();
        }
        if ( layer->type() == Layer::BITMAP )
        {
            mScribbleArea->selectAll();
            copy();
            addNewKey();
            paste();
        }
    }
}

void Editor::addKeyFame( int layerNumber, int frameIndex )
{
    Layer* layer = mObject->getLayer( layerNumber );
    if ( layer == NULL )
    {
        return;
    }

    bool isOK = false;

    switch ( layer->type() )
    {
    case Layer::BITMAP:
    case Layer::VECTOR:
    case Layer::CAMERA:
        isOK = layer->addNewKeyAt( frameIndex );
        break;
    default:
        break;
    }

    if ( isOK )
    {
        scrubTo( frameIndex );
        getScribbleArea()->updateCurrentFrame();
    }
    else
    {
        addKeyFame( layerNumber, frameIndex + 1 );
    }
}

void Editor::removeKey()
{
    Layer* layer = layers()->currentLayer();
    if ( layer != NULL )
    {
        switch ( layer->type() )
        {
        case Layer::BITMAP:
        case Layer::VECTOR:
        case Layer::CAMERA:
            layer->removeKeyFrame( currentFrame() );
            break;
        default:
            break;
        }
        scrubBackward();
        mScribbleArea->updateCurrentFrame();
    }
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

void Editor::zoomIn()
{
    view()->scale( 1.2f );
}

void Editor::zoomOut()
{
    view()->scale( 0.8f );
}

void Editor::rotatecw()
{
    view()->rotate( 15.f );
}

void Editor::rotateacw()
{
    view()->rotate( -15.f );
}

void Editor::resetView()
{
    view()->resetView();
}

void Editor::importSound( QString filePath )
{
}
