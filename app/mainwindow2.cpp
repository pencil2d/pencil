/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2008-2009 Mj Mendoza IV
Copyright (C) 2011-2015 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "mainwindow2.h"
#include "ui_mainwindow2.h"

// standard headers
#include <memory>

// Qt headers
#include <QList>
#include <QMenu>
#include <QFile>
#include <QScopedPointer>
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QGraphicsDropShadowEffect>
#include <QStatusBar>

#include "pencildef.h"
#include "pencilsettings.h"
#include "object.h"
#include "filemanager.h"
#include "editor.h"
#include "colormanager.h"
#include "layermanager.h"
#include "layercamera.h"
#include "toolmanager.h"
#include "playbackmanager.h"
#include "soundmanager.h"
#include "actioncommands.h"

#include "scribblearea.h"
#include "colorbox.h"
#include "colorpalettewidget.h"
#include "displayoptionwidget.h"
#include "tooloptiondockwidget.h"
//#include "popupcolorpalettewidget.h"
#include "preferencesdialog.h"
#include "timeline.h"
#include "toolbox.h"
#include "preview.h"
#include "timeline2.h"

#include "colorbox.h"
#include "util.h"

#include "fileformat.h"     //contains constants used by Pencil File Format
#include "JlCompress.h"     //compress and decompress New Pencil File Format
#include "recentfilemenu.h"

#include "exportimageseqdialog.h"
#include "shortcutfilter.h"

MainWindow2::MainWindow2( QWidget *parent ) : QMainWindow( parent )
{
    ui = new Ui::MainWindow2;
    ui->setupUi( this );

    mBackground = new BackgroundWidget( this );

    mScribbleArea = new ScribbleArea( mBackground );
    mScribbleArea->setFocusPolicy( Qt::StrongFocus );

    // Show the UI over the background
    //
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(mScribbleArea);

    mBackground->setLayout(layout);

    // Central widget
    setCentralWidget(mBackground);


    Object* object = new Object();
    object->init();

    mEditor = new Editor( this );
    mEditor->setScribbleArea(mScribbleArea);
    mEditor->init();
    mEditor->setObject( object );

    mScribbleArea->setCore( mEditor );
    mScribbleArea->init();

    mEditor->setScribbleArea( mScribbleArea );
    makeConnections( mEditor, mScribbleArea );

    mCommands = new ActionCommands( this );
    mCommands->setCore( mEditor );

    createDockWidgets();
    createMenus();
    setupKeyboardShortcuts();

    readSettings();

    connect( mEditor, &Editor::needSave, this, &MainWindow2::saveDocument );
    connect( mToolBox, &ToolBoxWidget::clearButtonClicked, mEditor, &Editor::clearCurrentFrame );

    //connect( mScribbleArea, &ScribbleArea::refreshPreview, mPreview, &PreviewWidget::updateImage );

    mEditor->setCurrentLayer( mEditor->object()->getLayerCount() - 1 );
    mEditor->tools()->setDefaultTool();

    mBackground->init(mEditor->preference());

    mEditor->updateObject();
    mEditor->color()->setColorNumber(0);
    
    connect( mEditor->view(), &ViewManager::viewChanged, this, &MainWindow2::updateZoomLabel );
}

MainWindow2::~MainWindow2()
{
    delete ui;
}


void MainWindow2::createDockWidgets()
{
    mTimeLine = new TimeLine( this );
    mTimeLine->setObjectName( "TimeLine" );

    mColorWheel = new ColorBox( tr("Color Wheel"), this );
    mColorWheel->setToolTip( tr( "color palette:<br>use <b>(C)</b><br>toggle at cursor" ) );
    mColorWheel->setObjectName( "ColorWheel" );
    mColorWheel->setMaximumHeight(390);

    mColorPalette = new ColorPaletteWidget( this );
    mColorPalette->setObjectName( "ColorPalette" );

    mDisplayOptionWidget = new DisplayOptionWidget( this );
    mDisplayOptionWidget->setObjectName( "DisplayOption" );

    mToolOptions = new ToolOptionWidget( this );
    mToolOptions->setObjectName( "ToolOption" );

    mToolBox = new ToolBoxWidget( tr( "Tools", "Window title of tool box." ), this );
    mToolBox->setObjectName( "ToolBox" );

    mDockWidgets
        << mTimeLine
        << mColorWheel
        << mColorPalette
        << mDisplayOptionWidget
        << mToolOptions
        << mToolBox;

    /*
    mTimeline2 = new Timeline2;
    mTimeline2->setObjectName( "Timeline2" );
    mDockWidgets.append( mTimeline2 );
    */
    addDockWidget(Qt::RightDockWidgetArea,  mColorWheel);
    addDockWidget(Qt::RightDockWidgetArea,  mColorPalette);
    addDockWidget(Qt::RightDockWidgetArea,  mDisplayOptionWidget);
    addDockWidget(Qt::LeftDockWidgetArea,   mToolBox);
    addDockWidget(Qt::LeftDockWidgetArea,   mToolOptions);
    addDockWidget(Qt::BottomDockWidgetArea, mTimeLine);
    //addDockWidget( Qt::BottomDockWidgetArea, mTimeline2);

    for ( BaseDockWidget* pWidget : mDockWidgets )
    {
        pWidget->setCore( mEditor );
        pWidget->initUI();
        pWidget->setFeatures( QDockWidget::AllDockWidgetFeatures );
        pWidget->setFocusPolicy( Qt::NoFocus );

        qDebug() << "Init Dock widget: " << pWidget->objectName();
    }

    /*
    mPreview = new PreviewWidget( this );
    mPreview->setImage( mScribbleArea->mBufferImg );
    mPreview->setFeatures( QDockWidget::DockWidgetFloatable );
    mPreview->setFocusPolicy( Qt::NoFocus );
    addDockWidget( Qt::RightDockWidgetArea, mPreview );
    */

    makeConnections( mEditor, mTimeLine );
    makeConnections( mEditor, mColorWheel );
    makeConnections( mEditor, mColorPalette );
    makeConnections( mEditor, mDisplayOptionWidget );
    makeConnections( mEditor, mToolOptions );

    for ( BaseDockWidget* w : mDockWidgets )
    {
        w->updateUI();
    }
}


