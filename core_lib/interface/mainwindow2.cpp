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

#include "pencildef.h"
#include "pencilsettings.h"

#include "object.h"
#include "layer.h"
#include "layersound.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "objectsaveloader.h"

#include "editor.h"
#include "colormanager.h"
#include "layermanager.h"
#include "toolmanager.h"

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

    m_pObject = new Object();
    m_pObject->init();

    m_pEditor = new Editor( this );
    m_pEditor->initialize( m_pScribbleArea );
    m_pEditor->setObject( m_pObject );
    
    m_pScribbleArea->setCore( m_pEditor );
    m_pEditor->setScribbleArea( m_pScribbleArea );
    makeConnections( m_pEditor, m_pScribbleArea );

    createDockWidgets();
    createMenus();
    loadAllShortcuts();

    m_pEditor->resetUI();

    readSettings();

    makeColorWheelConnections();

    connect(m_pEditor, &Editor::needSave, this, &MainWindow2::saveDocument);
    connect(m_pToolBox, &ToolBoxWidget::clearButtonClicked, m_pEditor, &Editor::clearCurrentFrame);
    connect(m_pEditor, SIGNAL(changeTool(ToolType)), m_pToolBox, SLOT(setCurrentTool(ToolType)));

    m_pEditor->setCurrentLayer( m_pEditor->object()->getLayerCount() - 1 );
}

MainWindow2::~MainWindow2()
{
    delete ui;
}

void MainWindow2::makeTimeLineConnections()
{
    connect( m_pTimeLine, &TimeLine::endplayClick, m_pEditor, &Editor::endPlay );
    connect( m_pTimeLine, &TimeLine::startplayClick, m_pEditor, &Editor::startPlay );
    connect( m_pTimeLine, &TimeLine::duplicateKeyClick, m_pEditor, &Editor::duplicateKey );

    connect( m_pTimeLine, &TimeLine::modification, m_pEditor, &Editor::currentKeyFrameModification );
    connect( m_pTimeLine, &TimeLine::addKeyClick, m_pEditor, &Editor::addNewKey );
    connect( m_pTimeLine, &TimeLine::removeKeyClick, m_pEditor, &Editor::removeKey );

    connect( m_pTimeLine, &TimeLine::newBitmapLayer, m_pEditor, &Editor::newBitmapLayer );
    connect( m_pTimeLine, &TimeLine::newVectorLayer, m_pEditor, &Editor::newVectorLayer );
    connect( m_pTimeLine, &TimeLine::newSoundLayer, m_pEditor, &Editor::newSoundLayer );
    connect( m_pTimeLine, &TimeLine::newCameraLayer, m_pEditor, &Editor::newCameraLayer );
    connect( m_pTimeLine, &TimeLine::deleteCurrentLayer, m_pEditor, &Editor::deleteCurrentLayer );

    connect( m_pTimeLine, &TimeLine::playClick, m_pEditor, &Editor::play );
    connect( m_pTimeLine, &TimeLine::loopClick, m_pEditor, &Editor::setLoop );

    connect( m_pTimeLine, &TimeLine::loopControlClick, m_pEditor, &Editor::setLoopControl ); // adding LoopControlClick needs setLoopControl(bool)
    connect( m_pTimeLine, &TimeLine::loopStartClick, m_pEditor, &Editor::changeLoopStart );
    connect( m_pTimeLine, &TimeLine::loopEndClick, m_pEditor, &Editor::changeLoopEnd );


    connect( m_pTimeLine, &TimeLine::soundClick, m_pEditor, &Editor::setSound );
    connect( m_pTimeLine, &TimeLine::fpsClick, m_pEditor, &Editor::changeFps );

    connect( m_pEditor, &Editor::toggleLoop, m_pTimeLine, &TimeLine::loopToggled );
    connect( m_pTimeLine, &TimeLine::loopClick, m_pEditor, &Editor::loopToggled );

    connect( m_pEditor, SIGNAL( toggleLoopControl( bool ) ), m_pTimeLine, SIGNAL( toggleLoopControl( bool ) ) );
    connect(m_pTimeLine, SIGNAL(loopControlClick(bool)), m_pEditor, SIGNAL(loopControlToggled(bool)));
    
    //adding loopControlClick needs loopControlToggled(bool)

    m_pTimeLine->setFocusPolicy(Qt::NoFocus);
}

