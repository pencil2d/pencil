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
#include "pencildef.h"
#include "editor.h"
#include "object.h"
#include "layersound.h"
#include "scribblearea.h"
#include "interfaces.h"
#include "palette.h"
#include "displayoptiondockwidget.h"
#include "tooloptiondockwidget.h"
#include "preferences.h"
#include "timeline.h"
#include "pencilsettings.h"
#include "colorbox.h"

#include "mainwindow2.h"
#include "ui_mainwindow2.h"


MainWindow2::MainWindow2(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow2)
{
    ui->setupUi(this);

    m_object = new Object();
    m_object->defaultInitialisation();

    editor = new Editor(this);
    m_pScribbleArea = editor->getScribbleArea();
    m_pTimeLine = new TimeLine(this, editor);
    makeTimeLineConnections();

    arrangePalettes();
    createMenus();
    loadAllShortcuts();

    // must run after 'arragePalettes'
    editor->setObject(m_object);
    editor->resetUI();

    readSettings();

    connect(editor, SIGNAL(needSave()), this, SLOT(saveDocument()));
    connect(m_toolSet, SIGNAL(clearButtonClicked()), editor, SLOT(clearCurrentFrame()));
    connect(editor, SIGNAL(changeTool(ToolType)), m_toolSet, SLOT(setCurrentTool(ToolType)));
    //showPreferences();

    ColorBox* cbox = new ColorBox(this);
    //cbox->show();

    QDockWidget* pDock = new QDockWidget(this);
    pDock->setWidget(cbox);

    //pDock->show();
    addDockWidget(Qt::RightDockWidgetArea, pDock);
}

MainWindow2::~MainWindow2()
{
    delete ui;
}

void MainWindow2::makeTimeLineConnections()
{
    connect(m_pTimeLine, SIGNAL(endplayClick()), editor, SLOT(endPlay()));
    connect(m_pTimeLine, SIGNAL(startplayClick()), editor, SLOT(startPlay()));
    connect(m_pTimeLine, SIGNAL(duplicateKeyClick()), editor, SLOT(duplicateKey()));

    connect(m_pTimeLine, SIGNAL(modification()), editor, SLOT(modification()));
    connect(m_pTimeLine, SIGNAL(addKeyClick()), editor, SLOT(addKey()));
    connect(m_pTimeLine, SIGNAL(removeKeyClick()), editor, SLOT(removeKey()));

    connect(m_pTimeLine, SIGNAL(newBitmapLayer()), editor, SLOT(newBitmapLayer()));
    connect(m_pTimeLine, SIGNAL(newVectorLayer()), editor, SLOT(newVectorLayer()));
    connect(m_pTimeLine, SIGNAL(newSoundLayer()), editor, SLOT(newSoundLayer()));
    connect(m_pTimeLine, SIGNAL(newCameraLayer()), editor, SLOT(newCameraLayer()));
    connect(m_pTimeLine, SIGNAL(deleteCurrentLayer()), editor, SLOT(deleteCurrentLayer()));

    connect(m_pTimeLine, SIGNAL(playClick()), editor, SLOT(play()));
    connect(m_pTimeLine, SIGNAL(loopClick(bool)), editor, SLOT(setLoop(bool)));
    connect(m_pTimeLine, SIGNAL(soundClick()), editor, SLOT(setSound()));
    connect(m_pTimeLine, SIGNAL(fpsClick(int)), editor, SLOT(changeFps(int)));

    connect(editor, SIGNAL(toggleLoop(bool)), m_pTimeLine, SIGNAL(toggleLoop(bool)));
    connect(m_pTimeLine, SIGNAL(loopClick(bool)), editor, SIGNAL(loopToggled(bool)));

    m_pTimeLine->setFocusPolicy(Qt::NoFocus);
}

void MainWindow2::makePreferenceConnections()
{
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

    m_toolSet = editor->toolSet;

    addDockWidget(Qt::RightDockWidgetArea, m_colorPalette);
    addDockWidget(Qt::RightDockWidgetArea, m_displayOptionWidget);
    addDockWidget(Qt::LeftDockWidgetArea, editor->toolSet);
    addDockWidget(Qt::LeftDockWidgetArea, m_toolOptionWidget);
    addDockWidget(Qt::BottomDockWidgetArea, m_pTimeLine);

    editor->toolSet->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_toolOptionWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_displayOptionWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_pTimeLine->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    m_colorPalette->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
}