void MainWindow2::createMenus()
{
    // ---------- File Menu -------------
    connect( ui->actionNew, &QAction::triggered, this, &MainWindow2::newDocument );
    connect( ui->actionOpen, &QAction::triggered, this, &MainWindow2::openDocumentDialog );
    connect( ui->actionSave_as, &QAction::triggered, this, &MainWindow2::saveAsNewDocument );
    connect( ui->actionSave, &QAction::triggered, this, &MainWindow2::saveDocument );
    connect( ui->actionExit, &QAction::triggered, this, &MainWindow2::close );

    /// --- Export Menu ---
    //connect( ui->actionExport_X_sheet, &QAction::triggered, mEditor, &Editor::exportX );
    connect( ui->actionExport_Image, &QAction::triggered, this, &MainWindow2::exportImage );
    connect( ui->actionExport_Image_Sequence, &QAction::triggered, this, &MainWindow2::exportImageSequence );
    connect( ui->actionExport_Movie, &QAction::triggered, this, &MainWindow2::exportMovie );

    connect( ui->actionExport_Palette, &QAction::triggered, this, &MainWindow2::exportPalette );

    /// --- Import Menu ---
    //connect( ui->actionExport_Svg_Image, &QAction::triggered, editor, &Editor::saveSvg );
    connect( ui->actionImport_Image, &QAction::triggered, this, &MainWindow2::importImage );
    connect( ui->actionImport_Image_Sequence, &QAction::triggered, this, &MainWindow2::importImageSequence );
    connect( ui->actionImport_Movie, &QAction::triggered, this, &MainWindow2::importMovie );

    connect( ui->actionImport_Sound, &QAction::triggered, mCommands, &ActionCommands::importSound );
    connect( ui->actionImport_Palette, &QAction::triggered, this, &MainWindow2::importPalette );

    /// --- Edit Menu ---
    connect( ui->actionUndo, &QAction::triggered, mEditor, &Editor::undo );
    connect( ui->actionRedo, &QAction::triggered, mEditor, &Editor::redo );
    connect( ui->actionCut, &QAction::triggered, mEditor, &Editor::cut );
    connect( ui->actionCopy, &QAction::triggered, mEditor, &Editor::copy );
    connect( ui->actionPaste, &QAction::triggered, mEditor, &Editor::paste );
    connect( ui->actionClearFrame, &QAction::triggered, mEditor, &Editor::clearCurrentFrame );
    connect( ui->actionFlip_X, &QAction::triggered, mCommands, &ActionCommands::flipX );
    connect( ui->actionFlip_Y, &QAction::triggered, mCommands, &ActionCommands::flipY );
    connect( ui->actionSelect_All, &QAction::triggered, mEditor, &Editor::selectAll );
    connect( ui->actionDeselect_All, &QAction::triggered, mEditor, &Editor::deselectAll );
    connect( ui->actionPreference, &QAction::triggered, [=] { preferences(); } );
    ui->actionPreference->setMenuRole( QAction::PreferencesRole );

    ui->actionRedo->setEnabled( false );

    /// --- Layer Menu ---
    connect( ui->actionNew_Bitmap_Layer, &QAction::triggered, mCommands, &ActionCommands::addNewBitmapLayer );
    connect( ui->actionNew_Vector_Layer, &QAction::triggered, mCommands, &ActionCommands::addNewVectorLayer );
    connect( ui->actionNew_Sound_Layer, &QAction::triggered, mCommands, &ActionCommands::addNewSoundLayer );
    connect( ui->actionNew_Camera_Layer, &QAction::triggered, mCommands, &ActionCommands::addNewCameraLayer );
    connect( ui->actionDelete_Current_Layer, &QAction::triggered, mEditor->layers(), &LayerManager::deleteCurrentLayer );

    /// --- View Menu ---
    connect( ui->actionZoom_In, &QAction::triggered, mCommands, &ActionCommands::ZoomIn );
    connect( ui->actionZoom_Out, &QAction::triggered, mCommands, &ActionCommands::ZoomOut );
    connect( ui->actionRotate_Clockwise, &QAction::triggered, mCommands, &ActionCommands::rotateClockwise );
    connect( ui->actionRotate_Anticlosewise, &QAction::triggered, mCommands, &ActionCommands::rotateCounterClockwise );
    connect( ui->actionReset_Windows, &QAction::triggered, this, &MainWindow2::dockAllSubWidgets );
    connect( ui->actionReset_View, &QAction::triggered, mEditor->view(), &ViewManager::resetView );
    connect( ui->actionHorizontal_Flip, &QAction::triggered, mEditor, &Editor::toggleMirror );
    connect( ui->actionVertical_Flip, &QAction::triggered, mEditor, &Editor::toggleMirrorV );

    ui->actionPreview->setEnabled( false );
    //# connect(previewAct, SIGNAL(triggered()), editor, SLOT(getCameraLayer()));//TODO: Preview view

    setMenuActionChecked( ui->actionGrid, mEditor->preference()->isOn( SETTING::GRID ) );
    connect( ui->actionGrid, &QAction::triggered, mCommands, &ActionCommands::showGrid );

    bindActionWithSetting( ui->actionOnionPrev, SETTING::PREV_ONION );
    bindActionWithSetting( ui->actionOnionNext, SETTING::NEXT_ONION );
    bindActionWithSetting( ui->actionMultiLayerOnionSkin, SETTING::MULTILAYER_ONION );

    /// --- Animation Menu ---
    PlaybackManager* pPlaybackManager = mEditor->playback();
    connect( ui->actionPlay, &QAction::triggered, mCommands, &ActionCommands::PlayStop );

    connect( ui->actionLoop, &QAction::triggered, pPlaybackManager, &PlaybackManager::setLooping );
    connect( ui->actionLoopControl, &QAction::triggered, pPlaybackManager, &PlaybackManager::enableRangedPlayback );
    connect( pPlaybackManager, &PlaybackManager::loopStateChanged, ui->actionLoop, &QAction::setChecked );
    connect( pPlaybackManager, &PlaybackManager::rangedPlaybackStateChanged, ui->actionLoopControl, &QAction::setChecked );

    connect(ui->actionAdd_Frame, &QAction::triggered, mEditor, &Editor::addNewKey );
    connect(ui->actionRemove_Frame, &QAction::triggered, mEditor, &Editor::removeKey );
    connect(ui->actionNext_Frame, &QAction::triggered, mCommands, &ActionCommands::GotoNextFrame );
    connect(ui->actionPrevious_Frame, &QAction::triggered, mCommands, &ActionCommands::GotoPrevFrame );
    connect(ui->actionNext_KeyFrame, &QAction::triggered, mCommands, &ActionCommands::GotoNextKeyFrame );
    connect(ui->actionPrev_KeyFrame, &QAction::triggered, mCommands, &ActionCommands::GotoPrevKeyFrame );
    connect(ui->actionDuplicate_Frame, &QAction::triggered, mEditor, &Editor::duplicateKey );
    connect(ui->actionMove_Frame_Forward, &QAction::triggered, mEditor, &Editor::moveFrameForward ); //HERE
    connect(ui->actionMove_Frame_Backward, &QAction::triggered, mEditor, &Editor::moveFrameBackward );

    /// --- Tool Menu ---
    connect(ui->actionMove, &QAction::triggered, mToolBox, &ToolBoxWidget::moveOn );
    connect(ui->actionSelect, &QAction::triggered, mToolBox, &ToolBoxWidget::selectOn );
    connect(ui->actionBrush, &QAction::triggered, mToolBox, &ToolBoxWidget::brushOn );
    connect(ui->actionPolyline, &QAction::triggered, mToolBox, &ToolBoxWidget::polylineOn );
    connect(ui->actionSmudge, &QAction::triggered, mToolBox, &ToolBoxWidget::smudgeOn );
    connect(ui->actionPen, &QAction::triggered, mToolBox, &ToolBoxWidget::penOn );
    connect(ui->actionHand, &QAction::triggered, mToolBox, &ToolBoxWidget::handOn );
    connect(ui->actionPencil, &QAction::triggered, mToolBox, &ToolBoxWidget::pencilOn );
    connect(ui->actionBucket, &QAction::triggered, mToolBox, &ToolBoxWidget::bucketOn );
    connect(ui->actionEyedropper, &QAction::triggered, mToolBox, &ToolBoxWidget::eyedropperOn );
    connect(ui->actionEraser, &QAction::triggered, mToolBox, &ToolBoxWidget::eraserOn );
    connect(ui->actionTogglePalette, &QAction::triggered, mScribbleArea,&ScribbleArea::togglePopupPalette );
    connect(ui->actionResetToolsDefault, &QAction::triggered, mEditor->tools(), &ToolManager::resetAllTools );

    /// --- Window Menu ---
    QMenu* winMenu = ui->menuWindows;

    QAction* actions[] =
    {
        mToolBox->toggleViewAction(),
        mToolOptions->toggleViewAction(),
        mColorWheel->toggleViewAction(),
        mColorPalette->toggleViewAction(),
        mTimeLine->toggleViewAction(),
        mDisplayOptionWidget->toggleViewAction()
    };
    winMenu->clear();
    for ( QAction* action : actions )
    {
        action->setMenuRole( QAction::NoRole );
        winMenu->addAction( action );
    }

    /// --- Help Menu ---
    connect( ui->actionHelp, &QAction::triggered, this, &MainWindow2::helpBox);
    connect( ui->actionAbout, &QAction::triggered, this, &MainWindow2::aboutPencil );

    // --------------- Menus ------------------
    mRecentFileMenu = new RecentFileMenu( tr("Open Recent"), this );
    mRecentFileMenu->loadFromDisk();
    ui->menuFile->insertMenu( ui->actionSave, mRecentFileMenu );

    connect( mRecentFileMenu, &RecentFileMenu::loadRecentFile, this, &MainWindow2::openFile );

    connect( ui->menuEdit, SIGNAL( aboutToShow() ), this, SLOT( undoActSetText() ) );
    connect( ui->menuEdit, SIGNAL( aboutToHide() ), this, SLOT( undoActSetEnabled() ) );
}

