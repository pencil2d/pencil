/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2008-2009 Mj Mendoza IV

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <QList>
#include <QMenu>
#include <QScopedPointer>
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressDialog>
#include <QDesktopWidget>
//#include <QShortcut>

#include "pencildef.h"
#include "pencilsettings.h"

#include "object.h"
#include "objectsaveloader.h"

#include "editor.h"
#include "colormanager.h"
#include "layermanager.h"
#include "toolmanager.h"
#include "playbackmanager.h"

#include "scribblearea.h"
#include "colorpalettewidget.h"
#include "displayoptiondockwidget.h"
#include "tooloptiondockwidget.h"
#include "popupcolorpalettewidget.h"
#include "preferences.h"
#include "timeline.h"
#include "toolbox.h"

#include "colorbox.h"
#include "util.h"

#include "fileformat.h"		//contains constants used by Pencil File Format
#include "JlCompress.h"		//compress and decompress New Pencil File Format
#include "recentfilemenu.h"

#include "mainwindow2.h"
#include "ui_mainwindow2.h"


MainWindow2::MainWindow2( QWidget *parent )
    : QMainWindow( parent )
    , ui( new Ui::MainWindow2 )
{
    ui->setupUi( this );

    // Central widget
    m_pScribbleArea = new ScribbleArea( this );
    m_pScribbleArea->setObjectName( "ScribbleArea" );
    m_pScribbleArea->setFocusPolicy( Qt::StrongFocus );
    setCentralWidget( m_pScribbleArea );

    Object* object = new Object();
    object->init();

    m_pEditor = new Editor( this );
    m_pEditor->initialize( m_pScribbleArea );
    m_pEditor->setObject( object );

    m_pScribbleArea->setCore( m_pEditor );
    m_pEditor->setScribbleArea( m_pScribbleArea );
    makeConnections( m_pEditor, m_pScribbleArea );

    createDockWidgets();
    createMenus();
    setupKeyboardShortcuts();

    m_pEditor->resetUI();

    readSettings();

    connect(m_pEditor, &Editor::needSave, this, &MainWindow2::saveDocument);
    connect(m_pToolBox, &ToolBoxWidget::clearButtonClicked, m_pEditor, &Editor::clearCurrentFrame);

    m_pEditor->setCurrentLayer( m_pEditor->object()->getLayerCount() - 1 );
}

MainWindow2::~MainWindow2()
{
    delete ui;
}

void MainWindow2::createDockWidgets()
{
    m_pTimeLine = new TimeLine( this );
    m_pTimeLine->setObjectName( "TimeLine" );
    makeConnections( m_pEditor, m_pTimeLine );
    m_subWidgets.append( m_pTimeLine );

    m_pColorWheelWidget = new QDockWidget( tr("Color Wheel"), this );

    ColorBox* pColorBox = new ColorBox(this);
    pColorBox->setToolTip(tr("color palette:<br>use <b>(C)</b><br>toggle at cursor"));
    m_pColorWheelWidget->setWidget( pColorBox );
    m_pColorWheelWidget->setObjectName( "ColorWheel" );
    makeColorWheelConnections();

    m_pColorPalette = new ColorPaletteWidget( tr( "Color Palette" ), this );
    m_pColorPalette->setObjectName( "ColorPalette" );
    makeConnections( m_pEditor, m_pColorPalette );
    m_subWidgets.append( m_pColorPalette );

    m_pDisplayOptionWidget = new DisplayOptionWidget(this);
    m_pDisplayOptionWidget->setObjectName( "DisplayOption" );
    m_pDisplayOptionWidget->makeConnectionToEditor(m_pEditor);

    m_pToolOptionWidget = new ToolOptionWidget(this);
    m_pToolOptionWidget->setObjectName( "ToolOption" );
    m_pToolOptionWidget->makeConnectionToEditor(m_pEditor);

    m_pToolBox = new ToolBoxWidget( tr( "Tools" ), this );
    m_pToolBox->setObjectName( "ToolBox" );
    m_subWidgets.append( m_pToolBox );

    addDockWidget(Qt::RightDockWidgetArea,  m_pColorWheelWidget);
    addDockWidget(Qt::RightDockWidgetArea,  m_pColorPalette);
    addDockWidget(Qt::RightDockWidgetArea,  m_pDisplayOptionWidget);
    addDockWidget(Qt::LeftDockWidgetArea,   m_pToolBox);
    addDockWidget(Qt::LeftDockWidgetArea,   m_pToolOptionWidget);
    addDockWidget(Qt::BottomDockWidgetArea, m_pTimeLine);

    for ( BaseDockWidget* pWidget : m_subWidgets )
    {
        pWidget->setCore( m_pEditor );
        pWidget->initUI();
        qDebug() << "Init UI: " << pWidget->objectName();
        pWidget->setFeatures( QDockWidget::AllDockWidgetFeatures );
        pWidget->setFocusPolicy( Qt::NoFocus );
    }
}

