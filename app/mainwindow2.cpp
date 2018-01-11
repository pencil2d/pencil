/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2008-2009 Mj Mendoza IV
Copyright (C) 2011-2017 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

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
#include <QMessageBox>
#include <QProgressDialog>
#include <QFileIconProvider>

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
#include "tooloptionwidget.h"
#include "preferencesdialog.h"
#include "timeline.h"
#include "toolbox.h"
#include "preview.h"
#include "timeline2.h"
#include "errordialog.h"
#include "importimageseqdialog.h"

#include "colorbox.h"
#include "util.h"

#include "fileformat.h"     //contains constants used by Pencil File Format
#include "recentfilemenu.h"

#include "shortcutfilter.h"
#include "movieexporter.h"
#include "app_util.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define S__GIT_TIMESTAMP__ TOSTRING(GIT_TIMESTAMP)

#ifdef GIT_TIMESTAMP
#define BUILD_DATE S__GIT_TIMESTAMP__
#else
#define BUILD_DATE __DATE__
#endif

#ifdef NIGHTLY_BUILD
#define PENCIL_WINDOW_TITLE QString("[*]Pencil2D - Nightly Build %1").arg( BUILD_DATE )
#else
#define PENCIL_WINDOW_TITLE QString("[*]Pencil2D v%1").arg(APP_VERSION)
#endif



MainWindow2::MainWindow2(QWidget *parent) : QMainWindow(parent)
{
    ui = new Ui::MainWindow2;
    ui->setupUi(this);

    mBackground = new BackgroundWidget(this);

    mScribbleArea = new ScribbleArea(mBackground);
    mScribbleArea->setFocusPolicy(Qt::StrongFocus);

    // Show the UI over the background
    //
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(mScribbleArea);

    mBackground->setLayout(layout);

    // Central widget
    setCentralWidget(mBackground);

    // Initialize order
    // 1. object 2. editor 3. scribble area 4. other widgets
    Object* object = new Object();
    object->init();
    object->createDefaultLayers();

    mEditor = new Editor(this);
    mEditor->setScribbleArea(mScribbleArea);
    mEditor->init();
    mEditor->setObject(object);

    mScribbleArea->setCore(mEditor);
    mScribbleArea->init();

    mEditor->setScribbleArea(mScribbleArea);
    makeConnections(mEditor, mScribbleArea);

    mCommands = new ActionCommands(this);
    mCommands->setCore(mEditor);

    createDockWidgets();
    createMenus();
    setupKeyboardShortcuts();

    readSettings();

    connect(mEditor, &Editor::needSave, this, &MainWindow2::autoSave);
    connect(mToolBox, &ToolBoxWidget::clearButtonClicked, mEditor, &Editor::clearCurrentFrame);
    connect(mEditor->view(), &ViewManager::viewChanged, this, &MainWindow2::updateZoomLabel);

    //connect( mScribbleArea, &ScribbleArea::refreshPreview, mPreview, &PreviewWidget::updateImage );
    mEditor->tools()->setDefaultTool();
    mBackground->init(mEditor->preference());
    mEditor->updateObject();

    setWindowTitle(PENCIL_WINDOW_TITLE);
}

MainWindow2::~MainWindow2()
{
    delete ui;
}

void MainWindow2::createDockWidgets()
{
    mTimeLine = new TimeLine(this);
    mTimeLine->setObjectName("TimeLine");

    mColorWheel = new ColorBox(this);
    mColorWheel->setToolTip(tr("color palette:<br>use <b>(C)</b><br>toggle at cursor"));
    mColorWheel->setObjectName("ColorWheel");
    mColorWheel->setMaximumHeight(390);

    mColorPalette = new ColorPaletteWidget(this);
    mColorPalette->setObjectName("ColorPalette");

    mDisplayOptionWidget = new DisplayOptionWidget(this);
    mDisplayOptionWidget->setObjectName("DisplayOption");

    mToolOptions = new ToolOptionWidget(this);
    mToolOptions->setObjectName("ToolOption");

    mToolBox = new ToolBoxWidget(this);
    mToolBox->setObjectName("ToolBox");

    /*
    mTimeline2 = new Timeline2;
    mTimeline2->setObjectName( "Timeline2" );
    mDockWidgets.append( mTimeline2 );
    */

    mDockWidgets
        << mTimeLine
        << mColorWheel
        << mColorPalette
        << mDisplayOptionWidget
        << mToolOptions
        << mToolBox;

    mStartIcon = QIcon(":icons/controls/play.png");
    mStopIcon = QIcon(":icons/controls/stop.png");

    for (BaseDockWidget* pWidget : mDockWidgets)
    {
        pWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
        pWidget->setFeatures(QDockWidget::AllDockWidgetFeatures);
        pWidget->setFocusPolicy(Qt::NoFocus);

        pWidget->setEditor(mEditor);
        pWidget->initUI();
        pWidget->show();
        qDebug() << "Init Dock widget: " << pWidget->objectName();
    }

    addDockWidget(Qt::RightDockWidgetArea, mColorWheel);
    addDockWidget(Qt::RightDockWidgetArea, mColorPalette);
    addDockWidget(Qt::RightDockWidgetArea, mDisplayOptionWidget);
    addDockWidget(Qt::LeftDockWidgetArea, mToolBox);
    addDockWidget(Qt::LeftDockWidgetArea, mToolOptions);
    addDockWidget(Qt::BottomDockWidgetArea, mTimeLine);
    setDockNestingEnabled(true);
    //addDockWidget( Qt::BottomDockWidgetArea, mTimeline2);

    /*
    mPreview = new PreviewWidget( this );
    mPreview->setImage( mScribbleArea->mBufferImg );
    mPreview->setFeatures( QDockWidget::DockWidgetFloatable );
    mPreview->setFocusPolicy( Qt::NoFocus );
    addDockWidget( Qt::RightDockWidgetArea, mPreview );
    */

    makeConnections(mEditor);
    makeConnections(mEditor, mTimeLine);
    makeConnections(mEditor, mColorWheel);
    makeConnections(mEditor, mColorPalette);
    makeConnections(mEditor, mDisplayOptionWidget);
    makeConnections(mEditor, mToolOptions);

    for (BaseDockWidget* w : mDockWidgets)
    {
        w->updateUI();
        w->setFloating(false);
    }
}

