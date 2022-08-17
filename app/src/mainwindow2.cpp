/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2008-2009 Mj Mendoza IV
Copyright (C) 2012-2020 Matthew Chiawen Chang

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

// Qt headers
#include <QDir>
#include <QList>
#include <QMenu>
#include <QFile>
#include <QMessageBox>
#include <QProgressDialog>
#include <QTabletEvent>
#include <QStandardPaths>
#include <QDateTime>
#include <QLabel>
#include <QClipboard>
#include <QToolBar>

// core_lib headers
#include "pencildef.h"
#include "pencilsettings.h"
#include "object.h"
#include "editor.h"

#include "filemanager.h"
#include "colormanager.h"
#include "layermanager.h"
#include "toolmanager.h"
#include "playbackmanager.h"
#include "selectionmanager.h"
#include "soundmanager.h"
#include "viewmanager.h"
#include "selectionmanager.h"

#include "actioncommands.h"
#include "fileformat.h"     //contains constants used by Pencil File Format
#include "util.h"
#include "backupelement.h"

// app headers
#include "colorbox.h"
#include "colorinspector.h"
#include "colorpalettewidget.h"
#include "displayoptionwidget.h"
#include "tooloptionwidget.h"
#include "preferencesdialog.h"
#include "timeline.h"
#include "toolbox.h"
#include "onionskinwidget.h"
#include "pegbaralignmentdialog.h"
#include "repositionframesdialog.h"

//#include "preview.h"
//#include "timeline2.h"
#include "errordialog.h"
#include "filedialog.h"
#include "importimageseqdialog.h"
#include "importlayersdialog.h"
#include "importpositiondialog.h"
#include "layeropacitydialog.h"
#include "recentfilemenu.h"
#include "shortcutfilter.h"
#include "app_util.h"
#include "presetdialog.h"
#include "pegbaralignmentdialog.h"


#ifdef GIT_TIMESTAMP
#define BUILD_DATE S__GIT_TIMESTAMP
#else
#define BUILD_DATE __DATE__
#endif

#if defined(PENCIL2D_RELEASE_BUILD)
#define PENCIL_WINDOW_TITLE QString("[*]Pencil2D v%1").arg(APP_VERSION)
#elif defined(PENCIL2D_NIGHTLY_BUILD)
#define PENCIL_WINDOW_TITLE QString("[*]Pencil2D Nightly Build %1").arg(BUILD_DATE)
#else
#define PENCIL_WINDOW_TITLE QString("[*]Pencil2D Development Build %1").arg(BUILD_DATE)
#endif



MainWindow2::MainWindow2(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow2)
{
    ui->setupUi(this);

    // Initialize order
    // 1. editor 2. object 3. scribble area 4. other widgets
    mEditor = new Editor(this);
    mEditor->setScribbleArea(ui->scribbleArea);
    mEditor->init();

    newObject();

    ui->scribbleArea->setEditor(mEditor);
    ui->scribbleArea->init();

    ui->statusBar->setEditor(mEditor);
    ui->statusBar->updateZoomStatus();
    ui->statusBar->setVisible(mEditor->preference()->isOn(SETTING::SHOW_STATUS_BAR));

    mCommands = new ActionCommands(this);
    mCommands->setCore(mEditor);

    createDockWidgets();
    createMenus();
    createToolbars();
    setupKeyboardShortcuts();

    readSettings();

    selectionChanged();

    connect(mEditor, &Editor::needSave, this, &MainWindow2::autoSave);
    connect(mToolBox, &ToolBoxWidget::clearButtonClicked, mEditor, &Editor::clearCurrentFrame);

    mEditor->tools()->setDefaultTool();
    ui->background->init(mEditor->preference());

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

    mColorBox = new ColorBox(this);
    mColorBox->setToolTip(tr("color palette:<br>use <b>(C)</b><br>toggle at cursor"));
    mColorBox->setObjectName("ColorWheel");

    mColorInspector = new ColorInspector(this);
    mColorInspector->setToolTip(tr("Color inspector"));
    mColorInspector->setObjectName("Color Inspector");

    mColorPalette = new ColorPaletteWidget(this);
    mColorPalette->setCore(mEditor);
    mColorPalette->setObjectName("ColorPalette");

    mDisplayOptionWidget = new DisplayOptionWidget(this);
    mDisplayOptionWidget->setObjectName("DisplayOption");

    mOnionSkinWidget = new OnionSkinWidget(this);
    mOnionSkinWidget->setObjectName("Onion Skin");

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
        << mColorBox
        << mColorInspector
        << mColorPalette
        << mDisplayOptionWidget
        << mOnionSkinWidget
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

    addDockWidget(Qt::RightDockWidgetArea, mColorBox);
    addDockWidget(Qt::RightDockWidgetArea, mColorInspector);
    addDockWidget(Qt::RightDockWidgetArea, mColorPalette);
    addDockWidget(Qt::LeftDockWidgetArea, mToolBox);
    addDockWidget(Qt::LeftDockWidgetArea, mToolOptions);
    addDockWidget(Qt::LeftDockWidgetArea, mDisplayOptionWidget);
    addDockWidget(Qt::LeftDockWidgetArea, mOnionSkinWidget);
    addDockWidget(Qt::BottomDockWidgetArea, mTimeLine);
    setDockNestingEnabled(true);

    /*
    mPreview = new PreviewWidget( this );
    mPreview->setImage( mScribbleArea->mBufferImg );
    mPreview->setFeatures( QDockWidget::DockWidgetFloatable );
    mPreview->setFocusPolicy( Qt::NoFocus );
    addDockWidget( Qt::RightDockWidgetArea, mPreview );
    */

    makeConnections(mEditor, ui->scribbleArea);
    makeConnections(mEditor);
    makeConnections(mEditor, mTimeLine);
    makeConnections(mEditor, mColorBox);
    makeConnections(mEditor, mColorInspector);
    makeConnections(mEditor, mColorPalette);
    makeConnections(mEditor, mToolOptions);
    makeConnections(mEditor, mDisplayOptionWidget);
    makeConnections(mEditor, ui->statusBar);

    for (BaseDockWidget* w : mDockWidgets)
    {
        w->updateUI();
        w->setFloating(false);
    }
}