void MainWindow2::setMenuActionChecked( QAction* action, bool bChecked )
{
    SignalBlocker b( action );
    action->setChecked( bChecked );
}

void MainWindow2::setOpacity( int opacity )
{
    mEditor->preference()->set(SETTING::WINDOW_OPACITY, 100 - opacity);
    setWindowOpacity( opacity / 100.0 );
}


void MainWindow2::closeEvent( QCloseEvent* event )
{
    if ( maybeSave() )
    {
        writeSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow2::tabletEvent( QTabletEvent* event )
{
    event->ignore();
}

void MainWindow2::newDocument()
{
    if ( maybeSave() )
    {
        Object* object = new Object();
        object->init();
        mEditor->setObject( object );
        mEditor->scrubTo( 0 );
        //mEditor->view()->resetView();

        // Refresh the palette
        mColorPalette->refreshColorList();
        mEditor->color()->setColorNumber(0);

        setWindowTitle( PENCIL_WINDOW_TITLE );
    }
}

void MainWindow2::openDocumentDialog()
{
    if ( maybeSave() )
    {
        QSettings settings( PENCIL2D, PENCIL2D );

        QString strLastOpenPath = settings.value( LAST_FILE_PATH, QDir::homePath() ).toString();
        QString fileName = QFileDialog::getOpenFileName( this,
                                                         tr( "Open File..." ),
                                                         strLastOpenPath,
                                                         tr( PFF_OPEN_ALL_FILE_FILTER ) );
        openDocument(fileName);
    }
}

void MainWindow2::openDocument(const QString &fileName)
{
    if ( maybeSave() )
    {
        if ( fileName.isEmpty() )
        {
            return;
        }

        QFileInfo fileInfo( fileName );
        if ( fileInfo.isDir() )
        {
            return;
        }

        bool ok = openObject( fileName );

        if ( !ok )
        {
            QMessageBox::warning( this, tr("Warning"), tr("Pencil cannot read this file. If you want to import images, use the command import.") );
            newDocument();
        }
    }
}

bool MainWindow2::saveAsNewDocument()
{
    QSettings settings( PENCIL2D, PENCIL2D );

    QString strLastFolder = settings.value( LAST_FILE_PATH, QDir::homePath() ).toString();
    if ( strLastFolder.isEmpty() || !QDir(strLastFolder).exists() )
    {
        strLastFolder = QDir( QDir::homePath() ).filePath( PFF_DEFAULT_FILENAME );
    }

    QString fileName = QFileDialog::getSaveFileName( this,
                                                     tr( "Save As..." ),
                                                     strLastFolder,
                                                     tr( PFF_SAVE_ALL_FILE_FILTER ) );
    if ( fileName.isEmpty() )
    {
        return false;
    }

    if ( !fileName.endsWith( PFF_OLD_EXTENSION ) && !fileName.endsWith( PFF_EXTENSION ) )
    {
        fileName = fileName + PFF_EXTENSION;
    }
    settings.setValue( LAST_FILE_PATH, QVariant( fileName ) );

    return saveObject( fileName );

}

void MainWindow2::openFile( QString filename )
{
    qDebug() << "open recent file" << filename;
    bool ok = openObject( filename );
    if ( !ok )
    {
        QMessageBox::warning( this, tr("Warning"), tr("Pencil cannot read this file. If you want to import images, use the command import.") );
        newDocument();
    }
}

bool MainWindow2::openObject( QString strFilePath )
{
    QProgressDialog progress( tr("Opening document..."), tr("Abort"), 0, 100, this );

    progress.setWindowModality( Qt::WindowModal );
    progress.show();

    mEditor->setCurrentLayer( 0 );

    FileManager fm( this );
    Object* object = fm.load( strFilePath );

    if ( object == nullptr || !fm.error().ok() )
    {
        return false;
    }

    mEditor->setObject( object );

    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue( LAST_FILE_PATH, object->filePath() );

    mRecentFileMenu->addRecentFile( object->filePath() );
    mRecentFileMenu->saveToDisk();

    //qDebug() << "Current File Path=" << object->filePath();
    setWindowTitle( object->filePath() );

    // Refresh the Palette
    mColorPalette->refreshColorList();
    mEditor->color()->setColorNumber(0);

    // Reset view
    mEditor->scrubTo( 0 );
    mEditor->view()->resetView();

    progress.setValue( 100 );
    return true;
}

bool MainWindow2::saveObject( QString strSavedFileName )
{
    QProgressDialog progress( tr( "Saving document..." ), tr( "Abort" ), 0, 100, this );
    progress.setWindowModality( Qt::WindowModal );
    progress.show();

    FileManager* fm = new FileManager( this );
    Status st = fm->save( mEditor->object(), strSavedFileName );

    progress.setValue( 100 );
    
    if ( !st.ok() )
    {
        return false;
    }

    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue( LAST_FILE_PATH, strSavedFileName );

    mRecentFileMenu->addRecentFile( strSavedFileName );
    mRecentFileMenu->saveToDisk();

    mTimeLine->updateContent();

    setWindowTitle( strSavedFileName );

    return true;
}

void MainWindow2::saveDocument()
{
    if ( !mEditor->object()->filePath().isEmpty() )
    {
        saveObject( mEditor->object()->filePath() );
    }
    else
    {
        saveAsNewDocument();
    }
}

bool MainWindow2::maybeSave()
{
    if ( mEditor->object()->isModified() )
    {
        int ret = QMessageBox::warning( this, tr( "Warning" ),
                                        tr( "This animation has been modified.\n Do you want to save your changes?" ),
                                        QMessageBox::Yes | QMessageBox::Default,
                                        QMessageBox::No,
                                        QMessageBox::Cancel | QMessageBox::Escape );
        if ( ret == QMessageBox::Yes )
        {
            saveDocument();
            return true;
        }
        else if ( ret == QMessageBox::Cancel )
        {
            return false;
        }
    }
    return true;
}

void MainWindow2::importImage()
{
    QSettings settings( "Pencil", "Pencil" );
    QString initPath = settings.value( "lastImportPath", QDir::homePath() ).toString();

    QString strFilePath = QFileDialog::getOpenFileName( this,
                                                        tr( "Import image..." ),
                                                        initPath,
                                                        PENCIL_IMAGE_FILTER );
    if ( strFilePath.isEmpty() )
    {
        return;
    }

    if ( !QFile::exists( strFilePath ) )
    {
        return;
    }

    bool ok = mEditor->importImage( strFilePath );
    if ( !ok )
    {
        QMessageBox::warning( this,
                              tr( "Warning" ),
                              tr( "Unable to import image.<br><b>TIP:</b> Use Bitmap layer to import bitmaps." ),
                              QMessageBox::Ok,
                              QMessageBox::Ok );
        return;
    }

    settings.setValue( "lastImportPath", strFilePath );

    mScribbleArea->updateCurrentFrame();
    mTimeLine->updateContent();
}

void MainWindow2::importImageSequence()
{
    QFileDialog w;
    w.setFileMode( QFileDialog::AnyFile );

    QSettings settings( PENCIL2D, PENCIL2D );
    QString initialPath = settings.value( "lastImportPath", QVariant( QDir::homePath() ) ).toString();
    if ( initialPath.isEmpty() )
    {
        initialPath = QDir::homePath();
    }
    QStringList files = w.getOpenFileNames( this,
                                            "Select one or more files to open",
                                            initialPath,
                                            "Images (*.png *.jpg *.jpeg *.bmp)" );

    for ( QString strImgFile : files )
    {
        if ( strImgFile.endsWith( ".png" ) ||
             strImgFile.endsWith( ".jpg" ) ||
             strImgFile.endsWith( ".jpeg" ) ||
             strImgFile.endsWith( ".bmp" ) )
        {
            mEditor->importImage( strImgFile );
            mEditor->scrubForward();
        }
    }
}

void MainWindow2::importMovie()
{
    QSettings settings( "Pencil", "Pencil" );

    QString initialPath = settings.value( "lastExportPath", QDir::homePath() ).toString();
    QString filePath = QFileDialog::getOpenFileName( this,
                                                     tr( "Import movie" ),
                                                     initialPath,
                                                     PENCIL_MOVIE_EXT );
    if ( filePath.isEmpty() )
    {
        return;
    }
    mEditor->importMovie( filePath, mEditor->playback()->fps() );

    settings.setValue( "lastExportPath", filePath );
}

void MainWindow2::exportMovie()
{

}

void MainWindow2::exportImageSequence()
{
    QSettings settings( PENCIL2D, PENCIL2D );

    // Get the camera layer
    int cameraLayerId = mEditor->layers()->getLastCameraLayer();

    LayerCamera *cameraLayer = dynamic_cast< LayerCamera* >(mEditor->object()->getLayer(cameraLayerId));


    // Options
    auto dialog =  new ExportImageSeqDialog( this );
    OnScopeExit( dialog->deleteLater() );

    dialog->setExportSize( cameraLayer->getViewRect().size() );
    dialog->exec();

    QSize exportSize = dialog->getExportSize();
    QString exportFormat = dialog->getExportFormat();
    bool useTranparency = dialog->getTransparency();

    if ( dialog->result() == QDialog::Rejected )
    {
        return; // false;
    }

    // Path
    QString strInitPath = settings.value( "lastExportPath", QDir::homePath() + "/untitled.png" ).toString();

    QFileInfo info( strInitPath );
    strInitPath = info.path() + "/" + info.baseName() + "." + exportFormat.toLower();

    QString strFilePath = QFileDialog::getSaveFileName( this,
                                                        tr( "Save Image Sequence" ),
                                                        strInitPath);
    if ( strFilePath.isEmpty() )
    {
        // TODO:
        return; // false;
    }
    settings.setValue( "lastExportPath", strFilePath );


    // Export
    QTransform view = RectMapTransform( mScribbleArea->getViewRect(), QRectF( QPointF( 0, 0 ), exportSize ) );
//    view = mScribbleArea->getView() * view;


    int projectLength = mEditor->layers()->projectLength();
    mEditor->object()->exportFrames( 1,
                                     projectLength,
                                     cameraLayer,
                                     exportSize,
                                     strFilePath,
                                     exportFormat.toStdString().c_str(),
                                     -1,
                                     useTranparency,
                                     true,
                                     NULL,
                                     0 );
    //return true;
}

void MainWindow2::exportImage()
{
    QSettings settings( PENCIL2D, PENCIL2D );

    // Get the camera layer
    int cameraLayerId = mEditor->layers()->getLastCameraLayer();

    LayerCamera *cameraLayer = dynamic_cast< LayerCamera* >(mEditor->object()->getLayer(cameraLayerId));


    // Options
    auto dialog =  new ExportImageSeqDialog( this );
    OnScopeExit( dialog->deleteLater() );

    dialog->setExportSize( cameraLayer->getViewRect().size() );
    dialog->exec();

    QSize exportSize = dialog->getExportSize();
    QString exportFormat = dialog->getExportFormat();
    bool useTranparency = dialog->getTransparency();

    if ( dialog->result() == QDialog::Rejected )
    {
        return; // false;
    }


    // Path
    QString initPath = settings.value( "lastExportPath", QDir::homePath() + "/untitled.png" ).toString();

    QFileInfo info( initPath );
    initPath = info.path() + "/" + info.baseName() + "." + exportFormat.toLower();


    QString filePath = QFileDialog::getSaveFileName( this,
                                                     tr( "Save Image" ),
                                                     initPath);
    if ( filePath.isEmpty() )
    {
        qDebug() << "empty file";
        return;// false;
    }
    settings.setValue( "lastExportPath", QVariant( filePath ) );


    // Export
    QTransform view = RectMapTransform( mScribbleArea->getViewRect(), QRectF( QPointF( 0, 0 ), exportSize ) );
//    view = mScribbleArea->getView() * view;

    int projectLength = mEditor->layers()->projectLength();
    if ( !mEditor->object()->exportIm( mEditor->currentFrame(),
                                       projectLength,
                                       view,
                                       exportSize,
                                       filePath,
                                       exportFormat,
                                       true,
                                       useTranparency ) )
    {
        QMessageBox::warning( this,
                              tr( "Warning" ),
                              tr( "Unable to export image." ),
                              QMessageBox::Ok,
                              QMessageBox::Ok );
        return;// false;
    }
    return; // true;
}

void MainWindow2::preferences()
{
    PreferencesDialog* prefDialog = new PreferencesDialog( this );
    prefDialog->setAttribute( Qt::WA_DeleteOnClose );
    prefDialog->init( mEditor->preference() );

    connect( prefDialog, &PreferencesDialog::windowOpacityChange, this, &MainWindow2::setOpacity );
    connect( prefDialog, &PreferencesDialog::finished, [ &]
    { 
        qDebug() << "Preference dialog closed!";
        clearKeyboardShortcuts();
        setupKeyboardShortcuts();
    } );
    
    prefDialog->show();
}

void MainWindow2::dockAllSubWidgets()
{
    mToolBox->setFloating(false);
    mToolOptions->setFloating(false);
    mDisplayOptionWidget->setFloating(false);
    mTimeLine->setFloating(false);
    mColorPalette->setFloating(false);
    mColorWheel->setFloating( false );
}

void MainWindow2::readSettings()
{
    qDebug( "Restore last windows layout." );

    QSettings settings( PENCIL2D, PENCIL2D );
    restoreGeometry( settings.value( SETTING_WINDOW_GEOMETRY ).toByteArray() );
    restoreState( settings.value( SETTING_WINDOW_STATE ).toByteArray() );

    QString myPath = settings.value( LAST_FILE_PATH, QVariant( QDir::homePath() ) ).toString();
    mRecentFileMenu->addRecentFile( myPath );

    int opacity = mEditor->preference()->getInt(SETTING::WINDOW_OPACITY);

    setOpacity( 100 - opacity );
}

void MainWindow2::writeSettings()
{
    qDebug( "Save current windows layout." );

    QSettings settings( PENCIL2D, PENCIL2D );
    settings.setValue( SETTING_WINDOW_GEOMETRY, saveGeometry() );
    settings.setValue( SETTING_WINDOW_STATE, saveState() );
}

void MainWindow2::setupKeyboardShortcuts()
{
    checkExistingShortcuts();

    auto cmdKeySeq = []( QString strCommandName ) -> QKeySequence
    {
        strCommandName = QString( "shortcuts/" ) + strCommandName;
        QKeySequence keySequence( pencilSettings().value( strCommandName ).toString() );
        return keySequence;
    };

    ui->actionNew->setShortcut( cmdKeySeq( CMD_NEW_FILE ) );
    ui->actionOpen->setShortcut( cmdKeySeq( CMD_OPEN_FILE ) );
    ui->actionSave->setShortcut( cmdKeySeq( CMD_SAVE_FILE ) );
    ui->actionSave_as->setShortcut( cmdKeySeq( CMD_SAVE_AS ) );
    ui->actionPrint->setShortcut( cmdKeySeq( CMD_PRINT ) );

    ui->actionImport_Image->setShortcut( cmdKeySeq( CMD_IMPORT_IMAGE ) );
    ui->actionImport_Image_Sequence->setShortcut( cmdKeySeq( CMD_IMPORT_IMAGE_SEQ ) );
    ui->actionImport_Movie->setShortcut( cmdKeySeq( CMD_IMPORT_MOVIE ) );
    ui->actionImport_Palette->setShortcut( cmdKeySeq( CMD_IMPORT_PALETTE ) );
    ui->actionImport_Sound->setShortcut( cmdKeySeq( CMD_IMPORT_SOUND ) );

    ui->actionExport_Image->setShortcut( cmdKeySeq( CMD_EXPORT_IMAGE ) );
    ui->actionExport_Image_Sequence->setShortcut( cmdKeySeq( CMD_EXPORT_IMAGE_SEQ ) );
    ui->actionExport_Movie->setShortcut( cmdKeySeq( CMD_EXPORT_MOVIE ) );
    ui->actionExport_Palette->setShortcut( cmdKeySeq( CMD_EXPORT_PALETTE ) );
    ui->actionExport_Svg_Image->setShortcut( cmdKeySeq( CMD_EXPORT_SVG ) );
    ui->actionExport_X_sheet->setShortcut( cmdKeySeq( CMD_EXPORT_XSHEET ) );

    // edit menu
    ui->actionUndo->setShortcut( cmdKeySeq( CMD_UNDO ) );
    ui->actionRedo->setShortcut( cmdKeySeq( CMD_REDO ) );
    ui->actionCut->setShortcut( cmdKeySeq( CMD_CUT ) );
    ui->actionCopy->setShortcut( cmdKeySeq( CMD_COPY ) );
    ui->actionPaste->setShortcut( cmdKeySeq( CMD_PASTE ) );
    ui->actionClearFrame->setShortcut( cmdKeySeq( CMD_CLEAR_FRAME ) );
    ui->actionSelect_All->setShortcut( cmdKeySeq( CMD_SELECT_ALL ) );
    ui->actionDeselect_All->setShortcut( cmdKeySeq( CMD_DESELECT_ALL ) );
    ui->actionPreference->setShortcut( cmdKeySeq( CMD_PREFERENCE ) );

    ui->actionReset_Windows->setShortcut( cmdKeySeq( CMD_RESET_WINDOWS ) );
    ui->actionReset_View->setShortcut( cmdKeySeq( CMD_RESET_ZOOM_ROTATE ) );
    ui->actionZoom_In->setShortcut( cmdKeySeq( CMD_ZOOM_IN ) );
    ui->actionZoom_Out->setShortcut( cmdKeySeq( CMD_ZOOM_OUT ) );
    ui->actionRotate_Clockwise->setShortcut( cmdKeySeq( CMD_ROTATE_CLOCK ) );
    ui->actionRotate_Anticlosewise->setShortcut( cmdKeySeq( CMD_ROTATE_ANTI_CLOCK ) );
    ui->actionHorizontal_Flip->setShortcut( cmdKeySeq( CMD_FLIP_HORIZONTAL ) );
    ui->actionVertical_Flip->setShortcut( cmdKeySeq( CMD_FLIP_VERTICAL ) );
    ui->actionPreview->setShortcut( cmdKeySeq( CMD_PREVIEW ) );
    ui->actionGrid->setShortcut( cmdKeySeq( CMD_GRID ) );
    ui->actionOnionPrev->setShortcut( cmdKeySeq( CMD_ONIONSKIN_PREV ) );
    ui->actionOnionNext->setShortcut( cmdKeySeq( CMD_ONIONSKIN_NEXT ) );

    ui->actionPlay->setShortcut( cmdKeySeq( CMD_PLAY ) );
    ui->actionLoop->setShortcut( cmdKeySeq( CMD_LOOP ) );
    ui->actionPrevious_Frame->setShortcut( cmdKeySeq( CMD_GOTO_PREV_FRAME ) );
    ui->actionNext_Frame->setShortcut( cmdKeySeq( CMD_GOTO_NEXT_FRAME ) );
    ui->actionPrev_KeyFrame->setShortcut( cmdKeySeq( CMD_GOTO_PREV_KEY_FRAME ) );
    ui->actionNext_KeyFrame->setShortcut( cmdKeySeq( CMD_GOTO_NEXT_KEY_FRAME ) );
    ui->actionAdd_Frame->setShortcut( cmdKeySeq( CMD_ADD_FRAME ) );
    ui->actionDuplicate_Frame->setShortcut( cmdKeySeq( CMD_DUPLICATE_FRAME ) );
    ui->actionRemove_Frame->setShortcut( cmdKeySeq( CMD_REMOVE_FRAME ) );
    ui->actionMove_Frame_Backward->setShortcut( cmdKeySeq( CMD_MOVE_FRAME_BACKWARD ) );
    ui->actionMove_Frame_Forward->setShortcut( cmdKeySeq( CMD_MOVE_FRAME_FORWARD ) );

    ShortcutFilter* shortcutfilter = new ShortcutFilter( mScribbleArea );
    ui->actionMove->setShortcut( cmdKeySeq( CMD_TOOL_MOVE ) );
    ui->actionSelect->setShortcut( cmdKeySeq( CMD_TOOL_SELECT ) );
    ui->actionBrush->setShortcut( cmdKeySeq( CMD_TOOL_BRUSH ) );
    ui->actionPolyline->setShortcut( cmdKeySeq( CMD_TOOL_POLYLINE ) );
    ui->actionSmudge->setShortcut( cmdKeySeq( CMD_TOOL_SMUDGE ) );
    ui->actionPen->setShortcut( cmdKeySeq( CMD_TOOL_PEN ) );
    ui->actionHand->setShortcut( cmdKeySeq( CMD_TOOL_HAND ) );
    ui->actionPencil->setShortcut( cmdKeySeq( CMD_TOOL_PENCIL ) );
    ui->actionBucket->setShortcut( cmdKeySeq( CMD_TOOL_BUCKET ) );
    ui->actionEyedropper->setShortcut( cmdKeySeq( CMD_TOOL_EYEDROPPER ) );
    ui->actionEraser->setShortcut( cmdKeySeq( CMD_TOOL_ERASER ) );

    ui->actionMove->installEventFilter( shortcutfilter );
    ui->actionMove->installEventFilter( shortcutfilter );
    ui->actionSelect->installEventFilter( shortcutfilter );
    ui->actionBrush->installEventFilter( shortcutfilter );
    ui->actionPolyline->installEventFilter( shortcutfilter );
    ui->actionSmudge->installEventFilter( shortcutfilter );
    ui->actionPen->installEventFilter( shortcutfilter );
    ui->actionHand->installEventFilter( shortcutfilter );
    ui->actionPencil->installEventFilter( shortcutfilter );
    ui->actionBucket->installEventFilter( shortcutfilter );
    ui->actionEyedropper->installEventFilter( shortcutfilter );
    ui->actionEraser->installEventFilter( shortcutfilter );

    ui->actionTogglePalette->setShortcut( cmdKeySeq( CMD_TOGGLE_PALETTE ) );
    //mScribbleArea->getPopupPalette()->closeButton->setText( tr("close/toggle (") + pencilSettings()->value( QString( "shortcuts/" ) + CMD_TOGGLE_PALETTE ).toString() + ")" );
    //mScribbleArea->getPopupPalette()->closeButton->setShortcut( cmdKeySeq( CMD_TOGGLE_PALETTE ) );

    ui->actionNew_Bitmap_Layer->setShortcut( cmdKeySeq( CMD_NEW_BITMAP_LAYER ) );
    ui->actionNew_Vector_Layer->setShortcut( cmdKeySeq( CMD_NEW_VECTOR_LAYER ) );
    ui->actionNew_Camera_Layer->setShortcut( cmdKeySeq( CMD_NEW_CAMERA_LAYER ) );
    ui->actionNew_Sound_Layer->setShortcut( cmdKeySeq( CMD_NEW_SOUND_LAYER ) );

    mToolBox->toggleViewAction()->setShortcut( cmdKeySeq( CMD_TOGGLE_TOOLBOX ) );
    mToolOptions->toggleViewAction()->setShortcut( cmdKeySeq( CMD_TOGGLE_TOOL_OPTIONS ) );
    mColorWheel->toggleViewAction()->setShortcut( cmdKeySeq( CMD_TOGGLE_COLOR_WHEEL ) );
    mColorPalette->toggleViewAction()->setShortcut( cmdKeySeq( CMD_TOGGLE_COLOR_LIBRARY ) );
    mTimeLine->toggleViewAction()->setShortcut( cmdKeySeq( CMD_TOGGLE_TIMELINE ) );
    mDisplayOptionWidget->toggleViewAction()->setShortcut( cmdKeySeq( CMD_TOGGLE_DISPLAY_OPTIONS ) );

    ui->actionHelp->setShortcut( cmdKeySeq( CMD_HELP ) );
}

void MainWindow2::clearKeyboardShortcuts()
{
    QList<QAction*> actionList = this->findChildren<QAction*>();
    foreach( QAction* action, actionList )
    {
        action->setShortcut( QKeySequence( 0 ) );
    }
}

void MainWindow2::undoActSetText( void )
{
    if ( this->mEditor->mBackupIndex < 0 )
    {
        ui->actionUndo->setText( tr("Undo") );
        ui->actionUndo->setEnabled( false );
    }
    else
    {
        ui->actionUndo->setText( tr("Undo   ")
                                + QString::number( this->mEditor->mBackupIndex + 1 )
                                + " "
                                + this->mEditor->mBackupList.at( this->mEditor->mBackupIndex )->undoText );
        ui->actionUndo->setEnabled( true );
    }

    if ( this->mEditor->mBackupIndex + 2 < this->mEditor->mBackupList.size() )
    {
        ui->actionRedo->setText( tr("Redo   ")
                                + QString::number( this->mEditor->mBackupIndex + 2 )
                                + " "
                                + this->mEditor->mBackupList.at( this->mEditor->mBackupIndex + 1 )->undoText );
        ui->actionRedo->setEnabled( true );
    }
    else
    {
        ui->actionRedo->setText( tr("Redo") );
        ui->actionRedo->setEnabled( false );
    }
}

void MainWindow2::undoActSetEnabled( void )
{
    ui->actionUndo->setEnabled( true );
    ui->actionRedo->setEnabled( true );
}

void MainWindow2::exportPalette()
{
    QSettings settings( "Pencil", "Pencil" );
    QString initialPath = settings.value( "lastPalettePath", QVariant( QDir::homePath() ) ).toString();
    if ( initialPath.isEmpty() )
    {
        initialPath = QDir::homePath() + "/untitled.xml";
    }
    QString filePath = QFileDialog::getSaveFileName( this, tr( "Export As" ), initialPath );
    if ( !filePath.isEmpty() )
    {
        mEditor->object()->exportPalette( filePath );
        settings.setValue( "lastPalettePath", QVariant( filePath ) );
    }
}

void MainWindow2::importPalette()
{
    QSettings settings( "Pencil", "Pencil" );
    QString initialPath = settings.value( "lastPalettePath", QVariant( QDir::homePath() ) ).toString();
    if ( initialPath.isEmpty() )
    {
        initialPath = QDir::homePath() + "/untitled.xml";
    }
    QString filePath = QFileDialog::getOpenFileName( this, tr( "Import" ), initialPath );
    if ( !filePath.isEmpty() )
    {
        mEditor->object()->importPalette( filePath );
        mColorPalette->refreshColorList();
        mEditor->color()->setColorNumber(0);
        settings.setValue( "lastPalettePath", QVariant( filePath ) );
    }
}

void MainWindow2::aboutPencil()
{
    QFile aboutFile( ":resources/about.html" );
    bool isOpenOK = aboutFile.open( QIODevice::ReadOnly | QIODevice::Text );

    if ( isOpenOK )
    {
        QString strAboutText = QTextStream( &aboutFile ).readAll();
        QMessageBox::about( this, PENCIL_WINDOW_TITLE, strAboutText );
    }
}

void MainWindow2::helpBox()
{
    //qDebug() << "Open help manual.";

    QString url = "http://www.pencil2d.org/documentation/";
    QDesktopServices::openUrl( QUrl(url) );
}

void MainWindow2::makeConnections( Editor* editor, ColorBox* colorBox )
{
    connect( colorBox, &ColorBox::colorChanged, editor->color(), &ColorManager::setColor );
    connect( editor->color(), &ColorManager::colorChanged, colorBox, &ColorBox::setColor );
}

void MainWindow2::makeConnections( Editor* editor, ScribbleArea* scribbleArea )
{
    connect( editor->tools(), &ToolManager::toolChanged, scribbleArea, &ScribbleArea::setCurrentTool );
    connect( editor->tools(), &ToolManager::toolPropertyChanged, scribbleArea, &ScribbleArea::updateToolCursor );
    connect( editor->layers(), &LayerManager::currentLayerChanged, scribbleArea, &ScribbleArea::updateAllFrames );

    connect( editor, &Editor::currentFrameChanged, scribbleArea, &ScribbleArea::updateFrame );
    connect( editor, &Editor::selectAll, scribbleArea, &ScribbleArea::selectAll );

    connect( editor->view(), &ViewManager::viewChanged, scribbleArea, &ScribbleArea::updateAllFrames );
//    connect( editor->preference(), &PreferenceManager::preferenceChanged, scribbleArea, &ScribbleArea::onPreferencedChanged );
}

void MainWindow2::makeConnections( Editor* pEditor, TimeLine* pTimeline )
{
    PlaybackManager* pPlaybackManager = pEditor->playback();
    connect( pTimeline, &TimeLine::duplicateKeyClick, pEditor, &Editor::duplicateKey );

    connect( pTimeline, &TimeLine::loopStartClick, pPlaybackManager, &PlaybackManager::setRangedStartFrame );
    connect( pTimeline, &TimeLine::loopEndClick, pPlaybackManager, &PlaybackManager::setRangedEndFrame );

    connect( pTimeline, &TimeLine::soundClick, pPlaybackManager, &PlaybackManager::enbaleSound );
    connect( pTimeline, &TimeLine::fpsClick, pPlaybackManager, &PlaybackManager::setFps );

    connect( pTimeline, &TimeLine::addKeyClick, mCommands, &ActionCommands::addNewKey );
    connect( pTimeline, &TimeLine::removeKeyClick, mCommands, &ActionCommands::removeKey );
    
    connect( pTimeline, &TimeLine::newBitmapLayer, mCommands, &ActionCommands::addNewBitmapLayer );
    connect( pTimeline, &TimeLine::newVectorLayer, mCommands, &ActionCommands::addNewVectorLayer );
    connect( pTimeline, &TimeLine::newSoundLayer, mCommands, &ActionCommands::addNewSoundLayer );
    connect( pTimeline, &TimeLine::newCameraLayer, mCommands, &ActionCommands::addNewCameraLayer );

    connect( pTimeline, &TimeLine::toogleAbsoluteOnionClick, pEditor, &Editor::toogleOnionSkinType );


    connect( pEditor->layers(), &LayerManager::currentLayerChanged, pTimeline, &TimeLine::updateUI );
    connect( pEditor->layers(), &LayerManager::layerCountChanged,   pTimeline, &TimeLine::updateUI );
    connect( pEditor->sound(), &SoundManager::soundClipDurationChanged, pTimeline, &TimeLine::updateUI );
    connect( pEditor, &Editor::updateTimeLine,   pTimeline, &TimeLine::updateUI );

}

void MainWindow2::makeConnections(Editor* editor, DisplayOptionWidget* display)
{
    display->makeConnectionToEditor( editor );
}

void MainWindow2::makeConnections( Editor* editor, ToolOptionWidget* toolOptions )
{
    toolOptions->makeConnectionToEditor( editor );
}


void MainWindow2::makeConnections( Editor* pEditor, ColorPaletteWidget* pColorPalette )
{
    connect( pEditor, &Editor::fileLoaded, pColorPalette, &ColorPaletteWidget::updateUI );

    ColorManager* pColorManager = pEditor->color();
    ScribbleArea* pScribbleArea = pEditor->getScribbleArea();

    connect( pColorPalette, &ColorPaletteWidget::colorChanged, pColorManager, &ColorManager::setColor );
    connect( pColorPalette, &ColorPaletteWidget::colorNumberChanged, pColorManager, &ColorManager::setColorNumber );

    connect( pColorPalette, &ColorPaletteWidget::colorChanged, pScribbleArea, &ScribbleArea::paletteColorChanged );

    connect( pColorManager, &ColorManager::colorChanged, pColorPalette, &ColorPaletteWidget::setColor );
    connect( pColorManager, &ColorManager::colorNumberChanged, pColorPalette, &ColorPaletteWidget::selectColorNumber );
}

void MainWindow2::bindActionWithSetting( QAction* action, SETTING setting )
{
    PreferenceManager* prefs = mEditor->preference();

    // set initial state
    action->setChecked( prefs->isOn( setting ) );

    // 2-way binding
    connect( action, &QAction::triggered, prefs, [ = ] ( bool b )
    {
        prefs->set( setting, b );
    } );

    connect( prefs, &PreferenceManager::optionChanged, action, [ = ]( SETTING s )
    {
        if ( s == setting )
        {
            action->setChecked( prefs->isOn( setting ) );
        }
    } );
}

void MainWindow2::updateZoomLabel()
{
    float zoom = mEditor->view()->scaling() * 100.f;
    statusBar()->showMessage( QString( "Zoom: %0%1" ).arg( zoom, 0, 'f', 1 ).arg("%") );
}
