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

#include "colorbox.h"
#include "util.h"

#include "fileformat.h"		//contains constants used by Pencil File Format
#include "JlCompress.h"		//compress and decompress New Pencil File Format
#include "recentfilemenu.h"

#include "mainwindow2.h"
#include "ui_mainwindow2.h"




MainWindow2::MainWindow2( QWidget *parent ) :
QMainWindow( parent ),
ui( new Ui::MainWindow2 )
{
    ui->setupUi( this );
    
    m_object = new Object();
    m_object->defaultInitialisation();

    editor = new Editor( this );
    m_pScribbleArea = editor->getScribbleArea();
    m_pTimeLine = new TimeLine( this, editor );
    makeTimeLineConnections();

    arrangePalettes();
    createMenus();
    loadAllShortcuts();

    // must run after 'arragePalettes'
    editor->setObject( m_object );
    editor->resetUI();

    readSettings();

    makeColorPaletteConnections();
	makeColorWheelConnections();

    connect(editor, SIGNAL(needSave()), this, SLOT(saveDocument()));
    connect(m_pToolSet, SIGNAL(clearButtonClicked()), editor, SLOT(clearCurrentFrame()));
    connect(editor, SIGNAL(changeTool(ToolType)), m_pToolSet, SLOT(setCurrentTool(ToolType)));

    editor->setCurrentLayer( this->editor->m_pObject->getLayerCount() - 1 );
}

MainWindow2::~MainWindow2()
{
    delete ui;
}

void MainWindow2::makeTimeLineConnections()
{
    connect( m_pTimeLine, SIGNAL( endplayClick() ), editor, SLOT( endPlay() ) );
    connect( m_pTimeLine, SIGNAL( startplayClick() ), editor, SLOT( startPlay() ) );
    connect( m_pTimeLine, SIGNAL( duplicateKeyClick() ), editor, SLOT( duplicateKey() ) );

    connect( m_pTimeLine, SIGNAL( modification() ), editor, SLOT( modification() ) );
    connect( m_pTimeLine, SIGNAL( addKeyClick() ), editor, SLOT( addKey() ) );
    connect( m_pTimeLine, SIGNAL( removeKeyClick() ), editor, SLOT( removeKey() ) );

    connect( m_pTimeLine, SIGNAL( newBitmapLayer() ), editor, SLOT( newBitmapLayer() ) );
    connect( m_pTimeLine, SIGNAL( newVectorLayer() ), editor, SLOT( newVectorLayer() ) );
    connect( m_pTimeLine, SIGNAL( newSoundLayer() ), editor, SLOT( newSoundLayer() ) );
    connect( m_pTimeLine, SIGNAL( newCameraLayer() ), editor, SLOT( newCameraLayer() ) );
    connect( m_pTimeLine, SIGNAL( deleteCurrentLayer() ), editor, SLOT( deleteCurrentLayer() ) );

    connect(m_pTimeLine, SIGNAL(playClick()), editor, SLOT(play()));
    connect(m_pTimeLine, SIGNAL(loopClick(bool)), editor, SLOT(setLoop(bool)));

    connect(m_pTimeLine, SIGNAL(loopControlClick(bool)), editor, SLOT(setLoopControl(bool))); // adding LoopControlClick needs setLoopControl(bool)
    connect(m_pTimeLine, SIGNAL(loopStartClick(int)), editor, SLOT(changeLoopStart(int)));
    connect(m_pTimeLine, SIGNAL(loopEndClick(int)), editor, SLOT(changeLoopEnd(int)));


    connect(m_pTimeLine, SIGNAL(soundClick()), editor, SLOT(setSound()));
    connect(m_pTimeLine, SIGNAL(fpsClick(int)), editor, SLOT(changeFps(int)));

    connect( editor, SIGNAL( toggleLoop( bool ) ), m_pTimeLine, SIGNAL( toggleLoop( bool ) ) );
    connect( m_pTimeLine, SIGNAL( loopClick( bool ) ), editor, SIGNAL( loopToggled( bool ) ) );


    connect(editor, SIGNAL(toggleLoopControl(bool)), m_pTimeLine, SIGNAL(toggleLoopControl(bool)));
    connect(m_pTimeLine, SIGNAL(loopControlClick(bool)), editor, SIGNAL(loopControlToggled(bool)));//adding loopControlClick needs loopControlToggled(bool)

    m_pTimeLine->setFocusPolicy(Qt::NoFocus);
}