void MainWindow2::createMenus()
{
    //--- File Menu ---
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow2::newDocument);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow2::openDocument);
    connect(ui->actionSave_as, &QAction::triggered, this, &MainWindow2::saveAsNewDocument);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow2::saveDocument);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow2::close);

    //--- Export Menu ---
    connect(ui->actionExport_Image, &QAction::triggered, mCommands, &ActionCommands::exportImage);
    connect(ui->actionExport_ImageSeq, &QAction::triggered, mCommands, &ActionCommands::exportImageSequence);
    connect(ui->actionExport_Movie, &QAction::triggered, mCommands, &ActionCommands::exportMovie);
    connect(ui->actionExport_Animated_GIF, &QAction::triggered, mCommands, &ActionCommands::exportGif);

    connect(ui->actionExport_Palette, &QAction::triggered, this, &MainWindow2::exportPalette);

    //--- Import Menu ---
    //connect( ui->actionExport_Svg_Image, &QAction::triggered, editor, &Editor::saveSvg );
    connect(ui->actionImport_Image, &QAction::triggered, this, &MainWindow2::importImage);
    connect(ui->actionImport_ImageSeq, &QAction::triggered, this, &MainWindow2::importImageSequence);
    connect(ui->actionImport_ImageSeqNum, &QAction::triggered, this, &MainWindow2::importPredefinedImageSet);
    connect(ui->actionImportLayers_from_pclx, &QAction::triggered, this, &MainWindow2::importLayers);
    connect(ui->actionImport_MovieVideo, &QAction::triggered, this, &MainWindow2::importMovieVideo);
    connect(ui->actionImport_Gif, &QAction::triggered, this, &MainWindow2::importGIF);

    connect(ui->actionImport_Sound, &QAction::triggered, [=] { mCommands->importSound(FileType::SOUND); });
    connect(ui->actionImport_MovieAudio, &QAction::triggered, [=] { mCommands->importSound(FileType::MOVIE); });

    connect(ui->actionImport_Append_Palette, &QAction::triggered, this, &MainWindow2::importPalette);
    connect(ui->actionImport_Replace_Palette, &QAction::triggered, this, &MainWindow2::openPalette);

    //--- Edit Menu ---
    connect(mEditor, &Editor::updateBackup, this, &MainWindow2::undoActSetText);
    connect(ui->actionUndo, &QAction::triggered, mEditor, &Editor::undo);
    connect(ui->actionRedo, &QAction::triggered, mEditor, &Editor::redo);
    connect(ui->actionCut, &QAction::triggered, mEditor, &Editor::copyAndCut);
    connect(ui->actionCopy, &QAction::triggered, mEditor, &Editor::copy);
    connect(ui->actionPaste_Previous, &QAction::triggered, mEditor, &Editor::pasteFromPreviousFrame);
    connect(ui->actionPaste, &QAction::triggered, mEditor, &Editor::paste);
    connect(ui->actionClearFrame, &QAction::triggered, mEditor, &Editor::clearCurrentFrame);
    connect(mEditor->select(), &SelectionManager::selectionChanged, this, &MainWindow2::selectionChanged);
    connect(ui->actionFlip_X, &QAction::triggered, mCommands, &ActionCommands::flipSelectionX);
    connect(ui->actionFlip_Y, &QAction::triggered, mCommands, &ActionCommands::flipSelectionY);
    connect(ui->actionPegbarAlignment, &QAction::triggered, this, &MainWindow2::openPegAlignDialog);
    connect(ui->actionSelect_All, &QAction::triggered, mCommands, &ActionCommands::selectAll);
    connect(ui->actionDeselect_All, &QAction::triggered, mCommands, &ActionCommands::deselectAll);
    connect(ui->actionReposition_Selected_Frames, &QAction::triggered, this, &MainWindow2::openRepositionDialog);
    connect(ui->actionPreference, &QAction::triggered, [=] { preferences(); });

    //--- Layer Menu ---
    connect(ui->actionNew_Bitmap_Layer, &QAction::triggered, mCommands, &ActionCommands::addNewBitmapLayer);
    connect(ui->actionNew_Vector_Layer, &QAction::triggered, mCommands, &ActionCommands::addNewVectorLayer);
    connect(ui->actionNew_Sound_Layer, &QAction::triggered, mCommands, &ActionCommands::addNewSoundLayer);
    connect(ui->actionNew_Camera_Layer, &QAction::triggered, mCommands, &ActionCommands::addNewCameraLayer);
    connect(ui->actionDelete_Current_Layer, &QAction::triggered, mCommands, &ActionCommands::deleteCurrentLayer);
    connect(ui->actionChangeLineColorCurrent_keyframe, &QAction::triggered, mCommands, &ActionCommands::changeKeyframeLineColor);
    connect(ui->actionChangeLineColorAll_keyframes_on_layer, &QAction::triggered, mCommands, &ActionCommands::changeallKeyframeLineColor);
    connect(ui->actionChangeLayerOpacity, &QAction::triggered, this, &MainWindow2::openLayerOpacityDialog);

    QList<QAction*> visibilityActions = ui->menuLayer_Visibility->actions();
    auto visibilityGroup = new QActionGroup(this);
    visibilityGroup->setExclusive(true);
    for (int i = 0; i < visibilityActions.size(); i++) {
        QAction* action = visibilityActions[i];
        visibilityGroup->addAction(action);
        connect(action, &QAction::triggered, [=] { mCommands->setLayerVisibilityIndex(i); });
    }
    visibilityActions[mEditor->preference()->getInt(SETTING::LAYER_VISIBILITY)]->setChecked(true);
    connect(mEditor->preference(), &PreferenceManager::optionChanged, [=](SETTING e) {
        if (e == SETTING::LAYER_VISIBILITY) {
            visibilityActions[mEditor->preference()->getInt(SETTING::LAYER_VISIBILITY)]->setChecked(true);
        }
    });

    // --- View Menu ---
    connect(ui->actionZoom_In, &QAction::triggered, mCommands, &ActionCommands::ZoomIn);
    connect(ui->actionZoom_Out, &QAction::triggered, mCommands, &ActionCommands::ZoomOut);
    connect(ui->actionRotate_Clockwise, &QAction::triggered, mCommands, &ActionCommands::rotateClockwise);
    connect(ui->actionRotate_Anticlockwise, &QAction::triggered, mCommands, &ActionCommands::rotateCounterClockwise);
    connect(ui->actionReset_Rotation, &QAction::triggered, mEditor->view(), &ViewManager::resetRotation);
    connect(ui->actionReset_View, &QAction::triggered, mEditor->view(), &ViewManager::resetView);
    connect(ui->actionCenter_View, &QAction::triggered, mEditor->view(), &ViewManager::centerView);
    connect(ui->actionZoom400, &QAction::triggered, mEditor->view(), &ViewManager::scale400);
    connect(ui->actionZoom300, &QAction::triggered, mEditor->view(), &ViewManager::scale300);
    connect(ui->actionZoom200, &QAction::triggered, mEditor->view(), &ViewManager::scale200);
    connect(ui->actionZoom100, &QAction::triggered, mEditor->view(), &ViewManager::scale100);
    connect(ui->actionZoom50, &QAction::triggered, mEditor->view(), &ViewManager::scale50);
    connect(ui->actionZoom33, &QAction::triggered, mEditor->view(), &ViewManager::scale33);
    connect(ui->actionZoom25, &QAction::triggered, mEditor->view(), &ViewManager::scale25);
    connect(ui->actionHorizontal_Flip, &QAction::triggered, mEditor->view(), &ViewManager::flipHorizontal);
    connect(ui->actionVertical_Flip, &QAction::triggered, mEditor->view(), &ViewManager::flipVertical);
    connect(mEditor->view(), &ViewManager::viewFlipped, this, &MainWindow2::viewFlipped);

    PreferenceManager* prefs = mEditor->preference();
    connect(ui->actionStatusBar, &QAction::triggered, ui->statusBar, &QStatusBar::setVisible);
    bindPreferenceSetting(ui->actionStatusBar, prefs, SETTING::SHOW_STATUS_BAR);
    bindPreferenceSetting(ui->actionGrid, prefs, SETTING::GRID);
    bindPreferenceSetting(ui->actionOnionPrev, prefs, SETTING::PREV_ONION);
    bindPreferenceSetting(ui->actionOnionNext, prefs, SETTING::NEXT_ONION);

    //--- Animation Menu ---
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
    connect(pPlaybackManager, &PlaybackManager::playStateChanged, ui->scribbleArea, &ScribbleArea::onPlayStateChanged);
    connect(ui->actionFlip_inbetween, &QAction::triggered, pPlaybackManager, &PlaybackManager::playFlipInBetween);
    connect(ui->actionFlip_rolling, &QAction::triggered, pPlaybackManager, &PlaybackManager::playFlipRoll);

    connect(ui->actionAdd_Frame, &QAction::triggered, mCommands, &ActionCommands::insertKeyFrameAtCurrentPosition);
    connect(ui->actionRemove_Frame, &QAction::triggered, mCommands, &ActionCommands::removeKey);
    connect(ui->actionAdd_Frame_Exposure, &QAction::triggered, mCommands, &ActionCommands::addExposureToSelectedFrames);
    connect(ui->actionSubtract_Frame_Exposure, &QAction::triggered, mCommands, &ActionCommands::subtractExposureFromSelectedFrames);
    connect(ui->actionNext_Frame, &QAction::triggered, mCommands, &ActionCommands::GotoNextFrame);
    connect(ui->actionPrevious_Frame, &QAction::triggered, mCommands, &ActionCommands::GotoPrevFrame);
    connect(ui->actionNext_KeyFrame, &QAction::triggered, mCommands, &ActionCommands::GotoNextKeyFrame);
    connect(ui->actionPrev_KeyFrame, &QAction::triggered, mCommands, &ActionCommands::GotoPrevKeyFrame);
    connect(ui->actionDuplicate_Frame, &QAction::triggered, mCommands, &ActionCommands::duplicateKey);
    connect(ui->actionMove_Frame_Forward, &QAction::triggered, mCommands, &ActionCommands::moveFrameForward);
    connect(ui->actionMove_Frame_Backward, &QAction::triggered, mCommands, &ActionCommands::moveFrameBackward);

    connect(ui->actionReverse_Frames_Order, &QAction::triggered, mCommands, &ActionCommands::reverseSelectedFrames);
    connect(ui->actionRemove_Frames, &QAction::triggered, mCommands, &ActionCommands::removeSelectedFrames);

    //--- Tool Menu ---
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

    //--- Window Menu ---
    QMenu* winMenu = ui->menuWindows;
    const std::vector<QAction*> actions
    {
        mToolBox->toggleViewAction(),
        mToolOptions->toggleViewAction(),
        mColorBox->toggleViewAction(),
        mColorPalette->toggleViewAction(),
        mTimeLine->toggleViewAction(),
        mDisplayOptionWidget->toggleViewAction(),
        mColorInspector->toggleViewAction(),
        mOnionSkinWidget->toggleViewAction()
    };

    for (QAction* action : actions)
    {
        action->setMenuRole(QAction::NoRole);
        winMenu->addAction(action);
    }
    connect(ui->actionResetWindows, &QAction::triggered, this, &MainWindow2::resetAndDockAllSubWidgets);
    connect(ui->actionLockWindows, &QAction::toggled, this, &MainWindow2::lockWidgets);
    bindPreferenceSetting(ui->actionLockWindows, prefs, SETTING::LAYOUT_LOCK);

    //--- Help Menu ---
    connect(ui->actionHelp, &QAction::triggered, mCommands, &ActionCommands::help);
    connect(ui->actionQuick_Guide, &QAction::triggered, mCommands, &ActionCommands::quickGuide);
    connect(ui->actionWebsite, &QAction::triggered, mCommands, &ActionCommands::website);
    connect(ui->actionForum, &QAction::triggered, mCommands, &ActionCommands::forum);
    connect(ui->actionDiscord, &QAction::triggered, mCommands, &ActionCommands::discord);
    connect(ui->actionCheck_for_Updates, &QAction::triggered, mCommands, &ActionCommands::checkForUpdates);
    connect(ui->actionReport_Bug, &QAction::triggered, mCommands, &ActionCommands::reportbug);
    connect(ui->actionOpen_Temporary_Directory, &QAction::triggered, mCommands, &ActionCommands::openTemporaryDirectory);
    connect(ui->actionAbout, &QAction::triggered, mCommands, &ActionCommands::about);

    //--- Menus ---
    mRecentFileMenu = new RecentFileMenu(tr("Open Recent"), this);
    mRecentFileMenu->loadFromDisk();
    ui->menuFile->insertMenu(ui->actionSave, mRecentFileMenu);

    connect(mRecentFileMenu, &RecentFileMenu::loadRecentFile, this, &MainWindow2::openFile);
}