void MainWindow2::createMenus()
{
    // ---------- File Menu -------------
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow2::newDocument);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow2::openDocument);
    connect(ui->actionSave_as, &QAction::triggered, this, &MainWindow2::saveAsNewDocument);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow2::saveDocument);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow2::close);

    /// --- Export Menu ---
    //connect( ui->actionExport_X_sheet, &QAction::triggered, mEditor, &Editor::exportX );
    connect(ui->actionExport_Image, &QAction::triggered, mCommands, &ActionCommands::exportImage);
    connect(ui->actionExport_ImageSeq, &QAction::triggered, mCommands, &ActionCommands::exportImageSequence);
    connect(ui->actionExport_Movie, &QAction::triggered, mCommands, &ActionCommands::exportMovie);

    connect(ui->actionExport_Palette, &QAction::triggered, this, &MainWindow2::exportPalette);

    /// --- Import Menu ---
    //connect( ui->actionExport_Svg_Image, &QAction::triggered, editor, &Editor::saveSvg );
    connect(ui->actionImport_Image, &QAction::triggered, this, &MainWindow2::importImage);
    connect(ui->actionImport_ImageSeq, &QAction::triggered, this, &MainWindow2::importImageSequence);
    connect(ui->actionImport_Movie, &QAction::triggered, this, &MainWindow2::importMovie);

    connect(ui->actionImport_Sound, &QAction::triggered, mCommands, &ActionCommands::importSound);
    connect(ui->actionImport_Palette, &QAction::triggered, this, &MainWindow2::importPalette);

    /// --- Edit Menu ---
    connect(ui->actionUndo, &QAction::triggered, mEditor, &Editor::undo);
    connect(ui->actionRedo, &QAction::triggered, mEditor, &Editor::redo);
    connect(ui->actionCut, &QAction::triggered, mEditor, &Editor::cut);
    connect(ui->actionCopy, &QAction::triggered, mEditor, &Editor::copy);
    connect(ui->actionPaste, &QAction::triggered, mEditor, &Editor::paste);
    connect(ui->actionClearFrame, &QAction::triggered, mEditor, &Editor::clearCurrentFrame);
    connect(ui->actionFlip_X, &QAction::triggered, mCommands, &ActionCommands::flipSelectionX);
    connect(ui->actionFlip_Y, &QAction::triggered, mCommands, &ActionCommands::flipSelectionY);
    connect(ui->actionSelect_All, &QAction::triggered, mEditor, &Editor::selectAll);
    connect(ui->actionDeselect_All, &QAction::triggered, mEditor, &Editor::deselectAll);
    connect(ui->actionPreference, &QAction::triggered, [=] { preferences(); });
    ui->actionPreference->setMenuRole(QAction::PreferencesRole);

    ui->actionRedo->setEnabled(false);

    /// --- Layer Menu ---
    connect(ui->actionNew_Bitmap_Layer, &QAction::triggered, mCommands, &ActionCommands::addNewBitmapLayer);
    connect(ui->actionNew_Vector_Layer, &QAction::triggered, mCommands, &ActionCommands::addNewVectorLayer);
    connect(ui->actionNew_Sound_Layer, &QAction::triggered, mCommands, &ActionCommands::addNewSoundLayer);
    connect(ui->actionNew_Camera_Layer, &QAction::triggered, mCommands, &ActionCommands::addNewCameraLayer);
    connect(ui->actionDelete_Current_Layer, &QAction::triggered, mCommands, &ActionCommands::deleteCurrentLayer);

    /// --- View Menu ---
    connect(ui->actionZoom_In, &QAction::triggered, mCommands, &ActionCommands::ZoomIn);
    connect(ui->actionZoom_Out, &QAction::triggered, mCommands, &ActionCommands::ZoomOut);
    connect(ui->actionRotate_Clockwise, &QAction::triggered, mCommands, &ActionCommands::rotateClockwise);
    connect(ui->actionRotate_Anticlosewise, &QAction::triggered, mCommands, &ActionCommands::rotateCounterClockwise);
    connect(ui->actionReset_Windows, &QAction::triggered, this, &MainWindow2::dockAllSubWidgets);
    connect(ui->actionReset_View, &QAction::triggered, mEditor->view(), &ViewManager::resetView);
    connect(ui->actionHorizontal_Flip, &QAction::triggered, mCommands, &ActionCommands::toggleMirror);
    connect(ui->actionVertical_Flip, &QAction::triggered, mCommands, &ActionCommands::toggleMirrorV);

    ui->actionPreview->setEnabled(false);
    //# connect(previewAct, SIGNAL(triggered()), editor, SLOT(getCameraLayer()));//TODO: Preview view

    setMenuActionChecked(ui->actionGrid, mEditor->preference()->isOn(SETTING::GRID));
    connect(ui->actionGrid, &QAction::triggered, mCommands, &ActionCommands::showGrid);

    bindActionWithSetting(ui->actionOnionPrev, SETTING::PREV_ONION);
    bindActionWithSetting(ui->actionOnionNext, SETTING::NEXT_ONION);
    bindActionWithSetting(ui->actionMultiLayerOnionSkin, SETTING::MULTILAYER_ONION);

    /// --- Animation Menu ---
    PlaybackManager* pPlaybackManager = mEditor->playback();
    connect(ui->actionPlay, &QAction::triggered, mCommands, &ActionCommands::PlayStop);

    connect(ui->actionLoop, &QAction::triggered, pPlaybackManager, &PlaybackManager::setLooping);
    connect(ui->actionLoopControl, &QAction::triggered, pPlaybackManager, &PlaybackManager::enableRangedPlayback);
    connect(pPlaybackManager, &PlaybackManager::loopStateChanged, ui->actionLoop, &QAction::setChecked);
    connect(pPlaybackManager, &PlaybackManager::loopStateChanged, mTimeLine, &TimeLine::setLoop);
    connect(pPlaybackManager, &PlaybackManager::rangedPlaybackStateChanged, ui->actionLoopControl, &QAction::setChecked);
    connect(pPlaybackManager, &PlaybackManager::rangedPlaybackStateChanged, mTimeLine, &TimeLine::setRangeState);
    connect(pPlaybackManager, &PlaybackManager::playStateChanged, mTimeLine, &TimeLine::setPlaying);
    connect(pPlaybackManager, &PlaybackManager::playStateChanged, this, &MainWindow2::changePlayState);

    connect(ui->actionAdd_Frame, &QAction::triggered, mEditor, &Editor::addNewKey);
    connect(ui->actionRemove_Frame, &QAction::triggered, mEditor, &Editor::removeKey);
    connect(ui->actionNext_Frame, &QAction::triggered, mCommands, &ActionCommands::GotoNextFrame);
    connect(ui->actionPrevious_Frame, &QAction::triggered, mCommands, &ActionCommands::GotoPrevFrame);
    connect(ui->actionNext_KeyFrame, &QAction::triggered, mCommands, &ActionCommands::GotoNextKeyFrame);
    connect(ui->actionPrev_KeyFrame, &QAction::triggered, mCommands, &ActionCommands::GotoPrevKeyFrame);
    connect(ui->actionDuplicate_Frame, &QAction::triggered, mCommands, &ActionCommands::duplicateKey);
    connect(ui->actionMove_Frame_Forward, &QAction::triggered, mEditor, &Editor::moveFrameForward);
    connect(ui->actionMove_Frame_Backward, &QAction::triggered, mEditor, &Editor::moveFrameBackward);

    /// --- Tool Menu ---
    connect(ui->actionMove, &QAction::triggered, mToolBox, &ToolBoxWidget::moveOn);
    connect(ui->actionSelect, &QAction::triggered, mToolBox, &ToolBoxWidget::selectOn);
    connect(ui->actionBrush, &QAction::triggered, mToolBox, &ToolBoxWidget::brushOn);
    connect(ui->actionPolyline, &QAction::triggered, mToolBox, &ToolBoxWidget::polylineOn);
    connect(ui->actionSmudge, &QAction::triggered, mToolBox, &ToolBoxWidget::smudgeOn);
    connect(ui->actionPen, &QAction::triggered, mToolBox, &ToolBoxWidget::penOn);
    connect(ui->actionHand, &QAction::triggered, mToolBox, &ToolBoxWidget::handOn);
    connect(ui->actionPencil, &QAction::triggered, mToolBox, &ToolBoxWidget::pencilOn);
    connect(ui->actionBucket, &QAction::triggered, mToolBox, &ToolBoxWidget::bucketOn);
    connect(ui->actionEyedropper, &QAction::triggered, mToolBox, &ToolBoxWidget::eyedropperOn);
    connect(ui->actionEraser, &QAction::triggered, mToolBox, &ToolBoxWidget::eraserOn);
    connect(ui->actionResetToolsDefault, &QAction::triggered, mEditor->tools(), &ToolManager::resetAllTools);

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
    for (QAction* action : actions)
    {
        action->setMenuRole(QAction::NoRole);
        winMenu->addAction(action);
    }

    winMenu->addSeparator();
    QAction *lockWidgets = new QAction(tr("Lock Windows"), winMenu);
    lockWidgets->setCheckable(true);
    winMenu->addAction(lockWidgets);
    connect(lockWidgets, &QAction::toggled, this, &MainWindow2::lockWidgets);
    bindActionWithSetting(lockWidgets, SETTING::LAYOUT_LOCK);

    // -------------- Help Menu ---------------
    connect(ui->actionHelp, &QAction::triggered, mCommands, &ActionCommands::help);
    connect(ui->actionAbout, &QAction::triggered, mCommands, &ActionCommands::about);
    connect(ui->actionWebsite, &QAction::triggered, mCommands, &ActionCommands::website);
    connect(ui->actionReport_Bug, &QAction::triggered, mCommands, &ActionCommands::reportbug);

    // --------------- Menus ------------------
    mRecentFileMenu = new RecentFileMenu(tr("Open Recent"), this);
    mRecentFileMenu->loadFromDisk();
    ui->menuFile->insertMenu(ui->actionSave, mRecentFileMenu);

    connect(mRecentFileMenu, &RecentFileMenu::loadRecentFile, this, &MainWindow2::openFile);

    connect(ui->menuEdit, &QMenu::aboutToShow, this, &MainWindow2::undoActSetText);
    connect(ui->menuEdit, &QMenu::aboutToHide, this, &MainWindow2::undoActSetEnabled);
}

