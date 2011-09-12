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
#include "editor.h"
#include "mainwindow.h"
#include "object.h"
#include <interfaces.h>


MainWindow::MainWindow() {
	editor = new Editor(this);
	//Object* object = new Object();
	//object->defaultInitialisation();
	//editor->setObject( object );
	editor->newObject();

	arrangePalettes();

	//editor->getTimeLine()->close();

	createMenus();
	//loadPlugins();
	readSettings();
}

void MainWindow::arrangePalettes() {
	setCentralWidget(editor);
	addDockWidget(Qt::RightDockWidgetArea, editor->getPalette());
	addDockWidget(Qt::RightDockWidgetArea, editor->getToolSet()->displayPalette);
	//editor->getPalette()->close();
	addDockWidget(Qt::LeftDockWidgetArea, editor->getToolSet()->drawPalette);
	addDockWidget(Qt::LeftDockWidgetArea, editor->getToolSet()->optionPalette);
	//addDockWidget(Qt::LeftDockWidgetArea, editor->getToolSet()->onionPalette);
	//addDockWidget(Qt::LeftDockWidgetArea, editor->getToolSet()->keyPalette);

	//addDockWidget(Qt::BottomDockWidgetArea, editor->getToolSet()->timePalette);
	addDockWidget(Qt::BottomDockWidgetArea, editor->getTimeLine());

	editor->getToolSet()->drawPalette->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	editor->getToolSet()->optionPalette->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	//editor->getToolSet()->keyPalette->setFeatures(QDockWidget::NoDockWidgetFeatures);
	//editor->getToolSet()->onionPalette->setFeatures(QDockWidget::NoDockWidgetFeatures);
	editor->getToolSet()->displayPalette->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	editor->getToolSet()->keyPalette->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	editor->getToolSet()->onionPalette->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	editor->getToolSet()->timePalette->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	editor->getTimeLine()->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
}