void MainWindow2::setOpacity(int opacity)
{
    mEditor->preference()->set(SETTING::WINDOW_OPACITY, 100 - opacity);
    setWindowOpacity(opacity / 100.0);
}

void MainWindow2::updateSaveState()
{
    const bool hasUnsavedChanges = mEditor->currentBackup() != mBackupAtSave;
    setWindowModified(hasUnsavedChanges);
    ui->statusBar->updateModifiedStatus(hasUnsavedChanges);
}

void MainWindow2::openPegAlignDialog()
{
    if (mPegAlign != nullptr)
    {
        QMessageBox::information(this, nullptr,
                                 tr("Dialog is already open!"),
                                 QMessageBox::Ok);
        return;
    }

    mPegAlign = new PegBarAlignmentDialog(mEditor, this);
    mPegAlign->setAttribute(Qt::WA_DeleteOnClose);

    Qt::WindowFlags flags = mPegAlign->windowFlags();
    flags |= Qt::WindowStaysOnTopHint;
    flags &= (~Qt::WindowContextHelpButtonHint);
    mPegAlign->setWindowFlags(flags);
    mPegAlign->show();
    connect(mPegAlign, &PegBarAlignmentDialog::finished, [=]
    {
        mPegAlign = nullptr;
    });
}

void MainWindow2::openLayerOpacityDialog()
{
    if (mLayerOpacityDialog != nullptr)
    {
        QMessageBox::information(this, nullptr,
                                 tr("Dialog is already open!"),
                                 QMessageBox::Ok);
        return;
    }
    mLayerOpacityDialog = new LayerOpacityDialog(this);
    mLayerOpacityDialog->setAttribute(Qt::WA_DeleteOnClose);
    mLayerOpacityDialog->setCore(mEditor);
    mLayerOpacityDialog->initUI();
    mLayerOpacityDialog->setWindowFlags(mLayerOpacityDialog->windowFlags() | Qt::WindowStaysOnTopHint);
    mLayerOpacityDialog->show();

    connect(mLayerOpacityDialog, &LayerOpacityDialog::finished, [=]
    {
        mLayerOpacityDialog = nullptr;
    });
}

void MainWindow2::openRepositionDialog()
{
    if (mEditor->layers()->currentLayer()->getSelectedFramesByPos().count() < 2)
    {
        QMessageBox::information(this, nullptr,
                                 tr("Please select at least 2 frames!"),
                                 QMessageBox::Ok);
        return;
    }
    if (mReposDialog != nullptr)
    {
        return;
    }

    mReposDialog = new RepositionFramesDialog(this);
    mReposDialog->setAttribute(Qt::WA_DeleteOnClose);
    mReposDialog->setWindowFlag(Qt::WindowStaysOnTopHint);
    hideQuestionMark(*mReposDialog);
    mReposDialog->setCore(mEditor);
    mReposDialog->initUI();
    mEditor->tools()->setCurrentTool(ToolType::MOVE);
    connect(mReposDialog, &RepositionFramesDialog::finished, this, &MainWindow2::closeRepositionDialog);
    mReposDialog->show();
}

void MainWindow2::closeRepositionDialog()
{
    selectionChanged();
    mReposDialog = nullptr;
}

void MainWindow2::currentLayerChanged()
{
    bool isBitmap = (mEditor->layers()->currentLayer()->type() == Layer::BITMAP);
    ui->menuChange_line_color->setEnabled(isBitmap);
}

void MainWindow2::selectionChanged()
{
    bool somethingSelected = mEditor->select()->somethingSelected();
    ui->menuSelection->setEnabled(somethingSelected);
}

void MainWindow2::viewFlipped()
{
    ui->actionHorizontal_Flip->setChecked(mEditor->view()->isFlipHorizontal());
    ui->actionVertical_Flip->setChecked(mEditor->view()->isFlipVertical());
}

void MainWindow2::closeEvent(QCloseEvent* event)
{
    if (m2ndCloseEvent)
    {
        // https://bugreports.qt.io/browse/QTBUG-43344
        event->accept();
        return;
    }

    if (!maybeSave())
    {
        event->ignore();
        return;
    }
    writeSettings();
    event->accept();
    m2ndCloseEvent = true;
}

void MainWindow2::tabletEvent(QTabletEvent* event)
{
    event->ignore();
}

void MainWindow2::newDocument()
{
    if (maybeSave() && !tryLoadPreset())
    {
        newObject();
    }
}

void MainWindow2::openDocument()
{
    if (!maybeSave())
    {
        return;
    }
    QString fileName = FileDialog::getOpenFileName(this, FileType::ANIMATION);
    if (!fileName.isEmpty())
    {
        openObject(fileName);
    }
}