void MainWindow2::makeColorWheelConnections()
{
    ColorBox* pColorBox = static_cast<ColorBox*>(m_pColorWheelWidget->widget());
    Q_ASSERT( pColorBox );

    connect( pColorBox, &ColorBox::colorChanged, m_pEditor->color(), &ColorManager::setColor );
    connect( m_pEditor->color(), &ColorManager::colorChanged, pColorBox, &ColorBox::setColor );
}

void MainWindow2::createMenus()
{
    // ---------- File Menu -------------
    connect( ui->actionNew, &QAction::triggered, this, &MainWindow2::newDocument );
    connect( ui->actionOpen, &QAction::triggered, this, &MainWindow2::openDocument );
    connect( ui->actionSave_as, &QAction::triggered, this, &MainWindow2::saveAsNewDocument );
    connect( ui->actionSave, &QAction::triggered, this, &MainWindow2::saveDocument );
    connect( ui->actionExit, &QAction::triggered, this, &MainWindow2::close );

    /// --- Export Menu ---
    connect( ui->actionExport_X_sheet, &QAction::triggered, m_pEditor, &Editor::exportX );
    connect( ui->actionExport_Image_Sequence, &QAction::triggered, m_pEditor, &Editor::exportImageSequence );
    connect( ui->actionExport_Image, &QAction::triggered, m_pEditor, &Editor::exportImage );
    connect( ui->actionExport_Movie, &QAction::triggered, m_pEditor, &Editor::exportMov );

    //exportFlashAct = new QAction(tr("&Flash/SWF..."), this);
    //exportFlashAct->setShortcut(tr("Ctrl+Alt+F"));
    //connect(exportFlashAct, SIGNAL(triggered()), editor, SLOT(exportFlash()));

    connect( ui->actionExport_Palette, &QAction::triggered, this, &MainWindow2::exportPalette );

    /// --- Import Menu ---
    //connect( ui->actionExport_Svg_Image, &QAction::triggered, editor, &Editor::saveSvg );
    connect( ui->actionImport_Image, &QAction::triggered, m_pEditor, &Editor::importImageFromDialog );
    connect( ui->actionImport_Image_Sequence, &QAction::triggered, m_pEditor, &Editor::importImageSequence );
    connect( ui->actionImport_Movie, &QAction::triggered, m_pEditor, &Editor::importMov );
    //connect( ui->actionImport_Sound, &QAction::triggered, editor, &Editor::importSound );
    ui->actionImport_Sound->setEnabled( false );
    connect( ui->actionImport_Palette, &QAction::triggered, this, &MainWindow2::importPalette );

    /// --- Edit Menu ---
    connect( ui->actionUndo, &QAction::triggered, m_pEditor, &Editor::undo );
    connect( ui->actionRedo, &QAction::triggered, m_pEditor, &Editor::redo );
    connect( ui->actionCut, &QAction::triggered, m_pEditor, &Editor::cut );
    connect( ui->actionCopy, &QAction::triggered, m_pEditor, &Editor::copy );
    connect( ui->actionPaste, &QAction::triggered, m_pEditor, &Editor::paste );
    connect( ui->actionClearFrame, &QAction::triggered, m_pEditor, &Editor::clearCurrentFrame );
    connect( ui->actionFlip_X, &QAction::triggered, m_pEditor, &Editor::flipX );
    connect( ui->actionFlip_Y, &QAction::triggered, m_pEditor, &Editor::flipY );
    connect( ui->actionSelect_All, &QAction::triggered, m_pEditor, &Editor::selectAll );
    connect( ui->actionDeselect_All, &QAction::triggered, m_pEditor, &Editor::deselectAll );
    connect( ui->actionPreference, &QAction::triggered, this, &MainWindow2::showPreferences );

    ui->actionRedo->setEnabled( false );

    /// --- Layer Menu ---
    connect( ui->actionNew_Bitmap_Layer, &QAction::triggered, m_pEditor, &Editor::newBitmapLayer );
    connect( ui->actionNew_Vector_Layer, &QAction::triggered, m_pEditor, &Editor::newVectorLayer );
    connect( ui->actionNew_Sound_Layer, &QAction::triggered, m_pEditor, &Editor::newSoundLayer );
    connect( ui->actionNew_Camera_Layer, &QAction::triggered, m_pEditor, &Editor::newCameraLayer );
    connect( ui->actionDelete_Current_Layer, &QAction::triggered, m_pEditor, &Editor::deleteCurrentLayer );

    /// --- View Menu ---
    connect( ui->actionZoom_In, &QAction::triggered, m_pEditor, &Editor::zoomIn );
    connect( ui->actionZoom_Out, &QAction::triggered, m_pEditor, &Editor::zoomOut );
    connect( ui->actionRotate_Clockwise, &QAction::triggered, m_pEditor, &Editor::rotatecw );
    connect( ui->actionRotate_Anticlosewise, &QAction::triggered, m_pEditor, &Editor::rotateacw );
    connect( ui->actionReset_Windows, &QAction::triggered, this, &MainWindow2::dockAllPalettes );
    connect( ui->actionReset_View, &QAction::triggered, m_pEditor, &Editor::resetView );
    connect( ui->actionHorizontal_Flip, &QAction::triggered, m_pEditor, &Editor::toggleMirror );
    connect( ui->actionVertical_Flip, &QAction::triggered, m_pEditor, &Editor::toggleMirrorV );

    ui->actionPreview->setEnabled( false );
    //#	connect(previewAct, SIGNAL(triggered()), editor, SLOT(getCameraLayer()));//TODO: Preview view

    ui->actionGrid->setEnabled( false );
    connect( ui->actionGrid, &QAction::triggered, m_pEditor, &Editor::gridview ); //TODO: Grid view

    connect( ui->actionOnionPrevious, &QAction::triggered, m_pEditor, &Editor::toggleOnionPrev );
    connect( ui->actionOnionNext, &QAction::triggered, m_pEditor, &Editor::toggleOnionNext );
    connect( ui->actionMultiLayerOnionSkin, &QAction::triggered, m_pEditor, &Editor::toggleMultiLayerOnionSkin );

    connect( m_pEditor, &Editor::onionPrevChanged, ui->actionOnionPrevious, &QAction::setChecked );
    connect( m_pEditor, &Editor::onionNextChanged, ui->actionOnionNext, &QAction::setChecked );
    connect( m_pEditor, SIGNAL(multiLayerOnionSkinChanged(bool)), ui->actionMultiLayerOnionSkin, SLOT(setChecked(bool)));

    /// --- Animation Menu ---
    PlaybackManager* pPlaybackManager = m_pEditor->playback();
    connect( ui->actionPlay, &QAction::triggered, pPlaybackManager, &PlaybackManager::play );

    connect( ui->actionLoop, &QAction::triggered, pPlaybackManager, &PlaybackManager::setLoop );
    connect( ui->actionLoopControl, &QAction::triggered, pPlaybackManager, &PlaybackManager::enableRangedPlayback );
    connect( pPlaybackManager, &PlaybackManager::loopStateChanged, ui->actionLoop, &QAction::setChecked );
    connect( pPlaybackManager, &PlaybackManager::rangedPlaybackStateChanged, ui->actionLoopControl, &QAction::setChecked );

    connect(ui->actionAdd_Frame, &QAction::triggered, m_pEditor, &Editor::addNewKey );
    connect(ui->actionRemove_Frame, &QAction::triggered, m_pEditor, &Editor::removeKey );
    //connect(ui->actionNext_Frame, &QAction::triggered, m_pEditor, &Editor::playNextFrame );
    //connect(ui->actionPrevious_Frame, &QAction::triggered, m_pEditor, &Editor::playPrevFrame );
    connect(ui->actionNext_KeyFrame, &QAction::triggered, m_pEditor, &Editor::scrubNextKeyFrame );
    connect(ui->actionPrev_KeyFrame, &QAction::triggered, m_pEditor, &Editor::scrubPreviousKeyFrame );
    connect(ui->actionDuplicate_Frame, &QAction::triggered, m_pEditor, &Editor::duplicateKey );

    /// --- Tool Menu ---
    connect(ui->actionMove, &QAction::triggered, m_pToolBox, &ToolBoxWidget::moveOn );
    connect(ui->actionSelect, &QAction::triggered, m_pToolBox, &ToolBoxWidget::selectOn );
    connect(ui->actionBrush, &QAction::triggered, m_pToolBox, &ToolBoxWidget::brushOn );
    connect(ui->actionPolyline, &QAction::triggered, m_pToolBox, &ToolBoxWidget::polylineOn );
    connect(ui->actionSmudge, &QAction::triggered, m_pToolBox, &ToolBoxWidget::smudgeOn );
    connect(ui->actionPen, &QAction::triggered, m_pToolBox, &ToolBoxWidget::penOn );
    connect(ui->actionHand, &QAction::triggered, m_pToolBox, &ToolBoxWidget::handOn );
    connect(ui->actionPencil, &QAction::triggered, m_pToolBox, &ToolBoxWidget::pencilOn );
    connect(ui->actionBucket, &QAction::triggered, m_pToolBox, &ToolBoxWidget::bucketOn );
    connect(ui->actionEyedropper, &QAction::triggered, m_pToolBox, &ToolBoxWidget::eyedropperOn );
    connect(ui->actionEraser, &QAction::triggered, m_pToolBox, &ToolBoxWidget::eraserOn );
    connect(ui->actionTogglePalette, &QAction::triggered, m_pScribbleArea,&ScribbleArea::togglePopupPalette );
    connect(ui->actionResetToolsDefault, &QAction::triggered, this, &MainWindow2::resetToolsSettings );

    /// --- Window Menu ---
    QMenu* pWinMenu = ui->menuWindows;
    pWinMenu->addAction( m_pToolBox->toggleViewAction() );
    pWinMenu->addAction( m_pToolOptionWidget->toggleViewAction() );
    pWinMenu->addAction( m_pColorWheelWidget->toggleViewAction() );
    pWinMenu->addAction( m_pColorPalette->toggleViewAction() );
    pWinMenu->addAction( m_pTimeLine->toggleViewAction() );
    pWinMenu->addAction( m_pDisplayOptionWidget->toggleViewAction() );

    /// --- Help Menu ---
    connect( ui->actionHelp, &QAction::triggered, this, &MainWindow2::helpBox);
    connect( ui->actionAbout, &QAction::triggered, this, &MainWindow2::aboutPencil );

    // --------------- Menus ------------------
    m_recentFileMenu = new RecentFileMenu( tr("Open Recent"), this );
    m_recentFileMenu->loadFromDisk();
    ui->menuFile->insertMenu( ui->actionSave, m_recentFileMenu );

    QObject::connect( m_recentFileMenu, SIGNAL( loadRecentFile( QString ) ),
        this, SLOT( openFile( QString ) ) );

    connect( ui->menuEdit, SIGNAL( aboutToShow() ), this, SLOT( undoActSetText() ) );
    connect( ui->menuEdit, SIGNAL( aboutToHide() ), this, SLOT( undoActSetEnabled() ) );
}