void MainWindow2::arrangePalettes()
{
    setCentralWidget(editor);

    m_pColorWheelWidget = new QDockWidget( tr("Color Wheel"), this );
    m_pColorWheelWidget->setFocusPolicy( Qt::NoFocus );

    ColorBox* pColorBox = new ColorBox(this);
    pColorBox->setToolTip(tr("color palette:<br>use <b>(C)</b><br>toggle at cursor"));
    m_pColorWheelWidget->setWidget( pColorBox );

    m_pColorPalette = new ColorPaletteWidget(editor);
    m_pColorPalette->setFocusPolicy( Qt::NoFocus );

    m_pDisplayOptionWidget = new DisplayOptionDockWidget(this);
    m_pDisplayOptionWidget->makeConnectionToEditor(editor);

    m_pToolOptionWidget = new ToolOptionWidget(this);
    m_pToolOptionWidget->makeConnectionToEditor(editor);

    m_pToolSet = editor->m_pToolSet;

    addDockWidget(Qt::RightDockWidgetArea, m_pColorWheelWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_pColorPalette);
    addDockWidget(Qt::RightDockWidgetArea, m_pDisplayOptionWidget);
    addDockWidget(Qt::LeftDockWidgetArea, editor->m_pToolSet);
    addDockWidget(Qt::LeftDockWidgetArea, m_pToolOptionWidget);
    addDockWidget(Qt::BottomDockWidgetArea, m_pTimeLine);


    editor->m_pToolSet->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    m_pToolOptionWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    m_pDisplayOptionWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    m_pTimeLine->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    m_pColorPalette->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    m_pColorWheelWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
}

void MainWindow2::makeColorPaletteConnections()
{
	connect(m_pColorPalette, SIGNAL(colorChanged(QColor)),
		editor->colorManager(), SLOT(pickColor(QColor)));

	connect(m_pColorPalette, SIGNAL(colorNumberChanged(int)),
		editor->colorManager(), SLOT(pickColorNumber(int)));

	connect(editor->colorManager(), SIGNAL(colorChanged(QColor)),
		m_pColorPalette, SLOT(setColor(QColor)));

	connect(editor->colorManager(), SIGNAL(colorNumberChanged(int)),
		m_pColorPalette, SLOT(selectColorNumber(int)));
}