void MainWindow2::setMenuActionChecked(QAction* action, bool bChecked)
{
    SignalBlocker b(action);
    action->setChecked(bChecked);
}

void MainWindow2::setOpacity(int opacity)
{
    mEditor->preference()->set(SETTING::WINDOW_OPACITY, 100 - opacity);
    setWindowOpacity(opacity / 100.0);
}

void MainWindow2::updateSaveState()
{
    setWindowModified(mEditor->currentBackup() != mBackupAtSave);
}

void MainWindow2::clearRecentFilesList()
{
    QStringList recentFilesList = mRecentFileMenu->getRecentFiles();
    if (!recentFilesList.isEmpty())
    {
        mRecentFileMenu->clear();
        QMessageBox::information(this, 0,
                                 tr("\n\n You have successfully cleared the list"),
                                 QMessageBox::Ok);
    }
    getPrefDialog()->updateRecentListBtn(!recentFilesList.isEmpty());
}

void MainWindow2::closeEvent(QCloseEvent* event)
{
    if (m2ndCloseEvent)
    {
        // https://bugreports.qt.io/browse/QTBUG-43344
        event->accept();
        return;
    }

    if (maybeSave())
    {
        writeSettings();
        event->accept();
        m2ndCloseEvent = true;
    }
    else
    {
        event->ignore();
    }
}