bool MainWindow2::saveAsNewDocument()
{
    QString fileName = FileDialog::getSaveFileName(this, FileType::ANIMATION);
    if (fileName.isEmpty())
    {
        return false;
    }
    return saveObject(fileName);
}

void MainWindow2::openStartupFile(const QString& filename)
{
    if (tryRecoverUnsavedProject())
    {
        return;
    }

    if (!filename.isEmpty() && openObject(filename))
    {
        return;
    }

    loadMostRecent() || tryLoadPreset();
}

void MainWindow2::openFile(const QString& filename)
{
    if (maybeSave())
    {
        openObject(filename);
    }
}

bool MainWindow2::openObject(const QString& strFilePath)
{
    QProgressDialog progress(tr("Opening document..."), tr("Abort"), 0, 100, this);
    hideQuestionMark(progress);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    Status s = mEditor->openObject(strFilePath, [&progress](int p)
    {
        progress.setValue(p);
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }, [&progress](int max)
    {
        progress.setRange(0, max);
    });

    if (!s.ok())
    {
        ErrorDialog errorDialog(s.title(), s.description(), s.details().str());
        errorDialog.exec();
        emptyDocumentWhenErrorOccurred();
        return false;
    }

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(LAST_PCLX_PATH, mEditor->object()->filePath());

    // Add to recent file list, but only if we are
    if (!mEditor->object()->filePath().isEmpty())
    {
        mRecentFileMenu->addRecentFile(mEditor->object()->filePath());
        mRecentFileMenu->saveToDisk();
    }

    closeDialogs();

    setWindowTitle(mEditor->object()->filePath().prepend("[*]"));
    setWindowModified(false);
    ui->statusBar->updateModifiedStatus(false);

    progress.setValue(progress.maximum());

    updateSaveState();
    undoActSetText();

    if (!QFileInfo(strFilePath).isWritable())
    {
        QMessageBox::warning(this, tr("Warning"),
                             tr("This program does not currently have permission to write to the file you have selected. "
                                "Please make sure you have write permission for this file before attempting to save it. "
                                "Alternatively, you can use the Save As... menu option to save to a writable location."),
                             QMessageBox::Ok);
    }

    return true;
}

bool MainWindow2::saveObject(QString strSavedFileName)
{
    QProgressDialog progress(tr("Saving document..."), tr("Abort"), 0, 100, this);
    hideQuestionMark(progress);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    mEditor->prepareSave();

    FileManager fm(this);

    connect(&fm, &FileManager::progressChanged, [&progress](int p)
    {
        progress.setValue(p);
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    });
    connect(&fm, &FileManager::progressRangeChanged, [&progress](int max)
    {
        progress.setRange(0, max + 3);
    });

    Status st = fm.save(mEditor->object(), strSavedFileName);


    if (!st.ok())
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
        QDir errorLogFolder(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
#else
        QDir errorLogFolder(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
#endif
        errorLogFolder.mkpath("./logs");
        errorLogFolder.cd("logs");

        QDateTime dt = QDateTime::currentDateTime();
        dt.setTimeSpec(Qt::UTC);
        QFile eLog(errorLogFolder.absoluteFilePath(QString("error-%1.txt").arg(dt.toString(Qt::ISODate))));
        if (eLog.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&eLog);
            out << st.details().str();
        }
        eLog.close();

        ErrorDialog errorDialog(st.title(),
                                st.description().append(tr("<br><br>An error has occurred and your file may not have saved successfully."
                                                           "If you believe that this error is an issue with Pencil2D, please create a new issue at:"
                                                           "<br><a href='https://github.com/pencil2d/pencil/issues'>https://github.com/pencil2d/pencil/issues</a><br>"
                                                           "Please be sure to include the following details in your issue:")), st.details().html());
        errorDialog.exec();
        return false;
    }

    mEditor->object()->setFilePath(strSavedFileName);
    mEditor->object()->setModified(false);

    mEditor->clearTemporary();

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(LAST_PCLX_PATH, strSavedFileName);

    mRecentFileMenu->addRecentFile(strSavedFileName);
    mRecentFileMenu->saveToDisk();

    mTimeLine->updateContent();

    setWindowTitle(strSavedFileName.prepend("[*]"));
    mBackupAtSave = mEditor->currentBackup();
    updateSaveState();

    progress.setValue(progress.maximum());

    mEditor->resetAutoSaveCounter();

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
    if (mEditor->currentBackup() == mBackupAtSave)
    {
        return true;
    }

    int ret = QMessageBox::warning(this, tr("Warning"),
                                   tr("This animation has been modified.\n Do you want to save your changes?"),
                                   QMessageBox::Discard | QMessageBox::Save | QMessageBox::Cancel);
    if (ret == QMessageBox::Save)
        return saveDocument();
    else
        return ret == QMessageBox::Discard;
}

bool MainWindow2::autoSave()
{
    if (!mEditor->object()->filePath().isEmpty())
    {
        return saveDocument();
    }

    if (mEditor->autoSaveNeverAskAgain())
        return false;

    if(mSuppressAutoSaveDialog)
        return false;

    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setWindowTitle(tr("AutoSave Reminder"));
    msgBox.setText(tr("The animation is not saved yet.\n Do you want to save now?"));
    msgBox.addButton(tr("Never ask again", "AutoSave reminder button"), QMessageBox::RejectRole);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);

    int ret = msgBox.exec();
    if (ret == QMessageBox::Yes)
    {
        return saveDocument();
    }
    if (ret != QMessageBox::No) // Never ask again
    {
        mEditor->dontAskAutoSave(true);
    }

    return false;
}

void MainWindow2::emptyDocumentWhenErrorOccurred()
{
    newObject();

    setWindowTitle(PENCIL_WINDOW_TITLE);
    updateSaveState();
}

void MainWindow2::importImage()
{
    QString strFilePath = FileDialog::getOpenFileName(this, FileType::IMAGE);

    if (strFilePath.isEmpty()) { return; }
    if (!QFile::exists(strFilePath)) { return; }

    ImportPositionDialog* positionDialog = new ImportPositionDialog(this);
    OnScopeExit(delete positionDialog)

    positionDialog->setCore(mEditor);
    positionDialog->exec();

    if (positionDialog->result() != QDialog::Accepted)
    {
        return;
    }

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

    ui->scribbleArea->updateCurrentFrame();
    mTimeLine->updateContent();
}

void MainWindow2::importImageSequence()
{
    mSuppressAutoSaveDialog = true;

    ImportImageSeqDialog* imageSeqDialog = new ImportImageSeqDialog(this);
    OnScopeExit(delete imageSeqDialog)
    imageSeqDialog->setCore(mEditor);

    connect(imageSeqDialog, &ImportImageSeqDialog::notifyAnimationLengthChanged, mEditor->layers(), &LayerManager::notifyAnimationLengthChanged);

    imageSeqDialog->exec();
    if (imageSeqDialog->result() == QDialog::Rejected)
    {
        return;
    }

    ImportPositionDialog* positionDialog = new ImportPositionDialog(this);
    OnScopeExit(delete positionDialog)

    positionDialog->setCore(mEditor);
    positionDialog->exec();
    if (positionDialog->result() != QDialog::Accepted)
    {
        return;
    }

    imageSeqDialog->importArbitrarySequence();

    mSuppressAutoSaveDialog = false;
}

void MainWindow2::importPredefinedImageSet()
{
    ImportImageSeqDialog* imageSeqDialog = new ImportImageSeqDialog(this, ImportExportDialog::Import, FileType::IMAGE, ImportCriteria::PredefinedSet);
    OnScopeExit(delete imageSeqDialog)
    imageSeqDialog->setCore(mEditor);

    connect(imageSeqDialog, &ImportImageSeqDialog::notifyAnimationLengthChanged, mEditor->layers(), &LayerManager::notifyAnimationLengthChanged);

    mSuppressAutoSaveDialog = true;
    imageSeqDialog->exec();
    if (imageSeqDialog->result() == QDialog::Rejected)
    {
        return;
    }

    ImportPositionDialog* positionDialog = new  ImportPositionDialog(this);
    OnScopeExit(delete positionDialog)

    positionDialog->setCore(mEditor);
    positionDialog->exec();
    if (positionDialog->result() != QDialog::Accepted)
    {
        return;
    }

    imageSeqDialog->importPredefinedSet();
    mSuppressAutoSaveDialog = false;
}