void MainWindow2::makeColorWheelConnections()
{
    ColorBox* pColorBox = static_cast<ColorBox*>(m_pColorWheelWidget->widget());
    Q_ASSERT( pColorBox );

    connect( pColorBox, SIGNAL(colorChanged(QColor)), editor->colorManager(), SLOT(pickColor(QColor)));
	connect( editor->colorManager(), SIGNAL(colorChanged(QColor)), pColorBox, SLOT(setColor(QColor)));
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
    connect( ui->actionExport_X_sheet, &QAction::triggered, editor, &Editor::exportX );
    connect( ui->actionExport_Image_Sequence, &QAction::triggered, editor, &Editor::exportSeq );
    connect( ui->actionExport_Image, &QAction::triggered, editor, &Editor::exportImage );
    connect( ui->actionExport_Movie, &QAction::triggered, editor, &Editor::exportMov );

    //exportFlashAct = new QAction(tr("&Flash/SWF..."), this);
    //exportFlashAct->setShortcut(tr("Ctrl+Alt+F"));
    //connect(exportFlashAct, SIGNAL(triggered()), editor, SLOT(exportFlash()));

    connect( ui->actionExport_Palette, &QAction::triggered, this, &MainWindow2::exportPalette );
    
    /// --- Import Menu ---
    //connect( ui->actionExport_Svg_Image, &QAction::triggered, editor, &Editor::saveSvg );
    connect( ui->actionImport_Image, &QAction::triggered, editor, &Editor::importImageFromDialog );
    connect( ui->actionImport_Image_Sequence, &QAction::triggered, editor, &Editor::importImageSequence );
    connect( ui->actionImport_Movie, &QAction::triggered, editor, &Editor::importMov );
    //connect( ui->actionImport_Sound, &QAction::triggered, editor, &Editor::importSound );
    ui->actionImport_Sound->setEnabled( false );
    connect( ui->actionImport_Palette, &QAction::triggered, this, &MainWindow2::importPalette );
    
    /// --- Edit Menu ---
    connect( ui->actionUndo, &QAction::triggered, editor, &Editor::undo );
    connect( ui->actionRedo, &QAction::triggered, editor, &Editor::redo );
    connect( ui->actionCut, &QAction::triggered, editor, &Editor::cut );
    connect( ui->actionCopy, &QAction::triggered, editor, &Editor::copy );
    connect( ui->actionPaste, &QAction::triggered, editor, &Editor::paste );
    connect( ui->actionClearFrame, &QAction::triggered, editor, &Editor::clearCurrentFrame );
    connect( ui->actionCrop, &QAction::triggered, editor, &Editor::crop );
    connect( ui->actionCrop_To_Selection, &QAction::triggered, editor, &Editor::croptoselect );
    connect( ui->actionSelect_All, &QAction::triggered, editor, &Editor::selectAll );
    connect( ui->actionDeselect_All, &QAction::triggered, editor, &Editor::deselectAll );
    connect( ui->actionPreference, &QAction::triggered, this, &MainWindow2::showPreferences );

    ui->actionRedo->setEnabled( false );
    
    /// --- Layer Menu ---
    connect( ui->actionNew_Bitmap_Layer, &QAction::triggered, editor, &Editor::newBitmapLayer );
    connect( ui->actionNew_Vector_Layer, &QAction::triggered, editor, &Editor::newVectorLayer );
    connect( ui->actionNew_Sound_Layer, &QAction::triggered, editor, &Editor::newSoundLayer );
    connect( ui->actionNew_Camera_Layer, &QAction::triggered, editor, &Editor::newCameraLayer );
    connect( ui->actionDelete_Current_Layer, &QAction::triggered, editor, &Editor::deleteCurrentLayer );

    /// --- View Menu ---
    connect( ui->actionZoom_In, &QAction::triggered, editor, &Editor::setzoom );
    connect( ui->actionZoom_Out, &QAction::triggered, editor, &Editor::setzoom1 );
    connect( ui->actionRotate_Clockwise, &QAction::triggered, editor, &Editor::rotatecw );
    connect( ui->actionRotate_Anticlosewise, &QAction::triggered, editor, &Editor::rotateacw );
    connect( ui->actionReset_Windows, &QAction::triggered, this, &MainWindow2::dockAllPalettes );
    connect( ui->actionReset_View, &QAction::triggered, editor, &Editor::resetView );
    connect( ui->actionHorizontal_Flip, &QAction::triggered, editor, &Editor::toggleMirror );
    connect( ui->actionVertical_Flip, &QAction::triggered, editor, &Editor::toggleMirrorV );

    ui->actionPreview->setEnabled( false );
    //#	connect(previewAct, SIGNAL(triggered()), editor, SLOT(getCameraLayer()));//TODO: Preview view

    ui->actionGrid->setEnabled( false );
    connect( ui->actionGrid, &QAction::triggered, editor, &Editor::gridview ); //TODO: Grid view

    connect( ui->actionOnionPrevious, &QAction::triggered, editor, &Editor::toggleOnionPrev );
    connect( ui->actionOnionNext, &QAction::triggered, editor, &Editor::toggleOnionNext );
    connect( ui->actionMultiLayerOnionSkin, &QAction::triggered, editor, &Editor::toggleMultiLayerOnionSkin );

    connect( editor, SIGNAL( onionPrevChanged( bool ) ), ui->actionOnionPrevious, SLOT( setChecked( bool ) ) );
    connect( editor, SIGNAL( onionNextChanged( bool ) ), ui->actionOnionNext, SLOT( setChecked( bool ) ) );
    connect( editor, SIGNAL(multiLayerOnionSkinChanged(bool)), ui->actionMultiLayerOnionSkin, SLOT(setChecked(bool)));

    /// --- Animation Menu ---
    connect( ui->actionPlay, &QAction::triggered, editor, &Editor::play );
    connect( ui->actionLoop, &QAction::triggered, editor, &Editor::setLoop );
    connect( ui->actionLoop, SIGNAL(toggled(bool) ), editor, SIGNAL(toggleLoop(bool)) ); //TODO: WTF?
    connect( editor, SIGNAL( loopToggled(bool) ), ui->actionLoop, SLOT(setChecked(bool)) );

    // Loop Control
    connect(ui->actionLoopControl, SIGNAL(triggered(bool)), editor, SLOT(setLoopControl(bool)));
    connect(ui->actionLoopControl, SIGNAL(toggled(bool)), editor, SIGNAL(toggleLoopControl(bool)));
    connect(editor, SIGNAL(loopControlToggled(bool)), ui->actionLoopControl, SLOT(setChecked(bool)));
    // Loop Control

    connect(ui->actionAdd_Frame, &QAction::triggered, editor, &Editor::addNewKey );
    connect(ui->actionRemove_Frame, &QAction::triggered, editor, &Editor::removeKey );
    connect(ui->actionNext_Frame, &QAction::triggered, editor, &Editor::playNextFrame );
    connect(ui->actionPrevious_Frame, &QAction::triggered, editor, &Editor::playPrevFrame );
    connect(ui->actionNext_Keyframe, &QAction::triggered, editor, &Editor::scrubNextKeyframe );
    connect(ui->actionPrev_Keyframe, &QAction::triggered, editor, &Editor::scrubPreviousKeyframe );
    connect(ui->actionDuplicate_Frame, &QAction::triggered, editor, &Editor::duplicateKey );

    /// --- Tool Menu ---
    connect(ui->actionMove, &QAction::triggered, m_pToolSet, &ToolSetWidget::moveOn );
    connect(ui->actionSelect, &QAction::triggered, m_pToolSet, &ToolSetWidget::selectOn );
    connect(ui->actionBrush, &QAction::triggered, m_pToolSet, &ToolSetWidget::brushOn );
    connect(ui->actionPolyline, &QAction::triggered, m_pToolSet, &ToolSetWidget::polylineOn );
    connect(ui->actionSmudge, &QAction::triggered, m_pToolSet, &ToolSetWidget::smudgeOn );
    connect(ui->actionPen, &QAction::triggered, m_pToolSet, &ToolSetWidget::penOn );
    connect(ui->actionHand, &QAction::triggered, m_pToolSet, &ToolSetWidget::handOn );
    connect(ui->actionPencil, &QAction::triggered, m_pToolSet, &ToolSetWidget::pencilOn );
    connect(ui->actionBucket, &QAction::triggered, m_pToolSet, &ToolSetWidget::bucketOn );
    connect(ui->actionEyedropper, &QAction::triggered, m_pToolSet, &ToolSetWidget::eyedropperOn );
    connect(ui->actionEraser, &QAction::triggered, m_pToolSet, &ToolSetWidget::eraserOn );
    connect(ui->actionTogglePalette, &QAction::triggered, m_pScribbleArea,&ScribbleArea::togglePopupPalette );
    connect(ui->actionResetToolsDefault, &QAction::triggered, this, &MainWindow2::resetToolsSettings );

    /// --- Window Menu ---
    connect(ui->actionToolsWidget, SIGNAL(toggled(bool)), editor->m_pToolSet, SLOT(setVisible(bool)));
    connect(editor->m_pToolSet, SIGNAL(visibilityChanged(bool)), ui->actionToolsWidget, SLOT(setChecked(bool)));
    connect(ui->actionOptionsWidget, SIGNAL(toggled(bool)), m_pToolOptionWidget, SLOT(setVisible(bool)));
    connect(m_pToolOptionWidget, SIGNAL(visibilityChanged(bool)), ui->actionOptionsWidget, SLOT(setChecked(bool)));
    connect(ui->actionColorWheel, SIGNAL(toggled(bool)), m_pColorPalette, SLOT(setVisible(bool)));
    connect(m_pColorPalette, SIGNAL(visibilityChanged(bool)), ui->actionColorWheel, SLOT(setChecked(bool)));
    connect(ui->actionTimeline, SIGNAL(toggled(bool)), m_pTimeLine, SLOT(setVisible(bool)));
    connect(m_pTimeLine, SIGNAL(visibilityChanged(bool)), ui->actionTimeline, SLOT(setChecked(bool)));
    connect(ui->actionDisplayOptions, SIGNAL(toggled(bool)), m_pDisplayOptionWidget, SLOT(setVisible(bool)));
    connect(m_pDisplayOptionWidget, SIGNAL(visibilityChanged(bool)), ui->actionDisplayOptions, SLOT(setChecked(bool)));

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
        m_object->deleteLater();
        // default size

        m_object = new Object();
        m_object->defaultInitialisation();

        editor->setObject( m_object );
        editor->resetUI();

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
            editor->updateMaxFrame();
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
        pObject->defaultInitialisation();

        editor->setObject( pObject );
        editor->resetUI();
    }
    else
    {
        editor->updateMaxFrame();
    }
}

