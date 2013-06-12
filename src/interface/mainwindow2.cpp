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

#include <QtGui>
#include <QList>
#include <QMenu>
#include "editor.h"
#include "mainwindow.h"
#include "object.h"
#include "interfaces.h"
#include "palette.h"
#include "displayoptiondockwidget.h"
#include "tooloptiondockwidget.h"

#include "mainwindow2.h"
#include "ui_mainwindow2.h"

MainWindow2::MainWindow2(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow2)
{
    ui->setupUi(this);

    object = new Object();
    object->defaultInitialisation();

    editor = new Editor(this);

    arrangePalettes();
    createMenus();

    // must run after 'arragePalettes'
    editor->setObject(object);
    editor->resetUI();

    readSettings();
}

MainWindow2::~MainWindow2()
{
    delete ui;
}

void MainWindow2::arrangePalettes()
{
    setCentralWidget(editor);

    m_colorPalette = new Palette(editor);
    m_colorPalette->setFocusPolicy(Qt::NoFocus);

    m_displayOptionWidget = new DisplayOptionDockWidget(this);
    m_displayOptionWidget->makeConnectionToEditor(editor);

    m_toolOptionWidget = new ToolOptionDockWidget(this);
    m_toolOptionWidget->makeConnectionToEditor(editor);

    addDockWidget(Qt::RightDockWidgetArea, m_colorPalette);
    addDockWidget(Qt::RightDockWidgetArea, m_displayOptionWidget);
    addDockWidget(Qt::LeftDockWidgetArea, editor->toolSet->drawPalette);
    addDockWidget(Qt::LeftDockWidgetArea, m_toolOptionWidget);
    addDockWidget(Qt::BottomDockWidgetArea, editor->getTimeLine());

    editor->toolSet->drawPalette->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_toolOptionWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_displayOptionWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    editor->getTimeLine()->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
}