void MainWindow2::importLayers()
{
    ImportLayersDialog* importLayers = new ImportLayersDialog(this);
    importLayers->setCore(mEditor);
    importLayers->setAttribute(Qt::WA_DeleteOnClose);
    importLayers->open();
}

void MainWindow2::importGIF()
{
    auto gifDialog = new ImportImageSeqDialog(this, ImportExportDialog::Import, FileType::GIF);
    gifDialog->exec();
    if (gifDialog->result() == QDialog::Rejected)
    {
        return;
    }

    // Flag this so we don't prompt the user about auto-save in the middle of the import.
    mSuppressAutoSaveDialog = true;

    ImportPositionDialog* positionDialog = new  ImportPositionDialog(this);
    OnScopeExit(delete positionDialog)

    positionDialog->setCore(mEditor);
    positionDialog->exec();
    if (positionDialog->result() != QDialog::Accepted)
    {
        return;
    }

    int space = gifDialog->getSpace();

    // Show a progress dialog, as this could take a while if the gif is huge
    QProgressDialog progress(tr("Importing Animated GIF..."), tr("Abort"), 0, 100, this);
    hideQuestionMark(progress);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    QString strImgFileLower = gifDialog->getFilePath();
    bool importOK = strImgFileLower.toLower().endsWith(".gif");

    if (importOK)
    {
        bool ok = mEditor->importGIF(strImgFileLower, space);
        if (!ok)
            importOK = false;

        progress.setValue(50);
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);  // Required to make progress bar update
    }

    if (!importOK)
    {
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("was unable to import %1").arg(strImgFileLower),
                             QMessageBox::Ok,
                             QMessageBox::Ok);
    }

    progress.setValue(100);
    progress.close();

    mSuppressAutoSaveDialog = false;
}

void MainWindow2::lockWidgets(bool shouldLock)
{
    QDockWidget::DockWidgetFeatures feat = shouldLock ? QDockWidget::NoDockWidgetFeatures : QDockWidget::AllDockWidgetFeatures;

    for (QDockWidget* d : mDockWidgets)
    {
        d->setFeatures(feat);

        // https://doc.qt.io/qt-5/qdockwidget.html#setTitleBarWidget
        // A empty QWidget looks like the tittle bar is hidden.
        // nullptr means removing the custom title bar and restoring the default one
        QWidget* customTitleBarWidget = shouldLock ? (new QWidget) : nullptr;
        d->setTitleBarWidget(customTitleBarWidget);
    }
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

    connect(mPrefDialog, &PreferencesDialog::windowOpacityChange, this, &MainWindow2::setOpacity);
    connect(mPrefDialog, &PreferencesDialog::soundScrubChanged, mEditor->playback(), &PlaybackManager::setSoundScrubActive);
    connect(mPrefDialog, &PreferencesDialog::soundScrubMsecChanged, mEditor->playback(), &PlaybackManager::setSoundScrubMsec);
    connect(mPrefDialog, &PreferencesDialog::finished, [&]
    {
        clearKeyboardShortcuts();
        setupKeyboardShortcuts();
        ui->scribbleArea->updateCanvasCursor();
        mPrefDialog = nullptr;
    });

    mPrefDialog->show();
}

void MainWindow2::resetAndDockAllSubWidgets()
{
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.remove(SETTING_WINDOW_GEOMETRY);
    settings.remove(SETTING_WINDOW_STATE);

    for (BaseDockWidget* dock : mDockWidgets)
    {
        dock->setFloating(false);
        dock->raise();
        dock->show();
    }

    addDockWidget(Qt::RightDockWidgetArea, mColorBox);
    addDockWidget(Qt::RightDockWidgetArea, mColorInspector);
    addDockWidget(Qt::RightDockWidgetArea, mColorPalette);
    addDockWidget(Qt::LeftDockWidgetArea, mToolBox);
    addDockWidget(Qt::LeftDockWidgetArea, mToolOptions);
    addDockWidget(Qt::LeftDockWidgetArea, mDisplayOptionWidget);
    addDockWidget(Qt::LeftDockWidgetArea, mOnionSkinWidget);
    addDockWidget(Qt::BottomDockWidgetArea, mTimeLine);
}

void MainWindow2::newObject()
{
    auto object = new Object();
    object->init();

    // default layers
    object->addNewCameraLayer();
    object->addNewBitmapLayer();
    // Layers are counted bottom up
    // 0 - Camera Layer
    // 1 - Bitmap Layer
    object->data()->setCurrentLayer(1);

    mEditor->setObject(object);

    closeDialogs();

    setWindowTitle(PENCIL_WINDOW_TITLE);
    undoActSetText();
}

bool MainWindow2::newObjectFromPresets(int presetIndex)
{
    QString presetFilePath = PresetDialog::getPresetPath(presetIndex);

    if (presetFilePath.isEmpty())
    {
        return false;
    }

    FileManager fm(this);
    Object* object = fm.load(presetFilePath);

    if (!fm.error().ok() || object == nullptr)
    {
        return false;
    }

    mEditor->setObject(object);
    object->setFilePath(QString());

    setWindowTitle(PENCIL_WINDOW_TITLE);
    updateSaveState();
    undoActSetText();

    return true;
}

bool MainWindow2::loadMostRecent()
{
    if(!mEditor->preference()->isOn(SETTING::LOAD_MOST_RECENT))
    {
        return false;
    }

    QSettings settings(PENCIL2D, PENCIL2D);
    QString myPath = settings.value(LAST_PCLX_PATH, QVariant("")).toString();
    if (myPath.isEmpty() || !QFile::exists(myPath))
    {
        return false;
    }
    return openObject(myPath);
}

bool MainWindow2::tryLoadPreset()
{
    if (!mEditor->preference()->isOn(SETTING::ASK_FOR_PRESET))
    {
        int defaultPreset = mEditor->preference()->getInt(SETTING::DEFAULT_PRESET);
        return newObjectFromPresets(defaultPreset);
    }

    PresetDialog* presetDialog = new PresetDialog(mEditor->preference(), this);
    presetDialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(presetDialog, &PresetDialog::finished, [=](int result)
    {
        if (result != QDialog::Accepted)
        {
            return;
        }

        int presetIndex = presetDialog->getPresetIndex();
        if (presetDialog->shouldAlwaysUse())
        {
            mEditor->preference()->set(SETTING::ASK_FOR_PRESET, false);
            mEditor->preference()->set(SETTING::DEFAULT_PRESET, presetIndex);
        }
        if (!newObjectFromPresets(presetIndex))
        {
            newObject();
        }
    });
    presetDialog->open();
    return true;
}

void MainWindow2::closeDialogs()
{
    for (auto dialog : findChildren<QDialog*>(QString(), Qt::FindDirectChildrenOnly)) {
        dialog->close();
    }
}

void MainWindow2::readSettings()
{
    QSettings settings(PENCIL2D, PENCIL2D);

    QVariant winGeometry = settings.value(SETTING_WINDOW_GEOMETRY);
    restoreGeometry(winGeometry.toByteArray());

    QVariant winState = settings.value(SETTING_WINDOW_STATE);
    restoreState(winState.toByteArray());

    int opacity = mEditor->preference()->getInt(SETTING::WINDOW_OPACITY);
    setOpacity(100 - opacity);

    bool isWindowsLocked = mEditor->preference()->isOn(SETTING::LAYOUT_LOCK);
    lockWidgets(isWindowsLocked);
}