void MainWindow2::setOpacity( int opacity )
{
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue( "windowOpacity", 100 - opacity );
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
        m_pEditor->setObject( object );
        m_pEditor->resetUI();

        setWindowTitle( PENCIL_WINDOW_TITLE );
    }
}

void MainWindow2::openDocument()
{
    if ( maybeSave() )
    {
        QSettings settings( "Pencil", "Pencil" );

        QString strLastOpenPath = settings.value( "lastFilePath", QDir::homePath() ).toString();
        QString fileName = QFileDialog::getOpenFileName( this,
                                                         tr( "Open File..." ),
                                                         strLastOpenPath,
                                                         tr( PFF_OPEN_ALL_FILE_FILTER ) );

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
    QSettings settings( "Pencil", "Pencil" );

    QString strLastFolder = settings.value( "lastFilePath", QDir::homePath() ).toString();
    if ( strLastFolder.isEmpty() )
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
    settings.setValue( "lastFilePath", QVariant( fileName ) );

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

    m_pEditor->setCurrentLayer( 0 );
    m_pScribbleArea->setMyView( QMatrix() );

    ObjectSaveLoader objectLoader( this );
    Object* object = objectLoader.load( strFilePath );

    if ( object == nullptr || objectLoader.error().code() != PCL_OK )
    {
        return false;
    }

    m_pEditor->setObject( object );

    object->setFilePath( strFilePath );
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue( "LastFilePath", QVariant( object->filePath() ) );

    m_recentFileMenu->addRecentFile( object->filePath() );
    m_recentFileMenu->saveToDisk();

    //qDebug() << "Current File Path=" << object->filePath();
    setWindowTitle( object->filePath() );

    progress.setValue( 100 );
    return true;
}
/*
bool MainWindow2::loadDomElement( QDomElement docElem, QString filePath )
{
    Q_UNUSED( filePath );

    if ( docElem.isNull() )
    {
        return false;
    }
    QDomNode tag = docElem.firstChild();
    while ( !tag.isNull() )
    {
        QDomElement element = tag.toElement(); // try to convert the node to an element.
        if ( !element.isNull() )
        {
            if ( element.tagName() == "currentLayer" )
            {
                int nCurrentLayerIndex = element.attribute( "value" ).toInt();
                m_pEditor->setCurrentLayer( nCurrentLayerIndex );
            }
            if ( element.tagName() == "currentFrame" )
            {
                m_pEditor->scrubTo( element.attribute( "value" ).toInt() );
            }
            if ( element.tagName() == "currentFps" )
            {
                // TODO: save fps
                //timer->setInterval(1000/fps);
                //m_pTimeLine->setFps( m_pEditor->fps );
            }
            if ( element.tagName() == "currentView" )
            {
                qreal m11 = element.attribute( "m11" ).toDouble();
                qreal m12 = element.attribute( "m12" ).toDouble();
                qreal m21 = element.attribute( "m21" ).toDouble();
                qreal m22 = element.attribute( "m22" ).toDouble();
                qreal dx = element.attribute( "dx" ).toDouble();
                qreal dy = element.attribute( "dy" ).toDouble();
                m_pScribbleArea->setMyView( QMatrix( m11, m12, m21, m22, dx, dy ) );
            }
        }
        tag = tag.nextSibling();
    }
    return true;
}
*/


// Added here (mainWindow2) to be easily located
// TODO: Find a better place for this function
void MainWindow2::resetToolsSettings()
{
    m_pEditor->tools()->resetAllTools();
}

bool MainWindow2::saveObject( QString strSavedFileName )
{
    QProgressDialog progress( tr( "Saving document..." ), tr( "Abort" ), 0, 100, this );
    progress.setWindowModality( Qt::WindowModal );
    progress.show();

    ObjectSaveLoader* saveLoader = new ObjectSaveLoader( this );
    bool ok = saveLoader->save( m_pEditor->object(), strSavedFileName );

    progress.setValue( 100 );

    //m_pObject->setModified( false );
    m_pTimeLine->updateContent();

    //m_pObject->setFilePath( strSavedFileName );

    m_recentFileMenu->addRecentFile( strSavedFileName );
    m_recentFileMenu->saveToDisk();

    setWindowTitle( strSavedFileName );

    return true;
}

void MainWindow2::saveDocument()
{
    if ( !m_pEditor->object()->filePath().isEmpty() )
    {
        saveObject( m_pEditor->object()->filePath() );
    }
    else
    {
        saveAsNewDocument();
    }
}

bool MainWindow2::maybeSave()
{
    if ( m_pEditor->object()->isModified() )
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

/*
QDomElement MainWindow2::createDomElement( QDomDocument& doc )
{
    QDomElement tag = doc.createElement( "editor" );

    QDomElement tag1 = doc.createElement( "currentLayer" );
    tag1.setAttribute( "value", m_pEditor->layers()->currentLayerIndex() );
    tag.appendChild( tag1 );
    QDomElement tag2 = doc.createElement( "currentFrame" );
    tag2.setAttribute( "value", m_pEditor->currentFrame() );
    tag.appendChild( tag2 );
    QDomElement tag3 = doc.createElement( "currentView" );

    QMatrix myView = m_pScribbleArea->getMyView();
    tag3.setAttribute( "m11", myView.m11() );
    tag3.setAttribute( "m12", myView.m12() );
    tag3.setAttribute( "m21", myView.m21() );
    tag3.setAttribute( "m22", myView.m22() );
    tag3.setAttribute( "dx", myView.dx() );
    tag3.setAttribute( "dy", myView.dy() );
    tag.appendChild( tag3 );

    return tag;
}
*/

void MainWindow2::showPreferences()
{
    m_pPreferences = new Preferences( this );

    connect( m_pPreferences, SIGNAL( lengthSizeChange( QString ) ), m_pTimeLine, SIGNAL( lengthChange( QString ) ) );
    connect( m_pPreferences, SIGNAL( fontSizeChange( int ) ), m_pTimeLine, SIGNAL( fontSizeChange( int ) ) );
    connect( m_pPreferences, SIGNAL( frameSizeChange( int ) ), m_pTimeLine, SIGNAL( frameSizeChange( int ) ) );
    connect( m_pPreferences, SIGNAL( labelChange( int ) ), m_pTimeLine, SIGNAL( labelChange( int ) ) );
    connect( m_pPreferences, SIGNAL( scrubChange( int ) ), m_pTimeLine, SIGNAL( scrubChange( int ) ) );

    connect( m_pPreferences, SIGNAL( windowOpacityChange( int ) ), this, SLOT( setOpacity( int ) ) );
    connect( m_pPreferences, SIGNAL( curveOpacityChange( int ) ), m_pScribbleArea, SLOT( setCurveOpacity( int ) ) );
    connect( m_pPreferences, SIGNAL( curveSmoothingChange( int ) ), m_pScribbleArea, SLOT( setCurveSmoothing( int ) ) );
    connect( m_pPreferences, SIGNAL( highResPositionChange( int ) ), m_pScribbleArea, SLOT( setHighResPosition( int ) ) );
    connect( m_pPreferences, SIGNAL( antialiasingChange( int ) ), m_pScribbleArea, SLOT( setAntialiasing( int ) ) );
    connect( m_pPreferences, SIGNAL( gradientsChange( int ) ), m_pScribbleArea, SLOT( setGradients( int ) ) );
    connect( m_pPreferences, SIGNAL( backgroundChange( int ) ), m_pScribbleArea, SLOT( setBackground( int ) ) );
    connect( m_pPreferences, SIGNAL( shadowsChange( int ) ), m_pScribbleArea, SLOT( setShadows( int ) ) );
    connect( m_pPreferences, SIGNAL( toolCursorsChange( int ) ), m_pScribbleArea, SLOT( setToolCursors( int ) ) );
    connect( m_pPreferences, SIGNAL( styleChanged( int ) ), m_pScribbleArea, SLOT( setStyle( int ) ) );

    connect( m_pPreferences, SIGNAL( autosaveChange( int ) ), m_pEditor, SLOT( changeAutosave( int ) ) );
    connect( m_pPreferences, SIGNAL( autosaveNumberChange( int ) ), m_pEditor, SLOT( changeAutosaveNumber( int ) ) );

    connect( m_pPreferences, SIGNAL( onionLayer1OpacityChange( int ) ), m_pEditor, SLOT( onionLayer1OpacityChangeSlot( int ) ) );
    connect( m_pPreferences, SIGNAL( onionLayer2OpacityChange( int ) ), m_pEditor, SLOT( onionLayer2OpacityChangeSlot( int ) ) );
    connect( m_pPreferences, SIGNAL( onionLayer3OpacityChange( int ) ), m_pEditor, SLOT( onionLayer3OpacityChangeSlot( int ) ) );

    clearKeyboardShortcuts();

    connect( m_pPreferences, SIGNAL( destroyed() ),
        this, SLOT( setupKeyboardShortcuts() ) );

    m_pPreferences->show();
}

void MainWindow2::dockAllPalettes()
{
    m_pToolBox->setFloating(false);
    m_pToolOptionWidget->setFloating(false);
    m_pDisplayOptionWidget->setFloating(false);
    m_pTimeLine->setFloating(false);
    m_pColorPalette->setFloating(false);
    m_pColorWheelWidget->setFloating( false );
}

void MainWindow2::readSettings()
{
    qDebug( "Restore last windows layout." );

    QSettings settings( PENCIL2D, PENCIL2D );
    restoreGeometry( settings.value( SETTING_WINDOW_GEOMETRY ).toByteArray() );
    restoreState( settings.value( SETTING_WINDOW_STATE ).toByteArray() );

    QString myPath = settings.value( "lastFilePath", QVariant( QDir::homePath() ) ).toString();
    m_recentFileMenu->addRecentFile( myPath );

    setOpacity( 100 - settings.value( "windowOpacity" ).toInt() );
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
        QKeySequence keySequence( pencilSettings()->value( strCommandName ).toString() );
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

    // edit manu
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
    ui->actionOnionPrevious->setShortcut( cmdKeySeq( CMD_ONIONSKIN_PREV ) );
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

    ui->actionTogglePalette->setShortcut( cmdKeySeq( CMD_TOGGLE_PALETTE ) );
    m_pScribbleArea->getPopupPalette()->closeButton->setText( tr("close/toggle (") + pencilSettings()->value( QString( "shortcuts/" ) + CMD_TOGGLE_PALETTE ).toString() + ")" );
    m_pScribbleArea->getPopupPalette()->closeButton->setShortcut( cmdKeySeq( CMD_TOGGLE_PALETTE ) );

    ui->actionNew_Bitmap_Layer->setShortcut( cmdKeySeq( CMD_NEW_BITMAP_LAYER ) );
    ui->actionNew_Vector_Layer->setShortcut( cmdKeySeq( CMD_NEW_VECTOR_LAYER ) );
    ui->actionNew_Camera_Layer->setShortcut( cmdKeySeq( CMD_NEW_CAMERA_LAYER ) );
    ui->actionNew_Sound_Layer->setShortcut( cmdKeySeq( CMD_NEW_SOUND_LAYER ) );

    m_pToolBox->toggleViewAction()->setShortcut( cmdKeySeq( CMD_TOGGLE_TOOLBOX ) );
    m_pToolOptionWidget->toggleViewAction()->setShortcut( cmdKeySeq( CMD_TOGGLE_TOOL_OPTIONS ) );
    m_pColorWheelWidget->toggleViewAction()->setShortcut( cmdKeySeq( CMD_TOGGLE_COLOR_WHEEL ) );
    m_pColorPalette->toggleViewAction()->setShortcut( cmdKeySeq( CMD_TOGGLE_COLOR_LIBRARY ) );
    m_pTimeLine->toggleViewAction()->setShortcut( cmdKeySeq( CMD_TOGGLE_TIMELINE ) );
    m_pDisplayOptionWidget->toggleViewAction()->setShortcut( cmdKeySeq( CMD_TOGGLE_DISPLAY_OPTIONS ) );

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
    if ( this->m_pEditor->mBackupIndex < 0 )
    {
        ui->actionUndo->setText( tr("Undo") );
        ui->actionUndo->setEnabled( false );
    }
    else
    {
        ui->actionUndo->setText( tr("Undo   ") + QString::number( this->m_pEditor->mBackupIndex + 1 ) + " " + this->m_pEditor->mBackupList.at( this->m_pEditor->mBackupIndex )->undoText );
        ui->actionUndo->setEnabled( true );
    }

    if ( this->m_pEditor->mBackupIndex + 2 < this->m_pEditor->mBackupList.size() )
    {
        ui->actionRedo->setText( tr("Redo   ") + QString::number( this->m_pEditor->mBackupIndex + 2 ) + " " + this->m_pEditor->mBackupList.at( this->m_pEditor->mBackupIndex + 1 )->undoText );
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
        m_pEditor->object()->exportPalette( filePath );
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
        m_pEditor->object()->importPalette( filePath );
        m_pColorPalette->refreshColorList();
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
        QMessageBox::about( this, tr( PENCIL_WINDOW_TITLE ), strAboutText );
    }
}

void MainWindow2::helpBox()
{
    qDebug() << "Open help manual.";

    QString url = "http://www.pencil2d.org/documentation/";
    QDesktopServices::openUrl( QUrl(url) );
}

void MainWindow2::makeConnections( Editor* pEditor, ScribbleArea* pScribbleArea )
{
    connect( pEditor->tools(), &ToolManager::toolChanged, pScribbleArea, &ScribbleArea::setCurrentTool );
    connect( pEditor->tools(), &ToolManager::toolPropertyChanged, pScribbleArea, &ScribbleArea::updateToolCursor );

    connect( pEditor, &Editor::currentFrameChanged, pScribbleArea, &ScribbleArea::updateFrame );

    connect( pEditor, &Editor::toggleOnionPrev, pScribbleArea, &ScribbleArea::toggleOnionPrev );
    connect( pEditor, &Editor::toggleOnionNext, pScribbleArea, &ScribbleArea::toggleOnionNext );
    connect( pEditor, &Editor::toggleMultiLayerOnionSkin, pScribbleArea, &ScribbleArea::toggleMultiLayerOnionSkin );

    connect( pScribbleArea, &ScribbleArea::thinLinesChanged, pEditor, &Editor::changeThinLinesButton );
    connect( pScribbleArea, &ScribbleArea::outlinesChanged, pEditor, &Editor::changeOutlinesButton );
    connect( pScribbleArea, &ScribbleArea::onionPrevChanged, pEditor, &Editor::onionPrevChanged );
    connect( pScribbleArea, &ScribbleArea::onionNextChanged, pEditor, &Editor::onionNextChanged );
    //connect( pScribbleArea, &ScribbleArea::multiLayerOnionSkin, this, &Editor::multiLayerOnionSkin );

    connect( pEditor, &Editor::selectAll, pScribbleArea, &ScribbleArea::selectAll );
}

void MainWindow2::makeConnections( Editor* pEditor, TimeLine* pTimeline )
{
    PlaybackManager* pPlaybackManager = pEditor->playback();
    LayerManager* pLayerManager = pEditor->layers();

    connect( pTimeline, &TimeLine::endplayClick, [ = ]{ pLayerManager->gotoLastKeyFrame(); } );
    connect( pTimeline, &TimeLine::startplayClick, [ = ]{ pLayerManager->gotoFirstKeyFrame(); } );
    connect( pTimeline, &TimeLine::duplicateKeyClick, pEditor, &Editor::duplicateKey );

    connect( pTimeline, &TimeLine::playClick, [ = ]{ pPlaybackManager->play(); } );
    connect( pTimeline, &TimeLine::loopClick, pPlaybackManager, &PlaybackManager::setLoop );

    connect( pTimeline, &TimeLine::loopControlClick, pPlaybackManager, &PlaybackManager::enableRangedPlayback );
    connect( pTimeline, &TimeLine::loopStartClick, pPlaybackManager, &PlaybackManager::setRangedStartFrame );
    connect( pTimeline, &TimeLine::loopEndClick, pPlaybackManager, &PlaybackManager::setRangedEndFrame );

    connect( pTimeline, &TimeLine::soundClick, pPlaybackManager, &PlaybackManager::enbaleSound );
    connect( pTimeline, &TimeLine::fpsClick, pPlaybackManager, &PlaybackManager::setFps );


    connect( pTimeline, &TimeLine::addKeyClick, pEditor, &Editor::addNewKey );
    connect( pTimeline, &TimeLine::removeKeyClick, pEditor, &Editor::removeKey );

    connect( pTimeline, &TimeLine::newBitmapLayer, pEditor, &Editor::newBitmapLayer );
    connect( pTimeline, &TimeLine::newVectorLayer, pEditor, &Editor::newVectorLayer );
    connect( pTimeline, &TimeLine::newSoundLayer, pEditor, &Editor::newSoundLayer );
    connect( pTimeline, &TimeLine::newCameraLayer, pEditor, &Editor::newCameraLayer );
    connect( pTimeline, &TimeLine::deleteCurrentLayer, pEditor, &Editor::deleteCurrentLayer );

    connect( pEditor, SIGNAL( toggleLoopControl( bool ) ), pTimeline, SIGNAL( toggleLoopControl( bool ) ) );
    connect( pTimeline, SIGNAL( loopControlClick( bool ) ), pEditor, SIGNAL( loopControlToggled( bool ) ) );
}

void MainWindow2::makeConnections(Editor* pEditor, DisplayOptionWidget* pDisplay)
{
}

void MainWindow2::makeConnections( Editor* pEditor, ColorPaletteWidget* pColorPalette )
{
    connect( pEditor, &Editor::fileLoaded, pColorPalette, &ColorPaletteWidget::updateUI );

    ColorManager* pColorManager = pEditor->color();
    connect( pColorPalette, &ColorPaletteWidget::colorChanged, pColorManager, &ColorManager::setColor );
    connect( pColorPalette, &ColorPaletteWidget::colorNumberChanged, pColorManager, &ColorManager::setColorNumber );

    connect( pColorManager, &ColorManager::colorChanged, pColorPalette, &ColorPaletteWidget::setColor );
    connect( pColorManager, &ColorManager::colorNumberChanged, pColorPalette, &ColorPaletteWidget::selectColorNumber );
}
