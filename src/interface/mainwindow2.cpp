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
#include "layersound.h"
#include "scribblearea.h"
#include "interfaces.h"
#include "palette.h"
#include "displayoptiondockwidget.h"
#include "tooloptiondockwidget.h"
#include "preferences.h"
#include "timeline.h"

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
    m_pScribbleArea = editor->getScribbleArea();
    m_pTimeLine = editor->getTimeLine();

    arrangePalettes();
    createMenus();

    // must run after 'arragePalettes'
    editor->setObject(object);
    editor->resetUI();

    readSettings();

    m_pPreferences = new Preferences(this);
    makePreferenceConnections();

    connect(editor, SIGNAL(needSave()), this, SLOT(saveForce()));
}

MainWindow2::~MainWindow2()
{
    delete ui;
}

void MainWindow2::makePreferenceConnections()
{
    connect(m_pPreferences, SIGNAL(windowOpacityChange(int)), this, SLOT(setOpacity(int)));
    connect(m_pPreferences, SIGNAL(curveOpacityChange(int)), m_pScribbleArea, SLOT(setCurveOpacity(int)));
    connect(m_pPreferences, SIGNAL(curveSmoothingChange(int)), m_pScribbleArea, SLOT(setCurveSmoothing(int)));
    connect(m_pPreferences, SIGNAL(highResPositionChange(int)), m_pScribbleArea, SLOT(setHighResPosition(int)));
    connect(m_pPreferences, SIGNAL(antialiasingChange(int)), m_pScribbleArea, SLOT(setAntialiasing(int)));
    connect(m_pPreferences, SIGNAL(gradientsChange(int)), m_pScribbleArea, SLOT(setGradients(int)));
    connect(m_pPreferences, SIGNAL(backgroundChange(int)), m_pScribbleArea, SLOT(setBackground(int)));
    connect(m_pPreferences, SIGNAL(shadowsChange(int)), m_pScribbleArea, SLOT(setShadows(int)));
    connect(m_pPreferences, SIGNAL(toolCursorsChange(int)), m_pScribbleArea, SLOT(setToolCursors(int)));
    connect(m_pPreferences, SIGNAL(styleChange(int)), m_pScribbleArea, SLOT(setStyle(int)));

    connect(m_pPreferences, SIGNAL(autosaveChange(int)), this, SLOT(changeAutosave(int)));
    connect(m_pPreferences, SIGNAL(autosaveNumberChange(int)), editor, SLOT(changeAutosaveNumber(int)));

    connect(m_pPreferences, SIGNAL(lengthSizeChange(QString)), m_pTimeLine, SIGNAL(lengthChange(QString)));
    connect(m_pPreferences, SIGNAL(fontSizeChange(int)), m_pTimeLine, SIGNAL(fontSizeChange(int)));
    connect(m_pPreferences, SIGNAL(frameSizeChange(int)), m_pTimeLine, SIGNAL(frameSizeChange(int)));
    connect(m_pPreferences, SIGNAL(labelChange(int)), m_pTimeLine, SIGNAL(labelChange(int)));
    connect(m_pPreferences, SIGNAL(scrubChange(int)), m_pTimeLine, SIGNAL(scrubChange(int)));

    connect(m_pPreferences, SIGNAL(onionLayer1OpacityChange(int)), editor, SLOT(onionLayer1OpacityChangeSlot(int)));
    connect(m_pPreferences, SIGNAL(onionLayer2OpacityChange(int)), editor, SLOT(onionLayer2OpacityChangeSlot(int)));
    connect(m_pPreferences, SIGNAL(onionLayer3OpacityChange(int)), editor, SLOT(onionLayer3OpacityChangeSlot(int)));
    connect(m_pPreferences, SIGNAL(windowOpacityChange(int)), editor, SLOT(setOpacity(int)));
    connect(m_pPreferences, SIGNAL(curveOpacityChange(int)), m_pScribbleArea, SLOT(setCurveOpacity(int)));
    connect(m_pPreferences, SIGNAL(curveSmoothingChange(int)), m_pScribbleArea, SLOT(setCurveSmoothing(int)));
    connect(m_pPreferences, SIGNAL(highResPositionChange(int)), m_pScribbleArea, SLOT(setHighResPosition(int)));
    connect(m_pPreferences, SIGNAL(antialiasingChange(int)), m_pScribbleArea, SLOT(setAntialiasing(int)));
    connect(m_pPreferences, SIGNAL(gradientsChange(int)), m_pScribbleArea, SLOT(setGradients(int)));
    connect(m_pPreferences, SIGNAL(backgroundChange(int)), m_pScribbleArea, SLOT(setBackground(int)));
    connect(m_pPreferences, SIGNAL(shadowsChange(int)), m_pScribbleArea, SLOT(setShadows(int)));
    connect(m_pPreferences, SIGNAL(toolCursorsChange(int)), m_pScribbleArea, SLOT(setToolCursors(int)));
    connect(m_pPreferences, SIGNAL(styleChange(int)), m_pScribbleArea, SLOT(setStyle(int)));

    connect(m_pPreferences, SIGNAL(autosaveChange(int)), editor, SLOT(changeAutosave(int)));
    connect(m_pPreferences, SIGNAL(autosaveNumberChange(int)), editor, SLOT(changeAutosaveNumber(int)));

    connect(m_pPreferences, SIGNAL(lengthSizeChange(QString)), m_pTimeLine, SIGNAL(lengthChange(QString)));
    connect(m_pPreferences, SIGNAL(fontSizeChange(int)), m_pTimeLine, SIGNAL(fontSizeChange(int)));
    connect(m_pPreferences, SIGNAL(frameSizeChange(int)), m_pTimeLine, SIGNAL(frameSizeChange(int)));
    connect(m_pPreferences, SIGNAL(labelChange(int)), m_pTimeLine, SIGNAL(labelChange(int)));
    connect(m_pPreferences, SIGNAL(scrubChange(int)), m_pTimeLine, SIGNAL(scrubChange(int)));

    connect(m_pPreferences, SIGNAL(onionLayer1OpacityChange(int)), editor, SLOT(onionLayer1OpacityChangeSlot(int)));
    connect(m_pPreferences, SIGNAL(onionLayer2OpacityChange(int)), editor, SLOT(onionLayer2OpacityChangeSlot(int)));
    connect(m_pPreferences, SIGNAL(onionLayer3OpacityChange(int)), editor, SLOT(onionLayer3OpacityChangeSlot(int)));

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
    connect(ui->actionSave_as, SIGNAL(triggered()), this, SLOT(saveAsNewDocument()));

    ui->actionSave->setShortcut(tr("Ctrl+S"));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveForce()));

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

    connect(ui->actionPreference, SIGNAL(triggered()), this, SLOT(showPreferences()));


    /// --- Layer Menu ---
    connect(ui->actionNew_Bitmap_Layer, SIGNAL(triggered()), editor, SLOT(newBitmapLayer()));

    connect(ui->actionNew_Vector_Layer, SIGNAL(triggered()), editor, SLOT(newVectorLayer()));

    connect(ui->actionNew_Sound_Layer, SIGNAL(triggered()), editor, SLOT(newSoundLayer()));

    connect(ui->actionNew_Camera_Layer, SIGNAL(triggered()), editor, SLOT(newCameraLayer()));

    connect(ui->actionDelete_Current_Layer, SIGNAL(triggered()), editor, SLOT(deleteCurrentLayer()));


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

    /// --- Tool Menu ---
    ui->actionMove->setShortcut(Qt::Key_Q);
    connect(ui->actionMove, SIGNAL(triggered()), editor, SLOT(move_clicked()));

    ui->actionClear->setShortcut(Qt::Key_L);
    connect(ui->actionClear, SIGNAL(triggered()), editor, SLOT(clear_clicked()));

    ui->actionSelect->setShortcut(Qt::Key_V);
    connect(ui->actionSelect, SIGNAL(triggered()), editor, SLOT(select_clicked()));

    ui->actionBrush->setShortcut(Qt::Key_B);
    connect(ui->actionBrush, SIGNAL(triggered()), editor, SLOT(color_clicked()));

    ui->actionPolyline->setShortcut(Qt::Key_Y);
    connect(ui->actionPolyline, SIGNAL(triggered()), editor, SLOT(polyline_clicked()));

    ui->actionSmudge->setShortcut(Qt::Key_A);
    connect(ui->actionSmudge, SIGNAL(triggered()), editor, SLOT(smudge_clicked()));

    ui->actionPen->setShortcut(Qt::Key_P);
    connect(ui->actionPen, SIGNAL(triggered()), editor, SLOT(pen_clicked()));

    ui->actionHand->setShortcut(Qt::Key_H);
    connect(ui->actionHand, SIGNAL(triggered()), editor, SLOT(hand_clicked()));

    ui->actionPencil->setShortcut(Qt::Key_N);
    connect(ui->actionPencil, SIGNAL(triggered()), editor, SLOT(pencil_clicked()));

    ui->actionBucket->setShortcut(Qt::Key_K);
    connect(ui->actionBucket, SIGNAL(triggered()), editor, SLOT(bucket_clicked()));

    ui->actionEyedropper->setShortcut(Qt::Key_I);
    connect(ui->actionEyedropper, SIGNAL(triggered()), editor, SLOT(eyedropper_clicked()));

    ui->actionEraser->setShortcut(Qt::Key_E);
    connect(ui->actionEraser, SIGNAL(triggered()), editor, SLOT(eraser_clicked()));

    /// --- Help Menu ---
    ui->actionHelp->setShortcut(tr("F1"));
    connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(helpBox()));

    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(aboutPencil()));
    
    
    // --------------- Menus ------------------
    openRecentMenu = new QMenu(tr("Open recent"), this);

    connect(ui->menuEdit, SIGNAL(aboutToShow()), this, SLOT(undoActSetText()));
    connect(ui->menuEdit, SIGNAL(aboutToHide()), this, SLOT(undoActSetEnabled()));

    m_pMenuList = new QList<QMenu*>();

    m_pMenuList->append(ui->menuFile);
    m_pMenuList->append(ui->menuEdit);
    m_pMenuList->append(ui->menuView);
    m_pMenuList->append(ui->menuAnimation);
    m_pMenuList->append(ui->menuTools);
    m_pMenuList->append(ui->menuLayer);
    m_pMenuList->append(ui->menuHelp);
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
    settings.setValue("windowOpacity", 100 - opacity);
    setWindowOpacity(opacity / 100.0);
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