void MainWindow2::createMenus()
{
    // ---------- File Menu -------------
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newDocument()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openDocument()));
    connect(ui->actionSave_as, SIGNAL(triggered()), this, SLOT(saveAsNewDocument()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveDocument()));
    connect(ui->actionPrint, SIGNAL(triggered()), editor, SLOT(print()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));

    /// --- Export Menu ---
    connect(ui->actionExport_X_sheet , SIGNAL(triggered()), editor, SLOT(exportX()));
    connect(ui->actionExport_Image_Sequence, SIGNAL(triggered()), editor, SLOT(exportSeq()));
    connect(ui->actionExport_Image, SIGNAL(triggered()), editor, SLOT(exportImage()));
    connect(ui->actionExport_Movie, SIGNAL(triggered()), editor, SLOT(exportMov()));

    //exportFlashAct = new QAction(tr("&Flash/SWF..."), this);
    //exportFlashAct->setShortcut(tr("Ctrl+Alt+F"));
    //connect(exportFlashAct, SIGNAL(triggered()), editor, SLOT(exportFlash()));

    connect(ui->actionExport_Palette, SIGNAL(triggered()), this, SLOT(exportPalette()));

    /// --- Import Menu ---
    connect(ui->actionExport_Svg_Image, SIGNAL(triggered()), editor, SLOT(saveSvg()));
    connect(ui->actionImport_Image, SIGNAL(triggered()), editor, SLOT(importImage()));
    connect(ui->actionImport_Image_Sequence, SIGNAL(triggered()), editor, SLOT(importImageSequence()));
    connect(ui->actionImport_Movie, SIGNAL(triggered()), editor, SLOT(importMov()));
    connect(ui->actionImport_Sound, SIGNAL(triggered()), editor, SLOT(importSound()));
    connect(ui->actionImport_Palette, SIGNAL(triggered()), this, SLOT(importPalette()));

    /// --- Edit Menu ---
    connect(ui->actionUndo, SIGNAL(triggered()), editor, SLOT(undo()));
    connect(ui->actionRedo, SIGNAL(triggered()), editor, SLOT(redo()));
    connect(ui->actionCut, SIGNAL(triggered()), editor, SLOT(cut()));
    connect(ui->actionCopy, SIGNAL(triggered()), editor, SLOT(copy()));
    connect(ui->actionPaste, SIGNAL(triggered()), editor, SLOT(paste()));
    connect(ui->actionDelete, SIGNAL(triggered()), editor, SLOT(clearCurrentFrame()));
    connect(ui->actionCrop, SIGNAL(triggered()), editor, SLOT(crop()));
    connect(ui->actionCrop_To_Selection, SIGNAL(triggered()), editor, SLOT(croptoselect()));
    connect(ui->actionSelect_All, SIGNAL(triggered()), editor, SIGNAL(selectAll()));
    connect(ui->actionDeselect_All, SIGNAL(triggered()), editor, SLOT(deselectAll()));
    connect(ui->actionPreference, SIGNAL(triggered()), this, SLOT(showPreferences()));

    ui->actionRedo->setEnabled(false);

    /// --- Layer Menu ---
    connect(ui->actionNew_Bitmap_Layer, SIGNAL(triggered()), editor, SLOT(newBitmapLayer()));
    connect(ui->actionNew_Vector_Layer, SIGNAL(triggered()), editor, SLOT(newVectorLayer()));
    connect(ui->actionNew_Sound_Layer, SIGNAL(triggered()), editor, SLOT(newSoundLayer()));
    connect(ui->actionNew_Camera_Layer, SIGNAL(triggered()), editor, SLOT(newCameraLayer()));
    connect(ui->actionDelete_Current_Layer, SIGNAL(triggered()), editor, SLOT(deleteCurrentLayer()));

    /// --- View Menu ---
    connect(ui->actionZoom_In, SIGNAL(triggered()), editor, SLOT(setzoom()));
    connect(ui->actionZoom_Out, SIGNAL(triggered()), editor, SLOT(setzoom1()));
    connect(ui->actionRotate_Clockwise, SIGNAL(triggered()), editor, SLOT(rotatecw()));
    connect(ui->actionRotate_Anticlosewise, SIGNAL(triggered()), editor, SLOT(rotateacw()));
    connect(ui->actionReset_Windows, SIGNAL(triggered()), this, SLOT(dockAllPalettes()));
    connect(ui->actionReset_View, SIGNAL(triggered()), editor, SLOT(resetView()));
    connect(ui->actionHorizontal_Flip, SIGNAL(triggered()), editor, SLOT(toggleMirror()));
    connect(ui->actionVertical_Flip, SIGNAL(triggered()), editor, SLOT(toggleMirrorV()));

    ui->actionPreview->setEnabled(false);
    //#	connect(previewAct, SIGNAL(triggered()), editor, SLOT(getCameraLayer()));//TODO: Preview view

    ui->actionGrid->setEnabled(false);
    connect(ui->actionGrid, SIGNAL(triggered()), editor, SLOT(gridview())); //TODO: Grid view

    connect(ui->actionOnionPrevious, SIGNAL(triggered(bool)), editor, SIGNAL(toggleOnionPrev(bool)));
    connect(editor, SIGNAL(onionPrevChanged(bool)), ui->actionOnionPrevious, SLOT(setChecked(bool)));

    connect(ui->actionOnionNext, SIGNAL(triggered(bool)), editor, SIGNAL(toggleOnionNext(bool)));
    connect(editor, SIGNAL(onionNextChanged(bool)), ui->actionOnionNext, SLOT(setChecked(bool)));

    /// --- Animation Menu ---
    connect(ui->actionPlay, SIGNAL(triggered()), editor, SLOT(play()));
    connect(ui->actionLoop, SIGNAL(triggered(bool)), editor, SLOT(setLoop(bool)));
    connect(ui->actionLoop, SIGNAL(toggled(bool)), editor, SIGNAL(toggleLoop(bool))); //TODO: WTF?
    connect(editor, SIGNAL(loopToggled(bool)), ui->actionLoop, SLOT(setChecked(bool)));

    connect(ui->actionAdd_Frame, SIGNAL(triggered()), editor, SLOT(addKey()));
    connect(ui->actionRemove_Frame, SIGNAL(triggered()), editor, SLOT(removeKey()));
    connect(ui->actionNext_Frame, SIGNAL(triggered()), editor, SLOT(playNextFrame()));
    connect(ui->actionPrevious_Frame, SIGNAL(triggered()), editor, SLOT(playPrevFrame()));
    connect(ui->actionNext_Keyframe, SIGNAL(triggered()), editor, SLOT(scrubNextKeyframe()));
    connect(ui->actionPrev_Keyframe, SIGNAL(triggered()), editor, SLOT(scrubPreviousKeyframe()));
    connect(ui->actionDuplicate_Frame, SIGNAL(triggered()), editor, SLOT(duplicateKey()));

    /// --- Tool Menu ---
    connect(ui->actionClear, SIGNAL(triggered()), editor, SLOT(clearCurrentFrame()));

    connect(ui->actionMove, SIGNAL(triggered()), m_toolSet, SLOT(moveOn()));
    connect(ui->actionSelect, SIGNAL(triggered()), m_toolSet, SLOT(selectOn()));
    connect(ui->actionBrush, SIGNAL(triggered()), m_toolSet, SLOT(brushOn()));
    connect(ui->actionPolyline, SIGNAL(triggered()), m_toolSet, SLOT(polylineOn()));
    connect(ui->actionSmudge, SIGNAL(triggered()), m_toolSet, SLOT(smudgeOn()));
    connect(ui->actionPen, SIGNAL(triggered()), m_toolSet, SLOT(penOn()));
    connect(ui->actionHand, SIGNAL(triggered()), m_toolSet, SLOT(handOn()));
    connect(ui->actionPencil, SIGNAL(triggered()), m_toolSet, SLOT(pencilOn()));
    connect(ui->actionBucket, SIGNAL(triggered()), m_toolSet, SLOT(bucketOn()));
    connect(ui->actionEyedropper, SIGNAL(triggered()), m_toolSet, SLOT(eyedropperOn()));
    connect(ui->actionEraser, SIGNAL(triggered()), m_toolSet, SLOT(eraserOn()));

    connect(ui->actionResetToolsDefault, SIGNAL(triggered()), this, SLOT(resetToolsSettings()));

    /// --- Help Menu ---
    connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(helpBox()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(aboutPencil()));

    // --------------- Menus ------------------
    openRecentMenu = new QMenu(tr("Open recent"), this);

    connect(ui->menuEdit, SIGNAL(aboutToShow()), this, SLOT(undoActSetText()));
    connect(ui->menuEdit, SIGNAL(aboutToHide()), this, SLOT(undoActSetEnabled()));


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
    if (exportPlugin)
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

        editor->setObject(m_object);
        editor->resetUI();

        setWindowTitle( PENCIL_WINDOW_TITLE );
    }
}