void MainWindow2::writeSettings()
{
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
    ui->actionImport_MovieVideo->setShortcut(cmdKeySeq(CMD_IMPORT_MOVIE_VIDEO));
    ui->actionImport_MovieAudio->setShortcut(cmdKeySeq(CMD_IMPORT_MOVIE_AUDIO));
    ui->actionImport_Append_Palette->setShortcut(cmdKeySeq(CMD_IMPORT_PALETTE));
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
    ui->actionPaste_Previous->setShortcut(cmdKeySeq(CMD_PASTE_FROM_PREVIOUS));
    ui->actionPaste->setShortcut(cmdKeySeq(CMD_PASTE));
    ui->actionClearFrame->setShortcut(cmdKeySeq(CMD_CLEAR_FRAME));
    ui->actionSelect_All->setShortcut(cmdKeySeq(CMD_SELECT_ALL));
    ui->actionDeselect_All->setShortcut(cmdKeySeq(CMD_DESELECT_ALL));
    ui->actionPreference->setShortcut(cmdKeySeq(CMD_PREFERENCE));

    // View menu
    ui->actionResetWindows->setShortcut(cmdKeySeq(CMD_RESET_WINDOWS));
    ui->actionReset_View->setShortcut(cmdKeySeq(CMD_RESET_ZOOM_ROTATE));
    ui->actionCenter_View->setShortcut(cmdKeySeq(CMD_CENTER_VIEW));
    ui->actionZoom_In->setShortcut(cmdKeySeq(CMD_ZOOM_IN));
    ui->actionZoom_Out->setShortcut(cmdKeySeq(CMD_ZOOM_OUT));
    ui->actionZoom400->setShortcut(cmdKeySeq(CMD_ZOOM_400));
    ui->actionZoom300->setShortcut(cmdKeySeq(CMD_ZOOM_300));
    ui->actionZoom200->setShortcut(cmdKeySeq(CMD_ZOOM_200));
    ui->actionZoom100->setShortcut(cmdKeySeq(CMD_ZOOM_100));
    ui->actionZoom50->setShortcut(cmdKeySeq(CMD_ZOOM_50));
    ui->actionZoom33->setShortcut(cmdKeySeq(CMD_ZOOM_33));
    ui->actionZoom25->setShortcut(cmdKeySeq(CMD_ZOOM_25));
    ui->actionRotate_Clockwise->setShortcut(cmdKeySeq(CMD_ROTATE_CLOCK));
    ui->actionRotate_Anticlockwise->setShortcut(cmdKeySeq(CMD_ROTATE_ANTI_CLOCK));
    ui->actionReset_Rotation->setShortcut(cmdKeySeq(CMD_RESET_ROTATION));
    ui->actionHorizontal_Flip->setShortcut(cmdKeySeq(CMD_FLIP_HORIZONTAL));
    ui->actionVertical_Flip->setShortcut(cmdKeySeq(CMD_FLIP_VERTICAL));
    ui->actionPreview->setShortcut(cmdKeySeq(CMD_PREVIEW));
    ui->actionGrid->setShortcut(cmdKeySeq(CMD_GRID));
    ui->actionOnionPrev->setShortcut(cmdKeySeq(CMD_ONIONSKIN_PREV));
    ui->actionOnionNext->setShortcut(cmdKeySeq(CMD_ONIONSKIN_NEXT));
    ui->actionStatusBar->setShortcut(cmdKeySeq(CMD_TOGGLE_STATUS_BAR));

    ui->actionPlay->setShortcut(cmdKeySeq(CMD_PLAY));
    ui->actionLoop->setShortcut(cmdKeySeq(CMD_LOOP));
    ui->actionPrevious_Frame->setShortcut(cmdKeySeq(CMD_GOTO_PREV_FRAME));
    ui->actionNext_Frame->setShortcut(cmdKeySeq(CMD_GOTO_NEXT_FRAME));
    ui->actionPrev_KeyFrame->setShortcut(cmdKeySeq(CMD_GOTO_PREV_KEY_FRAME));
    ui->actionNext_KeyFrame->setShortcut(cmdKeySeq(CMD_GOTO_NEXT_KEY_FRAME));
    ui->actionAdd_Frame->setShortcut(cmdKeySeq(CMD_ADD_FRAME));
    ui->actionDuplicate_Frame->setShortcut(cmdKeySeq(CMD_DUPLICATE_FRAME));
    ui->actionRemove_Frame->setShortcut(cmdKeySeq(CMD_REMOVE_FRAME));
    ui->actionAdd_Frame_Exposure->setShortcut(cmdKeySeq(CMD_SELECTION_ADD_FRAME_EXPOSURE));
    ui->actionSubtract_Frame_Exposure->setShortcut(cmdKeySeq(CMD_SELECTION_SUBTRACT_FRAME_EXPOSURE));
    ui->actionReverse_Frames_Order->setShortcut(cmdKeySeq(CMD_REVERSE_SELECTED_FRAMES));
    ui->actionRemove_Frames->setShortcut(cmdKeySeq(CMD_REMOVE_SELECTED_FRAMES));
    ui->actionMove_Frame_Backward->setShortcut(cmdKeySeq(CMD_MOVE_FRAME_BACKWARD));
    ui->actionMove_Frame_Forward->setShortcut(cmdKeySeq(CMD_MOVE_FRAME_FORWARD));
    ui->actionFlip_inbetween->setShortcut(cmdKeySeq(CMD_FLIP_INBETWEEN));
    ui->actionFlip_rolling->setShortcut(cmdKeySeq(CMD_FLIP_ROLLING));

    ShortcutFilter* shortcutFilter = new ShortcutFilter(ui->scribbleArea, this);
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

    ui->actionMove->installEventFilter(shortcutFilter);
    ui->actionMove->installEventFilter(shortcutFilter);
    ui->actionSelect->installEventFilter(shortcutFilter);
    ui->actionBrush->installEventFilter(shortcutFilter);
    ui->actionPolyline->installEventFilter(shortcutFilter);
    ui->actionSmudge->installEventFilter(shortcutFilter);
    ui->actionPen->installEventFilter(shortcutFilter);
    ui->actionHand->installEventFilter(shortcutFilter);
    ui->actionPencil->installEventFilter(shortcutFilter);
    ui->actionBucket->installEventFilter(shortcutFilter);
    ui->actionEyedropper->installEventFilter(shortcutFilter);
    ui->actionEraser->installEventFilter(shortcutFilter);

    ui->actionNew_Bitmap_Layer->setShortcut(cmdKeySeq(CMD_NEW_BITMAP_LAYER));
    ui->actionNew_Vector_Layer->setShortcut(cmdKeySeq(CMD_NEW_VECTOR_LAYER));
    ui->actionNew_Camera_Layer->setShortcut(cmdKeySeq(CMD_NEW_CAMERA_LAYER));
    ui->actionNew_Sound_Layer->setShortcut(cmdKeySeq(CMD_NEW_SOUND_LAYER));
    ui->actionDelete_Current_Layer->setShortcut(cmdKeySeq(CMD_DELETE_CUR_LAYER));

    ui->actionVisibilityCurrentLayerOnly->setShortcut(cmdKeySeq(CMD_CURRENT_LAYER_VISIBILITY));
    ui->actionVisibilityRelative->setShortcut(cmdKeySeq(CMD_RELATIVE_LAYER_VISIBILITY));
    ui->actionVisibilityAll->setShortcut(cmdKeySeq(CMD_ALL_LAYER_VISIBILITY));

    mToolBox->toggleViewAction()->setShortcut(cmdKeySeq(CMD_TOGGLE_TOOLBOX));
    mToolOptions->toggleViewAction()->setShortcut(cmdKeySeq(CMD_TOGGLE_TOOL_OPTIONS));
    mColorBox->toggleViewAction()->setShortcut(cmdKeySeq(CMD_TOGGLE_COLOR_WHEEL));
    mColorPalette->toggleViewAction()->setShortcut(cmdKeySeq(CMD_TOGGLE_COLOR_LIBRARY));
    mTimeLine->toggleViewAction()->setShortcut(cmdKeySeq(CMD_TOGGLE_TIMELINE));
    mDisplayOptionWidget->toggleViewAction()->setShortcut(cmdKeySeq(CMD_TOGGLE_DISPLAY_OPTIONS));
    mColorInspector->toggleViewAction()->setShortcut(cmdKeySeq(CMD_TOGGLE_COLOR_INSPECTOR));
    mOnionSkinWidget->toggleViewAction()->setShortcut(cmdKeySeq(CMD_TOGGLE_ONION_SKIN));

    ui->actionHelp->setShortcut(cmdKeySeq(CMD_HELP));
    ui->actionExit->setShortcut(cmdKeySeq(CMD_EXIT));
}