void MainWindow2::tabletEvent(QTabletEvent* event)
{
    event->ignore();
}

void MainWindow2::newDocument()
{
    if (maybeSave())
    {
        Object* object = new Object();
        object->init();
        object->createDefaultLayers();
        mEditor->setObject(object);
        mEditor->scrubTo(0);
        mEditor->view()->resetView();

        // Refresh the palette
        mColorPalette->refreshColorList();
        mEditor->color()->setColorNumber(0);

        setWindowTitle(PENCIL_WINDOW_TITLE);
        updateSaveState();
    }
}

void MainWindow2::openDocument()
{
    if (maybeSave())
    {
        FileDialog fileDialog(this);
        QString fileName = fileDialog.openFile(FileType::ANIMATION);
        if (fileName.isEmpty())
        {
            return;
        }
        QFileInfo fileInfo(fileName);
        if (fileInfo.isDir())
        {
            return;
        }

        bool ok = openObject(fileName);
        if (!ok)
        {
            QMessageBox::warning(this, tr("Warning"), tr("Pencil cannot read this file. If you want to import images, use the command import."));
            newDocument();
        }
    }
    updateSaveState();
}

bool MainWindow2::saveAsNewDocument()
{
    FileDialog fileDialog(this);
    QString fileName = fileDialog.saveFile(FileType::ANIMATION);
    if (fileName.isEmpty())
    {
        return false;
    }

    if (!fileName.endsWith(PFF_OLD_EXTENSION) && !fileName.endsWith(PFF_EXTENSION))
    {
        fileName = fileName + PFF_EXTENSION;
    }
    return saveObject(fileName);
}

void MainWindow2::openFile(QString filename)
{
    bool ok = openObject(filename);
    if (!ok)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Pencil cannot read this file. If you want to import images, use the command import."));
        newDocument();
    }
}

bool MainWindow2::openObject(QString strFilePath)
{
    QProgressDialog progress(tr("Opening document..."), tr("Abort"), 0, 100, this);

    // Don't show progress bar if running without a GUI (aka. when rendering from command line)
    if (this->isVisible())
    {
        hideQuestionMark(progress);
        progress.setWindowModality(Qt::WindowModal);
        progress.show();
    }

    mEditor->setCurrentLayer(0);

    FileManager fm(this);
    connect(&fm, &FileManager::progressUpdated, [&progress](float f)
    {
        progress.setValue((int)(f * 100.f));
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    });

    Object* object = fm.load(strFilePath);

    if (object == nullptr || !fm.error().ok())
    {
        return false;
    }

    mEditor->setObject(object);

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(LAST_PCLX_PATH, object->filePath());

    mRecentFileMenu->addRecentFile(object->filePath());
    mRecentFileMenu->saveToDisk();

    setWindowTitle(object->filePath().prepend("[*]"));
    setWindowModified(false);
    setWindowIcon(QFileIconProvider().icon(strFilePath));

    // Refresh the Palette
    mColorPalette->refreshColorList();
    mEditor->color()->setColorNumber(0);

    // Reset view
    mEditor->scrubTo(0);
    mEditor->view()->resetView();

    progress.setValue(100);
    return true;
}