void MainWindow2::createDockWidgets()
{
    m_pTimeLine = new TimeLine( this );
    m_pTimeLine->setObjectName( "TimeLine" );
    m_subWidgets.append( m_pTimeLine );

    m_pColorWheelWidget = new QDockWidget( tr("Color Wheel"), this );
    m_pColorWheelWidget->setFocusPolicy( Qt::NoFocus );

    ColorBox* pColorBox = new ColorBox(this);
    pColorBox->setToolTip(tr("color palette:<br>use <b>(C)</b><br>toggle at cursor"));
    m_pColorWheelWidget->setWidget( pColorBox );
    m_pColorWheelWidget->setObjectName( "ColorWheel" );

    m_pColorPalette = new ColorPaletteWidget( tr( "Color Palette" ), this );
    m_pColorPalette->setObjectName( "ColorPalette" );
    m_pColorPalette->setFocusPolicy( Qt::NoFocus );
    makeConnections( m_pEditor, m_pColorPalette );
    m_subWidgets.append( m_pColorPalette );

    m_pDisplayOptionWidget = new DisplayOptionDockWidget(this);
    m_pDisplayOptionWidget->setObjectName( "DisplayOption" );
    m_pDisplayOptionWidget->makeConnectionToEditor(m_pEditor);

    m_pToolOptionWidget = new ToolOptionWidget(this);
    m_pToolOptionWidget->setObjectName( "ToolOption" );
    m_pToolOptionWidget->makeConnectionToEditor(m_pEditor);

    m_pToolBox = new ToolBoxWidget( tr( "Tools" ), this );
    m_pToolBox->setObjectName( "ToolBox" );

    addDockWidget(Qt::RightDockWidgetArea,  m_pColorWheelWidget);
    addDockWidget(Qt::RightDockWidgetArea,  m_pColorPalette);
    addDockWidget(Qt::RightDockWidgetArea,  m_pDisplayOptionWidget);
    addDockWidget(Qt::LeftDockWidgetArea,   m_pToolBox);
    addDockWidget(Qt::LeftDockWidgetArea,   m_pToolOptionWidget);
    addDockWidget(Qt::BottomDockWidgetArea, m_pTimeLine);

    makeTimeLineConnections();
    for ( BaseDockWidget* pWidget : m_subWidgets )
    {
        pWidget->setCore( m_pEditor );
        pWidget->initUI();
        pWidget->setFeatures( QDockWidget::AllDockWidgetFeatures );
    }

    m_pToolBox->setFeatures( QDockWidget::AllDockWidgetFeatures );
    m_pToolOptionWidget->setFeatures( QDockWidget::AllDockWidgetFeatures );
    m_pDisplayOptionWidget->setFeatures( QDockWidget::AllDockWidgetFeatures );
    m_pColorPalette->setFeatures( QDockWidget::AllDockWidgetFeatures );
    m_pColorWheelWidget->setFeatures( QDockWidget::AllDockWidgetFeatures );
}