void MainWindow2::clearKeyboardShortcuts()
{
    QList<QAction*> actionList = findChildren<QAction*>();
    for (QAction* action : actionList)
    {
        action->setShortcut(QKeySequence(0));
    }
}

void MainWindow2::undoActSetText()
{
    if (mEditor->mBackupIndex < 0)
    {
        ui->actionUndo->setText(tr("Undo", "Menu item text"));
        ui->actionUndo->setEnabled(false);
    }
    else
    {
        ui->actionUndo->setText(QString("%1   %2 %3").arg(tr("Undo", "Menu item text"))
                                .arg(mEditor->mBackupIndex + 1)
                                .arg(mEditor->mBackupList.at(mEditor->mBackupIndex)->undoText));
        ui->actionUndo->setEnabled(true);
    }

    if (mEditor->mBackupIndex + 2 < mEditor->mBackupList.size())
    {
        ui->actionRedo->setText(QString("%1   %2 %3").arg(tr("Redo", "Menu item text"))
                                .arg(mEditor->mBackupIndex + 2)
                                .arg(mEditor->mBackupList.at(mEditor->mBackupIndex + 1)->undoText));
        ui->actionRedo->setEnabled(true);
    }
    else
    {
        ui->actionRedo->setText(tr("Redo", "Menu item text"));
        ui->actionRedo->setEnabled(false);
    }
}

void MainWindow2::exportPalette()
{
    QString filePath = FileDialog::getSaveFileName(this, FileType::PALETTE);
    if (!filePath.isEmpty())
    {
        mEditor->object()->exportPalette(filePath);
    }
}

void MainWindow2::importPalette()
{
    QString filePath = FileDialog::getOpenFileName(this, FileType::PALETTE);
    if (!filePath.isEmpty())
    {
        mEditor->object()->importPalette(filePath);
        mColorPalette->refreshColorList();
        mColorPalette->adjustSwatches();
        mEditor->color()->setColorNumber(0);
    }
}

void MainWindow2::openPalette()
{
    for (int i = 0; i < mEditor->object()->getColorCount(); i++)
    {
        if (!mEditor->object()->isColorInUse(i))
        {
            continue;
        }

        QMessageBox msgBox;
        msgBox.setText(tr("Opening a palette will replace the old palette.\n"
                          "Color(s) in strokes will be altered by this action!"));
        msgBox.addButton(tr("Open Palette"), QMessageBox::AcceptRole);
        msgBox.addButton(QMessageBox::Cancel);

        if (msgBox.exec() == QMessageBox::Cancel) {
            return;
        }
        break;
    }

    QString filePath = FileDialog::getOpenFileName(this, FileType::PALETTE);
    if (filePath.isEmpty())
    {
        return;
    }

    mEditor->object()->openPalette(filePath);
    mColorPalette->refreshColorList();
    mEditor->color()->setColorNumber(0);
}

void MainWindow2::makeConnections(Editor* editor)
{
    connect(editor, &Editor::updateBackup, this, &MainWindow2::updateSaveState);
    connect(editor, &Editor::needDisplayInfo, this, &MainWindow2::displayMessageBox);
    connect(editor, &Editor::needDisplayInfoNoTitle, this, &MainWindow2::displayMessageBoxNoTitle);
    connect(editor->layers(), &LayerManager::currentLayerChanged, this, &MainWindow2::currentLayerChanged);
    connect(editor->select(), &SelectionManager::selectionChanged, this, &MainWindow2::selectionChanged);
    connect(editor, &Editor::canCopyChanged, this, [=](bool canCopy) {
        ui->actionCopy->setEnabled(canCopy);
        ui->actionCut->setEnabled(canCopy);
    });
    connect(editor, &Editor::canPasteChanged, ui->actionPaste, &QAction::setEnabled);

}

void MainWindow2::makeConnections(Editor* editor, ColorBox* colorBox)
{
    connect(colorBox, &ColorBox::colorChanged, editor->color(), &ColorManager::setFrontColor);
    connect(editor->color(), &ColorManager::colorChanged, colorBox, &ColorBox::setColor);
}

void MainWindow2::makeConnections(Editor* editor, ColorInspector* colorInspector)
{
    connect(colorInspector, &ColorInspector::colorChanged, editor->color(), &ColorManager::setFrontColor);
    connect(editor->color(), &ColorManager::colorChanged, colorInspector, &ColorInspector::setColor);
}

void MainWindow2::makeConnections(Editor* editor, ScribbleArea* scribbleArea)
{
    connect(editor->tools(), &ToolManager::toolChanged, scribbleArea, &ScribbleArea::setCurrentTool);
    connect(editor->tools(), &ToolManager::toolChanged, mToolBox, &ToolBoxWidget::onToolSetActive);
    connect(editor->tools(), &ToolManager::toolPropertyChanged, scribbleArea, &ScribbleArea::updateToolCursor);


    connect(editor->layers(), &LayerManager::currentLayerChanged, scribbleArea, &ScribbleArea::onLayerChanged);
    connect(editor->layers(), &LayerManager::layerDeleted, scribbleArea, &ScribbleArea::onLayerChanged);
    connect(editor, &Editor::scrubbed, scribbleArea, &ScribbleArea::onScrubbed);
    connect(editor, &Editor::frameModified, scribbleArea, &ScribbleArea::onFrameModified);
    connect(editor, &Editor::framesModified, scribbleArea, &ScribbleArea::onFramesModified);
    connect(editor, &Editor::objectLoaded, scribbleArea, &ScribbleArea::onObjectLoaded);
    connect(editor->view(), &ViewManager::viewChanged, scribbleArea, &ScribbleArea::onViewChanged);
}

void MainWindow2::makeConnections(Editor* pEditor, TimeLine* pTimeline)
{
    PlaybackManager* pPlaybackManager = pEditor->playback();
    connect(pTimeline, &TimeLine::duplicateLayerClick, mCommands, &ActionCommands::duplicateLayer);
    connect(pTimeline, &TimeLine::duplicateKeyClick, mCommands, &ActionCommands::duplicateKey);

    connect(pTimeline, &TimeLine::soundClick, pPlaybackManager, &PlaybackManager::enableSound);
    connect(pTimeline, &TimeLine::fpsChanged, pPlaybackManager, &PlaybackManager::setFps);
    connect(pTimeline, &TimeLine::fpsChanged, pEditor, &Editor::setFps);

    connect(pTimeline, &TimeLine::insertKeyClick, mCommands, &ActionCommands::insertKeyFrameAtCurrentPosition);
    connect(pTimeline, &TimeLine::removeKeyClick, mCommands, &ActionCommands::removeKey);

    connect(pTimeline, &TimeLine::newBitmapLayer, mCommands, &ActionCommands::addNewBitmapLayer);
    connect(pTimeline, &TimeLine::newVectorLayer, mCommands, &ActionCommands::addNewVectorLayer);
    connect(pTimeline, &TimeLine::newSoundLayer, mCommands, &ActionCommands::addNewSoundLayer);
    connect(pTimeline, &TimeLine::newCameraLayer, mCommands, &ActionCommands::addNewCameraLayer);
    connect(mTimeLine, &TimeLine::playButtonTriggered, mCommands, &ActionCommands::PlayStop);

    // Clipboard state handling
    connect(QApplication::clipboard(), &QClipboard::dataChanged, mEditor, &Editor::clipboardChanged);
    connect(ui->menuEdit, &QMenu::aboutToShow, this, &MainWindow2::updateCopyCutPasteEnabled);
    connect(pTimeline, &TimeLine::selectionChanged, this, &MainWindow2::updateCopyCutPasteEnabled);
    connect(this, &MainWindow2::windowActivated, this, &MainWindow2::updateCopyCutPasteEnabled);
    connect(mEditor->select(), &SelectionManager::selectionChanged, this, &MainWindow2::updateCopyCutPasteEnabled);

    connect(pEditor->layers(), &LayerManager::currentLayerChanged, pTimeline, &TimeLine::updateUI);
    connect(pEditor->layers(), &LayerManager::layerCountChanged, pTimeline, &TimeLine::updateUI);
    connect(pEditor->layers(), &LayerManager::animationLengthChanged, pTimeline, &TimeLine::extendLength);
    connect(pEditor->sound(), &SoundManager::soundClipDurationChanged, pTimeline, &TimeLine::updateUI);

    connect(pEditor, &Editor::objectLoaded, pTimeline, &TimeLine::onObjectLoaded);
    connect(pEditor, &Editor::updateTimeLine, pTimeline, &TimeLine::updateUI);

    connect(pEditor->layers(), &LayerManager::currentLayerChanged, mToolOptions, &ToolOptionWidget::updateUI);
}