bool MainWindow2::saveObject(QString strSavedFileName)
{
    QProgressDialog progress(tr("Saving document..."), tr("Abort"), 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    mEditor->prepareSave();

    FileManager* fm = new FileManager(this);
    Status st = fm->save(mEditor->object(), strSavedFileName);

    progress.setValue(100);

    if (!st.ok())
    {
        QDateTime dt = QDateTime::currentDateTime();
        dt.setTimeSpec(Qt::UTC);
#if QT_VERSION >= 0x050400
        QDir errorLogFolder(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
#else
        QDir errorLogFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
#endif
        errorLogFolder.mkpath("./logs");
        errorLogFolder.cd("logs");
        QFile eLog(errorLogFolder.absoluteFilePath(QString("error-%1.txt").arg(dt.toString(Qt::ISODate))));
        if (eLog.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&eLog);
            out << st.details().replace("<br>", "\n", Qt::CaseInsensitive);
        }

        ErrorDialog errorDialog(st.title(),
                                st.description().append(tr("<br><br>An error has occurred and your file may not have saved successfully."
                                                           "If you believe that this error is an issue with Pencil2D, please create a new issue at:"
                                                           "<br><a href='https://github.com/pencil2d/pencil/issues'>https://github.com/pencil2d/pencil/issues</a><br>"
                                                           "Please be sure to include the following details in your issue:")), st.details());
        errorDialog.exec();
        return false;
    }

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(LAST_PCLX_PATH, strSavedFileName);

    mRecentFileMenu->addRecentFile(strSavedFileName);
    mRecentFileMenu->saveToDisk();

    mTimeLine->updateContent();

    setWindowTitle(strSavedFileName.prepend("[*]"));
    mBackupAtSave = mEditor->currentBackup();
    updateSaveState();

    return true;
}

bool MainWindow2::saveDocument()
{
    if (!mEditor->object()->filePath().isEmpty())
        return saveObject(mEditor->object()->filePath());
    else
        return saveAsNewDocument();
}

bool MainWindow2::maybeSave()
{
    if (mEditor->currentBackup() != mBackupAtSave)
    {
        int ret = QMessageBox::warning(this, tr("Warning"),
                                       tr("This animation has been modified.\n Do you want to save your changes?"),
                                       QMessageBox::Discard | QMessageBox::Save | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return saveDocument();
        else if (ret == QMessageBox::Discard)
            return true;
        else
            return false;
    }
    return true;
}

bool MainWindow2::autoSave()
{
    if (!mEditor->object()->filePath().isEmpty())
    {
        return saveDocument();
    }

    if (mEditor->autoSaveNeverAskAgain())
        return false;

    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setWindowTitle("AutoSave Reminder");
    msgBox.setText(tr("The animation is not saved yet.\n Do you want to save now?"));
    msgBox.addButton(tr("Never ask again", "AutoSave reminder button"), QMessageBox::RejectRole);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);

    int ret = msgBox.exec();
    if (ret == QMessageBox::Yes)
    {
        return saveDocument();
    }
    else if (ret != QMessageBox::No) // Never ask again
    {
        mEditor->dontAskAutoSave(true);
    }

    return false;
}

void MainWindow2::importImage()
{
    FileDialog fileDialog(this);
    QString strFilePath = fileDialog.openFile(FileType::IMAGE);

    if (strFilePath.isEmpty()) { return; }
    if (!QFile::exists(strFilePath)) { return; }

    bool ok = mEditor->importImage(strFilePath);
    if (!ok)
    {
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("Unable to import image.<br><b>TIP:</b> Use Bitmap layer to import bitmaps."),
                             QMessageBox::Ok,
                             QMessageBox::Ok);
        return;
    }

    mScribbleArea->updateCurrentFrame();
    mTimeLine->updateContent();
}

void MainWindow2::importImageSequence()
{
    auto imageSeqDialog = new ImportImageSeqDialog(this);
    imageSeqDialog->exec();
    if (imageSeqDialog->result() == QDialog::Rejected)
    {
        return;
    }

    QStringList files = imageSeqDialog->getFilePaths();
    int number = imageSeqDialog->getSpace();
    for (QString strImgFile : files)
    {
        if (strImgFile.endsWith(".png") ||
            strImgFile.endsWith(".jpg") ||
            strImgFile.endsWith(".jpeg") ||
            strImgFile.endsWith(".tif") ||
            strImgFile.endsWith(".tiff") ||
            strImgFile.endsWith(".bmp"))
        {
            mEditor->importImage(strImgFile);
            for (int i = 1; i < number; i++)
            {
                mEditor->scrubForward();
            }
        }
    }
}

void MainWindow2::importMovie()
{
    FileDialog fileDialog(this);
    QString filePath = fileDialog.openFile(FileType::MOVIE);
    if (filePath.isEmpty())
    {
        return;
    }
    mEditor->importMovie(filePath, mEditor->playback()->fps());
}

void MainWindow2::lockWidgets(bool shouldLock)
{
    QDockWidget::DockWidgetFeature feat = shouldLock ? QDockWidget::DockWidgetClosable : QDockWidget::AllDockWidgetFeatures;

    mColorWheel->setFeatures(feat);
    mColorPalette->setFeatures(feat);
    mDisplayOptionWidget->setFeatures(feat);
    mToolOptions->setFeatures(feat);
    mToolBox->setFeatures(feat);
    mTimeLine->setFeatures(feat);
}

void MainWindow2::preferences()
{
    if (mPrefDialog)
    {
        mPrefDialog->activateWindow();
        mPrefDialog->raise();
        return;
    }
    mPrefDialog = new PreferencesDialog(this);
    mPrefDialog->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    mPrefDialog->setAttribute(Qt::WA_DeleteOnClose);
    mPrefDialog->init(mEditor->preference());

    connect(mPrefDialog, &PreferencesDialog::clearRecentList, this, &MainWindow2::clearRecentFilesList);
    connect(mPrefDialog, &PreferencesDialog::windowOpacityChange, this, &MainWindow2::setOpacity);
    connect(mPrefDialog, &PreferencesDialog::finished, [&]
    {
        clearKeyboardShortcuts();
        setupKeyboardShortcuts();
        mScribbleArea->updateCanvasCursor();
        mPrefDialog = nullptr;
    });

    mPrefDialog->show();
}

void MainWindow2::dockAllSubWidgets()
{
    mToolBox->setFloating(false);
    mToolOptions->setFloating(false);
    mDisplayOptionWidget->setFloating(false);
    mTimeLine->setFloating(false);
    mColorPalette->setFloating(false);
    mColorWheel->setFloating(false);
}

void MainWindow2::readSettings()
{
    QSettings settings(PENCIL2D, PENCIL2D);

    QVariant winGeometry = settings.value(SETTING_WINDOW_GEOMETRY);
    restoreGeometry(winGeometry.toByteArray());

    QVariant winState = settings.value(SETTING_WINDOW_STATE);
    restoreState(winState.toByteArray());

    QString myPath = settings.value(LAST_PCLX_PATH, QVariant(QDir::homePath())).toString();
    mRecentFileMenu->addRecentFile(myPath);

    int opacity = mEditor->preference()->getInt(SETTING::WINDOW_OPACITY);

    setOpacity(100 - opacity);
}