void MainWindow2::createMenus()
{
    // ---------- Actions -------------
    ui->actionExit->setShortcut(tr("Ctrl+Q"));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));

    ui->actionNew->setShortcut(tr("Ctrl+N"));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newDocument()));

    ui->actionOpen->setShortcut(tr("Ctrl+O"));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openDocument()));

    ui->actionSave_as->setShortcut(tr("Ctrl+Shift+S"));
    connect(ui->actionSave_as, SIGNAL(triggered()), editor, SLOT(saveDocument()));

    ui->actionSave->setShortcut(tr("Ctrl+S"));
    connect(ui->actionSave, SIGNAL(triggered()), editor, SLOT(saveForce()));

    ui->actionPrint->setShortcut(tr("Ctrl+P"));
    connect(ui->actionPrint, SIGNAL(triggered()), editor, SLOT(print()));

    //exportXAct = new QAction(tr("&X-Sheet..."), this);
    ui->actionExport_X_sheet->setShortcut(tr("Ctrl+Alt+X"));
    connect(ui->actionExport_X_sheet , SIGNAL(triggered()), editor, SLOT(exportX()));

    //exportAct = new QAction(tr("&Image Sequence..."), this);
    ui->actionExport_Image_Sequence->setShortcut(tr("Shift+Alt+S"));
    connect(ui->actionExport_Image_Sequence, SIGNAL(triggered()), editor, SLOT(exportSeq()));

    //exportimageAct = new QAction(tr("&Image..."), this);
    ui->actionExport_Image->setShortcut(tr("Ctrl+Alt+S"));
    connect(ui->actionExport_Image, SIGNAL(triggered()), editor, SLOT(exportImage()));

    //exportMovAct = new QAction(tr("&Movie..."), this);
    ui->actionExport_Movie->setShortcut(tr("Ctrl+Alt+M"));
    connect(ui->actionExport_Movie, SIGNAL(triggered()), editor, SLOT(exportMov()));

    //exportFlashAct = new QAction(tr("&Flash/SWF..."), this);
    //exportFlashAct->setShortcut(tr("Ctrl+Alt+F"));
    //connect(exportFlashAct, SIGNAL(triggered()), editor, SLOT(exportFlash()));

    //exportPaletteAct = new QAction(tr("Palette..."), this);
    connect(ui->actionExport_Palette, SIGNAL(triggered()), editor, SLOT(exportPalette()));

    //savesvgAct = new QAction(tr("&Svg Image"), this);
    ui->actionExport_Svg_Image->setShortcut(tr("Ctrl+I"));
    connect(ui->actionExport_Svg_Image, SIGNAL(triggered()), editor, SLOT(saveSvg()));

    importPaletteAct = new QAction(tr("Palette..."), this);
    connect(importPaletteAct, SIGNAL(triggered()), editor, SLOT(importPalette()));

    importAct = new QAction(tr("&Image..."), this);
    importAct->setShortcut(tr("Ctrl+Shift+R"));
    connect(importAct, SIGNAL(triggered()), editor, SLOT(importImage()));

    importMovAct = new QAction(tr("&Image Sequence..."), this);
    importMovAct->setShortcut(tr("Ctrl+R"));
    connect(importMovAct, SIGNAL(triggered()), editor, SLOT(importImageSequence()));

    importMovieAct = new QAction(tr("&Movie..."), this);
    importMovieAct->setShortcut(tr("Ctrl+R"));
    connect(importMovieAct, SIGNAL(triggered()), editor, SLOT(importMov()));

    importSndAct = new QAction(tr("&Sound..."), this);
    importSndAct->setShortcut(tr("Ctrl+I"));
    connect(importSndAct, SIGNAL(triggered()), editor, SLOT(importSound()));


    QAction* helpMeAct = new QAction(tr("&Help"), this);
    helpMeAct->setShortcut(tr("F1"));
    connect(helpMeAct, SIGNAL(triggered()), this, SLOT(helpBox()));

    QAction* aboutPencilAct = new QAction(tr("&About"), this);
    aboutPencilAct->setShortcut(tr("F2"));
    connect(aboutPencilAct, SIGNAL(triggered()), this, SLOT(aboutPencil()));

    undoAct = new QAction(QIcon(":icons/undo.png"), tr("Undo"), this);
    undoAct->setShortcut(tr("Ctrl+Z"));
    connect(undoAct, SIGNAL(triggered()), editor, SLOT(undo()));

    redoAct = new QAction(QIcon(":icons/redo.png"), tr("Redo"), this);
    redoAct->setShortcut(tr("Ctrl+Y"));
    redoAct->setEnabled(false);
    connect(redoAct, SIGNAL(triggered()), editor, SLOT(redo()));

    cutAct = new QAction(QIcon(":icons/cut.png"), tr("Cut"), this);
    cutAct->setShortcut(tr("Ctrl+X"));
    connect(cutAct, SIGNAL(triggered()), editor, SLOT(cut()));

    cropAct = new QAction( tr("Crop"), this);
    cropAct->setShortcut(tr("Ctrl+W"));
    connect(cropAct, SIGNAL(triggered()), editor, SLOT(crop()));

    trimAct = new QAction( tr("Crop To Selection"), this);
    trimAct->setShortcut(tr("Ctrl+T"));
    connect(trimAct, SIGNAL(triggered()), editor, SLOT(croptoselect()));

    inbetweenAct = new QAction( tr("Inbetween Frames"), this);
    inbetweenAct->setShortcut(tr("Alt+Z"));
    connect(inbetweenAct, SIGNAL(triggered()), editor, SLOT(inbetween()));

    inbetweenActV = new QAction( tr("Inbetween Vector Frames"), this);
    inbetweenActV->setShortcut(tr("Alt+Y"));
    connect(inbetweenActV, SIGNAL(triggered()), editor, SLOT(inbetweenV()));

    copyAct = new QAction(QIcon(":icons/copy.png"), tr("Copy"), this);
    copyAct->setShortcut(tr("Ctrl+C"));
    connect(copyAct, SIGNAL(triggered()), editor, SLOT(copy()));

    copyframesAct = new QAction( tr("Blank Frames"), this);
    copyframesAct->setShortcut(tr("Ctrl+F"));
    connect(copyframesAct, SIGNAL(triggered()), editor, SLOT(copyFrames()));

    pasteAct = new QAction(QIcon(":icons/paste.png"), tr("Paste"), this);
    pasteAct->setShortcut(tr("Ctrl+V"));
    connect(pasteAct, SIGNAL(triggered()), editor, SLOT(paste()));

    pasteframesAct = new QAction( tr("Duplicate Frames"), this);
    pasteframesAct->setShortcut(tr("Ctrl+U"));
    connect(pasteframesAct, SIGNAL(triggered()), editor, SLOT(pasteFrames()));

    deleteAct = new QAction(QIcon(":icons/clear.png"), tr("Delete"), this);
    deleteAct->setShortcut(tr("Ctrl+D"));
    connect(deleteAct, SIGNAL(triggered()), editor, SLOT(clear_clicked()));

    selectAllAct = new QAction(tr("Select All"), this);
    selectAllAct->setShortcut(tr("Ctrl+A"));
    connect(selectAllAct, SIGNAL(triggered()), editor, SIGNAL(selectAll()));

    deselectAllAct = new QAction(tr("Deselect All"), this);
    deselectAllAct->setShortcut(tr("Ctrl+Shift+A"));
    connect(deselectAllAct, SIGNAL(triggered()), editor, SLOT(deselectAll()));


    preferencesAct = new QAction(tr("Preferences"), this);
    connect(preferencesAct, SIGNAL(triggered()), editor, SLOT(showPreferences()));

    newBitmapLayerAct = new QAction(QIcon(":icons/layer-bitmap.png"), tr("New Bitmap Layer"), this);
    connect(newBitmapLayerAct, SIGNAL(triggered()), editor, SLOT(newBitmapLayer()));

    newVectorLayerAct = new QAction(QIcon(":icons/layer-vector.png"), tr("New Vector Layer"), this);
    connect(newVectorLayerAct, SIGNAL(triggered()), editor, SLOT(newVectorLayer()));

    newSoundLayerAct = new QAction(QIcon(":icons/layer-sound.png"), tr("New Sound Layer"), this);
    connect(newSoundLayerAct, SIGNAL(triggered()), editor, SLOT(newSoundLayer()));

    newCameraLayerAct = new QAction(QIcon(":icons/layer-camera.png"), tr("New Camera Layer"), this);
    connect(newCameraLayerAct, SIGNAL(triggered()), editor, SLOT(newCameraLayer()));

    deleteLayerAct = new QAction(tr("Delete Current Layer"), this);
    connect(deleteLayerAct, SIGNAL(triggered()), editor, SLOT(deleteCurrentLayer()));


    /*~~~~~~~~View menu~~~~~~~~~*/
    zoomAct = new QAction(QIcon(":icons/magnify.png"),tr("In"), this);
    zoomAct->setShortcut(Qt::Key_Up+ Qt::CTRL);
    connect(zoomAct, SIGNAL(triggered()), editor, SLOT(setzoom()));

    zoomAct1 = new QAction(QIcon(":icons/magnify.png"),tr("Out"), this);
    zoomAct1->setShortcut(Qt::Key_Down+ Qt::CTRL);
    connect(zoomAct1, SIGNAL(triggered()), editor, SLOT(setzoom1()));

    rotateAct = new QAction(tr("Clockwise"), this);
    rotateAct->setShortcut(Qt::Key_R);
    connect(rotateAct, SIGNAL(triggered()), editor, SLOT(rotatecw()));

    rotateAct1 = new QAction(tr("AntiClockwise"), this);
    rotateAct1->setShortcut(Qt::Key_Z);
    connect(rotateAct1, SIGNAL(triggered()), editor, SLOT(rotateacw()));
    resetpaletteAct = new QAction(tr("Reset Windows"), this);
    resetpaletteAct->setShortcut(Qt::Key_H + Qt::CTRL);
    connect(resetpaletteAct, SIGNAL(triggered()), this, SLOT(dockAllPalettes()));

    horiMirrorAct = new QAction(QIcon(":icons/mirror.png"),tr("Horizontal Flip"), this);
    horiMirrorAct->setShortcut(Qt::Key_H + Qt::SHIFT);
    connect(horiMirrorAct, SIGNAL(triggered()), editor, SLOT(toggleMirror()));

    vertiMirrorAct = new QAction(QIcon(":icons/mirrorV.png"),tr("Vertical Flip"), this);
    vertiMirrorAct->setShortcut(Qt::Key_V + Qt::SHIFT);
    connect(vertiMirrorAct, SIGNAL(triggered()), editor, SLOT(toggleMirrorV()));

    previewAct = new QAction(tr("Preview"), this);
    previewAct->setShortcut(Qt::Key_P + Qt::ALT);
    previewAct->setEnabled(true);
    //#	connect(previewAct, SIGNAL(triggered()), editor, SLOT(getCameraLayer()));//TODO Preview view
    connect(previewAct, SIGNAL(triggered()), editor, SLOT(addcolorbutton()));

    gridAct = new QAction(tr("Grid"), this);
    gridAct->setShortcut(Qt::Key_G);
    gridAct->setEnabled(false);
    //#	connect(gridAct, SIGNAL(triggered()), editor, SLOT(gridview()));//TODO Grid view

    onionskinpAct = new QAction(QIcon(":icons/onionPrev.png"),tr("Previous"), this);
    onionskinpAct->setShortcut(Qt::Key_O);
    onionskinpAct->setCheckable(true);
    connect(onionskinpAct, SIGNAL(triggered(bool)), editor, SIGNAL(toggleOnionPrev(bool)));
    connect(editor, SIGNAL(onionPrevChanged(bool)), onionskinpAct, SLOT(setChecked(bool)));

    onionskinnAct = new QAction(QIcon(":icons/onionNext.png"),tr("Next"), this);
    onionskinnAct->setShortcut(Qt::Key_O+Qt::ALT);
    onionskinnAct->setCheckable(true);
    connect(onionskinnAct, SIGNAL(triggered(bool)), editor, SIGNAL(toggleOnionNext(bool)));
    connect(editor, SIGNAL(onionNextChanged(bool)), onionskinnAct, SLOT(setChecked(bool)));

    /*~~~~Animation Menu~~~~~~*/
    playAnimationAct = new QAction(tr("Play/Stop"), this);
    playAnimationAct->setShortcut(Qt::Key_Return);
    connect(playAnimationAct, SIGNAL(triggered()), editor, SLOT(play()));

    loopAnimationAct = new QAction(tr("&Loop"), this);
    loopAnimationAct->setCheckable(true);
    loopAnimationAct->setShortcut(tr("Ctrl+L"));
    connect(loopAnimationAct, SIGNAL(triggered(bool)), editor, SLOT(setLoop(bool)));
    connect(loopAnimationAct, SIGNAL(toggled(bool)), editor, SIGNAL(toggleLoop(bool)));
    connect(editor, SIGNAL(loopToggled(bool)), this, SLOT(toggleLoop(bool)));

    extendFrameAct = new QAction(tr("&Extend Frame"), this);
    extendFrameAct->setShortcut(Qt::Key_F5);
    extendFrameAct->setEnabled(false);
    //connect(extendFrameAct, SIGNAL(triggered()), editor, SLOT(addFrame(editor->currentFrame)));

    addFrameAct = new QAction(QIcon(":icons/add.png"), tr("&Add Frame"), this);
    addFrameAct->setShortcut(Qt::Key_F7);
    connect(addFrameAct, SIGNAL(triggered()), editor, SLOT(addKey()));

    duplicateFrameAct = new QAction(tr("&Duplicate Frame"), this);
    duplicateFrameAct->setShortcut(Qt::Key_F6);
    connect(duplicateFrameAct, SIGNAL(triggered()), editor, SLOT(duplicateKey()));

    removeFrameAct = new QAction(QIcon(":icons/remove.png"), tr("&Remove Frame"), this);
    removeFrameAct->setShortcut(tr("Shift+F5"));
    connect(removeFrameAct, SIGNAL(triggered()), editor, SLOT(removeKey()));

    nextFrameAct = new QAction(QIcon(":icons/next.png"), tr("&Next Frame"), this);
    nextFrameAct->setShortcut(Qt::Key_Period);
    connect(nextFrameAct, SIGNAL(triggered()), editor, SLOT(playNextFrame()));

    prevFrameAct = new QAction(QIcon(":icons/prev.png"), tr("&Previous Frame"), this);
    prevFrameAct->setShortcut(Qt::Key_Comma);
    connect(prevFrameAct, SIGNAL(triggered()), editor, SLOT(playPrevFrame()));

    /*~~~~Tools Menu~~~~~~*/
    moveToolAct = new QAction(QIcon(":icons/arrow.png"),tr("Move"), this);
    moveToolAct->setShortcut(Qt::Key_Q);
    connect(moveToolAct, SIGNAL(triggered()), editor, SLOT(move_clicked()));

    clearToolAct = new QAction(QIcon(":icons/clear.png"), tr("Clear"), this);
    clearToolAct->setShortcut(Qt::Key_L);
    connect(clearToolAct, SIGNAL(triggered()), editor, SLOT(clear_clicked()));

    selectToolAct = new QAction(QIcon(":icons/select.png"),tr("Select"), this);
    selectToolAct->setShortcut(Qt::Key_V);
    connect(selectToolAct, SIGNAL(triggered()), editor, SLOT(select_clicked()));

    brushToolAct = new QAction(QIcon(":icons/brush.png"),tr("Brush"), this);
    brushToolAct->setShortcut(Qt::Key_B);
    connect(brushToolAct, SIGNAL(triggered()), editor, SLOT(color_clicked()));

    polylineToolAct = new QAction(QIcon(":icons/polyline.png"),tr("Polyline"), this);
    polylineToolAct->setShortcut(Qt::Key_Y);
    connect(polylineToolAct, SIGNAL(triggered()), editor, SLOT(polyline_clicked()));

    smudgeToolAct = new QAction(QIcon(":icons/smudge.png"),tr("Smudge"), this);
    smudgeToolAct->setShortcut(Qt::Key_A);
    connect(smudgeToolAct, SIGNAL(triggered()), editor, SLOT(smudge_clicked()));

    penToolAct = new QAction(QIcon(":icons/pen.png"),tr("Pen"), this);
    penToolAct->setShortcut(Qt::Key_P);
    connect(penToolAct, SIGNAL(triggered()), editor, SLOT(pen_clicked()));

    handToolAct = new QAction(QIcon(":icons/hand.png"),tr("Hand"), this);
    handToolAct->setShortcut(Qt::Key_H);
    connect(handToolAct, SIGNAL(triggered()), editor, SLOT(hand_clicked()));

    zoomToolAct = new QAction(tr("Reset Zoom/Rotate"), this);
    zoomToolAct->setShortcut(Qt::Key_H + Qt::CTRL);
    connect(zoomToolAct, SIGNAL(triggered()), editor, SLOT(hand_clicked()));

    pencilToolAct = new QAction(QIcon(":icons/pencil2.png"),tr("Pencil"), this);
    pencilToolAct->setShortcut(Qt::Key_N);
    connect(pencilToolAct, SIGNAL(triggered()), editor, SLOT(pencil_clicked()));

    bucketToolAct = new QAction(QIcon(":icons/bucket.png"),tr("Paintbucket"), this);
    bucketToolAct->setShortcut(Qt::Key_K);
    connect(bucketToolAct, SIGNAL(triggered()), editor, SLOT(bucket_clicked()));

    eyedropToolAct = new QAction(QIcon(":icons/eyedropper.png"),tr("Eyedropper"), this);
    eyedropToolAct->setShortcut(Qt::Key_I);
    connect(eyedropToolAct, SIGNAL(triggered()), editor, SLOT(eyedropper_clicked()));

    eraserToolAct = new QAction(QIcon(":icons/hand.png"),tr("Eraser"), this);
    eraserToolAct->setShortcut(Qt::Key_E);
    connect(eraserToolAct, SIGNAL(triggered()), editor, SLOT(eraser_clicked()));

    // --------------- Menus ------------------
    importMenu = new QMenu(tr("Import"), this);

    importMenu->addAction(importAct);
    importMenu->addAction(importMovAct);
    importMenu->addAction(importMovieAct);
    importMenu->addAction(importSndAct);
    importMenu->addSeparator();
    importMenu->addAction(importPaletteAct);

    openRecentMenu = new QMenu(tr("Open recent"), this);

    zoomMenu = new QMenu(tr("Zoom"), this);
    zoomMenu->addAction(zoomAct);
    zoomMenu->addAction(zoomAct1);
    rotateMenu= new QMenu(tr("Rotate"), this) ;
    rotateMenu->addAction(rotateAct);
    rotateMenu->addAction(rotateAct1);
    onionskinMenu= new QMenu(tr("Onion Skin"), this) ;
    onionskinMenu->addAction(onionskinpAct);
    onionskinMenu->addAction(onionskinnAct);
    InsertMenu= new QMenu(tr("Insert"), this) ;
    InsertMenu->addAction(copyframesAct);
    InsertMenu->addAction(pasteframesAct);
    InsertMenu->addAction(inbetweenAct);
    InsertMenu->addAction(inbetweenActV);
    MirrorMenu= new QMenu(tr("Mirror"), this) ;
    MirrorMenu->addAction(horiMirrorAct);
    MirrorMenu->addAction(vertiMirrorAct);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(resetpaletteAct);
    viewMenu->addMenu(zoomMenu);
    viewMenu->addAction(zoomToolAct);
    viewMenu->addSeparator();
    viewMenu->addMenu(rotateMenu);
    viewMenu->addMenu(MirrorMenu);
    viewMenu->addSeparator();
    viewMenu->addAction(previewAct);
    viewMenu->addSeparator();
    viewMenu->addAction(gridAct);
    viewMenu->addSeparator();
    viewMenu->addMenu(onionskinMenu);
    viewMenu->addSeparator();

    editMenu = new QMenu(tr("&Edit"), this);
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addMenu(InsertMenu);
    editMenu->addAction(deleteAct);
    editMenu->addSeparator();
    editMenu->addAction(cropAct);
    editMenu->addAction(trimAct);
    editMenu->addSeparator();
    editMenu->addAction(selectAllAct);
    editMenu->addAction(deselectAllAct);
    editMenu->addSeparator();
    editMenu->addAction(preferencesAct);
    connect(editMenu, SIGNAL(aboutToShow()), this, SLOT(undoActSetText()));
    connect(editMenu, SIGNAL(aboutToHide()), this, SLOT(undoActSetEnabled()));

    layerMenu = new QMenu(tr("&Layer"), this);
    layerMenu->addAction(newBitmapLayerAct);
    layerMenu->addAction(newVectorLayerAct);
    layerMenu->addAction(newSoundLayerAct);
    layerMenu->addAction(newCameraLayerAct);
    layerMenu->addSeparator();
    layerMenu->addAction(deleteLayerAct);

    animationMenu = new QMenu(tr("&Animation"), this);
    animationMenu->addAction(playAnimationAct);
    animationMenu->addAction(loopAnimationAct);
    animationMenu->addSeparator();
    animationMenu->addAction(nextFrameAct);
    animationMenu->addAction(prevFrameAct);
    animationMenu->addSeparator();
    animationMenu->addAction(extendFrameAct);
    animationMenu->addAction(addFrameAct);
    animationMenu->addAction(duplicateFrameAct);
    animationMenu->addAction(removeFrameAct);

    toolsMenu = new QMenu(tr("Tools"), this);
    toolsMenu->addAction(moveToolAct);
    toolsMenu->addAction(clearToolAct);
    toolsMenu->addAction(selectToolAct);
    toolsMenu->addAction(brushToolAct);
    toolsMenu->addAction(polylineToolAct);
    toolsMenu->addAction(smudgeToolAct);
    toolsMenu->addAction(penToolAct);
    toolsMenu->addAction(handToolAct);
    toolsMenu->addAction(pencilToolAct);
    toolsMenu->addAction(bucketToolAct);
    toolsMenu->addAction(eyedropToolAct);
    toolsMenu->addAction(eraserToolAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(helpMeAct);
    helpMenu->addAction(aboutPencilAct);

    m_pMenuList = new QList<QMenu*>();

    //m_pMenuList->append(fileMenu);
    m_pMenuList->append(editMenu);
    m_pMenuList->append(viewMenu);
    m_pMenuList->append(animationMenu);
    m_pMenuList->append(toolsMenu);
    m_pMenuList->append(layerMenu);
    m_pMenuList->append(helpMenu);

    foreach (QMenu* pMenu, *m_pMenuList)
    {
        menuBar()->addMenu(pMenu);
    }
}