bool MainWindow2::saveAsNewDocument()
{
    QSettings settings("Pencil","Pencil");
    
    QString strDefaultFileName = settings.value("lastFilePath", QVariant(QDir::homePath())).toString();
    if (strDefaultFileName.isEmpty()) 
    {
        strDefaultFileName = QDir::homePath() + "/untitled.pcl";
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As..."),strDefaultFileName ,tr("PCL (*.pcl)"));

    if (fileName.isEmpty())
    {
        return false;
    }
    else
    {
        if(! fileName.endsWith(".pcl"))
        {
            fileName =  fileName + ".pcl";
        }
        QSettings settings("Pencil","Pencil");
        settings.setValue("lastFilePath", QVariant(fileName));

        return saveObject(fileName);
    }
}

bool MainWindow2::saveObject(QString strSavedFilename)
{
    QString filePath = strSavedFilename;

    QFileInfo fileInfo(filePath);
    if(fileInfo.isDir()) return false;

    QFileInfo dataInfo(filePath+".data");
    if(!dataInfo.exists())
    {
        QDir dir(fileInfo.absolutePath()); // the directory where filePath is or will be saved
        dir.mkpath(filePath+".data"); // creates a directory with the same name +".data"
    }

    //savedName = filePath;
    this->setWindowTitle( filePath );

    QProgressDialog progress("Saving document...", "Abort", 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
    int progressValue = 0;

    // save data
    int nLayers = object->getLayerCount();
    for (int i=0; i < nLayers; i++)
    {
        Layer* layer = object->getLayer(i);
        qDebug() << "Saving Layer " << i << "(" <<layer->name << ")";
        progressValue = (i*100)/nLayers;
        progress.setValue(progressValue);
        if(layer->type == Layer::BITMAP) ((LayerBitmap*)layer)->saveImages(filePath+".data", i);
        if(layer->type == Layer::VECTOR) ((LayerVector*)layer)->saveImages(filePath+".data", i);
        if(layer->type == Layer::SOUND) ((LayerSound*)layer)->saveImages(filePath+".data", i);
    }

    // save palette
    object->savePalette(filePath+".data");

    // -------- save main XML file -----------
    QFile* file = new QFile(filePath);
    if (!file->open(QFile::WriteOnly | QFile::Text))
    {
        //QMessageBox::warning(this, "Warning", "Cannot write file");
        return false;
    }
    QTextStream out(file);
    QDomDocument doc("PencilDocument");
    QDomElement root = doc.createElement("document");
    doc.appendChild(root);

    // save editor information
    QDomElement editorElement = createDomElement(doc);
    root.appendChild(editorElement);
    // save object
    QDomElement objectElement = object->createDomElement(doc);
    root.appendChild(objectElement);

    int IndentSize = 2;
    doc.save(out, IndentSize);
    // -----------------------------------

    progress.setValue(100);

    object->modified = false;
    m_pTimeLine->updateContent();
    return true;
}

void MainWindow2::saveForce()
{
    if ( object->strCurrentFilePath != "" )
    {
        saveObject(object->strCurrentFilePath);
    }    
    else 
    {
        saveAsNewDocument();
    }
}

QDomElement MainWindow2::createDomElement(QDomDocument& doc)
{
    QDomElement tag = doc.createElement("editor");

    QDomElement tag1 = doc.createElement("currentLayer");
    tag1.setAttribute("value", editor->currentLayer);
    tag.appendChild(tag1);
    QDomElement tag2 = doc.createElement("currentFrame");
    tag2.setAttribute("value", editor->currentFrame);
    tag.appendChild(tag2);
    QDomElement tag2a = doc.createElement("currentFps");
    tag2a.setAttribute("value", editor->fps);
    tag.appendChild(tag2a);
    QDomElement tag3 = doc.createElement("currentView");
    QMatrix myView = m_pScribbleArea->getMyView();
    tag3.setAttribute("m11", myView.m11());
    tag3.setAttribute("m12", myView.m12());
    tag3.setAttribute("m21", myView.m21());
    tag3.setAttribute("m22", myView.m22());
    tag3.setAttribute("dx", myView.dx());
    tag3.setAttribute("dy", myView.dy());
    tag.appendChild(tag3);

    return tag;
}

void MainWindow2::showPreferences()
{
    m_pPreferences->show();
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
    QMessageBox::about(this, tr("Pencil Animation 0.5.0.2 beta (chchwy Branch)"),
                       tr("<table style='background-color: #DDDDDD' border='0'><tr><td valign='top'>"
                          "<img src=':icons/logo.png' width='318' height='123' border='0'><br></td></tr><tr><td>"
                          "Developed by: <i>Pascal Naidon</i> &  <i>Patrick Corrieri</i><br>"                          
                          "Version: <b>0.5.2</b> (13 June 2013)<br><br>"
                          "<b>Thanks to:</b><br>"
                          "the Qt libraries <a href='http://qt-project.org'>http://qt-project.org</a><br>"
                          "Roland for the Movie export functions<br>"
                          "Axel for his help with Qt<br>"
                          "Mark for his help with Qt and SVN<br><br>"
                          "<a href='http://pencil2d.org'>http://pencil2d.org</a><br><br>"
                          "Distributed under the <a href='http://www.gnu.org/copyleft/gpl.html'>GPL License</a>."
                          "</td></tr></table>"));
}

void MainWindow2::helpBox()
{
    qDebug() << "Open help manual.";

    QUrl url = QUrl::fromLocalFile(QDir::currentPath() + "/Help/User Manual.pdf" );
    QDesktopServices::openUrl( url );
}