void MainWindow2::writeSettings()
{
    qDebug("Save current windows layout.");

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(SETTING_WINDOW_GEOMETRY, saveGeometry());
    settings.setValue(SETTING_WINDOW_STATE, saveState());
}

void MainWindow2::setupKeyboardShortcuts()
{
    checkExistingShortcuts();

    auto cmdKeySeq = [](QString strCommandName) -> QKeySequence
    {
        strCommandName = QString("shortcuts/") + strCommandName;
        QKeySequence keySequence(pencilSettings().value(strCommandName).toString());
        return keySequence;
    };

    ui->actionNew->setShortcut(cmdKeySeq(CMD_NEW_FILE));
    ui->actionOpen->setShortcut(cmdKeySeq(CMD_OPEN_FILE));
    ui->actionSave->setShortcut(cmdKeySeq(CMD_SAVE_FILE));
    ui->actionSave_as->setShortcut(cmdKeySeq(CMD_SAVE_AS));

    ui->actionImport_Image->setShortcut(cmdKeySeq(CMD_IMPORT_IMAGE));
    ui->actionImport_ImageSeq->setShortcut(cmdKeySeq(CMD_IMPORT_IMAGE_SEQ));
    ui->actionImport_Movie->setShortcut(cmdKeySeq(CMD_IMPORT_MOVIE));
    ui->actionImport_Palette->setShortcut(cmdKeySeq(CMD_IMPORT_PALETTE));
    ui->actionImport_Sound->setShortcut(cmdKeySeq(CMD_IMPORT_SOUND));

    ui->actionExport_Image->setShortcut(cmdKeySeq(CMD_EXPORT_IMAGE));
    ui->actionExport_ImageSeq->setShortcut(cmdKeySeq(CMD_EXPORT_IMAGE_SEQ));
    ui->actionExport_Movie->setShortcut(cmdKeySeq(CMD_EXPORT_MOVIE));
    ui->actionExport_Palette->setShortcut(cmdKeySeq(CMD_EXPORT_PALETTE));

    // edit menu
    ui->actionUndo->setShortcut(cmdKeySeq(CMD_UNDO));
    ui->actionRedo->setShortcut(cmdKeySeq(CMD_REDO));
    ui->actionCut->setShortcut(cmdKeySeq(CMD_CUT));
    ui->actionCopy->setShortcut(cmdKeySeq(CMD_COPY));
    ui->actionPaste->setShortcut(cmdKeySeq(CMD_PASTE));
    ui->actionClearFrame->setShortcut(cmdKeySeq(CMD_CLEAR_FRAME));
    ui->actionSelect_All->setShortcut(cmdKeySeq(CMD_SELECT_ALL));
    ui->actionDeselect_All->setShortcut(cmdKeySeq(CMD_DESELECT_ALL));
    ui->actionPreference->setShortcut(cmdKeySeq(CMD_PREFERENCE));

    // View menu
    ui->actionReset_Windows->setShortcut(cmdKeySeq(CMD_RESET_WINDOWS));
    ui->actionReset_View->setShortcut(cmdKeySeq(CMD_RESET_ZOOM_ROTATE));
    ui->actionZoom_In->setShortcut(cmdKeySeq(CMD_ZOOM_IN));
    ui->actionZoom_Out->setShortcut(cmdKeySeq(CMD_ZOOM_OUT));
    ui->actionRotate_Clockwise->setShortcut(cmdKeySeq(CMD_ROTATE_CLOCK));
    ui->actionRotate_Anticlosewise->setShortcut(cmdKeySeq(CMD_ROTATE_ANTI_CLOCK));
    ui->actionHorizontal_Flip->setShortcut(cmdKeySeq(CMD_FLIP_HORIZONTAL));
    ui->actionVertical_Flip->setShortcut(cmdKeySeq(CMD_FLIP_VERTICAL));
    ui->actionPreview->setShortcut(cmdKeySeq(CMD_PREVIEW));
    ui->actionGrid->setShortcut(cmdKeySeq(CMD_GRID));
    ui->actionOnionPrev->setShortcut(cmdKeySeq(CMD_ONIONSKIN_PREV));
    ui->actionOnionNext->setShortcut(cmdKeySeq(CMD_ONIONSKIN_NEXT));

    ui->actionPlay->setShortcut(cmdKeySeq(CMD_PLAY));
    ui->actionLoop->setShortcut(cmdKeySeq(CMD_LOOP));
    ui->actionPrevious_Frame->setShortcut(cmdKeySeq(CMD_GOTO_PREV_FRAME));
    ui->actionNext_Frame->setShortcut(cmdKeySeq(CMD_GOTO_NEXT_FRAME));
    ui->actionPrev_KeyFrame->setShortcut(cmdKeySeq(CMD_GOTO_PREV_KEY_FRAME));
    ui->actionNext_KeyFrame->setShortcut(cmdKeySeq(CMD_GOTO_NEXT_KEY_FRAME));
    ui->actionAdd_Frame->setShortcut(cmdKeySeq(CMD_ADD_FRAME));
    ui->actionDuplicate_Frame->setShortcut(cmdKeySeq(CMD_DUPLICATE_FRAME));
    ui->actionRemove_Frame->setShortcut(cmdKeySeq(CMD_REMOVE_FRAME));
    ui->actionMove_Frame_Backward->setShortcut(cmdKeySeq(CMD_MOVE_FRAME_BACKWARD));
    ui->actionMove_Frame_Forward->setShortcut(cmdKeySeq(CMD_MOVE_FRAME_FORWARD));

    ShortcutFilter* shortcutfilter = new ShortcutFilter(mScribbleArea, this);
    ui->actionMove->setShortcut(cmdKeySeq(CMD_TOOL_MOVE));
    ui->actionSelect->setShortcut(cmdKeySeq(CMD_TOOL_SELECT));
    ui->actionBrush->setShortcut(cmdKeySeq(CMD_TOOL_BRUSH));
    ui->actionPolyline->setShortcut(cmdKeySeq(CMD_TOOL_POLYLINE));
    ui->actionSmudge->setShortcut(cmdKeySeq(CMD_TOOL_SMUDGE));
    ui->actionPen->setShortcut(cmdKeySeq(CMD_TOOL_PEN));
    ui->actionHand->setShortcut(cmdKeySeq(CMD_TOOL_HAND));
    ui->actionPencil->setShortcut(cmdKeySeq(CMD_TOOL_PENCIL));
    ui->actionBucket->setShortcut(cmdKeySeq(CMD_TOOL_BUCKET));
    ui->actionEyedropper->setShortcut(cmdKeySeq(CMD_TOOL_EYEDROPPER));
    ui->actionEraser->setShortcut(cmdKeySeq(CMD_TOOL_ERASER));

    ui->actionMove->installEventFilter(shortcutfilter);
    ui->actionMove->installEventFilter(shortcutfilter);
    ui->actionSelect->installEventFilter(shortcutfilter);
    ui->actionBrush->installEventFilter(shortcutfilter);
    ui->actionPolyline->installEventFilter(shortcutfilter);
    ui->actionSmudge->installEventFilter(shortcutfilter);
    ui->actionPen->installEventFilter(shortcutfilter);
    ui->actionHand->installEventFilter(shortcutfilter);
    ui->actionPencil->installEventFilter(shortcutfilter);
    ui->actionBucket->installEventFilter(shortcutfilter);
    ui->actionEyedropper->installEventFilter(shortcutfilter);
    ui->actionEraser->installEventFilter(shortcutfilter);

    ui->actionNew_Bitmap_Layer->setShortcut(cmdKeySeq(CMD_NEW_BITMAP_LAYER));
    ui->actionNew_Vector_Layer->setShortcut(cmdKeySeq(CMD_NEW_VECTOR_LAYER));
    ui->actionNew_Camera_Layer->setShortcut(cmdKeySeq(CMD_NEW_CAMERA_LAYER));
    ui->actionNew_Sound_Layer->setShortcut(cmdKeySeq(CMD_NEW_SOUND_LAYER));
    ui->actionDelete_Current_Layer->setShortcut(cmdKeySeq(CMD_DELETE_CUR_LAYER));

    mToolBox->toggleViewAction()->setShortcut(cmdKeySeq(CMD_TOGGLE_TOOLBOX));
    mToolOptions->toggleViewAction()->setShortcut(cmdKeySeq(CMD_TOGGLE_TOOL_OPTIONS));
    mColorWheel->toggleViewAction()->setShortcut(cmdKeySeq(CMD_TOGGLE_COLOR_WHEEL));
    mColorPalette->toggleViewAction()->setShortcut(cmdKeySeq(CMD_TOGGLE_COLOR_LIBRARY));
    mTimeLine->toggleViewAction()->setShortcut(cmdKeySeq(CMD_TOGGLE_TIMELINE));
    mDisplayOptionWidget->toggleViewAction()->setShortcut(cmdKeySeq(CMD_TOGGLE_DISPLAY_OPTIONS));

    ui->actionHelp->setShortcut(cmdKeySeq(CMD_HELP));
}