void MainWindow2::loadPlugins()
{
    /*
     * since this is not yet really implemented, I commented it on MainWindow2() -- mj
     */

    qDebug() << "MainWindow loadplugins" << this << this->thread();
    // foreach (QObject *plugin, QPluginLoader::staticInstances()) populateMenus(plugin); // static plugins
    QDir pluginsDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS")
    {
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");
}

void MainWindow2::populateMenus(QObject* plugin)
{
    qDebug() << "MainWindow populateMenus" << this << this->thread();
    qDebug() << "MainWindow populateMenus" << plugin << plugin->thread();
    /*BrushInterface *iBrush = qobject_cast<BrushInterface *>(plugin);
    if (iBrush) addToMenu(plugin, iBrush->brushes(), brushMenu, SLOT(changeBrush()), brushActionGroup);

    ShapeInterface *iShape = qobject_cast<ShapeInterface *>(plugin);
    if (iShape) addToMenu(plugin, iShape->shapes(), shapesMenu, SLOT(insertShape()));

    FilterInterface *iFilter = qobject_cast<FilterInterface *>(plugin);
    if (iFilter) addToMenu(plugin, iFilter->filters(), filterMenu, SLOT(applyFilter()));*/

    //ExportInterface* exportPlugin = qobject_cast<ExportInterface*>(plugin);
    //if (exportPlugin) addToMenu(plugin, exportPlugin->name(), exportMenu, SLOT(exportFile()));
}

void MainWindow2::addToMenu(QObject* plugin, const QString text, QMenu* menu, const char* member, QActionGroup* actionGroup)
{
    qDebug() << "MainWindow populateMenus" << this << this->thread();
    qDebug() << "MainWindow populateMenus" << plugin << plugin->thread();
    qDebug() << "addToMenu 1";
    QAction* action = new QAction(text, plugin);
    qDebug() << "addToMenu 2";
    connect(action, SIGNAL(triggered()), this, member);
    menu->addAction(action);
    if (actionGroup)
    {
        action->setCheckable(true);
        actionGroup->addAction(action);
    }
}

void MainWindow2::exportFile()
{
    QAction* action = qobject_cast<QAction*>(sender());
    ExportInterface* exportPlugin = qobject_cast<ExportInterface*>(action->parent());
    if(exportPlugin)
    {
        //exportPlugin->exportFile();
    }
    else
    {
        qDebug() << "exportPlugin is null";
    }
    //const QImage image = iFilter->filterImage(action->text(), paintArea->image(), this);
    //paintArea->setImage(image);
}

void MainWindow2::setOpacity(int opacity)
{
    QSettings settings("Pencil","Pencil");
    settings.setValue("windowOpacity", 100-opacity);
    setWindowOpacity(opacity/100.0);
}

void MainWindow2::closeEvent(QCloseEvent* event)
{
    if (editor->maybeSave())
    {
        writeSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}


// ==== SLOT ====

void MainWindow2::newDocument()
{
    if ( editor->maybeSave() )
    {
        // default size
        Object* pObject = new Object();
        pObject->defaultInitialisation();

        editor->setObject(pObject);
        editor->resetUI();
    }
}

void MainWindow2::openDocument()
{
    if ( editor->maybeSave() )
    {
        QSettings settings("Pencil","Pencil");

        QString myPath = settings.value("lastFilePath", QVariant(QDir::homePath())).toString();
        QString fileName = QFileDialog::getOpenFileName(
                    this,
                    tr("Open File..."),
                    myPath,
                    tr("PCL (*.pcl);;Any files (*)"));

        if ( fileName.isEmpty() )
        {
            return ;
        }

        QFileInfo fileInfo(fileName);
        if( fileInfo.isDir() )
        {
            return;
        }

        bool ok = editor->openObject(fileName);
        if (!ok)
        {
            QMessageBox::warning(this, "Warning", "Pencil cannot read this file. If you want to import images, use the command import.");
            Object* pObject = new Object();
            pObject->defaultInitialisation();

            editor->setObject(pObject);
            editor->resetUI();
        }
        else
        {
            editor->updateMaxFrame();
        }

    }
}

void MainWindow2::dockAllPalettes()
{
    editor->toolSet->drawPalette->setFloating(false);
    m_toolOptionWidget->setFloating(false);
    m_displayOptionWidget->setFloating(false);
    editor->toolSet->onionPalette->setFloating(false);
    editor->getTimeLine()->setFloating(false);
    m_colorPalette->setFloating(false);
}

// ==== Key Event ====

void MainWindow2::keyPressEvent( QKeyEvent* e )
{
    switch (e->key())
    {
        //case Qt::Key_Y:
        //	editor->polyline_clicked();
        //	break;
        //case Qt::Key_B:
        //	editor->color_clicked();
        //	break;
        /*
        case Qt::Key_H:
            //editor->switchVisibilityOfLayer(editor->currentLayer);
            break;
        case Qt::Key_D:
            //editor->scrubForward();
            break;
        case Qt::Key_Right:
            //editor->scrubForward();
            break;
        case Qt::Key_S:
            //editor->scrubBackward();
            break;
        case Qt::Key_Left:
            //editor->scrubBackward();
            break;
        case Qt::Key_Up:
            //editor->previousLayer();
            break;
        case Qt::Key_Down:
            //editor->nextLayer();
            break;
        case Qt::Key_Space:
            //editor->play();addKey();
            break;
        case Qt::Key_Alt:
            //editor->altPress();
            break;
        case Qt::Key_Return:
            //editor->play();
            break;
            */
    default:
        QWidget::keyPressEvent(e);
    }
}

void MainWindow2::keyReleaseEvent( QKeyEvent* e )
{
    switch (e->key())
    {
    case Qt::Key_Alt:
        //editor->altRelease();
        break;
    default:
        QWidget::keyPressEvent(e);
    }
}

void MainWindow2::readSettings()
{
    QSettings settings("Pencil", "Pencil");
    QRect desktopRect = QApplication::desktop()->screenGeometry();
    desktopRect.adjust(80,80,-80,-80);

    QPoint pos = settings.value("editorPosition", desktopRect.topLeft() ).toPoint();
    QSize size = settings.value("editorSize", desktopRect.size() ).toSize();

    move(pos);
    resize(size);

    editor->restorePalettesSettings(true, true, true);

    QString myPath = settings.value("lastFilePath", QVariant(QDir::homePath())).toString();
    addRecentFile(myPath);

    setOpacity(100 - settings.value("windowOpacity").toInt());
}

void MainWindow2::writeSettings()
{
    QSettings settings("Pencil", "Pencil");
    settings.setValue("editorPosition", pos());
    settings.setValue("editorSize", size());

    Palette* colourPalette = m_colorPalette;
    if(colourPalette != NULL)
    {
        settings.setValue("colourPalettePosition", colourPalette->pos());
        settings.setValue("colourPaletteSize", colourPalette->size());
        settings.setValue("colourPaletteFloating", colourPalette->isFloating());
    }

    TimeLine* timelinePalette = editor->getTimeLine();
    if(timelinePalette != NULL)
    {
        settings.setValue("timelinePalettePosition", timelinePalette->pos());
        settings.setValue("timelinePaletteSize", timelinePalette->size());
        settings.setValue("timelinePaletteFloating", timelinePalette->isFloating());
    }

    QDockWidget* drawPalette = editor->toolSet->drawPalette;
    if(drawPalette != NULL)
    {
        settings.setValue("drawPalettePosition", drawPalette->pos());
        settings.setValue("drawPaletteSize", drawPalette->size());
        settings.setValue("drawPaletteFloating", drawPalette->isFloating());
    }

    QDockWidget* optionPalette = m_toolOptionWidget;
    if(optionPalette != NULL)
    {
        settings.setValue("optionPalettePosition", optionPalette->pos());
        settings.setValue("optionPaletteSize", optionPalette->size());
        settings.setValue("optionPaletteFloating", optionPalette->isFloating());
    }

    QDockWidget* displayPalette = m_displayOptionWidget;
    if(displayPalette != NULL)
    {
        settings.setValue("displayPalettePosition", displayPalette->pos());
        settings.setValue("displayPaletteSize", displayPalette->size());
        settings.setValue("displayPaletteFloating", displayPalette->isFloating());
    }

}

void MainWindow2::addRecentFile(QString filePath)
{
    QAction* openThisFileAct = new QAction(filePath, this);
    connect(openThisFileAct, SIGNAL(triggered()), editor, SLOT(openRecent()));
    openRecentMenu->addAction(openThisFileAct);
}

void MainWindow2::toggleLoop(bool checked)
{
    loopAnimationAct->setChecked(checked);
}

void MainWindow2::undoActSetText(void)
{
    if (this->editor->backupIndex < 0)
    {
        editMenu->actions().at(0)->setText("Undo");
        editMenu->actions().at(0)->setEnabled(false);
    }
    else
    {
        editMenu->actions().at(0)->setText("Undo   " + QString::number(this->editor->backupIndex+1) + " " + this->editor->backupList.at(this->editor->backupIndex)->undoText);
        editMenu->actions().at(0)->setEnabled(true);
    }

    if (this->editor->backupIndex+2 < this->editor->backupList.size())
    {
        editMenu->actions().at(1)->setText("Redo   " + QString::number(this->editor->backupIndex+2) + " " + this->editor->backupList.at(this->editor->backupIndex+1)->undoText);
        editMenu->actions().at(1)->setEnabled(true);
    }
    else
    {
        editMenu->actions().at(1)->setText("Redo");
        editMenu->actions().at(1)->setEnabled(false);
    }
}

void MainWindow2::undoActSetEnabled(void)
{
    editMenu->actions().at(0)->setEnabled(true);
    editMenu->actions().at(1)->setEnabled(true);
}

void MainWindow2::aboutPencil()
{
    QMessageBox::about(this, tr("Pencil Animation 0.5 beta (Morevna Branch)"),
                       tr("<table style='background-color: #DDDDDD' border='0'><tr><td valign='top'>"
                          "<img src=':icons/logo.png' width='318' height='123' border='0'><br></td></tr><tr><td>"
                          "Developed by: <i>Pascal Naidon</i> &  <i>Patrick Corrieri</i><br>"
                          "Patches by: <i>Mj Mendoza IV and D.F.</i><br>"
                          "Version: <b>0.5</b> (12 sep 2011)<br><br>"
                          "<b>Thanks to:</b><br>"
                          "Trolltech for the Qt libraries<br>"
                          "Roland for the Movie export functions<br>"
                          "Axel for his help with Qt<br>"
                          "Mark for his help with Qt and SVN<br><br>"
                          "<a href='http://www.pencil-animation.org'>http://www.pencil-animation.org</a><br><br>"
                          "Distributed under the <a href='http://www.gnu.org/copyleft/gpl.html'>GPL License</a>."
                          "</td></tr></table>"));
}

void MainWindow2::helpBox()
{
    qDebug() << "Open help manual.";

    QUrl url = QUrl::fromLocalFile(QDir::currentPath() + "/Help/User Manual.pdf" );
    QDesktopServices::openUrl( url );
}
