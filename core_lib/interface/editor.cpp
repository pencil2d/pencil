/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
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

#include "layerbitmap.h"
#include "layervector.h"
#include "layersound.h"
#include "layercamera.h"
#include "layerimage.h"
#include "mainwindow2.h"
#include "displayoptiondockwidget.h"
#include "tooloptiondockwidget.h"
#include "toolbox.h"
#include "colormanager.h"
#include "colorpalettewidget.h"
#include "toolmanager.h"
#include "layermanager.h"
#include "playbackmanager.h"
#include "editor.h"

#define MIN(a,b) ((a)>(b)?(b):(a))

Editor::Editor( MainWindow2* parent )
    : QObject( parent )
    , exportFramesDialog( nullptr ) // will be created when needed
    , exportMovieDialog( nullptr )
    , exportFlashDialog( nullptr )
    , exportFramesDialog_hBox( nullptr )
    , exportFramesDialog_vBox( nullptr )
    , exportFramesDialog_format( nullptr )
    , exportMovieDialog_hBox( nullptr )
    , exportMovieDialog_vBox( nullptr )
    , exportMovieDialog_format( nullptr )
    , exportMovieDialog_fpsBox( nullptr )
    , m_clipboardVectorImage( nullptr )
{
    m_pMainWindow = parent;

    QSettings settings( "Pencil", "Pencil" );

    m_isAltPressed = false;
    numberOfModifications = 0;
    m_isAutosave = settings.value( "autosave" ).toBool();
    autosaveNumber = settings.value( "autosaveNumber" ).toInt();
    if ( autosaveNumber == 0 )
    {
        autosaveNumber = 20;
        settings.setValue( "autosaveNumber", 20 );
    }
    backupIndex = -1;
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
    if ( m_pObject != NULL )
    {
        delete m_pObject;
    }
    clearUndoStack();
}

bool Editor::initialize( ScribbleArea* pScribbleArea )
{
    m_pScribbleArea = pScribbleArea;

    // Initialize managers
    m_colorManager = new ColorManager( this );
    m_pLayerManager = new LayerManager( this );
    m_pToolManager = new ToolManager( this );
    m_pPlaybackManager = new PlaybackManager( this );

    BaseManager* allManagers[] =
    {
        m_colorManager,
        m_pToolManager,
        m_pLayerManager,
        m_pPlaybackManager
    };

    for ( BaseManager* pManager : allManagers )
    {
        pManager->setEditor( this );
        pManager->init();
    }

    layerManager()->setCurrentKeyFrame( 1 );
    layerManager()->setCurrentLayer( 0 );

    toolManager()->setCurrentTool( PENCIL );

    //setAcceptDrops( true ); // TODO: drop event

    // CONNECTIONS
    makeConnections();

    return true;
}


TimeLine* Editor::getTimeLine()
{
    return m_pMainWindow->m_pTimeLine;
}