void MainWindow::createMenus() {
    // ---------- Actions -------------
    exitAct = new QAction(QIcon(":icons/exit.png"),tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    newAct = new QAction(QIcon(":icons/new.png"), tr("&New"), this);
    newAct->setShortcut(tr("Ctrl+N"));
    connect(newAct, SIGNAL(triggered()), editor, SLOT(newDocument()));

    openAct = new QAction(QIcon(":icons/open.png"), tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), editor, SLOT(openDocument()));

    saveAct = new QAction(QIcon(":icons/save.png"), tr("Save &As..."), this);
    saveAct->setShortcut(tr("Ctrl+Shift+S"));
    connect(saveAct, SIGNAL(triggered()), editor, SLOT(saveDocument()));

    savAct = new QAction(QIcon(":icons/saveas.png"), tr("&Save"), this);
    savAct->setShortcut(tr("Ctrl+S"));
    connect(savAct, SIGNAL(triggered()), editor, SLOT(saveForce()));

    print = new QAction(QIcon(":icons/printer3.png"), tr("&Print"), this);
    print->setShortcut(tr("Ctrl+P"));
    connect(print, SIGNAL(triggered()), editor, SLOT(print()));

    exportXAct = new QAction(tr("&X-Sheet..."), this);
    exportXAct->setShortcut(tr("Ctrl+Alt+X"));
    connect(exportXAct, SIGNAL(triggered()), editor, SLOT(exportX()));

    exportAct = new QAction(tr("&Image Sequence..."), this);
    exportAct->setShortcut(tr("Shift+Alt+S"));
    connect(exportAct, SIGNAL(triggered()), editor, SLOT(exportSeq()));

    exportimageAct = new QAction(tr("&Image..."), this);
    exportimageAct->setShortcut(tr("Ctrl+Alt+S"));
    connect(exportimageAct, SIGNAL(triggered()), editor, SLOT(exportImage()));


    exportMovAct = new QAction(tr("&Movie..."), this);
    exportMovAct->setShortcut(tr("Ctrl+Alt+M"));
    connect(exportMovAct, SIGNAL(triggered()), editor, SLOT(exportMov()));

    exportFlashAct = new QAction(tr("&Flash/SWF..."), this);
    exportFlashAct->setShortcut(tr("Ctrl+Alt+F"));
    connect(exportFlashAct, SIGNAL(triggered()), editor, SLOT(exportFlash()));

    exportPaletteAct = new QAction(tr("Palette..."), this);
    connect(exportPaletteAct, SIGNAL(triggered()), editor, SLOT(exportPalette()));

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

    savesvgAct = new QAction(tr("&Svg Image"), this);
    savesvgAct->setShortcut(tr("Ctrl+I"));
    connect(savesvgAct, SIGNAL(triggered()), editor, SLOT(saveSvg()));

    helpMe = new QAction(tr("&Help"), this);
    helpMe->setShortcut(tr("F1"));
    connect(helpMe, SIGNAL(triggered()), editor, SLOT(helpBox()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setShortcut(tr("F2"));
    connect(aboutAct, SIGNAL(triggered()), editor, SLOT(about()));

    //aboutQtAct = new QAction(tr("About &Qt"), this);
    //connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    undoAct = new QAction(QIcon(":icons/undo.png"), tr("Undo"), this);
    undoAct->setShortcut(tr("Ctrl+Z"));
    undoAct->setEnabled(false);
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
    deleteAct->setShortcut(tr("Ctrl+A"));
    connect(deleteAct, SIGNAL(triggered()), editor, SLOT(clear_clicked()));

    selectAllAct = new QAction(tr("Select All"), this);
    selectAllAct->setShortcut(tr("Ctrl+B"));
    connect(selectAllAct, SIGNAL(triggered()), editor, SIGNAL(selectAll()));

    deselectAllAct = new QAction(tr("Deselect All"), this);
    deselectAllAct->setShortcut(tr("Ctrl+D"));
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


    //dockAllPalettesAct = new QAction(tr("Dock All Palettes"), this);
    //connect(dockAllPalettesAct, SIGNAL(triggered()), editor, SLOT(dockAllPalettes()));

    //detachAllPalettesAct = new QAction(tr("Detach All Palettes"), this);
    //connect(detachAllPalettesAct, SIGNAL(triggered()), editor, SLOT(detachAllPalettes()));
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
    connect(resetpaletteAct, SIGNAL(triggered()), editor, SLOT(dockAllPalettes()));

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

    exportMenu = new QMenu(tr("Export"), this);
    exportMenu->addAction(exportimageAct);
    exportMenu->addAction(exportAct);
    exportMenu->addAction(exportXAct);
    exportMenu->addAction(exportMovAct);
    exportMenu->addAction(exportFlashAct);
    exportMenu->addAction(savesvgAct);
    exportMenu->addSeparator();
    exportMenu->addAction(exportPaletteAct);

    openRecentMenu = new QMenu(tr("Open recent"), this);

    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addMenu(openRecentMenu);
    fileMenu->addAction(savAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addMenu(importMenu);
    fileMenu->addMenu(exportMenu);
    fileMenu->addSeparator();
    fileMenu->addAction(print);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

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

    //windowsMenu = new QMenu(tr("Windows"), this);
    //windowsMenu->addAction(dockAllPalettesAct);
    //windowsMenu->addAction(detachAllPalettesAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(helpMe);
    helpMenu->addAction(aboutAct);
    //helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(editMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(animationMenu);
    menuBar()->addMenu(toolsMenu);
    menuBar()->addMenu(layerMenu);
    //menuBar()->addMenu(windowsMenu);
    menuBar()->addMenu(helpMenu);
}

void MainWindow::loadPlugins() {
	/*
	 * since this is not yet really implemented, I commented it on MainWindow() -- mj
	 */

	qDebug() << "MainWindow loadplugins" << this << this->thread();
	// foreach (QObject *plugin, QPluginLoader::staticInstances()) populateMenus(plugin); // static plugins
	QDir pluginsDir = QDir(qApp->applicationDirPath());
 #if defined(Q_OS_WIN)
	if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
		pluginsDir.cdUp();
 #elif defined(Q_OS_MAC)
	if (pluginsDir.dirName() == "MacOS") {
		pluginsDir.cdUp();
	}
 #endif
	pluginsDir.cd("plugins");

	qDebug() << "Plugin dir = " << pluginsDir.dirName();
	/*foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
		QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
		QObject *plugin = loader.instance();
		qDebug() << "loader " << loader.thread();
		qDebug() << "plugin " << fileName << plugin << plugin->thread();
		if (plugin) {
			plugin->moveToThread(this->thread());
			populateMenus(plugin);
			//pluginFileNames += fileName;
		}
	}*/

	//brushMenu->setEnabled(!brushActionGroup->actions().isEmpty());
	//shapesMenu->setEnabled(!shapesMenu->actions().isEmpty());
	//filterMenu->setEnabled(!filterMenu->actions().isEmpty());
}

void MainWindow::populateMenus(QObject *plugin) {
	qDebug() << "MainWindow populateMenus" << this << this->thread();
	qDebug() << "MainWindow populateMenus" << plugin << plugin->thread();
	/*BrushInterface *iBrush = qobject_cast<BrushInterface *>(plugin);
	if (iBrush) addToMenu(plugin, iBrush->brushes(), brushMenu, SLOT(changeBrush()), brushActionGroup);

	ShapeInterface *iShape = qobject_cast<ShapeInterface *>(plugin);
	if (iShape) addToMenu(plugin, iShape->shapes(), shapesMenu, SLOT(insertShape()));

	FilterInterface *iFilter = qobject_cast<FilterInterface *>(plugin);
	if (iFilter) addToMenu(plugin, iFilter->filters(), filterMenu, SLOT(applyFilter()));*/

	ExportInterface *exportPlugin = qobject_cast<ExportInterface *>(plugin);
	if (exportPlugin) addToMenu(plugin, exportPlugin->name(), exportMenu, SLOT(exportFile()));
}

void MainWindow::addToMenu(QObject *plugin, const QString text, QMenu *menu, const char *member, QActionGroup *actionGroup) {
	qDebug() << "MainWindow populateMenus" << this << this->thread();
	qDebug() << "MainWindow populateMenus" << plugin << plugin->thread();
	qDebug() << "addToMenu 1";
	QAction *action = new QAction(text, plugin);
	qDebug() << "addToMenu 2";
	connect(action, SIGNAL(triggered()), this, member);
	menu->addAction(action);
	if (actionGroup) {
		action->setCheckable(true);
		actionGroup->addAction(action);
	}
}

void MainWindow::exportFile() {
	QAction *action = qobject_cast<QAction *>(sender());
	ExportInterface *exportPlugin = qobject_cast<ExportInterface *>(action->parent());
	if(exportPlugin) {
		//exportPlugin->exportFile();
	} else {
		qDebug() << "exportPlugin is null";
	}
	//const QImage image = iFilter->filterImage(action->text(), paintArea->image(), this);
	//paintArea->setImage(image);
}

void MainWindow::setOpacity(int opacity) {
	QSettings settings("Pencil","Pencil");
	settings.setValue("windowOpacity", 100-opacity);
	setWindowOpacity(opacity/100.0);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (editor->maybeSave()) {
		writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::keyPressEvent( QKeyEvent *e ) {
	switch (e->key()) {
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

void MainWindow::keyReleaseEvent( QKeyEvent *e ) {
	switch (e->key()) {
    	case Qt::Key_Alt:
			//editor->altRelease();
	break;
		default:
	QWidget::keyPressEvent(e);
    }
}

void MainWindow::readSettings() {
	QSettings settings("Pencil", "Pencil");
	QRect desktopRect = QApplication::desktop()->screenGeometry();
	desktopRect.adjust(80,80,-80,-80);
	//QPoint pos = settings.value("editorPosition", QPoint( qMax(0, (desktopRect.width()-800)/2), qMax(0, (desktopRect.height()-600)/2) )).toPoint();
	QPoint pos = settings.value("editorPosition", desktopRect.topLeft() ).toPoint();
	//QSize size = settings.value("editorSize", QSize(800, 600)).toSize();
	QSize size = settings.value("editorSize", desktopRect.size() ).toSize();
	move(pos);
	resize(size);

	editor->restorePalettesSettings(true, true, true);

	QString myPath = settings.value("lastFilePath", QVariant(QDir::homePath())).toString();
	addRecentFile(myPath);

	setOpacity(100-settings.value("windowOpacity").toInt());
	//initialiseStyle();
}

void MainWindow::writeSettings() {
	QSettings settings("Pencil", "Pencil");
	settings.setValue("editorPosition", pos());
	settings.setValue("editorSize", size());

	Palette* colourPalette = editor->getPalette();
	if(colourPalette != NULL) {
		settings.setValue("colourPalettePosition", colourPalette->pos());
		settings.setValue("colourPaletteSize", colourPalette->size());
		settings.setValue("colourPaletteFloating", colourPalette->isFloating());
	}

	TimeLine* timelinePalette = editor->getTimeLine();
	if(timelinePalette != NULL) {
		settings.setValue("timelinePalettePosition", timelinePalette->pos());
		settings.setValue("timelinePaletteSize", timelinePalette->size());
		settings.setValue("timelinePaletteFloating", timelinePalette->isFloating());
	}

	QDockWidget* drawPalette = editor->getToolSet()->drawPalette;
	if(drawPalette != NULL) {
		settings.setValue("drawPalettePosition", drawPalette->pos());
		settings.setValue("drawPaletteSize", drawPalette->size());
		settings.setValue("drawPaletteFloating", drawPalette->isFloating());
	}

	QDockWidget* optionPalette = editor->getToolSet()->optionPalette;
	if(optionPalette != NULL) {
		settings.setValue("optionPalettePosition", optionPalette->pos());
		settings.setValue("optionPaletteSize", optionPalette->size());
		settings.setValue("optionPaletteFloating", optionPalette->isFloating());
	}

	QDockWidget* displayPalette = editor->getToolSet()->displayPalette;
	if(displayPalette != NULL)  {
		settings.setValue("displayPalettePosition", displayPalette->pos());
		settings.setValue("displayPaletteSize", displayPalette->size());
		settings.setValue("displayPaletteFloating", displayPalette->isFloating());
	}

}

void MainWindow::addRecentFile(QString filePath) {
	QAction* openThisFileAct = new QAction(filePath, this);
	connect(openThisFileAct, SIGNAL(triggered()), editor, SLOT(openRecent()));
	openRecentMenu->addAction(openThisFileAct);
}

void MainWindow::toggleLoop(bool checked) {
    loopAnimationAct->setChecked(checked);
}

void MainWindow::undoActSetText(void)
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