void MainWindow2::clearKeyboardShortcuts()
{
    QList<QAction*> actionList = this->findChildren<QAction*>();
    for (QAction* action : actionList)
    {
        action->setShortcut(QKeySequence(0));
    }
}

void MainWindow2::undoActSetText(void)
{
    if (this->mEditor->mBackupIndex < 0)
    {
        ui->actionUndo->setText(tr("Undo"));
        ui->actionUndo->setEnabled(false);
    }
    else
    {
        ui->actionUndo->setText(tr("Undo   %1 %2")
                                .arg(QString::number(this->mEditor->mBackupIndex + 1))
                                .arg(this->mEditor->mBackupList.at(this->mEditor->mBackupIndex)->undoText));
        ui->actionUndo->setEnabled(true);
    }

    if (this->mEditor->mBackupIndex + 2 < this->mEditor->mBackupList.size())
    {
        ui->actionRedo->setText(tr("Redo   %1 %2")
                                .arg(QString::number(this->mEditor->mBackupIndex + 2))
                                .arg(this->mEditor->mBackupList.at(this->mEditor->mBackupIndex + 1)->undoText));
        ui->actionRedo->setEnabled(true);
    }
    else
    {
        ui->actionRedo->setText(tr("Redo"));
        ui->actionRedo->setEnabled(false);
    }
}

void MainWindow2::undoActSetEnabled(void)
{
    ui->actionUndo->setEnabled(true);
    ui->actionRedo->setEnabled(true);
}

void MainWindow2::exportPalette()
{
    FileDialog FileDialog(this);
    QString filePath = FileDialog.saveFile(FileType::PALETTE);
    if (!filePath.isEmpty())
    {
        mEditor->object()->exportPalette(filePath);
    }
}

void MainWindow2::importPalette()
{
    FileDialog fileDialog(this);
    QString filePath = fileDialog.openFile(FileType::PALETTE);
    if (!filePath.isEmpty())
    {
        mEditor->object()->importPalette(filePath);
        mColorPalette->refreshColorList();
        mEditor->color()->setColorNumber(0);
    }
}

void MainWindow2::makeConnections(Editor* editor)
{
    connect(editor, &Editor::updateBackup, this, &MainWindow2::updateSaveState);
}

