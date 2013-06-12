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
    // ---------- File Menu -------------
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

    ui->actionExit->setShortcut(tr("Ctrl+Q"));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));

    /// --- Export Menu ---
    ui->actionExport_X_sheet->setShortcut(tr("Ctrl+Alt+X"));
    connect(ui->actionExport_X_sheet , SIGNAL(triggered()), editor, SLOT(exportX()));

    ui->actionExport_Image_Sequence->setShortcut(tr("Shift+Alt+S"));
    connect(ui->actionExport_Image_Sequence, SIGNAL(triggered()), editor, SLOT(exportSeq()));

    ui->actionExport_Image->setShortcut(tr("Ctrl+Alt+S"));
    connect(ui->actionExport_Image, SIGNAL(triggered()), editor, SLOT(exportImage()));

    ui->actionExport_Movie->setShortcut(tr("Ctrl+Alt+M"));
    connect(ui->actionExport_Movie, SIGNAL(triggered()), editor, SLOT(exportMov()));

    //exportFlashAct = new QAction(tr("&Flash/SWF..."), this);
    //exportFlashAct->setShortcut(tr("Ctrl+Alt+F"));
    //connect(exportFlashAct, SIGNAL(triggered()), editor, SLOT(exportFlash()));

    connect(ui->actionExport_Palette, SIGNAL(triggered()), editor, SLOT(exportPalette()));

    /// --- Import Menu ---
    ui->actionExport_Svg_Image->setShortcut(tr("Ctrl+I"));
    connect(ui->actionExport_Svg_Image, SIGNAL(triggered()), editor, SLOT(saveSvg()));

    ui->actionImport_Image->setShortcut(tr("Ctrl+Shift+R"));
    connect(ui->actionImport_Image, SIGNAL(triggered()), editor, SLOT(importImage()));

    ui->actionImport_Image_Sequence->setShortcut(tr("Ctrl+R"));
    connect(ui->actionImport_Image_Sequence, SIGNAL(triggered()), editor, SLOT(importImageSequence()));

    ui->actionImport_Movie->setShortcut(tr("Ctrl+R"));
    connect(ui->actionImport_Movie, SIGNAL(triggered()), editor, SLOT(importMov()));

    ui->actionImport_Sound->setShortcut(tr("Ctrl+I"));
    connect(ui->actionImport_Sound, SIGNAL(triggered()), editor, SLOT(importSound()));

    connect(ui->actionImport_Palette, SIGNAL(triggered()), editor, SLOT(importPalette()));

    /// --- Help Menu ---
    QAction* helpMeAct = new QAction(tr("&Help"), this);
    helpMeAct->setShortcut(tr("F1"));
    connect(helpMeAct, SIGNAL(triggered()), this, SLOT(helpBox()));

    QAction* aboutPencilAct = new QAction(tr("&About"), this);
    aboutPencilAct->setShortcut(tr("F2"));
    connect(aboutPencilAct, SIGNAL(triggered()), this, SLOT(aboutPencil()));

    /// --- Edit Menu ---
    ui->actionUndo->setShortcut(tr("Ctrl+Z"));
    connect(ui->actionUndo, SIGNAL(triggered()), editor, SLOT(undo()));

    ui->actionRedo->setShortcut(tr("Ctrl+Shift+Z"));
    ui->actionRedo->setEnabled(false);
    connect(ui->actionRedo, SIGNAL(triggered()), editor, SLOT(redo()));

    ui->actionCut->setShortcut(tr("Ctrl+X"));
    connect(ui->actionCut, SIGNAL(triggered()), editor, SLOT(cut()));

    ui->actionCopy->setShortcut(tr("Ctrl+C"));
    connect(ui->actionCopy, SIGNAL(triggered()), editor, SLOT(copy()));

    ui->actionPaste->setShortcut(tr("Ctrl+V"));
    connect(ui->actionPaste, SIGNAL(triggered()), editor, SLOT(paste()));

    ui->actionDelete->setShortcut(tr("Ctrl+D"));
    connect(ui->actionDelete, SIGNAL(triggered()), editor, SLOT(clear_clicked()));

    ui->actionCrop->setShortcut(tr("Ctrl+W"));
    connect(ui->actionCrop, SIGNAL(triggered()), editor, SLOT(crop()));

    ui->actionCrop_To_Selection->setShortcut(tr("Ctrl+T"));
    connect(ui->actionCrop_To_Selection, SIGNAL(triggered()), editor, SLOT(croptoselect()));

    ui->actionSelect_All->setShortcut(tr("Ctrl+A"));
    connect(ui->actionSelect_All, SIGNAL(triggered()), editor, SIGNAL(selectAll()));

    ui->actionDeselect_All->setShortcut(tr("Ctrl+Shift+A"));
    connect(ui->actionDeselect_All, SIGNAL(triggered()), editor, SLOT(deselectAll()));

    connect(ui->actionPreference, SIGNAL(triggered()), editor, SLOT(showPreferences()));

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


    /// --- View Menu ---
    ui->actionZoom_In->setShortcut(Qt::Key_Up+ Qt::CTRL);
    connect(ui->actionZoom_In, SIGNAL(triggered()), editor, SLOT(setzoom()));

    ui->actionZoom_Out->setShortcut(Qt::Key_Down+ Qt::CTRL);
    connect(ui->actionZoom_Out, SIGNAL(triggered()), editor, SLOT(setzoom1()));

    ui->actionRotate_Clockwise->setShortcut(Qt::Key_R);
    connect(ui->actionRotate_Clockwise, SIGNAL(triggered()), editor, SLOT(rotatecw()));

    ui->actionRotate_Anticlosewise->setShortcut(Qt::Key_Z);
    connect(ui->actionRotate_Anticlosewise, SIGNAL(triggered()), editor, SLOT(rotateacw()));

    ui->actionReset_Windows->setShortcut(Qt::Key_H + Qt::CTRL);
    connect(ui->actionReset_Windows, SIGNAL(triggered()), this, SLOT(dockAllPalettes()));

    ui->actionReset_View->setShortcut(Qt::Key_H + Qt::CTRL);
    connect(ui->actionReset_View, SIGNAL(triggered()), editor, SLOT(hand_clicked()));

    ui->actionHorizontal_Flip->setShortcut(Qt::Key_H + Qt::SHIFT);
    connect(ui->actionHorizontal_Flip, SIGNAL(triggered()), editor, SLOT(toggleMirror()));

    ui->actionVertical_Flip->setShortcut(Qt::Key_V + Qt::SHIFT);
    connect(ui->actionVertical_Flip, SIGNAL(triggered()), editor, SLOT(toggleMirrorV()));

    ui->actionPreview->setShortcut(Qt::Key_P + Qt::ALT);
    ui->actionPreview->setEnabled(false);
    //#	connect(previewAct, SIGNAL(triggered()), editor, SLOT(getCameraLayer()));//TODO: Preview view

    ui->actionGrid->setShortcut(Qt::Key_G);
    ui->actionGrid->setEnabled(false);
    //#	connect(gridAct, SIGNAL(triggered()), editor, SLOT(gridview()));//TODO: Grid view

    ui->actionOnionPrevious->setShortcut(Qt::Key_O);
    connect(ui->actionOnionPrevious, SIGNAL(triggered(bool)), editor, SIGNAL(toggleOnionPrev(bool)));
    connect(editor, SIGNAL(onionPrevChanged(bool)), ui->actionOnionPrevious, SLOT(setChecked(bool)));

    ui->actionOnionNext = new QAction(QIcon(":icons/onionNext.png"),tr("Next"), this);
    ui->actionOnionNext->setShortcut(Qt::Key_O+Qt::ALT);
    connect(ui->actionOnionNext, SIGNAL(triggered(bool)), editor, SIGNAL(toggleOnionNext(bool)));
    connect(editor, SIGNAL(onionNextChanged(bool)), ui->actionOnionNext, SLOT(setChecked(bool)));

    /// --- Animation Menu ---
    ui->actionPlay->setShortcut(Qt::Key_Return);
    connect(ui->actionPlay, SIGNAL(triggered()), editor, SLOT(play()));

    ui->actionLoop->setShortcut(tr("Ctrl+L"));
    connect(ui->actionLoop, SIGNAL(triggered(bool)), editor, SLOT(setLoop(bool)));
    connect(ui->actionLoop, SIGNAL(toggled(bool)), editor, SIGNAL(toggleLoop(bool))); //TODO: WTF?
    connect(editor, SIGNAL(loopToggled(bool)), ui->actionLoop, SLOT(setChecked(bool)));

    ui->actionExtend_Frame->setShortcut(Qt::Key_F5);
    ui->actionExtend_Frame->setEnabled(false);
    //connect(extendFrameAct, SIGNAL(triggered()), editor, SLOT(addFrame(editor->currentFrame)));

    ui->actionAdd_Frame->setShortcut(Qt::Key_F7);
    connect(ui->actionAdd_Frame, SIGNAL(triggered()), editor, SLOT(addKey()));

    ui->actionRemove_Frame->setShortcut(tr("Shift+F5"));
    connect(ui->actionRemove_Frame, SIGNAL(triggered()), editor, SLOT(removeKey()));

    ui->actionNext_Frame->setShortcut(Qt::Key_Period);
    connect(ui->actionNext_Frame, SIGNAL(triggered()), editor, SLOT(playNextFrame()));

    ui->actionPrevious_Frame->setShortcut(Qt::Key_Comma);
    connect(ui->actionPrevious_Frame, SIGNAL(triggered()), editor, SLOT(playPrevFrame()));

    ui->actionDuplicate_Frame->setShortcut(Qt::Key_F6);
    connect(ui->actionDuplicate_Frame, SIGNAL(triggered()), editor, SLOT(duplicateKey()));

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

    openRecentMenu = new QMenu(tr("Open recent"), this);

    connect(ui->menuEdit, SIGNAL(aboutToShow()), this, SLOT(undoActSetText()));
    connect(ui->menuEdit, SIGNAL(aboutToHide()), this, SLOT(undoActSetEnabled()));

    layerMenu = new QMenu(tr("&Layer"), this);
    layerMenu->addAction(newBitmapLayerAct);
    layerMenu->addAction(newVectorLayerAct);
    layerMenu->addAction(newSoundLayerAct);
    layerMenu->addAction(newCameraLayerAct);
    layerMenu->addSeparator();
    layerMenu->addAction(deleteLayerAct);

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

    m_pMenuList->append(ui->menuFile);
    m_pMenuList->append(ui->menuEdit);
    m_pMenuList->append(ui->menuView);
    m_pMenuList->append(ui->menuAnimation);
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

void MainWindow2::undoActSetText(void)
{
    if (this->editor->backupIndex < 0)
    {
        ui->actionUndo->setText("Undo");
        ui->actionUndo->setEnabled(false);
    }
    else
    {
        ui->actionUndo->setText("Undo   " + QString::number(this->editor->backupIndex+1) + " " + this->editor->backupList.at(this->editor->backupIndex)->undoText);
        ui->actionUndo->setEnabled(true);
    }

    if (this->editor->backupIndex+2 < this->editor->backupList.size())
    {
        ui->actionRedo->setText("Redo   " + QString::number(this->editor->backupIndex+2) + " " + this->editor->backupList.at(this->editor->backupIndex+1)->undoText);
        ui->actionRedo->setEnabled(true);
    }
    else
    {
        ui->actionRedo->setText("Redo");
        ui->actionRedo->setEnabled(false);
    }
}

void MainWindow2::undoActSetEnabled(void)
{
    ui->actionUndo->setEnabled(true);
    ui->actionRedo->setEnabled(true);
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