bool MainWindow2::openObject( QString strFilePath )
{
    QProgressDialog progress( tr("Opening document..."), tr("Abort"), 0, 100, this );
    progress.setWindowModality( Qt::WindowModal );
    progress.show();

    editor->setCurrentLayer( 0 );
    editor->layerManager()->setCurrentFrameIndex( 1 );
    editor->fps = 12;
    m_pTimeLine->setFps( editor->fps );
    m_pScribbleArea->setMyView( QMatrix() );

    ObjectSaveLoader objectLoader( this );
    Object* pObject = objectLoader.loadFromFile( strFilePath );

    if ( pObject != NULL && objectLoader.error().code() == PCL_OK )
    {
        SafeDelete( m_object );
        m_object = pObject;

        pObject->setFilePath( strFilePath );
        QSettings settings( "Pencil", "Pencil" );
        settings.setValue( "lastFilePath", QVariant( pObject->filePath() ) );

        editor->setObject( pObject );
        editor->updateObject();

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
    editor->setObject( newObject );

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
        editor->updateObject();

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
        Object* objectToDelete = m_object;
        m_object = newObject;
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

    if ( docElem.isNull() ) return false;
    QDomNode tag = docElem.firstChild();
    while ( !tag.isNull() )
    {
        QDomElement element = tag.toElement(); // try to convert the node to an element.
        if ( !element.isNull() )
        {
            if ( element.tagName() == "currentLayer" )
            {
                int nCurrentLayerIndex = element.attribute( "value" ).toInt();
                editor->setCurrentLayer( nCurrentLayerIndex );
            }
            if ( element.tagName() == "currentFrame" )
            {
                editor->layerManager()->setCurrentFrameIndex( element.attribute( "value" ).toInt() );
            }
            if ( element.tagName() == "currentFps" )
            {
                editor->fps = element.attribute( "value" ).toInt();
                //timer->setInterval(1000/fps);
                m_pTimeLine->setFps( editor->fps );
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
}

// Added here (mainWindow2) to be easily located
// TODO: Find a better place for this function
void MainWindow2::resetToolsSettings()
{
    editor->toolManager()->resetAllTools();
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
    int nLayers = m_object->getLayerCount();
    qDebug( "Layer Count=%d", nLayers );

    for ( int i = 0; i < nLayers; i++ )
    {
        Layer* layer = m_object->getLayer( i );
        qDebug() << "Saving Layer " << i << "(" << layer->name << ")";

        progressValue = (i * 100) / nLayers;
        progress.setValue( progressValue );
        if ( layer->type() == Layer::BITMAP ) ((LayerBitmap*)layer)->saveImages( dataLayersDir, i );
        if ( layer->type() == Layer::VECTOR ) ((LayerVector*)layer)->saveImages( dataLayersDir, i );
        if ( layer->type() == Layer::SOUND ) ((LayerSound*)layer)->saveImages( dataLayersDir, i );
    }

    // save palette
    m_object->savePalette( dataLayersDir );

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
    QDomElement objectElement = m_object->createDomElement( doc );
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

    m_object->modified = false;
    m_pTimeLine->updateContent();

    m_object->setFilePath( strSavedFilename );

    m_recentFileMenu->addRecentFile( strSavedFilename );
    m_recentFileMenu->saveToDisk();

    return true;
}

void MainWindow2::saveDocument()
{
    if ( !m_object->filePath().isEmpty() )
    {
        saveObject( m_object->filePath() );
    }
    else
    {
        saveAsNewDocument();
    }
}

bool MainWindow2::maybeSave()
{
    if ( m_object->modified )
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
    tag1.setAttribute( "value", editor->layerManager()->currentLayerIndex() );
    tag.appendChild( tag1 );
    QDomElement tag2 = doc.createElement( "currentFrame" );
    tag2.setAttribute( "value", editor->layerManager()->currentFrameIndex() );
    tag.appendChild( tag2 );
    QDomElement tag2a = doc.createElement( "currentFps" );
    tag2a.setAttribute( "value", editor->fps );
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

    connect( m_pPreferences, SIGNAL( autosaveChange( int ) ), editor, SLOT( changeAutosave( int ) ) );
    connect( m_pPreferences, SIGNAL( autosaveNumberChange( int ) ), editor, SLOT( changeAutosaveNumber( int ) ) );

    connect( m_pPreferences, SIGNAL( onionLayer1OpacityChange( int ) ), editor, SLOT( onionLayer1OpacityChangeSlot( int ) ) );
    connect( m_pPreferences, SIGNAL( onionLayer2OpacityChange( int ) ), editor, SLOT( onionLayer2OpacityChangeSlot( int ) ) );
    connect( m_pPreferences, SIGNAL( onionLayer3OpacityChange( int ) ), editor, SLOT( onionLayer3OpacityChangeSlot( int ) ) );

    unloadAllShortcuts();

    connect( m_pPreferences, SIGNAL( destroyed() ),
        this, SLOT( loadAllShortcuts() ) );

    m_pPreferences->show();
}

void MainWindow2::dockAllPalettes()
{
    editor->m_pToolSet->setFloating(false);
    m_pToolOptionWidget->setFloating(false);
    m_pDisplayOptionWidget->setFloating(false);
    m_pTimeLine->setFloating(false);
    m_pColorPalette->setFloating(false);
}

void MainWindow2::readSettings()
{
    QSettings* settings = pencilSettings();
    QRect desktopRect = QApplication::desktop()->screenGeometry();
    desktopRect.adjust( 80, 80, -80, -80 );

    QPoint pos = settings->value( "editorPosition", desktopRect.topLeft() ).toPoint();
    QSize size = settings->value( "editorSize", desktopRect.size() ).toSize();

    move( pos );
    resize( size );

    editor->restorePalettesSettings( true, true, true );

    QString myPath = settings->value( "lastFilePath", QVariant( QDir::homePath() ) ).toString();
    m_recentFileMenu->addRecentFile( myPath );

    setOpacity( 100 - settings->value( "windowOpacity" ).toInt() );
}

void MainWindow2::writeSettings()
{
    QSettings settings( "Pencil", "Pencil" );
    settings.setValue( "editorPosition", pos() );
    settings.setValue( "editorSize", size() );

    ColorPaletteWidget* colourPalette = m_pColorPalette;
    if ( colourPalette != NULL )
    {
        settings.setValue( "colourPalettePosition", colourPalette->pos() );
        settings.setValue( "colourPaletteSize", colourPalette->size() );
        settings.setValue( "colourPaletteFloating", colourPalette->isFloating() );
    }

    TimeLine* timelinePalette = editor->getTimeLine();
    if ( timelinePalette != NULL )
    {
        settings.setValue( "timelinePalettePosition", timelinePalette->pos() );
        settings.setValue( "timelinePaletteSize", timelinePalette->size() );
        settings.setValue( "timelinePaletteFloating", timelinePalette->isFloating() );
    }

    QDockWidget* toolWidget = editor->m_pToolSet;
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
    ui->actionPrev_Keyframe->setShortcut( cmdKeySeq( CMD_GOTO_PREV_KEY_FRAME ) );
    ui->actionNext_Keyframe->setShortcut( cmdKeySeq( CMD_GOTO_NEXT_KEY_FRAME ) );
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
    if ( this->editor->backupIndex < 0 )
    {
        ui->actionUndo->setText( tr("Undo") );
        ui->actionUndo->setEnabled( false );
    }
    else
    {
        ui->actionUndo->setText( tr("Undo   ") + QString::number( this->editor->backupIndex + 1 ) + " " + this->editor->backupList.at( this->editor->backupIndex )->undoText );
        ui->actionUndo->setEnabled( true );
    }

    if ( this->editor->backupIndex + 2 < this->editor->backupList.size() )
    {
        ui->actionRedo->setText( tr("Redo   ") + QString::number( this->editor->backupIndex + 2 ) + " " + this->editor->backupList.at( this->editor->backupIndex + 1 )->undoText );
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
        m_object->exportPalette( filePath );
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
        m_object->importPalette( filePath );
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