void MainWindow2::openDocument()
{
    if ( maybeSave() )
    {
        QSettings settings("Pencil","Pencil");

        QString myPath = settings.value("lastFilePath", QVariant(QDir::homePath())).toString();
        QString fileName = QFileDialog::getOpenFileName(
                    this,
                    tr("Open File..."),
                    myPath,
                    tr("PCL (*.pcl);;Any files (*)"));

        if (fileName.isEmpty())
        {
            return ;
        }

        QFileInfo fileInfo(fileName);
        if ( fileInfo.isDir() )
        {
            return;
        }

        bool ok = openObject(fileName);

        if (!ok)
        {
            QMessageBox::warning(this, "Warning", "Pencil cannot read this file. If you want to import images, use the command import.");
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
        if (! fileName.endsWith(".pcl"))
        {
            fileName =  fileName + ".pcl";
        }
        QSettings settings("Pencil","Pencil");
        settings.setValue("lastFilePath", QVariant(fileName));

        return saveObject(fileName);
    }
}

void MainWindow2::openRecent()
{
    QSettings settings("Pencil","Pencil");
    QString myPath = settings.value("lastFilePath", QVariant(QDir::homePath())).toString();
    bool ok = openObject(myPath);
    if ( !ok )
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

bool MainWindow2::openObject(QString filePath)
{
    // ---- test before opening ----
    QScopedPointer<QFile> file(new QFile(filePath));

    //QFile* file = new QFile(filePath);
    if (!file->open(QFile::ReadOnly))
    {
        return false;
    }

    QDomDocument doc;
    if (!doc.setContent(file.data()))
    {
        return false; // this is not a XML file
    }
    QDomDocumentType type = doc.doctype();
    if (type.name() != "PencilDocument" && type.name() != "MyObject")
    {
        return false; // this is not a Pencil document
    }

    // delete old object
    if (m_object != NULL)
    {
        m_object->deleteLater();
    }

    // -----------------------------

    QProgressDialog progress("Opening document...", "Abort", 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    //QSettings settings("Pencil","Pencil");
    //settings.setValue("lastFilePath", QVariant(object->strCurrentFilePath) );

    Object* newObject = new Object();
    if (!newObject->loadPalette(filePath+".data"))
    {
        newObject->loadDefaultPalette();
    }
    editor->setObject(newObject);

    newObject->strCurrentFilePath = filePath;

    // ------- reads the XML file -------
    bool ok = true;
    int prog = 0;
    QDomElement docElem = doc.documentElement();
    if (docElem.isNull())
    {
        return false;
    }

    if (docElem.tagName() == "document")
    {
        qDebug("Object Loader: start.");

        QDomNode tag = docElem.firstChild();
        while (!tag.isNull())
        {
            QDomElement element = tag.toElement(); // try to convert the node to an element.
            if (!element.isNull())
            {
                prog += std::min(prog + 10, 100);
                progress.setValue(prog);

                if (element.tagName() == "editor")
                {
                    qDebug("  Load editor");
                    loadDomElement(element, filePath);
                }
                else if (element.tagName() == "object")
                {
                    qDebug("  Load object");
                    ok = newObject->loadDomElement(element, filePath);
                    qDebug() << "    filePath:" << filePath;
                }
            }
            tag = tag.nextSibling();
        }
    }
    else
    {
        if (docElem.tagName() == "object" || docElem.tagName() == "MyOject")   // old Pencil format (<=0.4.3)
        {
            ok = newObject->loadDomElement(docElem, filePath);
        }
    }

    // ------------------------------
    if (ok)
    {
        editor->updateObject();

        qDebug() << "Current File Path=" << newObject->strCurrentFilePath;
        setWindowTitle(newObject->strCurrentFilePath);

        // FIXME: need to free the old object. but delete object will crash app, don't know why.
        m_object = newObject;
    }

    progress.setValue(100);
    return ok;
}

// Added here (mainWindow2) to be easily located
// TODO: Find a better place for this function
void MainWindow2::resetToolsSettings()
{
    m_pScribbleArea->resetTools();
    writeSettings();
    editor->setTool(m_pScribbleArea->currentTool()->type());
    qDebug("tools restored to default settings");
}

// TODO: need to move to other place
bool MainWindow2::loadDomElement(QDomElement docElem, QString filePath)
{
    Q_UNUSED(filePath);

    if (docElem.isNull()) return false;
    QDomNode tag = docElem.firstChild();
    while (!tag.isNull())
    {
        QDomElement element = tag.toElement(); // try to convert the node to an element.
        if (!element.isNull())
        {
            if (element.tagName() == "currentLayer")
            {
                int nCurrentLayerIndex = element.attribute("value").toInt();
                editor->setCurrentLayer(nCurrentLayerIndex);
            }
            if (element.tagName() == "currentFrame")
            {
                editor->m_nCurrentFrameIndex = element.attribute("value").toInt();
            }
            if (element.tagName() == "currentFps")
            {
                editor->fps = element.attribute("value").toInt();
                //timer->setInterval(1000/fps);
                m_pTimeLine->setFps(editor->fps);
            }
            if (element.tagName() == "currentView")
            {
                qreal m11 = element.attribute("m11").toDouble();
                qreal m12 = element.attribute("m12").toDouble();
                qreal m21 = element.attribute("m21").toDouble();
                qreal m22 = element.attribute("m22").toDouble();
                qreal dx = element.attribute("dx").toDouble();
                qreal dy = element.attribute("dy").toDouble();
                m_pScribbleArea->setMyView( QMatrix(m11,m12,m21,m22,dx,dy) );
            }
        }
        tag = tag.nextSibling();
    }
    return true;
}

bool MainWindow2::saveObject(QString strSavedFilename)
{
    QString filePath = strSavedFilename;

    QFileInfo fileInfo(filePath);
    if (fileInfo.isDir()) return false;

    QFileInfo dataInfo(filePath+".data");
    if (!dataInfo.exists())
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
    int nLayers = m_object->getLayerCount();
    qDebug("Layer Count=%d", nLayers);

    for (int i = 0; i < nLayers; i++)
    {
        Layer* layer = m_object->getLayer(i);
        qDebug() << "Saving Layer " << i << "(" <<layer->name << ")";

        progressValue = (i * 100) / nLayers;
        progress.setValue(progressValue);
        if (layer->type == Layer::BITMAP) ((LayerBitmap*)layer)->saveImages(filePath+".data", i);
        if (layer->type == Layer::VECTOR) ((LayerVector*)layer)->saveImages(filePath+".data", i);
        if (layer->type == Layer::SOUND) ((LayerSound*)layer)->saveImages(filePath+".data", i);
    }

    // save palette
    m_object->savePalette(filePath+".data");

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
    qDebug("Save Editor Node.");

    // save object
    QDomElement objectElement = m_object->createDomElement(doc);
    root.appendChild(objectElement);
    qDebug("Save Object Node.");

    int IndentSize = 2;
    doc.save(out, IndentSize);
    // -----------------------------------

    progress.setValue(100);

    m_object->modified = false;
    m_pTimeLine->updateContent();

    m_object->strCurrentFilePath = strSavedFilename;

    return true;
}

void MainWindow2::saveDocument()
{
    if ( !m_object->strCurrentFilePath.isEmpty() )
    {
        saveObject(m_object->strCurrentFilePath);
    }
    else
    {
        saveAsNewDocument();
    }
}

bool MainWindow2::maybeSave()
{
    if (m_object->modified)
    {
        int ret = QMessageBox::warning(this, tr("Warning"),
            tr("This animation has been modified.\n"
            "Do you want to save your changes?"),
            QMessageBox::Yes | QMessageBox::Default,
            QMessageBox::No,
            QMessageBox::Cancel | QMessageBox::Escape);
        if (ret == QMessageBox::Yes)
        {
            saveDocument();
            return true;
        }
        else if (ret == QMessageBox::Cancel)
        {
            return false;
        }
    }
    return true;
}

QDomElement MainWindow2::createDomElement(QDomDocument& doc)
{
    QDomElement tag = doc.createElement("editor");

    QDomElement tag1 = doc.createElement("currentLayer");
    tag1.setAttribute("value", editor->m_nCurrentLayerIndex);
    tag.appendChild(tag1);
    QDomElement tag2 = doc.createElement("currentFrame");
    tag2.setAttribute("value", editor->m_nCurrentFrameIndex);
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
    m_pPreferences = new Preferences(this);

    connect(m_pPreferences, SIGNAL(lengthSizeChange(QString)), m_pTimeLine, SIGNAL(lengthChange(QString)));
    connect(m_pPreferences, SIGNAL(fontSizeChange(int)), m_pTimeLine, SIGNAL(fontSizeChange(int)));
    connect(m_pPreferences, SIGNAL(frameSizeChange(int)), m_pTimeLine, SIGNAL(frameSizeChange(int)));
    connect(m_pPreferences, SIGNAL(labelChange(int)), m_pTimeLine, SIGNAL(labelChange(int)));
    connect(m_pPreferences, SIGNAL(scrubChange(int)), m_pTimeLine, SIGNAL(scrubChange(int)));

    connect(m_pPreferences, SIGNAL(windowOpacityChange(int)), this, SLOT(setOpacity(int)));
    connect(m_pPreferences, SIGNAL(curveOpacityChange(int)), m_pScribbleArea, SLOT(setCurveOpacity(int)));
    connect(m_pPreferences, SIGNAL(curveSmoothingChange(int)), m_pScribbleArea, SLOT(setCurveSmoothing(int)));
    connect(m_pPreferences, SIGNAL(highResPositionChange(int)), m_pScribbleArea, SLOT(setHighResPosition(int)));
    connect(m_pPreferences, SIGNAL(antialiasingChange(int)), m_pScribbleArea, SLOT(setAntialiasing(int)));
    connect(m_pPreferences, SIGNAL(gradientsChange(int)), m_pScribbleArea, SLOT(setGradients(int)));
    connect(m_pPreferences, SIGNAL(backgroundChange(int)), m_pScribbleArea, SLOT(setBackground(int)));
    connect(m_pPreferences, SIGNAL(shadowsChange(int)), m_pScribbleArea, SLOT(setShadows(int)));
    connect(m_pPreferences, SIGNAL(toolCursorsChange(int)), m_pScribbleArea, SLOT(setToolCursors(int)));
    connect(m_pPreferences, SIGNAL(styleChanged(int)), m_pScribbleArea, SLOT(setStyle(int)));

    connect(m_pPreferences, SIGNAL(autosaveChange(int)), editor, SLOT(changeAutosave(int)));
    connect(m_pPreferences, SIGNAL(autosaveNumberChange(int)), editor, SLOT(changeAutosaveNumber(int)));

    connect(m_pPreferences, SIGNAL(onionLayer1OpacityChange(int)), editor, SLOT(onionLayer1OpacityChangeSlot(int)));
    connect(m_pPreferences, SIGNAL(onionLayer2OpacityChange(int)), editor, SLOT(onionLayer2OpacityChangeSlot(int)));
    connect(m_pPreferences, SIGNAL(onionLayer3OpacityChange(int)), editor, SLOT(onionLayer3OpacityChangeSlot(int)));

    unloadAllShortcuts();

    connect(m_pPreferences, SIGNAL(destroyed()),
            this, SLOT(loadAllShortcuts()));

    m_pPreferences->show();
}

void MainWindow2::dockAllPalettes()
{
    editor->toolSet->setFloating(false);
    m_toolOptionWidget->setFloating(false);
    m_displayOptionWidget->setFloating(false);
    m_pTimeLine->setFloating(false);
    m_colorPalette->setFloating(false);
}

void MainWindow2::readSettings()
{

    QSettings* settings = pencilSettings();
    QRect desktopRect = QApplication::desktop()->screenGeometry();
    desktopRect.adjust(80,80,-80,-80);

    QPoint pos = settings->value("editorPosition", desktopRect.topLeft() ).toPoint();
    QSize size = settings->value("editorSize", desktopRect.size() ).toSize();

    move(pos);
    resize(size);

    editor->restorePalettesSettings(true, true, true);

    QString myPath = settings->value("lastFilePath", QVariant(QDir::homePath())).toString();
    addRecentFile(myPath);

    setOpacity(100 - settings->value("windowOpacity").toInt());
}

void MainWindow2::writeSettings()
{
    QSettings settings("Pencil", "Pencil");
    settings.setValue("editorPosition", pos());
    settings.setValue("editorSize", size());

    Palette* colourPalette = m_colorPalette;
    if (colourPalette != NULL)
    {
        settings.setValue("colourPalettePosition", colourPalette->pos());
        settings.setValue("colourPaletteSize", colourPalette->size());
        settings.setValue("colourPaletteFloating", colourPalette->isFloating());
    }

    TimeLine* timelinePalette = editor->getTimeLine();
    if (timelinePalette != NULL)
    {
        settings.setValue("timelinePalettePosition", timelinePalette->pos());
        settings.setValue("timelinePaletteSize", timelinePalette->size());
        settings.setValue("timelinePaletteFloating", timelinePalette->isFloating());
    }

    QDockWidget* toolWidget = editor->toolSet;
    if (toolWidget != NULL)
    {
        settings.setValue("drawPalettePosition", toolWidget->pos());
        settings.setValue("drawPaletteSize", toolWidget->size());
        settings.setValue("drawPaletteFloating", toolWidget->isFloating());
    }

    QDockWidget* optionPalette = m_toolOptionWidget;
    if (optionPalette != NULL)
    {
        settings.setValue("optionPalettePosition", optionPalette->pos());
        settings.setValue("optionPaletteSize", optionPalette->size());
        settings.setValue("optionPaletteFloating", optionPalette->isFloating());
    }

    QDockWidget* displayPalette = m_displayOptionWidget;
    if (displayPalette != NULL)
    {
        settings.setValue("displayPalettePosition", displayPalette->pos());
        settings.setValue("displayPaletteSize", displayPalette->size());
        settings.setValue("displayPaletteFloating", displayPalette->isFloating());
    }

}

void MainWindow2::loadAllShortcuts()
{
    if ( !pencilSettings()->contains(QString("shortcuts/%0").arg(CMD_NEW_FILE)) )
    {
        restoreShortcutsToDefault();
    }

    ui->actionNew->setShortcut( sc(CMD_NEW_FILE) );
    ui->actionOpen->setShortcut( sc(CMD_OPEN_FILE) );
    ui->actionSave->setShortcut( sc(CMD_SAVE_FILE) );
    ui->actionSave_as->setShortcut( sc(CMD_SAVE_AS) );
    ui->actionPrint->setShortcut( sc(CMD_PRINT) );

    ui->actionImport_Image->setShortcut( sc(CMD_IMPORT_IMAGE) );
    ui->actionImport_Image_Sequence->setShortcut( sc(CMD_IMPORT_IMAGE_SEQ) );
    ui->actionImport_Movie->setShortcut( sc(CMD_IMPORT_MOVIE) );
    ui->actionImport_Palette->setShortcut( sc(CMD_IMPORT_PALETTE) );
    ui->actionImport_Sound->setShortcut( sc(CMD_IMPORT_SOUND) );

    ui->actionExport_Image->setShortcut( sc(CMD_EXPORT_IMAGE) );
    ui->actionExport_Image_Sequence->setShortcut( sc(CMD_EXPORT_IMAGE_SEQ) );
    ui->actionExport_Movie->setShortcut( sc(CMD_EXPORT_MOVIE) );
    ui->actionExport_Palette->setShortcut( sc(CMD_EXPORT_PALETTE) );
    ui->actionExport_Svg_Image->setShortcut( sc(CMD_EXPORT_SVG) );
    ui->actionExport_X_sheet->setShortcut( sc(CMD_EXPORT_XSHEET) );

    ui->actionUndo->setShortcut( sc(CMD_UNDO) );
    ui->actionRedo->setShortcut( sc(CMD_REDO) );
    ui->actionCut->setShortcut( sc(CMD_CUT) );
    ui->actionCopy->setShortcut( sc(CMD_COPY) );
    ui->actionPaste->setShortcut( sc(CMD_PASTE) );
    ui->actionSelect_All->setShortcut( sc(CMD_SELECT_ALL));
    ui->actionDeselect_All->setShortcut( sc(CMD_DESELECT_ALL) );
    ui->actionPreference->setShortcut( sc(CMD_PREFERENCE) );

    ui->actionReset_Windows->setShortcut( sc(CMD_RESET_WINDOWS) );
    ui->actionReset_View->setShortcut( sc(CMD_RESET_ZOOM_ROTATE) );
    ui->actionZoom_In->setShortcut( sc(CMD_ZOOM_IN) );
    ui->actionZoom_Out->setShortcut(sc(CMD_ZOOM_OUT));
    ui->actionRotate_Clockwise->setShortcut(sc(CMD_ROTATE_CLOCK));
    ui->actionRotate_Anticlosewise->setShortcut(sc(CMD_ROTATE_ANTI_CLOCK));
    ui->actionHorizontal_Flip->setShortcut(sc(CMD_FLIP_HORIZONTAL));
    ui->actionVertical_Flip->setShortcut(sc(CMD_FLIP_VERTICAL));
    ui->actionPreview->setShortcut(sc(CMD_PREVIEW));
    ui->actionGrid->setShortcut(sc(CMD_GRID));
    ui->actionOnionPrevious->setShortcut(sc(CMD_ONIONSKIN_PREV));
    ui->actionOnionNext->setShortcut(sc(CMD_ONIONSKIN_NEXT));

    ui->actionPlay->setShortcut(sc(CMD_PLAY));
    ui->actionLoop->setShortcut(sc(CMD_LOOP));
    ui->actionPrevious_Frame->setShortcut(sc(CMD_GOTO_PREV_FRAME));
    ui->actionNext_Frame->setShortcut(sc(CMD_GOTO_NEXT_FRAME));
    ui->actionAdd_Frame->setShortcut(sc(CMD_ADD_FRAME));
    ui->actionDuplicate_Frame->setShortcut(sc(CMD_DUPLICATE_FRAME));
    ui->actionRemove_Frame->setShortcut(sc(CMD_REMOVE_FRAME));

    ui->actionMove->setShortcut(sc(CMD_TOOL_MOVE));
    ui->actionClear->setShortcut(sc(CMD_TOOL_CLEAR));
    ui->actionSelect->setShortcut(sc(CMD_TOOL_SELECT));
    ui->actionBrush->setShortcut(sc(CMD_TOOL_BRUSH));
    ui->actionPolyline->setShortcut(sc(CMD_TOOL_POLYLINE));
    ui->actionSmudge->setShortcut(sc(CMD_TOOL_SMUDGE));
    ui->actionPen->setShortcut(sc(CMD_TOOL_PEN));
    ui->actionHand->setShortcut(sc(CMD_TOOL_HAND));
    ui->actionPencil->setShortcut(sc(CMD_TOOL_PENCIL));
    ui->actionBucket->setShortcut(sc(CMD_TOOL_BUCKET));
    ui->actionEyedropper->setShortcut(sc(CMD_TOOL_EYEDROPPER));
    ui->actionEraser->setShortcut(sc(CMD_TOOL_ERASER));

    ui->actionNew_Bitmap_Layer->setShortcut(sc(CMD_NEW_BITMAP_LAYER));
    ui->actionNew_Vector_Layer->setShortcut(sc(CMD_NEW_VECTOR_LAYER));
    ui->actionNew_Camera_Layer->setShortcut(sc(CMD_NEW_CAMERA_LAYER));
    ui->actionNew_Sound_Layer->setShortcut(sc(CMD_NEW_SOUND_LAYER));

    ui->actionHelp->setShortcut(sc(CMD_HELP));
}

void MainWindow2::unloadAllShortcuts()
{
    QList<QAction*> actionList = this->findChildren<QAction*>();
    foreach (QAction* action, actionList)
    {
        action->setShortcut(QKeySequence(0));
    }
}

QString MainWindow2::sc(QString strActionName)
{
    strActionName = QString("shortcuts/") + strActionName;
    QString strKeySequence = pencilSettings()->value( strActionName ).toString();

    //qDebug() << strActionName << ": " << strKeySequence;

    return strKeySequence;
}

void MainWindow2::addRecentFile(QString filePath)
{
    QAction* openThisFileAct = new QAction(filePath, this);
    connect(openThisFileAct, SIGNAL(triggered()), this, SLOT(openRecent()));
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

void MainWindow2::exportPalette()
{
    QSettings settings("Pencil","Pencil");
    QString initialPath = settings.value("lastPalettePath",
                                         QVariant(QDir::homePath())).toString();
    if (initialPath.isEmpty())
    {
        initialPath = QDir::homePath() + "/untitled.xml";
    }
    QString filePath = QFileDialog::getSaveFileName(this, tr("Export As"),initialPath);
    if (!filePath.isEmpty())
    {
        m_object->exportPalette(filePath);
        settings.setValue("lastPalettePath", QVariant(filePath));
    }
}

void MainWindow2::importPalette()
{
    QSettings settings("Pencil","Pencil");
    QString initialPath = settings.value("lastPalettePath", QVariant(QDir::homePath())).toString();
    if (initialPath.isEmpty())
    {
        initialPath = QDir::homePath() + "/untitled.xml";
    }
    QString filePath = QFileDialog::getOpenFileName(this, tr("Import"),initialPath);
    if (!filePath.isEmpty())
    {
        m_object->importPalette(filePath);
        m_colorPalette->updateList();
        settings.setValue("lastPalettePath", QVariant(filePath));
    }
}

void MainWindow2::aboutPencil()
{
    QMessageBox::about(this, tr("Pencil2D Animation 0.5.4 beta"),
                       tr("<table style='background-color: #DDDDDD' border='0'><tr><td valign='top'>"
                          "<img src=':icons/logo.png' width='318' height='123' border='0'><br></td></tr><tr><td>"
                          "Developed by: <i>Pascal Naidon</i> &  <i>Patrick Corrieri</i><br>"
                          "Version: <b>0.5.4</b> (26 July 2013)<br><br>"
                          "<b>Thanks to:</b><br>"
                          "the Qt libraries <a href='http://qt-project.org'>http://qt-project.org</a><br>"                          
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