void Editor::makeConnections()
{
    connect( QApplication::clipboard(), SIGNAL( dataChanged() ), this, SLOT( clipboardChanged() ) );
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

void Editor::importImageSequence()
{
    QFileDialog w;
    w.setFileMode( QFileDialog::AnyFile );

    QSettings settings( "Pencil", "Pencil" );
    QString initialPath = settings.value( "lastImportPath", QVariant( QDir::homePath() ) ).toString();
    if ( initialPath.isEmpty() ) initialPath = QDir::homePath();
    QStringList files = w.getOpenFileNames( m_pMainWindow,
                                            "Select one or more files to open",
                                            initialPath,
                                            "Images (*.png *.xpm *.jpg *.jpeg)" );
    qDebug() << files;

    QStringListIterator i( files );
    for ( int i = 0; i < files.size(); ++i )
    {
        QString filePath;
        filePath = files.at( i ).toLocal8Bit().constData();
        if ( i > 0 ) scrubForward();
        {
            if ( filePath.endsWith( ".png" ) ||
                 filePath.endsWith( ".jpg" ) ||
                 filePath.endsWith( ".jpeg" ) )
            {
                importImage( filePath );
            }
        }
    }
}

bool Editor::importMov()
{
    QSettings settings( "Pencil", "Pencil" );

    QString initialPath = settings.value( "lastExportPath", QDir::homePath() ).toString();

    if ( initialPath.isEmpty() )
    {
        initialPath = QDir::homePath() + "/untitled.avi";
    }
    QString filePath = QFileDialog::getOpenFileName(
        m_pMainWindow,
        tr( "Import movie" ),
        initialPath,
        tr( "AVI (*.avi);;MPEG(*.mpg);;MOV(*.mov);;MP4(*.mp4);;SWF(*.swf);;FLV(*.flv);;WMV(*.wmv)" )
        );
    if ( filePath.isEmpty() )
    {
        return false;
    }
    else
    {
        settings.setValue( "lastExportPath", QVariant( filePath ) );
        int fps = playbackManager()->fps();
        importMovie( filePath, fps );
        return true;
    }
}

void Editor::changeAutosave( int x )
{
    QSettings settings( "Pencil", "Pencil" );
    if ( x == 0 )
    {
        m_isAutosave = false;
        settings.setValue( "autosave", "false" );
    }
    else
    {
        m_isAutosave = true;
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
    modification( layerManager()->currentLayerIndex() );
}

void Editor::modification( int layerNumber )
{
    if ( m_pObject != NULL )
    {
        m_pObject->modification();
    }
    lastModifiedFrame = layerManager()->currentFramePosition();
    lastModifiedLayer = layerNumber;

    m_pScribbleArea->update();
    getTimeLine()->updateContent();

    numberOfModifications++;
    if ( m_isAutosave && numberOfModifications > autosaveNumber )
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
    if ( lastModifiedLayer != layerManager()->currentLayerIndex() || lastModifiedFrame != layerManager()->currentFramePosition() )
    {
        backup( layerManager()->currentLayerIndex(), layerManager()->currentFramePosition(), undoText );
    }
}

void Editor::backup( int backupLayer, int backupFrame, QString undoText )
{
    while ( backupList.size() - 1 > backupIndex && backupList.size() > 0 )
    {
        delete backupList.takeLast();
    }
    while ( backupList.size() > 19 )   // we authorize only 20 levels of cancellation
    {
        delete backupList.takeFirst();
        backupIndex--;
    }
    Layer* layer = m_pObject->getLayer( backupLayer );
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
                backupList.append( element );
                backupIndex++;
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
                backupList.append( element );
                backupIndex++;
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
    if ( backupList.size() > 0 && backupIndex > -1 )
    {
        if ( backupIndex == backupList.size() - 1 )
        {
            BackupElement* lastBackupElement = backupList[ backupIndex ];
            if ( lastBackupElement->type() == BackupElement::BITMAP_MODIF )
            {
                BackupBitmapElement* lastBackupBitmapElement = ( BackupBitmapElement* )lastBackupElement;
                backup( lastBackupBitmapElement->layer, lastBackupBitmapElement->frame, "NoOp" );
                backupIndex--;
            }
            if ( lastBackupElement->type() == BackupElement::VECTOR_MODIF )
            {
                BackupVectorElement* lastBackupVectorElement = ( BackupVectorElement* )lastBackupElement;
                backup( lastBackupVectorElement->layer, lastBackupVectorElement->frame, "NoOp" );
                backupIndex--;
            }
        }
        //
        backupList[ backupIndex ]->restore( this );
        backupIndex--;
        m_pScribbleArea->calculateSelectionRect(); // really ugly -- to improve
    }
}

void Editor::redo()
{
    if ( backupList.size() > 0 && backupIndex < backupList.size() - 2 )
    {
        backupIndex++;
        backupList[ backupIndex + 1 ]->restore( this );
    }
}

void Editor::clearUndoStack()
{
    backupIndex = -1;
    while ( !backupList.isEmpty() )
    {
        delete backupList.takeLast();
    }
    lastModifiedLayer = -1;
    lastModifiedFrame = -1;
}

void Editor::cut()
{
    m_pScribbleArea->deleteSelection();
    m_pScribbleArea->deselectAll();
}

void Editor::flipX()
{
    toolManager()->setCurrentTool( MOVE );
    m_pScribbleArea->myFlipX = -m_pScribbleArea->myFlipX;
}

void Editor::flipY()
{
    toolManager()->setCurrentTool( MOVE );
    m_pScribbleArea->myFlipY = -m_pScribbleArea->myFlipY;
}

void Editor::copy()
{
    Layer* layer = m_pObject->getLayer( layerManager()->currentLayerIndex() );
    if ( layer != NULL )
    {
        if ( layer->type() == Layer::BITMAP )
        {
            if ( m_pScribbleArea->somethingSelected )
            {
                m_clipboardBitmapImage = ( ( LayerBitmap* )layer )->getLastBitmapImageAtFrame( layerManager()->currentFramePosition(), 0 )->copy( m_pScribbleArea->getSelection().toRect() );  // copy part of the image
                m_pScribbleArea->deselectAll();
            }
            else
            {
                m_clipboardBitmapImage = ( ( LayerBitmap* )layer )->getLastBitmapImageAtFrame( layerManager()->currentFramePosition(), 0 )->copy();  // copy the whole image
                m_pScribbleArea->deselectAll();
            }
            clipboardBitmapOk = true;
            if ( m_clipboardBitmapImage.m_pImage != NULL ) QApplication::clipboard()->setImage( *( m_clipboardBitmapImage.m_pImage ) );
        }
        if ( layer->type() == Layer::VECTOR )
        {
            clipboardVectorOk = true;
            m_clipboardVectorImage = *( ( ( LayerVector* )layer )->getLastVectorImageAtFrame( layerManager()->currentFramePosition(), 0 ) );  // copy the image (that works but I should also provide a copy() method)
            m_pScribbleArea->deselectAll();
        }
    }
}


void Editor::paste()
{
    Layer* layer = m_pObject->getLayer( layerManager()->currentLayerIndex() );
    if ( layer != NULL )
    {
        if ( layer->type() == Layer::BITMAP && m_clipboardBitmapImage.m_pImage != NULL )
        {
            backup( tr( "Paste" ) );
            BitmapImage tobePasted = m_clipboardBitmapImage.copy();
            qDebug() << "to be pasted --->" << tobePasted.m_pImage->size();
            if ( m_pScribbleArea->somethingSelected )
            {
                QRectF selection = m_pScribbleArea->getSelection();
                if ( m_clipboardBitmapImage.width() <= selection.width() && m_clipboardBitmapImage.height() <= selection.height() )
                {
                    tobePasted.moveTopLeft( selection.topLeft() );
                }
                else
                {
                    tobePasted.transform( selection, true );
                }
            }
        }
        if ( layer->type() == Layer::VECTOR && clipboardVectorOk )
        {
            backup( tr( "Paste" ) );
            m_pScribbleArea->deselectAll();
            VectorImage* vectorImage = ( ( LayerVector* )layer )->getLastVectorImageAtFrame( layerManager()->currentFramePosition(), 0 );
            vectorImage->paste( m_clipboardVectorImage );  // paste the clipboard
            m_pScribbleArea->setSelection( vectorImage->getSelectionRect(), true );
            //((LayerVector*)layer)->getLastVectorImageAtFrame(backupFrame, 0)->modification(); ????
        }
    }
    m_pScribbleArea->updateCurrentFrame();
}

void Editor::deselectAll()
{
    m_pScribbleArea->deselectAll();
}

void Editor::clipboardChanged()
{
    if ( clipboardBitmapOk == false )
    {
        m_clipboardBitmapImage.m_pImage = new QImage( QApplication::clipboard()->image() );
        m_clipboardBitmapImage.boundaries = QRect( m_clipboardBitmapImage.topLeft(), m_clipboardBitmapImage.m_pImage->size() );
        qDebug() << "New clipboard image" << m_clipboardBitmapImage.m_pImage->size();
    }
    else
    {
        clipboardBitmapOk = false;
        qDebug() << "The image has been saved in the clipboard";
    }
}

void Editor::newBitmapLayer()
{
    if ( m_pObject != NULL )
    {
        bool ok;
        QString text = QInputDialog::getText( NULL, tr( "Layer Properties" ),
                                              tr( "Layer name:" ), QLineEdit::Normal,
                                              tr( "Bitmap Layer" ), &ok );
        if ( ok && !text.isEmpty() )
        {
            Layer *layer = m_pObject->addNewBitmapLayer();
            layer->name = text;
            getTimeLine()->updateLayerNumber( m_pObject->getLayerCount() );
            setCurrentLayer( m_pObject->getLayerCount() - 1 );
        }
    }
}

void Editor::newVectorLayer()
{
    if ( m_pObject != NULL )
    {
        bool ok;
        QString text = QInputDialog::getText( NULL, tr( "Layer Properties" ),
                                              tr( "Layer name:" ), QLineEdit::Normal,
                                              tr( "Bitmap Layer" ), &ok );
        if ( ok && !text.isEmpty() )
        {
            Layer *layer = m_pObject->addNewVectorLayer();
            layer->name = text;
            getTimeLine()->updateLayerNumber( m_pObject->getLayerCount() );
            setCurrentLayer( m_pObject->getLayerCount() - 1 );
        }
    }
}

void Editor::newSoundLayer()
{
    if ( m_pObject != NULL )
    {
        bool ok;
        QString text = QInputDialog::getText( NULL, tr( "Layer Properties" ),
                                              tr( "Layer name:" ), QLineEdit::Normal,
                                              tr( "Bitmap Layer" ), &ok );
        if ( ok && !text.isEmpty() )
        {
            Layer *layer = m_pObject->addNewSoundLayer();
            layer->name = text;
            getTimeLine()->updateLayerNumber( m_pObject->getLayerCount() );
            setCurrentLayer( m_pObject->getLayerCount() - 1 );
        }
    }
}

void Editor::newCameraLayer()
{
    if ( m_pObject != NULL )
    {
        bool ok;
        QString text = QInputDialog::getText( NULL, tr( "Layer Properties" ),
                                              tr( "Layer name:" ), QLineEdit::Normal,
                                              tr( "Bitmap Layer" ), &ok );
        if ( ok && !text.isEmpty() )
        {
            Layer *layer = m_pObject->addNewCameraLayer();
            layer->name = text;
            getTimeLine()->updateLayerNumber( m_pObject->getLayerCount() );
            setCurrentLayer( m_pObject->getLayerCount() - 1 );
        }
    }
}

void Editor::deleteCurrentLayer()
{
    int ret = QMessageBox::warning( m_pMainWindow,
                                    tr( "Warning" ),
                                    tr( "Are you sure you want to delete layer: " ) + m_pObject->getLayer( layerManager()->currentLayerIndex() )->name + " ?",
                                    QMessageBox::Ok | QMessageBox::Cancel,
                                    QMessageBox::Ok );
    if ( ret == QMessageBox::Ok )
    {
        m_pObject->deleteLayer( layerManager()->currentLayerIndex() );
        if ( layerManager()->currentLayerIndex() == m_pObject->getLayerCount() ) setCurrentLayer( layerManager()->currentLayerIndex() - 1 );
        getTimeLine()->updateLayerNumber( m_pObject->getLayerCount() );
        //timeLine->update();
        m_pScribbleArea->updateAllFrames();
    }
}

void Editor::toggleMirror()
{
    m_pObject->toggleMirror();
    m_pScribbleArea->toggleMirror();
}

void Editor::toggleMirrorV()
{
    m_pObject->toggleMirror();
    m_pScribbleArea->toggleMirrorV();
}

void Editor::toggleShowAllLayers()
{
    m_pScribbleArea->toggleShowAllLayers();
    getTimeLine()->updateContent();
}

void Editor::resetMirror()
{
    m_pObject->resetMirror();
    //toolSet->resetMirror();
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
    layerManager()->setCurrentKeyFrame( 0 );
    scrubTo( 0 );
}

void Editor::setObject( Object* newObject )
{
    if ( newObject == NULL )
    {
        return;
    }
    if ( newObject == this->m_pObject )
    {
        return;
    }
    m_pObject = newObject;

    qDebug( "New object loaded." );

    // the default selected layer is the last one
    layerManager()->setCurrentLayer( m_pObject->getLayerCount() - 1 );
    layerManager()->setCurrentKeyFrame( 1 );
}

void Editor::updateObject()
{
    colorManager()->setColorNumber( 0 );

    if ( getTimeLine() )
    {
        getTimeLine()->updateLayerNumber( object()->getLayerCount() );
    }
    clearUndoStack();

    if ( m_pScribbleArea )
    {
        m_pScribbleArea->updateAllFrames();
    }
}

void Editor::createExportFramesSizeBox()
{
    int defaultWidth = 720;
    int defaultHeight = 540;
    exportFramesDialog_hBox = new QSpinBox( m_pMainWindow );
    exportFramesDialog_hBox->setMinimum( 1 );
    exportFramesDialog_hBox->setMaximum( 10000 );
    exportFramesDialog_hBox->setValue( defaultWidth );
    exportFramesDialog_hBox->setFixedWidth( 80 );
    exportFramesDialog_vBox = new QSpinBox( m_pMainWindow );
    exportFramesDialog_vBox->setMinimum( 1 );
    exportFramesDialog_vBox->setMaximum( 10000 );
    exportFramesDialog_vBox->setValue( defaultHeight );
    exportFramesDialog_vBox->setFixedWidth( 80 );
}

void Editor::createExportMovieSizeBox()
{
    int defaultWidth = 720;
    int defaultHeight = 540;
    int defaultFps = 25;
    exportMovieDialog_hBox = new QSpinBox( m_pMainWindow );
    exportMovieDialog_hBox->setMinimum( 1 );
    exportMovieDialog_hBox->setMaximum( 10000 );
    exportMovieDialog_hBox->setValue( defaultWidth );
    exportMovieDialog_hBox->setFixedWidth( 80 );
    exportMovieDialog_vBox = new QSpinBox( m_pMainWindow );
    exportMovieDialog_vBox->setMinimum( 1 );
    exportMovieDialog_vBox->setMaximum( 10000 );
    exportMovieDialog_vBox->setValue( defaultHeight );
    exportMovieDialog_vBox->setFixedWidth( 80 );

    exportMovieDialog_format = new QComboBox();
    exportMovieDialog_format->addItem( "AUTO" );
    exportMovieDialog_format->addItem( "MOV" );
    exportMovieDialog_format->addItem( "MPEG2/AVI" );
    exportMovieDialog_format->addItem( "MPEG4/AVI" );
    exportMovieDialog_format->addItem( "MPEG4/MP4" );
    exportMovieDialog_fpsBox = new QSpinBox( m_pMainWindow );
    exportMovieDialog_fpsBox->setMinimum( 1 );
    exportMovieDialog_fpsBox->setMaximum( 60 );
    exportMovieDialog_fpsBox->setValue( defaultFps );
    exportMovieDialog_fpsBox->setFixedWidth( 40 );
}

void Editor::createExportFramesDialog()
{
    exportFramesDialog = new QDialog( m_pMainWindow, Qt::Dialog );
    QGridLayout* mainLayout = new QGridLayout;

    QGroupBox* resolutionBox = new QGroupBox( tr( "Resolution" ) );
    if ( exportFramesDialog_hBox == NULL || exportFramesDialog_vBox == NULL )
    {
        createExportFramesSizeBox();
    }
    QGridLayout* resolutionLayout = new QGridLayout;
    resolutionLayout->addWidget( exportFramesDialog_hBox, 0, 0 );
    resolutionLayout->addWidget( exportFramesDialog_vBox, 0, 1 );
    resolutionBox->setLayout( resolutionLayout );

    QGroupBox* formatBox = new QGroupBox( tr( "Format" ) );
    exportFramesDialog_format = new QComboBox();
    exportFramesDialog_format->addItem( "PNG" );
    exportFramesDialog_format->addItem( "JPG" );
    exportFramesDialog_format->addItem( "TIF" );
    exportFramesDialog_format->addItem( "BMP" );
    QGridLayout* formatLayout = new QGridLayout;
    formatLayout->addWidget( exportFramesDialog_format, 0, 0 );
    formatBox->setLayout( formatLayout );

    QDialogButtonBox* buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    connect( buttonBox, SIGNAL( accepted() ), exportFramesDialog, SLOT( accept() ) );
    connect( buttonBox, SIGNAL( rejected() ), exportFramesDialog, SLOT( reject() ) );

    mainLayout->addWidget( resolutionBox, 0, 0 );
    mainLayout->addWidget( formatBox, 1, 0 );
    mainLayout->addWidget( buttonBox, 2, 0 );
    exportFramesDialog->setLayout( mainLayout );
    exportFramesDialog->setWindowTitle( tr( "Options" ) );
    exportFramesDialog->setModal( true );
}

void Editor::createExportMovieDialog()
{
    exportMovieDialog = new QDialog( m_pMainWindow, Qt::Dialog );
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
}

void Editor::createExportFlashDialog()
{
    exportFlashDialog = new QDialog( m_pMainWindow, Qt::Dialog );
    QGridLayout* mainLayout = new QGridLayout;

    QSettings settings( "Pencil", "Pencil" );

    exportFlashDialog_compression = new QSlider( Qt::Horizontal );
    exportFlashDialog_compression->setTickPosition( QSlider::TicksBelow );
    exportFlashDialog_compression->setMinimum( 0 );
    exportFlashDialog_compression->setMaximum( 10 );
    exportFlashDialog_compression->setValue( 10 - settings.value( "flashCompressionLevel" ).toInt() );

    QLabel* label1 = new QLabel( "Large file" );
    QLabel* label2 = new QLabel( "Small file" );

    QGroupBox* compressionBox = new QGroupBox( tr( "Compression" ) );
    QGridLayout* compressionLayout = new QGridLayout;
    compressionLayout->addWidget( label1, 0, 0 );
    compressionLayout->addWidget( exportFlashDialog_compression, 0, 1 );
    compressionLayout->addWidget( label2, 0, 2 );
    compressionBox->setLayout( compressionLayout );

    QDialogButtonBox* buttonBox = new QDialogButtonBox( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );
    connect( buttonBox, SIGNAL( accepted() ), exportFlashDialog, SLOT( accept() ) );
    connect( buttonBox, SIGNAL( rejected() ), exportFlashDialog, SLOT( reject() ) );

    mainLayout->addWidget( compressionBox, 0, 0 );
    mainLayout->addWidget( buttonBox, 1, 0 );
    exportFlashDialog->setLayout( mainLayout );
    exportFlashDialog->setWindowTitle( tr( "Export SWF Options" ) );
    exportFlashDialog->setModal( true );
}

QMatrix Editor::map( QRectF source, QRectF target )   // this method should be put somewhere else...
{
    qreal x1 = source.left();
    qreal y1 = source.top();
    qreal x2 = source.right();
    qreal y2 = source.bottom();
    qreal x1P = target.left();
    qreal y1P = target.top();
    qreal x2P = target.right();
    qreal y2P = target.bottom();
    QMatrix matrix;
    bool mirror = false;
    if ( ( x1 != x2 ) && ( y1 != y2 ) )
    {
        if ( !mirror )
        {
            matrix = QMatrix( ( x2P - x1P ) / ( x2 - x1 ), 0, 
                              0,  ( y2P - y1P ) / ( y2 - y1 ), 
                              ( x1P*x2 - x2P*x1 ) / ( x2 - x1 ), ( y1P*y2 - y2P*y1 ) / ( y2 - y1 ) );
        }
        else
        {
            matrix = QMatrix( ( x2P - x1P ) / ( x1 - x2 ), 0, 0, ( y2P - y1P ) / ( y2 - y1 ), ( x1P*x1 - x2P*x2 ) / ( x1 - x2 ), ( y1P*y2 - y2P*y1 ) / ( y2 - y1 ) );
        }
    }
    else
    {
        matrix.reset();
    }
    return matrix;
}

bool Editor::exportSeqCLI( QString filePath = "", QString format = "PNG" )
{
    int width = m_pScribbleArea->getViewRect().toRect().width();
    int height = m_pScribbleArea->getViewRect().toRect().height();

    QSize exportSize = QSize( width, height );
    QByteArray exportFormat( format.toLatin1() );

    QMatrix view = map( m_pScribbleArea->getViewRect(), QRectF( QPointF( 0, 0 ), exportSize ) );
    view = m_pScribbleArea->getView() * view;

    int projectLength = layerManager()->projectLength();

    m_pObject->exportFrames( 1, projectLength, getCurrentLayer(), 
                             exportSize, 
                             filePath, 
                             exportFormat, -1, false, true, NULL, 0 );
    return true;
}

bool Editor::exportImageSequence()
{
    QSettings settings( PENCIL2D, PENCIL2D );

    QString strDefaultPath = settings.value( "lastExportPath", QVariant( QDir::homePath() ) ).toString();
    if ( strDefaultPath.isEmpty() )
    {
        strDefaultPath= QDir::homePath() + "/untitled.png";
    }

    QString strFilePath = QFileDialog::getSaveFileName( m_pMainWindow, 
                                                     tr( "Save Image Sequence" ), 
                                                     strDefaultPath, 
                                                     tr( "PNG (*.png);;JPG(*.jpg *.jpeg);;TIFF(*.tiff);;TIF(*.tif);;BMP(*.bmp);;GIF(*.gif)" ) );
    if ( strFilePath.isEmpty() )
    {
        return false;
    }
    settings.setValue( "lastExportPath", QVariant( strFilePath ) );

    if ( !exportFramesDialog ) createExportFramesDialog();
    exportFramesDialog_hBox->setValue( m_pScribbleArea->getViewRect().toRect().width() );
    exportFramesDialog_vBox->setValue( m_pScribbleArea->getViewRect().toRect().height() );
    exportFramesDialog->exec();
    if ( exportFramesDialog->result() == QDialog::Rejected ) return false;

    QSize exportSize = QSize( exportFramesDialog_hBox->value(), exportFramesDialog_vBox->value() );
    //QMatrix view = map( QRectF(QPointF(0,0), scribbleArea->size() ), QRectF(QPointF(0,0), exportSize) );
    QMatrix view = map( m_pScribbleArea->getViewRect(), QRectF( QPointF( 0, 0 ), exportSize ) );
    view = m_pScribbleArea->getView() * view;

    QByteArray exportFormat( exportFramesDialog_format->currentText().toLatin1() );
    
    int projectLength = layerManager()->projectLength();
    m_pObject->exportFrames( 1, projectLength, 
                             getCurrentLayer(), 
                             exportSize, strFilePath, 
                             exportFormat, -1, false, true, NULL, 0 );
    return true;
}

bool Editor::exportX()
{
    QSettings settings( "Pencil", "Pencil" );
    QString initialPath = settings.value( "lastExportPath", QVariant( QDir::homePath() ) ).toString();
    if ( initialPath.isEmpty() ) initialPath = QDir::homePath() + "/untitled";
    QString filePath = QFileDialog::getSaveFileName( m_pMainWindow, tr( "Save As" ), initialPath );
    if ( filePath.isEmpty() )
    {
        qDebug() << "empty file";
        return false;
    }
    else
    {
        settings.setValue( "lastExportPath", QVariant( filePath ) );

        QSize exportSize = m_pScribbleArea->getViewRect().toRect().size();
        QMatrix view = map( m_pScribbleArea->getViewRect(), QRectF( QPointF( 0, 0 ), exportSize ) );
        view = m_pScribbleArea->getView() * view;

        int projectLength = layerManager()->projectLength();
        if ( !m_pObject->exportX( 1, projectLength, view, exportSize, filePath, true ) ) 
        {
            QMessageBox::warning( m_pMainWindow, tr( "Warning" ),
                                  tr( "Unable to export image." ),
                                  QMessageBox::Ok,
                                  QMessageBox::Ok );
            return false;
        }
        return true;
    }
}

bool Editor::exportImage()
{
    QSettings settings( "Pencil", "Pencil" );
    QString initialPath = settings.value( "lastExportPath", QVariant( QDir::homePath() ) ).toString();
    if ( initialPath.isEmpty() )
    {
        initialPath = QDir::homePath() + "/untitled.png";
    }

    QString filePath = QFileDialog::getSaveFileName( m_pMainWindow, tr( "Save Image" ), initialPath, tr( "PNG (*.png);;JPG(*.jpg *.jpeg);;TIFF(*.tiff);;TIF(*.tif);;BMP(*.bmp);;GIF(*.gif)" ) );
    QFileInfo fi( filePath );

    if ( fi.suffix().isEmpty() ) {
        // add PNG per default if the name has no suffix
        filePath += ".png";
    }

    if ( filePath.isEmpty() )
    {
        qDebug() << "empty file";
        return false;
    }
    else
    {
        settings.setValue( "lastExportPath", QVariant( filePath ) );

        QSize exportSize = m_pScribbleArea->getViewRect().toRect().size();
        QMatrix view = map( m_pScribbleArea->getViewRect(), QRectF( QPointF( 0, 0 ), exportSize ) );
        view = m_pScribbleArea->getView() * view;


        int projectLength = layerManager()->projectLength();
        if ( !m_pObject->exportIm( layerManager()->currentFramePosition(), projectLength, view, exportSize, filePath, true ) ) {
            QMessageBox::warning( m_pMainWindow, tr( "Warning" ),
                                  tr( "Unable to export image." ),
                                  QMessageBox::Ok,
                                  QMessageBox::Ok );
            return false;
        }

        return true;
    }
}

bool Editor::exportMov()
{
    QSettings settings( "Pencil", "Pencil" );
    QString initialPath = settings.value( "lastExportPath", QVariant( QDir::homePath() ) ).toString();
    if ( initialPath.isEmpty() ) initialPath = QDir::homePath() + "/untitled.avi";
    //  QString filePath = QFileDialog::getSaveFileName(this, tr("Export As"),initialPath);
    QString filePath = QFileDialog::getSaveFileName( m_pMainWindow, tr( "Export Movie As..." ), initialPath, tr( "AVI (*.avi);;MOV(*.mov);;WMV(*.wmv)" ) );
    if ( filePath.isEmpty() )
    {
        return false;
    }
    else
    {
        settings.setValue( "lastExportPath", QVariant( filePath ) );
        if ( !exportMovieDialog ) createExportMovieDialog();
        exportMovieDialog_hBox->setValue( m_pScribbleArea->getViewRect().toRect().width() );
        exportMovieDialog_vBox->setValue( m_pScribbleArea->getViewRect().toRect().height() );
        exportMovieDialog->exec();
        if ( exportMovieDialog->result() == QDialog::Rejected ) return false;

        QSize exportSize = QSize( exportMovieDialog_hBox->value(), exportMovieDialog_vBox->value() );
        QMatrix view = map( m_pScribbleArea->getViewRect(), QRectF( QPointF( 0, 0 ), exportSize ) );
        view = m_pScribbleArea->getView() * view;

        int projectLength = layerManager()->projectLength();
        int fps = playbackManager()->fps();
        m_pObject->exportMovie( 1, projectLength, view, getCurrentLayer(), exportSize, filePath, fps, exportMovieDialog_fpsBox->value(), exportMovieDialog_format->currentText() );
        return true;
    }
}

bool Editor::exportFlash()
{
    QSettings settings( "Pencil", "Pencil" );
    QString initialPath = settings.value( "lastExportPath", QVariant( QDir::homePath() ) ).toString();
    if ( initialPath.isEmpty() ) initialPath = QDir::homePath() + "/untitled.swf";
    //  QString filePath = QFileDialog::getSaveFileName(this, tr("Export SWF As"),initialPath);
    QString filePath = QFileDialog::getSaveFileName( m_pMainWindow, tr( "Export Movie As..." ), initialPath, tr( "SWF (*.swf)" ) );
    if ( filePath.isEmpty() )
    {
        return false;
    }
    else
    {
        settings.setValue( "lastExportPath", QVariant( filePath ) );
        if ( !exportFlashDialog )
        {
            createExportFlashDialog();
        }
        exportFlashDialog->exec();

        if ( exportFlashDialog->result() == QDialog::Rejected )
        {
            return false;
        }

        //settings.setValue( "flashCompressionLevel", 10 - exportFlashDialog_compression->value() );

        QSize exportSize = m_pScribbleArea->getViewRect().toRect().size();
        QMatrix view = map( m_pScribbleArea->getViewRect(), QRectF( QPointF( 0, 0 ), exportSize ) );
        view = m_pScribbleArea->getView() * view;

        int projectLength = layerManager()->projectLength();
        int fps = playbackManager()->fps();
        m_pObject->exportFlash( 1, projectLength, view, exportSize, filePath, fps, exportFlashDialog_compression->value() );
        return true;
    }
}

void Editor::importImageFromDialog()
{
    importImage( "fromDialog" );
}

void Editor::importImage( QString filePath )
{
    Layer* layer = m_pObject->getLayer( layerManager()->currentLayerIndex() );
    if ( layer == NULL )
    {
        return;
    }

    if ( layer->type() != Layer::BITMAP && layer->type() != Layer::VECTOR )
    {
        // create a new Bitmap layer ?
        QMessageBox::warning( m_pMainWindow, tr( "Warning" ),
                              tr( "Please select a Bitmap or Vector layer to import images." ),
                              QMessageBox::Ok,
                              QMessageBox::Ok );
        return;
    }

    if ( filePath == "fromDialog" )
    {
        QSettings settings( "Pencil", "Pencil" );
        QString initialPath = settings.value( "lastImportPath", QVariant( QDir::homePath() ) ).toString();
        if ( initialPath.isEmpty() ) initialPath = QDir::homePath();
        filePath = QFileDialog::getOpenFileName( m_pMainWindow, tr( "Import image..." ), initialPath, tr( "PNG (*.png);;JPG(*.jpg *.jpeg);;TIFF(*.tiff);;TIF(*.tif);;BMP(*.bmp);;GIF(*.gif)" ) );
        if ( !filePath.isEmpty() ) settings.setValue( "lastImportPath", QVariant( filePath ) );
    }

    if ( !filePath.isEmpty() )
    {
        backup( tr( "ImportImg" ) );
        // --- option 1
        //((LayerBitmap*)layer)->loadImageAtFrame(filePath, currentFrame);
        // --- option 2
        // TO BE IMPROVED
        if ( layer->type() == Layer::BITMAP )
        {
            QImageReader* importedImageReader = new QImageReader( filePath );
            QImage importedIm = importedImageReader->read();

            QImage* importedImage = &importedIm;

            int numImages = importedImageReader->imageCount();
            int timeLeft = importedImageReader->nextImageDelay();

            if ( !importedImage->isNull() )
            {
                do
                {
                    BitmapImage* bitmapImage = ( ( LayerBitmap* )layer )->getBitmapImageAtFrame( layerManager()->currentFramePosition() );
                    if ( bitmapImage == NULL )
                    {
                        addNewKey();
                        bitmapImage = ( ( LayerBitmap* )layer )->getBitmapImageAtFrame( layerManager()->currentFramePosition() );
                    }

                    QRect boundaries = importedImage->rect();
                    //boundaries.moveTopLeft( scribbleArea->getView().inverted().map(QPoint(0,0)) );
                    boundaries.moveTopLeft( m_pScribbleArea->getCentralPoint().toPoint() - QPoint( boundaries.width() / 2, boundaries.height() / 2 ) );
                    BitmapImage* importedBitmapImage = new BitmapImage( boundaries, *importedImage );
                    if ( m_pScribbleArea->somethingSelected )
                    {
                        QRectF selection = m_pScribbleArea->getSelection();
                        if ( importedImage->width() <= selection.width() && importedImage->height() <= selection.height() )
                        {
                            importedBitmapImage->boundaries.moveTopLeft( selection.topLeft().toPoint() );
                        }
                        else
                        {
                            importedBitmapImage->transform( selection.toRect(), true );
                        }
                    }

                    bitmapImage->paste( importedBitmapImage );
                    int fps = playbackManager()->fps();
                    timeLeft -= ( timeLeft / ( 1000 / fps ) + 1 )*( 1000 / fps );

                    while ( timeLeft<0 && numImages > 0 )
                    {
                        importedImageReader->read( importedImage );
                        numImages--;
                        if ( importedImage->isNull() || importedImageReader->nextImageDelay() <= 0 ) break;
                        timeLeft += importedImageReader->nextImageDelay();

                        int fps = playbackManager()->fps();
                        scrubTo( layerManager()->currentFramePosition() + ( timeLeft / ( 1000 / fps ) ) );
                    }
                } while ( numImages > 0 && !importedImage->isNull() );
            }
            else
            {
                QMessageBox::warning( m_pMainWindow, tr( "Warning" ),
                                      tr( "Unable to load bitmap image.<br><b>TIP:</b> Use Bitmap layer to import bitmaps." ),
                                      QMessageBox::Ok,
                                      QMessageBox::Ok );
            }
        }
        if ( layer->type() == Layer::VECTOR )
        {
            VectorImage* vectorImage = ( ( LayerVector* )layer )->getVectorImageAtFrame( layerManager()->currentFramePosition() );
            if ( vectorImage == NULL )
            {
                addNewKey();
                vectorImage = ( ( LayerVector* )layer )->getVectorImageAtFrame( layerManager()->currentFramePosition() );
            }
            VectorImage* importedVectorImage = new VectorImage( NULL );
            bool ok = importedVectorImage->read( filePath );
            if ( ok )
            {
                importedVectorImage->selectAll();
                vectorImage->paste( *importedVectorImage );
            }
            else
            {
                QMessageBox::warning( m_pMainWindow, 
                                      tr( "Warning" ),
                                      tr( "Unable to load vector image.<br><b>TIP:</b> Use Vector layer to import vectors." ),
                                      QMessageBox::Ok,
                                      QMessageBox::Ok );
            }
        }
        m_pScribbleArea->updateCurrentFrame();
        getTimeLine()->updateContent();
    }
}

void Editor::importSound( QString filePath )
{
    Layer* layer = m_pObject->getLayer( layerManager()->currentLayerIndex() );
    if ( layer == NULL )
    {
        QMessageBox msg;
        msg.setText( "You must select an empty sound layer as the destination for your sound before importing. Please create a new sound layer." );
        msg.setIcon( QMessageBox::Warning );
        msg.exec();
        return;
    }

    if ( layer->type() != Layer::SOUND )
    {
        QMessageBox msg;
        msg.setText( "No sound layer exists as a destination for your import. Create a new sound layer?" );
        QAbstractButton* acceptButton = msg.addButton( "Create sound layer", QMessageBox::AcceptRole );
        msg.addButton( "Don't create layer", QMessageBox::RejectRole );

        msg.exec();
        if ( msg.clickedButton() == acceptButton )
        {
            newSoundLayer();
            layer = m_pObject->getLayer( layerManager()->currentLayerIndex() );
        }
        else
        {
            return;
        }
    }

    if ( !( ( LayerSound* )layer )->isEmpty() )
    {
        QMessageBox msg;
        msg.setText( "The sound layer you have selected already contains a sound item. Please select another." );
        msg.exec();
        return;
    }

    if ( filePath.isEmpty() || filePath == "fromDialog" )
    {
        QSettings settings( "Pencil", "Pencil" );
        QString initialPath = settings.value( "lastImportPath", QVariant( QDir::homePath() ) ).toString();
        if ( initialPath.isEmpty() ) initialPath = QDir::homePath();
        filePath = QFileDialog::getOpenFileName( m_pMainWindow, tr( "Import sound..." ), initialPath, tr( "WAV(*.wav);;MP3(*.mp3)" ) );
        if ( !filePath.isEmpty() )
        {
            settings.setValue( "lastImportPath", QVariant( filePath ) );
        }
        else
        {
            return;
        }
    }
    ( ( LayerSound* )layer )->loadSoundAtFrame( filePath, layerManager()->currentFramePosition() );
    getTimeLine()->updateContent();
    modification( layerManager()->currentLayerIndex() );
}

void Editor::updateFrame( int frameNumber )
{
    m_pScribbleArea->updateFrame( frameNumber );
}

void Editor::updateFrameAndVector( int frameNumber )
{
    m_pScribbleArea->updateAllVectorLayersAt( frameNumber );
}

void Editor::scrubTo( int frameNumber )
{
    if ( m_pScribbleArea->shouldUpdateAll() )
    {
        m_pScribbleArea->updateAllFrames();
    }
    int oldFrame = layerManager()->currentFramePosition();
    if ( frameNumber < 1 )
    {
        frameNumber = 1;
    }
    
    layerManager()->setCurrentKeyFrame( frameNumber );

    getTimeLine()->updateFrame( oldFrame );
    getTimeLine()->updateFrame( layerManager()->currentFramePosition() );
    getTimeLine()->updateContent();
    
    m_pScribbleArea->update();
}

void Editor::scrubForward()
{
    scrubTo( layerManager()->currentFramePosition() + 1 );
}

void Editor::scrubBackward()
{
    if ( layerManager()->currentFramePosition() > 1 )
    {
        scrubTo( layerManager()->currentFramePosition() - 1 );
    }
}

void Editor::previousLayer()
{
    layerManager()->gotoPreviouslayer();

    getTimeLine()->updateContent();
    m_pScribbleArea->updateAllFrames();
}

void Editor::nextLayer()
{
    layerManager()->gotoNextLayer();

    getTimeLine()->updateContent();
    m_pScribbleArea->updateAllFrames();
}

void Editor::addNewKey()
{
    addKeyFame( layerManager()->currentLayerIndex(), layerManager()->currentFramePosition() );
}

void Editor::duplicateKey()
{
    Layer* layer = m_pObject->getLayer( layerManager()->currentLayerIndex() );
    if ( layer != NULL )
    {
        if ( layer->type() == Layer::VECTOR )
        {
            m_pScribbleArea->selectAll();
            clipboardVectorOk = true;
            m_clipboardVectorImage = *( ( ( LayerVector* )layer )->getLastVectorImageAtFrame( layerManager()->currentFramePosition(), 0 ) );  // copy the image (that works but I should also provide a copy() method)
            addNewKey();
            VectorImage* vectorImage = ( ( LayerVector* )layer )->getLastVectorImageAtFrame( layerManager()->currentFramePosition(), 0 );
            vectorImage->paste( m_clipboardVectorImage ); // paste the clipboard
            m_pScribbleArea->setModified( layerManager()->currentLayerIndex(), layerManager()->currentFramePosition() );
            m_pScribbleArea->update();
        }
        if ( layer->type() == Layer::BITMAP )
        {
            m_pScribbleArea->selectAll();
            copy();
            addNewKey();
            paste();
        }
    }
}

void Editor::addKeyFame( int layerNumber, int frameIndex )
{
    LayerImage* layer = static_cast<LayerImage*>( m_pObject->getLayer( layerNumber ) );
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
        isOK = layer->addNewKeyFrameAt( frameIndex );
        break;
    default:
        break;
    }

    if ( isOK )
    {
        getTimeLine()->updateContent();
        getScribbleArea()->updateCurrentFrame();
        layerManager()->setCurrentKeyFrame( frameIndex );
    }
    else
    {
        addKeyFame( layerNumber, frameIndex + 1 );
    }
}

void Editor::removeKey()
{
    Layer* layer = m_pObject->getLayer( layerManager()->currentLayerIndex() );
    if ( layer != NULL )
    {
        LayerImage* pLayerImg = static_cast< LayerImage* >( layer );
        switch ( pLayerImg->type() )
        {
        case Layer::BITMAP:
        case Layer::VECTOR:
        case Layer::CAMERA:
            pLayerImg->removeKeyFrame( layerManager()->currentFramePosition() );
            break;
        default:
            break;
        }
        scrubBackward();
        getTimeLine()->updateContent();
        m_pScribbleArea->updateCurrentFrame();
    }
}

void Editor::scrubNextKeyFrame()
{
    Layer* layer = m_pObject->getLayer( layerManager()->currentLayerIndex() );
    Q_ASSERT( layer );

    int nextPosition = layer->getNextKeyFramePosition( layerManager()->currentFramePosition() );
    layerManager()->setCurrentKeyFrame( nextPosition );
}

void Editor::scrubPreviousKeyFrame()
{
    Layer* layer = m_pObject->getLayer( layerManager()->currentLayerIndex() );
    Q_ASSERT( layer );

    int prevPosition = layer->getPreviousKeyFramePosition( layerManager()->currentFramePosition() );
    layerManager()->setCurrentKeyFrame( prevPosition );
}

void Editor::setCurrentLayer( int layerNumber )
{
    layerManager()->setCurrentLayer( layerNumber );
    getTimeLine()->updateContent();
    m_pScribbleArea->updateAllFrames();
}

void Editor::switchVisibilityOfLayer( int layerNumber )
{
    Layer* layer = m_pObject->getLayer( layerNumber );
    if ( layer != NULL ) layer->switchVisibility();
    m_pScribbleArea->updateAllFrames();
    getTimeLine()->updateContent();
}

void Editor::moveLayer( int i, int j )
{
    m_pObject->moveLayer( i, j );
    if ( j < i )
    {
        layerManager()->setCurrentLayer( j );
    }
    else
    {
        layerManager()->setCurrentLayer( j - 1 );
    }
    getTimeLine()->updateContent();
    m_pScribbleArea->updateAllFrames();
}

void Editor::clearCurrentFrame()
{
    m_pScribbleArea->clearImage();
}

void Editor::setzoom()
{
    m_pScribbleArea->zoom();
}

void Editor::setzoom1()
{
    m_pScribbleArea->zoom1();
}

void Editor::rotatecw()
{
    m_pScribbleArea->rotatecw();
}

void Editor::rotateacw()
{
    m_pScribbleArea->rotateacw();
}

void Editor::gridview()
{
    resetView();

    m_pScribbleArea->grid();
    QMessageBox msgBox;
    msgBox.setText( "Would you like to add a camera layer?" );
    msgBox.exec();
}

void Editor::getCameraLayer()
{
    for ( int i = 0; i < m_pObject->getLayerCount(); i++ )
    {
        Layer* layer = m_pObject->getLayer( i );
        // paints the bitmap images
        if ( layer->type() == Layer::BITMAP )
        {
            nextLayer();
        }
        if ( layer->type() == Layer::VECTOR )
        {
            nextLayer();
        }
        if ( layer->type() == Layer::CAMERA )
        {
        }
    }
}

void Editor::resetView()
{
    getScribbleArea()->resetView();
}

Layer* Editor::getCurrentLayer( int incr )
{
    return layerManager()->currentLayer( incr );
}