void MainWindow2::makeColorWheelConnections()
{
    ColorBox* pColorBox = static_cast<ColorBox*>(m_pColorWheelWidget->widget());
    Q_ASSERT( pColorBox );

    connect( pColorBox, &ColorBox::colorChanged, m_pEditor->colorManager(), &ColorManager::setColor );
    connect( m_pEditor->colorManager(), &ColorManager::colorChanged, pColorBox, &ColorBox::setColor );
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
    connect( ui->actionZoom_In, &QAction::triggered, m_pEditor, &Editor::setzoom );
    connect( ui->actionZoom_Out, &QAction::triggered, m_pEditor, &Editor::setzoom1 );
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
    connect( ui->actionPlay, &QAction::triggered, m_pEditor, &Editor::play );
    connect( ui->actionLoop, &QAction::triggered, m_pEditor, &Editor::setLoop );
    connect( ui->actionLoop, &QAction::toggled, m_pEditor, &Editor::toggleLoop ); //TODO: WTF?
    connect( m_pEditor, &Editor::loopToggled, ui->actionLoop, &QAction::setChecked );

    // Loop Control
    connect(ui->actionLoopControl, &QAction::triggered, m_pEditor, &Editor::setLoopControl );
    connect( ui->actionLoopControl, &QAction::toggled, m_pEditor, &Editor::toggleLoopControl );
    //connect(editor, &Editor::loopControlToggled, ui->actionLoopControl, &QAction::setChecked );

    connect(ui->actionAdd_Frame, &QAction::triggered, m_pEditor, &Editor::addNewKey );
    connect(ui->actionRemove_Frame, &QAction::triggered, m_pEditor, &Editor::removeKey );
    connect(ui->actionNext_Frame, &QAction::triggered, m_pEditor, &Editor::playNextFrame );
    connect(ui->actionPrevious_Frame, &QAction::triggered, m_pEditor, &Editor::playPrevFrame );
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

void MainWindow2::tabletEvent( QTabletEvent *event )
{
    event->ignore();
}

// ==== SLOT ====

void MainWindow2::newDocument()
{
    if ( maybeSave() )
    {
        //
        m_pObject->deleteLater();
        // default size

        m_pObject = new Object();
        m_pObject->init();

        m_pEditor->setObject( m_pObject );
        m_pEditor->resetUI();

        setWindowTitle( PENCIL_WINDOW_TITLE );
    }
}

void MainWindow2::openDocument()
{
    if ( maybeSave() )
    {
        QSettings settings( "Pencil", "Pencil" );

        QString myPath = settings.value( "lastFilePath", QVariant( QDir::homePath() ) ).toString();
        QString fileName = QFileDialog::getOpenFileName(
            this,
            tr( "Open File..." ),
            myPath,
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
        else
        {
            m_pEditor->updateMaxFrame();
        }
    }
}

bool MainWindow2::saveAsNewDocument()
{
    QSettings settings( "Pencil", "Pencil" );

    QString strDefaultFileName = settings.value( "lastFilePath", QVariant( QDir::homePath() ) ).toString();

    if ( strDefaultFileName.isEmpty() )
    {
        strDefaultFileName = QDir::homePath() + "/" + PFF_DEFAULT_FILENAME;
    }

    QString fileName = QFileDialog::getSaveFileName( this, tr( "Save As..." ), strDefaultFileName, tr( PFF_CLOSE_ALL_FILE_FILTER ) );

    if ( fileName.isEmpty() )
    {
        return false;
    }
    else
    {
        if ( !fileName.endsWith( PFF_OLD_EXTENSION ) && !fileName.endsWith( PFF_EXTENSION ) )
        {
            fileName = fileName + PFF_EXTENSION;
        }
        QSettings settings( "Pencil", "Pencil" );
        settings.setValue( "lastFilePath", QVariant( fileName ) );

        return saveObject( fileName );
    }
}

void MainWindow2::openFile( QString filename )
{
    qDebug() << "open recent file" << filename;
    bool ok = openObject( filename );
    if ( !ok )
    {
        QMessageBox::warning( this, tr("Warning"), tr("Pencil cannot read this file. If you want to import images, use the command import.") );
        Object* pObject = new Object();
        pObject->init();

        m_pEditor->setObject( pObject );
        m_pEditor->resetUI();
    }
    else
    {
        m_pEditor->updateMaxFrame();
    }
}

bool MainWindow2::openObject( QString strFilePath )
{
    QProgressDialog progress( tr("Opening document..."), tr("Abort"), 0, 100, this );
    progress.setWindowModality( Qt::WindowModal );
    progress.show();

    m_pEditor->setCurrentLayer( 0 );
    m_pEditor->layerManager()->setCurrentKeyFrame( 1 );
    m_pEditor->fps = 12;
    m_pTimeLine->setFps( m_pEditor->fps );
    m_pScribbleArea->setMyView( QMatrix() );

    ObjectSaveLoader objectLoader( this );
    Object* pObject = objectLoader.loadFromFile( strFilePath );

    if ( pObject != NULL && objectLoader.error().code() == PCL_OK )
    {
        SafeDelete( m_pObject );
        m_pObject = pObject;

        pObject->setFilePath( strFilePath );
        QSettings settings( "Pencil", "Pencil" );
        settings.setValue( "lastFilePath", QVariant( pObject->filePath() ) );

        m_pEditor->setObject( pObject );
        m_pEditor->updateObject();

        m_recentFileMenu->addRecentFile( pObject->filePath() );
        m_recentFileMenu->saveToDisk();

        qDebug() << "Current File Path=" << pObject->filePath();
        setWindowTitle( pObject->filePath() );
    }
    else
    {
        return false;
    }
    return true;

    //-------------------
    QString filePath = strFilePath;

    bool openingTheOLDWAY = true;
    QString realXMLFilePath = filePath;
    QString tmpFilePath;

    // ---- test before opening ----
    QStringList zippedFileList = JlCompress::getFileList( filePath );
    if ( !zippedFileList.empty() )
    {
        qDebug() << "Recognized New zipped Pencil File Format !";
        openingTheOLDWAY = false;

        // ---- now decompress PFF -----
        QFileInfo fileInfo( filePath );
        QDir dir( QDir::tempPath() );
        tmpFilePath = QDir::tempPath() + "/" + fileInfo.completeBaseName() + PFF_TMP_DECOMPRESS_EXT;
        if ( fileInfo.exists() ) {
            dir.rmpath( tmpFilePath ); // --removes an old decompression directory
            removePFFTmpDirectory( tmpFilePath ); // --removes an old decompression directory - better approach
        }
        dir.mkpath( tmpFilePath ); // --creates a new decompression directory

        JlCompress::extractDir( filePath, tmpFilePath );

        realXMLFilePath = tmpFilePath + "/" + PFF_XML_FILE_NAME;
    }
    else
    {
        qDebug() << "Recognized Old Pencil File Format !";
    }

    QScopedPointer<QFile> file( new QFile( realXMLFilePath ) );

    //QFile* file = new QFile(filePath);
    if ( !file->open( QFile::ReadOnly ) )
    {
        if ( !openingTheOLDWAY )
        {
            removePFFTmpDirectory( tmpFilePath ); // --removes temporary decompression directory
        }
        return false;
    }

    QDomDocument doc;
    if ( !doc.setContent( file.data() ) )
    {
        if ( !openingTheOLDWAY )
        {
            removePFFTmpDirectory( tmpFilePath ); // --removes temporary decompression directory
        }
        return false; // this is not a XML file
    }
    QDomDocumentType type = doc.doctype();
    if ( type.name() != "PencilDocument" && type.name() != "MyObject" )
    {
        if ( !openingTheOLDWAY )
        {
            removePFFTmpDirectory( tmpFilePath ); // --removes temporary decompression directory
        }
        return false; // this is not a Pencil document
    }

    // delete old object @sent foreward -> if (ok)
    /*if (m_object != NULL)
    {
    m_object->deleteLater();
    }*/

    // -----------------------------


    //QSettings settings("Pencil","Pencil");
    //settings.setValue("lastFilePath", QVariant(object->strCurrentFilePath) );

    QString dataLayersDir;
    if ( openingTheOLDWAY )
    {
        dataLayersDir = filePath + "." + PFF_LAYERS_DIR;
    }
    else
    {
        dataLayersDir = tmpFilePath + "/" + PFF_LAYERS_DIR;
    }

    Object* newObject = new Object();
    if ( !newObject->loadPalette( dataLayersDir ) )
    {
        newObject->loadDefaultPalette();
    }
    m_pEditor->setObject( newObject );

    newObject->setFilePath( filePath );

    // ------- reads the XML file -------
    bool ok = true;
    int progVal = 0;
    QDomElement docElem = doc.documentElement();
    if ( docElem.isNull() )
    {
        return false;
    }

    if ( docElem.tagName() == "document" )
    {
        qDebug( "Object Loader: start." );

        qreal rProgressValue = 0;
        qreal rProgressDelta = 100 / docElem.childNodes().count();

        QDomNode tag = docElem.firstChild();

        while ( !tag.isNull() )
        {
            QDomElement element = tag.toElement(); // try to convert the node to an element.
            if ( !element.isNull() )
            {
                progVal = qMin( (int)rProgressValue, 100 );
                progress.setValue( progVal );
                rProgressValue += rProgressDelta;

                if ( element.tagName() == "editor" )
                {
                    qDebug( "  Load editor" );
                    loadDomElement( element, filePath );
                }
                else if ( element.tagName() == "object" )
                {
                    qDebug( "  Load object" );
                    ok = newObject->loadDomElement( element, dataLayersDir );
                    qDebug() << "    dataDir:" << dataLayersDir;
                }
            }
            tag = tag.nextSibling();
        }
    }
    else
    {
        if ( docElem.tagName() == "object" || docElem.tagName() == "MyOject" )   // old Pencil format (<=0.4.3)
        {
            ok = newObject->loadDomElement( docElem, filePath );
        }
    }

    // ------------------------------
    if ( ok )
    {
        m_pEditor->updateObject();

        if ( !openingTheOLDWAY )
        {
            removePFFTmpDirectory( tmpFilePath ); // --removes temporary decompression directory
        }

        m_recentFileMenu->addRecentFile( filePath );
        m_recentFileMenu->saveToDisk();

        //qDebug() << "Current File Path=" << newObject->strCurrentFilePath;
        setWindowTitle( newObject->filePath() );

        // FIXME: need to free the old object. but delete object will crash app, don't know why.
        // fixed by shoshon... don't know if it's right
        Object* objectToDelete = m_pObject;
        m_pObject = newObject;
        if ( objectToDelete != NULL )
        {
            delete objectToDelete;
        }
    }

    progress.setValue( 100 );
    return true;
}

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
                m_pEditor->layerManager()->setCurrentKeyFrame( element.attribute( "value" ).toInt() );
            }
            if ( element.tagName() == "currentFps" )
            {
                m_pEditor->fps = element.attribute( "value" ).toInt();
                //timer->setInterval(1000/fps);
                m_pTimeLine->setFps( m_pEditor->fps );
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

// Added here (mainWindow2) to be easily located
// TODO: Find a better place for this function
void MainWindow2::resetToolsSettings()
{
    m_pEditor->toolManager()->resetAllTools();
}


bool MainWindow2::saveObject( QString strSavedFilename )
{
    QString filePath = strSavedFilename;

    bool savingTheOLDWAY = filePath.endsWith( PFF_OLD_EXTENSION );

    QFileInfo fileInfo( filePath );
    if ( fileInfo.isDir() ) return false;

    QString tmpFilePath;
    if ( !savingTheOLDWAY )
    {// create temporary directory for compressing files
        tmpFilePath = QDir::tempPath() + "/" + fileInfo.completeBaseName() + PFF_TMP_COMPRESS_EXT;
        QFileInfo tmpDataInfo( tmpFilePath );
        if ( !tmpDataInfo.exists() )
        {
            QDir dir( QDir::tempPath() ); // --the directory where filePath is or will be saved
            dir.mkpath( tmpFilePath ); // --creates a directory with the same name +".data"
        }
    }
    else
    {
        tmpFilePath = fileInfo.absolutePath();
    }


    QString dataLayersDir;
    if ( savingTheOLDWAY )
    {
        dataLayersDir = filePath + "." + PFF_LAYERS_DIR;
    }
    else
    {
        dataLayersDir = tmpFilePath + "/" + PFF_LAYERS_DIR;
    }
    QFileInfo dataInfo( dataLayersDir );
    if ( !dataInfo.exists() )
    {
        QDir dir( tmpFilePath ); // the directory where filePath is or will be saved
        dir.mkpath( dataLayersDir ); // creates a directory with the same name +".data"
    }

    //savedName = filePath;
    this->setWindowTitle( filePath );

    QProgressDialog progress( tr("Saving document..."), tr("Abort"), 0, 100, this );
    progress.setWindowModality( Qt::WindowModal );
    progress.show();
    int progressValue = 0;

    // save data
    int nLayers = m_pObject->getLayerCount();
    qDebug( "Layer Count=%d", nLayers );

    for ( int i = 0; i < nLayers; i++ )
    {
        Layer* layer = m_pObject->getLayer( i );
        qDebug() << "Saving Layer " << i << "(" << layer->name << ")";

        progressValue = (i * 100) / nLayers;
        progress.setValue( progressValue );
        switch ( layer->type() )
        {
        case Layer::BITMAP:
        case Layer::VECTOR:
        case Layer::SOUND:
            auto pLayerImg = static_cast<LayerImage*>( layer );
            pLayerImg->saveImages( dataLayersDir, i );
            pLayerImg->saveImages( dataLayersDir, i );
            pLayerImg->saveImages( dataLayersDir, i );
            break;
        }
    }

    // save palette
    m_pObject->savePalette( dataLayersDir );

    // -------- save main XML file -----------
    QString mainXMLfile;
    if ( !savingTheOLDWAY )
    {
        mainXMLfile = tmpFilePath + "/" + PFF_XML_FILE_NAME;
    }
    else
    {
        mainXMLfile = filePath;
    }
    QFile* file = new QFile( mainXMLfile );
    if ( !file->open( QFile::WriteOnly | QFile::Text ) )
    {
        //QMessageBox::warning(this, "Warning", "Cannot write file");
        return false;
    }
    QTextStream out( file );
    QDomDocument doc( "PencilDocument" );
    QDomElement root = doc.createElement( "document" );
    doc.appendChild( root );

    // save editor information
    QDomElement editorElement = createDomElement( doc );
    root.appendChild( editorElement );
    qDebug( "Save Editor Node." );

    // save object
    QDomElement objectElement = m_pObject->createDomElement( doc );
    root.appendChild( objectElement );
    qDebug( "Save Object Node." );

    int IndentSize = 2;
    doc.save( out, IndentSize );
    // -----------------------------------

    if ( !savingTheOLDWAY )
    {
        qDebug() << "Now compressing data to PFF - PCLX ...";

        JlCompress::compressDir( filePath, tmpFilePath );
        removePFFTmpDirectory( tmpFilePath ); // --removing temporary files

        qDebug() << "Compressed. File saved.";
    }

    progress.setValue( 100 );

    m_pObject->modified = false;
    m_pTimeLine->updateContent();

    m_pObject->setFilePath( strSavedFilename );

    m_recentFileMenu->addRecentFile( strSavedFilename );
    m_recentFileMenu->saveToDisk();

    return true;
}

void MainWindow2::saveDocument()
{
    if ( !m_pObject->filePath().isEmpty() )
    {
        saveObject( m_pObject->filePath() );
    }
    else
    {
        saveAsNewDocument();
    }
}

bool MainWindow2::maybeSave()
{
    if ( m_pObject->modified )
    {
        int ret = QMessageBox::warning( this, tr( "Warning" ),
            tr( "This animation has been modified.\n"
            "Do you want to save your changes?" ),
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

QDomElement MainWindow2::createDomElement( QDomDocument& doc )
{
    QDomElement tag = doc.createElement( "editor" );

    QDomElement tag1 = doc.createElement( "currentLayer" );
    tag1.setAttribute( "value", m_pEditor->layerManager()->currentLayerIndex() );
    tag.appendChild( tag1 );
    QDomElement tag2 = doc.createElement( "currentFrame" );
    tag2.setAttribute( "value", m_pEditor->layerManager()->currentFramePosition() );
    tag.appendChild( tag2 );
    QDomElement tag2a = doc.createElement( "currentFps" );
    tag2a.setAttribute( "value", m_pEditor->fps );
    tag.appendChild( tag2a );
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

    unloadAllShortcuts();

    connect( m_pPreferences, SIGNAL( destroyed() ),
        this, SLOT( loadAllShortcuts() ) );

    m_pPreferences->show();
}

void MainWindow2::dockAllPalettes()
{
    m_pToolBox->setFloating(false);
    m_pToolOptionWidget->setFloating(false);
    m_pDisplayOptionWidget->setFloating(false);
    m_pTimeLine->setFloating(false);
    m_pColorPalette->setFloating(false);
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

    return;

    settings.setValue( "editorPosition", pos() );
    settings.setValue( "editorSize", size() );

    ColorPaletteWidget* colourPalette = m_pColorPalette;
    if ( colourPalette != NULL )
    {
        settings.setValue( "colourPalettePosition", colourPalette->pos() );
        settings.setValue( "colourPaletteSize", colourPalette->size() );
        settings.setValue( "colourPaletteFloating", colourPalette->isFloating() );
    }

    TimeLine* timelinePalette = m_pTimeLine;
    if ( timelinePalette != NULL )
    {
        settings.setValue( "timelinePalettePosition", timelinePalette->pos() );
        settings.setValue( "timelinePaletteSize", timelinePalette->size() );
        settings.setValue( "timelinePaletteFloating", timelinePalette->isFloating() );
    }

    QDockWidget* toolWidget = m_pToolBox;
    if ( toolWidget != NULL )
    {
        settings.setValue( "drawPalettePosition", toolWidget->pos() );
        settings.setValue( "drawPaletteSize", toolWidget->size() );
        settings.setValue( "drawPaletteFloating", toolWidget->isFloating() );
    }

    QDockWidget* optionPalette = m_pToolOptionWidget;
    if ( optionPalette != NULL )
    {
        settings.setValue( "optionPalettePosition", optionPalette->pos() );
        settings.setValue( "optionPaletteSize", optionPalette->size() );
        settings.setValue( "optionPaletteFloating", optionPalette->isFloating() );
    }

    QDockWidget* displayPalette = m_pDisplayOptionWidget;
    if ( displayPalette != NULL )
    {
        settings.setValue( "displayPalettePosition", displayPalette->pos() );
        settings.setValue( "displayPaletteSize", displayPalette->size() );
        settings.setValue( "displayPaletteFloating", displayPalette->isFloating() );
    }
}

QKeySequence cmdKeySeq( QString strCommandName )
{
    strCommandName = QString( "shortcuts/" ) + strCommandName;
    QKeySequence keySequence( pencilSettings()->value( strCommandName ).toString() );

    return keySequence;
}

void MainWindow2::loadAllShortcuts()
{
    checkExistingShortcuts();

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

    ui->actionHelp->setShortcut( cmdKeySeq( CMD_HELP ) );
}

void MainWindow2::unloadAllShortcuts()
{
    QList<QAction*> actionList = this->findChildren<QAction*>();
    foreach( QAction* action, actionList )
    {
        action->setShortcut( QKeySequence( 0 ) );
    }
}

void MainWindow2::undoActSetText( void )
{
    if ( this->m_pEditor->backupIndex < 0 )
    {
        ui->actionUndo->setText( tr("Undo") );
        ui->actionUndo->setEnabled( false );
    }
    else
    {
        ui->actionUndo->setText( tr("Undo   ") + QString::number( this->m_pEditor->backupIndex + 1 ) + " " + this->m_pEditor->backupList.at( this->m_pEditor->backupIndex )->undoText );
        ui->actionUndo->setEnabled( true );
    }

    if ( this->m_pEditor->backupIndex + 2 < this->m_pEditor->backupList.size() )
    {
        ui->actionRedo->setText( tr("Redo   ") + QString::number( this->m_pEditor->backupIndex + 2 ) + " " + this->m_pEditor->backupList.at( this->m_pEditor->backupIndex + 1 )->undoText );
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
    QString initialPath = settings.value( "lastPalettePath",
        QVariant( QDir::homePath() ) ).toString();
    if ( initialPath.isEmpty() )
    {
        initialPath = QDir::homePath() + "/untitled.xml";
    }
    QString filePath = QFileDialog::getSaveFileName( this, tr( "Export As" ), initialPath );
    if ( !filePath.isEmpty() )
    {
        m_pObject->exportPalette( filePath );
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
        m_pObject->importPalette( filePath );
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

    QUrl url = QUrl::fromLocalFile( QDir::currentPath() + "/Help/User Manual.pdf" );
    QDesktopServices::openUrl( url );
}

void MainWindow2::makeConnections( Editor* pCore, ScribbleArea* pScribbleArea )
{
    connect( pCore->toolManager(), &ToolManager::toolChanged, pScribbleArea, &ScribbleArea::setCurrentTool );
    connect( pCore->toolManager(), &ToolManager::toolPropertyChanged, pScribbleArea, &ScribbleArea::updateToolCursor );

    connect( pCore, &Editor::toggleOnionPrev, pScribbleArea, &ScribbleArea::toggleOnionPrev );
    connect( pCore, &Editor::toggleOnionNext, pScribbleArea, &ScribbleArea::toggleOnionNext );
    connect( pCore, &Editor::toggleMultiLayerOnionSkin, pScribbleArea, &ScribbleArea::toggleMultiLayerOnionSkin );

    connect( pScribbleArea, &ScribbleArea::thinLinesChanged, pCore, &Editor::changeThinLinesButton );
    connect( pScribbleArea, &ScribbleArea::outlinesChanged, pCore, &Editor::changeOutlinesButton );
    connect( pScribbleArea, &ScribbleArea::onionPrevChanged, pCore, &Editor::onionPrevChanged );
    connect( pScribbleArea, &ScribbleArea::onionNextChanged, pCore, &Editor::onionNextChanged );
    //connect( pScribbleArea, &ScribbleArea::multiLayerOnionSkin, this, &Editor::multiLayerOnionSkin );

    connect( pCore, &Editor::selectAll, pScribbleArea, &ScribbleArea::selectAll );
}

void MainWindow2::makeConnections( Editor* pEditor, ColorPaletteWidget* pColorPalette )
{
    connect( pEditor, &Editor::fileLoaded, pColorPalette, &ColorPaletteWidget::updateUI );

    ColorManager* pColorManager = pEditor->colorManager();
    connect( pColorPalette, &ColorPaletteWidget::colorChanged, pColorManager, &ColorManager::setColor );
    connect( pColorPalette, &ColorPaletteWidget::colorNumberChanged, pColorManager, &ColorManager::setColorNumber );

    connect( pColorManager, &ColorManager::colorChanged, pColorPalette, &ColorPaletteWidget::setColor );
    connect( pColorManager, &ColorManager::colorNumberChanged, pColorPalette, &ColorPaletteWidget::selectColorNumber );
}