void MainWindow2::makeConnections(Editor* editor, ColorBox* colorBox)
{
    connect(colorBox, &ColorBox::colorChanged, editor->color(), &ColorManager::setColor);
    connect(editor->color(), &ColorManager::colorChanged, colorBox, &ColorBox::setColor);
}

void MainWindow2::makeConnections(Editor* editor, ScribbleArea* scribbleArea)
{
    connect(editor->tools(), &ToolManager::toolChanged, scribbleArea, &ScribbleArea::setCurrentTool);
    connect(editor->tools(), &ToolManager::toolPropertyChanged, scribbleArea, &ScribbleArea::updateToolCursor);
    connect(editor->layers(), &LayerManager::currentLayerChanged, scribbleArea, &ScribbleArea::updateAllFrames);

    connect(editor, &Editor::currentFrameChanged, scribbleArea, &ScribbleArea::updateFrame);
    connect(editor, &Editor::selectAll, scribbleArea, &ScribbleArea::selectAll);

    connect(editor->view(), &ViewManager::viewChanged, scribbleArea, &ScribbleArea::updateAllFrames);
    //connect( editor->preference(), &PreferenceManager::preferenceChanged, scribbleArea, &ScribbleArea::onPreferencedChanged );
}

void MainWindow2::makeConnections(Editor* pEditor, TimeLine* pTimeline)
{
    PlaybackManager* pPlaybackManager = pEditor->playback();
    connect(pTimeline, &TimeLine::duplicateKeyClick, mCommands, &ActionCommands::duplicateKey);

    connect(pTimeline, &TimeLine::soundClick, pPlaybackManager, &PlaybackManager::enableSound);
    connect(pTimeline, &TimeLine::fpsClick, pPlaybackManager, &PlaybackManager::setFps);

    connect(pTimeline, &TimeLine::addKeyClick, mCommands, &ActionCommands::addNewKey);
    connect(pTimeline, &TimeLine::removeKeyClick, mCommands, &ActionCommands::removeKey);

    connect(pTimeline, &TimeLine::newBitmapLayer, mCommands, &ActionCommands::addNewBitmapLayer);
    connect(pTimeline, &TimeLine::newVectorLayer, mCommands, &ActionCommands::addNewVectorLayer);
    connect(pTimeline, &TimeLine::newSoundLayer, mCommands, &ActionCommands::addNewSoundLayer);
    connect(pTimeline, &TimeLine::newCameraLayer, mCommands, &ActionCommands::addNewCameraLayer);

    connect(pTimeline, &TimeLine::toogleAbsoluteOnionClick, pEditor, &Editor::toogleOnionSkinType);

    connect(pEditor->layers(), &LayerManager::currentLayerChanged, pTimeline, &TimeLine::updateUI);
    connect(pEditor->layers(), &LayerManager::layerCountChanged, pTimeline, &TimeLine::updateUI);
    connect(pEditor->sound(), &SoundManager::soundClipDurationChanged, pTimeline, &TimeLine::updateUI);

    connect(pEditor, &Editor::objectLoaded, pTimeline, &TimeLine::onObjectLoaded);
    connect(pEditor, &Editor::updateTimeLine, pTimeline, &TimeLine::updateUI);

    connect(pEditor->layers(), &LayerManager::currentLayerChanged, mToolOptions, &ToolOptionWidget::updateUI);
}

void MainWindow2::makeConnections(Editor* editor, DisplayOptionWidget* display)
{
    display->makeConnectionToEditor(editor);
}

void MainWindow2::makeConnections(Editor* editor, ToolOptionWidget* toolOptions)
{
    toolOptions->makeConnectionToEditor(editor);
}

void MainWindow2::makeConnections(Editor* pEditor, ColorPaletteWidget* pColorPalette)
{
    connect(pEditor, &Editor::objectLoaded, pColorPalette, &ColorPaletteWidget::updateUI);

    ColorManager* pColorManager = pEditor->color();
    ScribbleArea* pScribbleArea = pEditor->getScribbleArea();

    connect(pColorPalette, &ColorPaletteWidget::colorChanged, pColorManager, &ColorManager::setColor);
    connect(pColorPalette, &ColorPaletteWidget::colorNumberChanged, pColorManager, &ColorManager::setColorNumber);

    connect(pColorPalette, &ColorPaletteWidget::colorChanged, pScribbleArea, &ScribbleArea::paletteColorChanged);

    connect(pColorManager, &ColorManager::colorChanged, pColorPalette, &ColorPaletteWidget::setColor);
    connect(pColorManager, &ColorManager::colorNumberChanged, pColorPalette, &ColorPaletteWidget::selectColorNumber);
}

void MainWindow2::bindActionWithSetting(QAction* action, SETTING setting)
{
    PreferenceManager* prefs = mEditor->preference();

    // set initial state
    action->setChecked(prefs->isOn(setting));

    // 2-way binding
    connect(action, &QAction::triggered, prefs, [=](bool b)
    {
        prefs->set(setting, b);
    });

    connect(prefs, &PreferenceManager::optionChanged, action, [=](SETTING s)
    {
        if (s == setting)
        {
            action->setChecked(prefs->isOn(setting));
        }
    });
}

void MainWindow2::updateZoomLabel()
{
    float zoom = mEditor->view()->scaling() * 100.f;
    statusBar()->showMessage(QString("Zoom: %0%1").arg(zoom, 0, 'f', 1).arg("%"));
}

void MainWindow2::changePlayState(bool isPlaying)
{
    if (isPlaying)
    {
        ui->actionPlay->setText(tr("Stop"));
        ui->actionPlay->setIcon(mStopIcon);
    }
    else
    {
        ui->actionPlay->setText(tr("Play"));
        ui->actionPlay->setIcon(mStartIcon);
    }
    update();
}