void MainWindow2::makeConnections(Editor* editor, DisplayOptionWidget* displayWidget)
{
    connect(editor->layers(), &LayerManager::currentLayerChanged, displayWidget, &DisplayOptionWidget::updateUI);
}

void MainWindow2::makeConnections(Editor*, OnionSkinWidget*)
{
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

    connect(pColorPalette, &ColorPaletteWidget::colorNumberChanged, pColorManager, &ColorManager::setColorNumber);
    connect(pColorManager, &ColorManager::colorNumberChanged, pScribbleArea, &ScribbleArea::paletteColorChanged);
    connect(pColorManager, &ColorManager::colorNumberChanged, pColorPalette, &ColorPaletteWidget::selectColorNumber);
}

void MainWindow2::makeConnections(Editor* editor, StatusBar *statusBar)
{
    connect(editor->tools(), &ToolManager::toolChanged, statusBar, &StatusBar::updateToolStatus);
    connect(editor->tools()->getTool(POLYLINE), &BaseTool::isActiveChanged, statusBar, &StatusBar::updateToolStatus);

    connect(editor->view(), &ViewManager::viewChanged, statusBar, &StatusBar::updateZoomStatus);
    connect(statusBar, &StatusBar::zoomChanged, editor->view(), &ViewManager::scale);
}

void MainWindow2::updateCopyCutPasteEnabled()
{
    bool canCopy = mEditor->canCopy();
    bool canPaste = mEditor->canPaste();

    ui->actionCopy->setEnabled(canCopy);
    ui->actionCut->setEnabled(canCopy);
    ui->actionPaste->setEnabled(canPaste);
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

void MainWindow2::importMovieVideo()
{
    // Flag this so we don't prompt the user about auto-save in the middle of the import.
    mSuppressAutoSaveDialog = true;

    mCommands->importMovieVideo();

    mSuppressAutoSaveDialog = false;
}

bool MainWindow2::event(QEvent* event)
{
    if(event->type() == QEvent::WindowActivate) {
        emit windowActivated();
        return true;
    }
    return QMainWindow::event(event);
}

void MainWindow2::displayMessageBox(const QString& title, const QString& body)
{
    QMessageBox::information(this, tr(qPrintable(title)), tr(qPrintable(body)), QMessageBox::Ok);
}

void MainWindow2::displayMessageBoxNoTitle(const QString& body)
{
    QMessageBox::information(this, nullptr, tr(qPrintable(body)), QMessageBox::Ok);
}

bool MainWindow2::tryRecoverUnsavedProject()
{
    FileManager fm;
    QStringList recoverables = fm.searchForUnsavedProjects();

    if (recoverables.empty())
    {
        return false;
    }

    QString caption = tr("Restore Project?");
    QString text = tr("Pencil2D didn't close correctly. Would you like to restore the project?");

    QString recoverPath = recoverables[0];

    QMessageBox* msgBox = new QMessageBox(this);
    msgBox->setWindowTitle(tr("Restore project"));
    msgBox->setWindowModality(Qt::ApplicationModal);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setIconPixmap(QPixmap(":/icons/logo.png"));
    msgBox->setText(QString("<h4>%1</h4>%2").arg(caption, text));
    msgBox->setInformativeText(QString("<b>%1</b>").arg(retrieveProjectNameFromTempPath(recoverPath)));
    msgBox->setStandardButtons(QMessageBox::Open | QMessageBox::Discard);
    msgBox->setProperty("RecoverPath", recoverPath);
    hideQuestionMark(*msgBox);

    connect(msgBox, &QMessageBox::finished, this, &MainWindow2::startProjectRecovery);
    msgBox->open();
    return true;
}

void MainWindow2::startProjectRecovery(int result)
{
    const QMessageBox* msgBox = dynamic_cast<QMessageBox*>(QObject::sender());
    const QString recoverPath = msgBox->property("RecoverPath").toString();

    if (result == QMessageBox::Discard)
    {
        // The user presses discard
        QDir(recoverPath).removeRecursively();
        tryLoadPreset();
        return;
    }
    Q_ASSERT(result == QMessageBox::Open);

    FileManager fm;
    Object* o = fm.recoverUnsavedProject(recoverPath);
    if (!fm.error().ok())
    {
        Q_ASSERT(o == nullptr);
        const QString title = tr("Recovery Failed.");
        const QString text = tr("Sorry! Pencil2D is unable to restore your project");
        QMessageBox::information(this, title, QString("<h4>%1</h4>%2").arg(title, text));
        return;
    }

    Q_ASSERT(o);
    mEditor->setObject(o);
    updateSaveState();
    undoActSetText();

    const QString title = tr("Recovery Succeeded!");
    const QString text = tr("Please save your work immediately to prevent loss of data");
    QMessageBox::information(this, title, QString("<h4>%1</h4>%2").arg(title, text));
}

void MainWindow2::createToolbars()
{
    mMainToolbar = addToolBar(tr("Main Toolbar"));
    mMainToolbar->setObjectName("mMainToolbar");
    mMainToolbar->addAction(ui->actionNew);
    mMainToolbar->addAction(ui->actionOpen);
    mMainToolbar->addAction(ui->actionSave);
    mMainToolbar->addSeparator();
    mMainToolbar->addAction(ui->actionUndo);
    mMainToolbar->addAction(ui->actionRedo);
    mMainToolbar->addSeparator();
    mMainToolbar->addAction(ui->actionCut);
    mMainToolbar->addAction(ui->actionCopy);
    mMainToolbar->addAction(ui->actionPaste);

    mViewToolbar = addToolBar(tr("View Toolbar"));
    mViewToolbar->setObjectName("mViewToolbar");
    mViewToolbar->addAction(ui->actionZoom_In);
    mViewToolbar->addAction(ui->actionZoom_Out);
    mViewToolbar->addAction(ui->actionReset_View);
    mViewToolbar->addAction(ui->actionHorizontal_Flip);
    mViewToolbar->addAction(ui->actionVertical_Flip);

    mOverlayToolbar = addToolBar(tr("Overlay Toolbar"));
    mOverlayToolbar->setObjectName("mOverlayToolbar");
    mOverlayToolbar->addAction(ui->actionGrid);

    mToolbars = { mMainToolbar, mViewToolbar, mOverlayToolbar };

    ui->menuWindows->addSeparator();
    QMenu* toolbarMenu = ui->menuWindows->addMenu(tr("Toolbars"));
    for (QToolBar* tb : mToolbars)
    {
        toolbarMenu->addAction(tb->toggleViewAction());
    }
}
