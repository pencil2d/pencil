/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <QBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QSvgGenerator>
#include <QMessageBox>

#include "editor.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layersound.h"
#include "layercamera.h"
#include "layerimage.h"
#include "mainwindow2.h"
#include "displayoptiondockwidget.h"
#include "tooloptiondockwidget.h"
#include "colormanager.h"
#include "colorpalettewidget.h"

#define MIN(a,b) ((a)>(b)?(b):(a))


Editor::Editor(MainWindow2* parent)
{
    mainWindow = parent;

    QSettings settings("Pencil","Pencil");

    object = NULL; // the editor is initialized with no object

    m_colorManager = new ColorManager(this, this);

    altpress = false;
    modified = false;
    numberOfModifications = 0;
    autosave = settings.value("autosave").toBool();
    autosaveNumber = settings.value("autosaveNumber").toInt();
    if (autosaveNumber == 0)
    {
        autosaveNumber = 20;
        settings.setValue("autosaveNumber", 20);
    }
    backupIndex = -1;
    clipboardBitmapOk = false;
    clipboardVectorOk = false;

    if (settings.value("onionLayer1Opacity").isNull()) settings.setValue("onionLayer1Opacity",50);
    if (settings.value("onionLayer2Opacity").isNull()) settings.setValue("onionLayer2Opacity",0);
    if (settings.value("onionLayer3Opacity").isNull()) settings.setValue("onionLayer3Opacity",0);
    onionLayer1Opacity = settings.value("onionLayer1Opacity").toInt();
    onionLayer2Opacity = settings.value("onionLayer2Opacity").toInt();
    onionLayer3Opacity = settings.value("onionLayer3Opacity").toInt();

    fps = settings.value("fps").toInt();
    if (fps == 0)
    {
        fps = 12;
        settings.setValue("fps", 12);
    }

    maxFrame = 1;
    timer = new QTimer(this);
    timer->setInterval(1000 / fps);
    connect(timer, SIGNAL(timeout()), this, SLOT(playNextFrame()));
    playing = false;
    looping = false;
    sound = true;

    frameList << 1;
    m_nCurrentFrameIndex = 1;
    m_nCurrentLayerIndex = 0;

    exportFramesDialog = NULL; // will be created when needed
    exportMovieDialog = NULL;
    exportFlashDialog = NULL;
    exportFramesDialog_hBox = NULL;
    exportFramesDialog_vBox = NULL;
    exportFramesDialog_format = NULL;
    exportMovieDialog_hBox = NULL;
    exportMovieDialog_vBox = NULL;
    exportMovieDialog_format = NULL;
    exportMovieDialog_fpsBox = NULL;

    exportFlashDialog_compression = NULL;

    // Layouts
    QHBoxLayout* mainLayout = new QHBoxLayout();

    scribbleArea = new ScribbleArea(this, this);
    toolSet = new ToolSetWidget(tr("Tools"), this);

    mainLayout->addWidget(scribbleArea);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);

    // FOCUS POLICY
    scribbleArea->setFocusPolicy(Qt::StrongFocus);

    // CONNECTIONS
    makeConnections();

    qDebug() << QLibraryInfo::location(QLibraryInfo::PluginsPath);
    qDebug() << QLibraryInfo::location(QLibraryInfo::BinariesPath);
    qDebug() << QLibraryInfo::location(QLibraryInfo::LibrariesPath);

    setTool(PENCIL);
    
    setAcceptDrops(true);
}

TimeLine* Editor::getTimeLine()
{
    return mainWindow->m_pTimeLine;
}

Editor::~Editor()
{
    // a lot more probably needs to be cleaned here...
    if ( object != NULL )
    {
        delete object;
    }
    clearBackup();
}

void Editor::makeConnections()
{
    connect(scribbleArea, SIGNAL(pencilOn()), toolSet, SLOT(pencilOn()));
    connect(scribbleArea, SIGNAL(eraserOn()), toolSet, SLOT(eraserOn()));
    connect(scribbleArea, SIGNAL(selectOn()), toolSet, SLOT(selectOn()));
    connect(scribbleArea, SIGNAL(moveOn()), toolSet, SLOT(moveOn()));
    connect(scribbleArea, SIGNAL(penOn()), toolSet, SLOT(penOn()));
    connect(scribbleArea, SIGNAL(handOn()), toolSet, SLOT(handOn()));
    connect(scribbleArea, SIGNAL(polylineOn()), toolSet, SLOT(polylineOn()));
    connect(scribbleArea, SIGNAL(bucketOn()), toolSet, SLOT(bucketOn()));
    connect(scribbleArea, SIGNAL(eyedropperOn()), toolSet, SLOT(eyedropperOn()));
    connect(scribbleArea, SIGNAL(brushOn()), toolSet, SLOT(brushOn()));
    connect(scribbleArea, SIGNAL(smudgeOn()), toolSet, SLOT(smudgeOn()));
    
    connect(this, SIGNAL(toggleOnionPrev(bool)), scribbleArea, SLOT(toggleOnionPrev(bool)));
    connect(this, SIGNAL(toggleOnionNext(bool)), scribbleArea, SLOT(toggleOnionNext(bool)));
    connect(scribbleArea, SIGNAL(thinLinesChanged(bool)), this, SIGNAL(changeThinLinesButton(bool)));
    connect(scribbleArea, SIGNAL(outlinesChanged(bool)), this, SIGNAL(changeOutlinesButton(bool)));
    connect(scribbleArea, SIGNAL(onionPrevChanged(bool)), this, SIGNAL(onionPrevChanged(bool)));
    connect(scribbleArea, SIGNAL(onionNextChanged(bool)), this, SIGNAL(onionNextChanged(bool)));

    connect(this, SIGNAL(selectAll()), scribbleArea, SLOT(selectAll()));

    connect(scribbleArea, SIGNAL(modification()), this, SLOT(modification()));
    connect(scribbleArea, SIGNAL(modification(int)), this, SLOT(modification(int)));

    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardChanged()) );
}

void Editor::dragEnterEvent(QDragEnterEvent* event)
{
    event->acceptProposedAction();
}

void Editor::dropEvent(QDropEvent* event)
{
    if ( event->mimeData()->hasUrls() )
    {
        for(int i=0; i < event->mimeData()->urls().size(); i++)
        {
            if (i > 0) scrubForward();
            QUrl url = event->mimeData()->urls()[i];
            QString filePath = url.toLocalFile();
            if (filePath.endsWith(".png") || filePath.endsWith(".jpg") || filePath.endsWith(".jpeg"))
                importImage( filePath );
            if (filePath.endsWith(".aif") || filePath.endsWith(".mp3") || filePath.endsWith(".wav"))
                importSound( filePath );
        }
    }
}

void Editor::importImageSequence ()
{
    QFileDialog w;
    w.setFileMode(QFileDialog::AnyFile);

    QSettings settings("Pencil","Pencil");
    QString initialPath = settings.value("lastImportPath", QVariant(QDir::homePath())).toString();
    if (initialPath.isEmpty()) initialPath = QDir::homePath();
    QStringList files = w.getOpenFileNames(this,
                                           "Select one or more files to open",
                                           initialPath,
                                           "Images (*.png *.xpm *.jpg *.jpeg)");
    qDebug() << files;

    QStringListIterator i(files);
    for (int i = 0; i < files.size(); ++i)
    {
        QString filePath;
        filePath = files.at(i).toLocal8Bit().constData();
        if (i > 0) scrubForward();
        {
            if (filePath.endsWith(".png") ||
                filePath.endsWith(".jpg") ||
                filePath.endsWith(".jpeg"))
            {
                importImage(filePath);
            }
        }
    }
}

bool Editor::importMov()
{
    QSettings settings("Pencil","Pencil");
    
    QString initialPath = settings.value("lastExportPath", QDir::homePath()).toString();

    if (initialPath.isEmpty()) 
    {
        initialPath = QDir::homePath() + "/untitled.avi";
    }
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Import movie"),
        initialPath ,
        tr("AVI (*.avi);;MPEG(*.mpg);;MOV(*.mov);;MP4(*.mp4);;SWF(*.swf);;FLV(*.flv);;WMV(*.wmv)")
        );
    if (filePath.isEmpty())
    {
        return false;
    }
    else
    {
        settings.setValue("lastExportPath", QVariant(filePath));
        importMovie(filePath, fps);
        return true;
    }
}

void Editor::setWidth(qreal width)
{
    //qDebug() << "editor setWdith" << width;
    scribbleArea->setWidth(width);
    emit penWidthValueChange(width);
}

void Editor::setToolProperties(const Properties& p)
{
    qDebug() << "set tool properties";
    setWidth(p.width);
    setFeather(p.feather);
    setPressure(p.pressure);
    setPreserveAlpha(p.preserveAlpha);
    setInvisibility(p.invisibility);
}

void Editor::applyWidth(qreal width)
{
    setWidth(width);
    Layer* layer = getCurrentLayer();
    if (layer == NULL) return;
    if (layer->type == Layer::VECTOR) ((LayerVector*)layer)->getLastVectorImageAtFrame(m_nCurrentFrameIndex, 0)->applyWidthToSelection(width);
}

void Editor::setFeather(qreal feather)
{
    scribbleArea->setFeather(feather);
    emit penFeatherValueChange(feather);
}

void Editor::applyFeather(qreal feather)
{
    setFeather(feather);
    Layer* layer = getCurrentLayer();
    if (layer == NULL) return;
    if (layer->type == Layer::VECTOR) ((LayerVector*)layer)->getLastVectorImageAtFrame(m_nCurrentFrameIndex, 0)->applyFeatherToSelection(feather);
}

void Editor::setInvisibility(int invisibility)
{
    if (invisibility >= 0)
    {
        scribbleArea->setInvisibility( invisibility > 0 );
    }
    emit penInvisiblityValueChange(invisibility);
}

void Editor::applyInvisibility(bool invisibility)
{
    setInvisibility(invisibility);
    Layer* layer = getCurrentLayer();
    if (layer == NULL) return;
    if (layer->type == Layer::VECTOR) 
    {
        ((LayerVector*)layer)
            ->getLastVectorImageAtFrame(m_nCurrentFrameIndex, 0)
            ->applyInvisibilityToSelection(invisibility);
    }
}

void Editor::setPreserveAlpha(int preserveAlpha)
{
    if ( preserveAlpha >= 0)
    {
        scribbleArea->setPreserveAlpha(preserveAlpha>0);
    }
    emit penPreserveAlphaValueChange(preserveAlpha);
}

void Editor::applyPreserveAlpha(bool preserveAlpha)
{
    setPreserveAlpha(preserveAlpha);
}

void Editor::setFollowContour(int followContour)
{
    if (followContour >= 0)
    {
        scribbleArea->setFollowContour(followContour > 0);
    }
    emit penFollowContourValueChange(followContour);
}

void Editor::applyFollowContour(bool followContour)
{
    setFollowContour(followContour);
}

void Editor::setPressure(int pressure)
{
    if ( pressure >= 0 )
    {
        scribbleArea->setPressure( pressure > 0 );
    }
    emit penPressureValueChange(pressure);
}

void Editor::applyPressure(bool pressure)
{
    setPressure(pressure);
    Layer* layer = getCurrentLayer();
    if (layer == NULL) return;
    if (layer->type == Layer::VECTOR) 
    {
        ((LayerVector*)layer)
            ->getLastVectorImageAtFrame(m_nCurrentFrameIndex, 0)
            ->applyVariableWidthToSelection(pressure);
    }
}

void Editor::selectAndApplyColour(int i)
{
    Layer* layer = getCurrentLayer();
    if (layer == NULL)
    {
        return;
    }
    if (layer->type == Layer::VECTOR)
    {
        ((LayerVector*)layer)->getLastVectorImageAtFrame(m_nCurrentFrameIndex, 0)->applyColourToSelection(i);
    }
}

/*
void Editor::setBitmapColour(QColor colour)
{
    colorManager()->pickColor(colour);
    mainWindow->m_colorPalette->setColour(colour);

    emit penColorValueChange(colour);
}
*/

void Editor::setFrontColour(int i, QColor newColour)
{
    if (newColour.isValid() && i > -1)
    {
        Layer* layer = object->getLayer(m_nCurrentLayerIndex);
        if (layer != NULL)
        {
            if (layer->type == Layer::VECTOR)
            {
                scribbleArea->setModified(m_nCurrentLayerIndex, m_nCurrentFrameIndex);
            }
        }
		colorManager()->pickColorNumber( i );
    }
}

void Editor::changeAutosave(int x)
{
    QSettings settings("Pencil","Pencil");
    if (x == 0) 
    { 
        autosave = false; 
        settings.setValue("autosave","false"); 
    }
    else 
    { 
        autosave = true; 
        settings.setValue("autosave","true"); 
    }
}

void Editor::changeAutosaveNumber(int number)
{
    autosaveNumber = number;
    QSettings settings("Pencil","Pencil");
    settings.setValue("autosaveNumber", number);
}

void Editor::onionLayer1OpacityChangeSlot(int number)
{
    onionLayer1Opacity = number;
    QSettings settings("Pencil","Pencil");
    settings.setValue("onionLayer1Opacity", number);
}

void Editor::onionLayer2OpacityChangeSlot(int number)
{
    onionLayer2Opacity = number;
    QSettings settings("Pencil","Pencil");
    settings.setValue("onionLayer2Opacity", number);
}

void Editor::onionLayer3OpacityChangeSlot(int number)
{
    onionLayer3Opacity = number;
    QSettings settings("Pencil","Pencil");
    settings.setValue("onionLayer3Opacity", number);
}

void Editor::modification()
{
    modification(m_nCurrentLayerIndex);
}

void Editor::modification(int layerNumber)
{
    modified = true;
    if (object != NULL) object->modification();
    lastModifiedFrame = m_nCurrentFrameIndex;
    lastModifiedLayer = layerNumber;
    scribbleArea->update();
    getTimeLine()->updateContent();
    numberOfModifications++;
    if (autosave && numberOfModifications > autosaveNumber)
    {
        numberOfModifications = 0;
        emit needSave();
    }
}

void Editor::backup(QString undoText)
{
    if (lastModifiedLayer>-1 && lastModifiedFrame > 0)
    {
        backup(lastModifiedLayer, lastModifiedFrame, undoText);
    }
    if ( lastModifiedLayer != m_nCurrentLayerIndex || lastModifiedFrame != m_nCurrentFrameIndex )
    {
        backup(m_nCurrentLayerIndex, m_nCurrentFrameIndex, undoText);
    }
}

void Editor::backup(int backupLayer, int backupFrame, QString undoText)
{
    while (backupList.size()-1 > backupIndex && backupList.size() > 0)
    {
        delete backupList.takeLast();
    }
    while (backupList.size() > 19)   // we authorize only 20 levels of cancellation
    {
        delete backupList.takeFirst();
        backupIndex--;
    }
    Layer* layer = object->getLayer(backupLayer);
    if (layer != NULL)
    {
        if (layer->type == Layer::BITMAP)
        {
            BackupBitmapElement* element = new BackupBitmapElement();
            element->layer = backupLayer;
            element->frame = backupFrame;
            element->undoText = undoText;
            element->somethingSelected = this->getScribbleArea()->somethingSelected;
            element->mySelection = this->getScribbleArea()->mySelection;
            element->myTransformedSelection = this->getScribbleArea()->myTransformedSelection;
            element->myTempTransformedSelection = this->getScribbleArea()->myTempTransformedSelection;
            BitmapImage* bitmapImage = ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(backupFrame, 0);
            if (bitmapImage != NULL)
            {
                element->bitmapImage =  bitmapImage->copy();  // copy the image
                backupList.append(element);
                backupIndex++;
            }
        }
        if (layer->type == Layer::VECTOR)
        {
            BackupVectorElement* element = new BackupVectorElement();
            element->layer = backupLayer;
            element->frame = backupFrame;
            element->undoText = undoText;
            element->somethingSelected = this->getScribbleArea()->somethingSelected;
            element->mySelection = this->getScribbleArea()->mySelection;
            element->myTransformedSelection = this->getScribbleArea()->myTransformedSelection;
            element->myTempTransformedSelection = this->getScribbleArea()->myTempTransformedSelection;
            VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(backupFrame, 0);
            if (vectorImage != NULL)
            {
                element->vectorImage = *vectorImage;  // copy the image (that works but I should also provide a copy() method)
                backupList.append(element);
                backupIndex++;
            }
        }
    }
}

void BackupBitmapElement::restore(Editor* editor)
{
    Layer* layer = editor->object->getLayer(this->layer);
    if (layer != NULL)
    {
        if (layer->type == Layer::BITMAP)
        {
            *(   ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(this->frame, 0)    ) = this->bitmapImage;  // restore the image
        }
    }
    editor->getScribbleArea()->somethingSelected = this->somethingSelected;
    editor->getScribbleArea()->mySelection = this->mySelection;
    editor->getScribbleArea()->myTransformedSelection = this->myTransformedSelection;
    editor->getScribbleArea()->myTempTransformedSelection = this->myTempTransformedSelection;

    editor->updateFrame(this->frame);
    editor->scrubTo(this->frame);
}

void BackupVectorElement::restore(Editor* editor)
{
    Layer* layer = editor->object->getLayer(this->layer);
    if (layer != NULL)
    {
        if (layer->type == Layer::VECTOR)
        {
            *(  ((LayerVector*)layer)->getLastVectorImageAtFrame(this->frame, 0)  ) = this->vectorImage;  // restore the image
            //((LayerVector*)layer)->getLastVectorImageAtFrame(this->frame, 0)->setModified(true); // why?
            //editor->scribbleArea->setModified(layer, this->frame);
        }
    }
    editor->getScribbleArea()->somethingSelected = this->somethingSelected;
    editor->getScribbleArea()->mySelection = this->mySelection;
    editor->getScribbleArea()->myTransformedSelection = this->myTransformedSelection;
    editor->getScribbleArea()->myTempTransformedSelection = this->myTempTransformedSelection;

    editor->updateFrameAndVector(this->frame);
    editor->scrubTo(this->frame);
}

void Editor::undo()
{
    if ( backupList.size() > 0 && backupIndex > -1)
    {
        if (backupIndex == backupList.size()-1)
        {
            BackupElement* lastBackupElement = backupList[backupIndex];
            if (lastBackupElement->type() == BackupElement::BITMAP_MODIF)
            {
                BackupBitmapElement* lastBackupBitmapElement = (BackupBitmapElement*)lastBackupElement;
                backup( lastBackupBitmapElement->layer, lastBackupBitmapElement->frame ,"NoOp");
                backupIndex--;

            }
            if (lastBackupElement->type() == BackupElement::VECTOR_MODIF)
            {
                BackupVectorElement* lastBackupVectorElement = (BackupVectorElement*)lastBackupElement;
                backup( lastBackupVectorElement->layer, lastBackupVectorElement->frame ,"NoOp");
                backupIndex--;
            }
        }
        //
        backupList[backupIndex]->restore(this);
        backupIndex--;
        scribbleArea->calculateSelectionRect(); // really ugly -- to improve
    }
}

void Editor::redo()
{
    if ( backupList.size() > 0 && backupIndex < backupList.size()-2)
    {
        backupIndex++;
        backupList[backupIndex+1]->restore(this);
    }
}

void Editor::clearBackup()
{
    backupIndex = -1;
    while ( !backupList.isEmpty() )
    {
        delete backupList.takeLast();
    }
    lastModifiedLayer = -1;
    lastModifiedFrame = -1;
}

void Editor::cut()
{
    scribbleArea->deleteSelection();
    scribbleArea->deselectAll();
}

void Editor::crop()
{
    // FIXME:
    //select_clicked();
    //move_clicked();
}

void Editor::croptoselect()
{
    // FIXME:
    //select_clicked();
    //copy();
    //clearCurrentFrame();
    //paste();
}

void Editor::copy()
{
    Layer* layer = object->getLayer(m_nCurrentLayerIndex);
    if (layer != NULL)
    {
        if (layer->type == Layer::BITMAP)
        {
            if (scribbleArea->somethingSelected)
            {
                clipboardBitmapImage =   ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(m_nCurrentFrameIndex, 0)->copy( scribbleArea->getSelection().toRect() );  // copy part of the image
                scribbleArea->deselectAll();
            }
            else
            {
                clipboardBitmapImage =   ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(m_nCurrentFrameIndex, 0)->copy();  // copy the whole image
                scribbleArea->deselectAll();
            }
            clipboardBitmapOk = true;
            if ( clipboardBitmapImage.image != NULL ) QApplication::clipboard()->setImage( *(clipboardBitmapImage.image) );
        }
        if (layer->type == Layer::VECTOR)
        {
            clipboardVectorOk = true;
            clipboardVectorImage = *(  ((LayerVector*)layer)->getLastVectorImageAtFrame(m_nCurrentFrameIndex, 0)  );  // copy the image (that works but I should also provide a copy() method)
            scribbleArea->deselectAll();
        }
    }
}

void Editor::copyFrames()
{
    bool ok;
    QString name;
    QString text = QInputDialog::getText(NULL, tr("Number of Frames\r\nEnter digits only"),tr("Blank Frames:\r\n(Enter digits only)"), QLineEdit::Normal,name, &ok);
    if (ok && !text.isEmpty())
    {
        name = text;
    }
    int a =text.toInt();
    QListIterator<int> z( frameList );
    for (int i = 0; i < a; ++i)
    {
        addKey();
    }

}

void Editor::pasteFrames()
{
    int a = frameList.count();
    QListIterator<int> z( frameList );
    for (int i = 0; i < a; ++i)
    {
        int b = z.next();
        qDebug()<< i;
        qDebug()<< a;
        scrubTo(i);
        copy();
        //int d = b+a;  Give us the interval between the frames
        scrubTo(b);//TODO scrub to selected frame copy() scrub to starting frame and paste()
        duplicateKey();
    }
}

void Editor::paste()
{
    Layer* layer = object->getLayer(m_nCurrentLayerIndex);
    if (layer != NULL)
    {
        if (layer->type == Layer::BITMAP && clipboardBitmapImage.image != NULL)   // clipboardBitmapOk
        {
            backup(tr("Paste"));
            BitmapImage tobePasted = clipboardBitmapImage.copy();
            qDebug() << "to be pasted --->" << tobePasted.image->size();
            if (scribbleArea->somethingSelected)
            {
                QRectF selection = scribbleArea->getSelection();
                if ( clipboardBitmapImage.width() <= selection.width() && clipboardBitmapImage.height() <= selection.height() )
                {
                    tobePasted.moveTopLeft( selection.topLeft() );
                }
                else
                {
                    tobePasted.transform( selection, true );
                }
            }
            ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(m_nCurrentFrameIndex, 0)->paste( &tobePasted ); // paste the clipboard
            setTool(MOVE);
        }
        if (layer->type == Layer::VECTOR && clipboardVectorOk)
        {
            backup(tr("Paste"));
            scribbleArea->deselectAll();
            VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(m_nCurrentFrameIndex, 0);
            vectorImage->paste( clipboardVectorImage );  // paste the clipboard
            scribbleArea->setSelection( vectorImage->getSelectionRect(), true );
            //((LayerVector*)layer)->getLastVectorImageAtFrame(backupFrame, 0)->modification(); ????
            setTool(MOVE);
        }
    }
    scribbleArea->updateFrame();
}

void Editor::deselectAll()
{
    scribbleArea->deselectAll();
}
void Editor::clipboardChanged()
{
    if (clipboardBitmapOk == false)
    {
        clipboardBitmapImage.image = new QImage( QApplication::clipboard()->image() );
        clipboardBitmapImage.boundaries = QRect( clipboardBitmapImage.topLeft(), clipboardBitmapImage.image->size() );
        qDebug() << "New clipboard image" << clipboardBitmapImage.image->size();
    }
    else
    {
        clipboardBitmapOk = false;
        qDebug() << "The image has been saved in the clipboard";
    }
}

void Editor::newBitmapLayer()
{
    if (object != NULL)
    {
        bool ok;
        QString text = QInputDialog::getText(NULL, tr("Layer Properties"),
                                             tr("Layer name:"), QLineEdit::Normal,
                                             "Bitmap Layer", &ok);
        if (ok && !text.isEmpty())
        {
            Layer *layer = object->addNewBitmapLayer();
            layer->name = text;
            getTimeLine()->updateLayerNumber( object->getLayerCount() );
            setCurrentLayer( object->getLayerCount()-1 );

        }
    }
}

void Editor::newVectorLayer()
{
    if (object != NULL)
    {
        bool ok;
        QString text = QInputDialog::getText(NULL, tr("Layer Properties"),
                                             tr("Layer name:"), QLineEdit::Normal,
                                             "Bitmap Layer", &ok);
        if (ok && !text.isEmpty())
        {
            Layer *layer = object->addNewVectorLayer();
            layer->name = text;
            getTimeLine()->updateLayerNumber( object->getLayerCount() );
            setCurrentLayer( object->getLayerCount()-1 );
        }
    }
}

void Editor::newSoundLayer()
{
    if (object != NULL)
    {
        bool ok;
        QString text = QInputDialog::getText(NULL, tr("Layer Properties"),
                                             tr("Layer name:"), QLineEdit::Normal,
                                             "Bitmap Layer", &ok);
        if (ok && !text.isEmpty())
        {
            Layer *layer = object->addNewSoundLayer();
            layer->name = text;
            getTimeLine()->updateLayerNumber( object->getLayerCount() );
            setCurrentLayer( object->getLayerCount()-1 );
        }
    }
}

void Editor::newCameraLayer()
{
    if (object != NULL)
    {
        bool ok;
        QString text = QInputDialog::getText(NULL, tr("Layer Properties"),
                                             tr("Layer name:"), QLineEdit::Normal,
                                             "Bitmap Layer", &ok);
        if (ok && !text.isEmpty())
        {
            Layer *layer = object->addNewCameraLayer();
            layer->name = text;
            getTimeLine()->updateLayerNumber( object->getLayerCount() );
            setCurrentLayer( object->getLayerCount()-1 );
        }
    }
}

void Editor::deleteCurrentLayer()
{
    int ret = QMessageBox::warning(this,
                                   tr("Warning"),
                                   "Are you sure you want to delete layer: "+object->getLayer(m_nCurrentLayerIndex)->name+" ?",
                                   QMessageBox::Ok | QMessageBox::Cancel,
                                   QMessageBox::Ok);
    if (ret == QMessageBox::Ok)
    {
        object->deleteLayer(m_nCurrentLayerIndex);
        if (m_nCurrentLayerIndex == object->getLayerCount()) setCurrentLayer( m_nCurrentLayerIndex-1 );
        getTimeLine()->updateLayerNumber( object->getLayerCount() );
        //timeLine->update();
        scribbleArea->updateAllFrames();
    }
}

void Editor::toggleMirror()
{
    object->toggleMirror();
    scribbleArea->toggleMirror();
}

void Editor::toggleMirrorV()
{
    object->toggleMirror();
    scribbleArea->toggleMirrorV();
}

void Editor::toggleShowAllLayers()
{
    scribbleArea->toggleShowAllLayers();
    getTimeLine()->updateContent();
}

void Editor::resetMirror()
{
    object->resetMirror();
    //toolSet->resetMirror();
}

void Editor::saveLength(QString x)
{
    bool ok;
    int dec = x.toInt(&ok, 10);
    QSettings settings("Pencil","Pencil");
    settings.setValue("length", dec);
}

void Editor::resetUI()
{
    updateObject();
    maxFrame = 0;
    m_nCurrentFrameIndex = 0;
    scrubTo(0);
}

void Editor::setObject(Object* newObject)
{
    if (newObject == NULL)
    {
        return;
    }
    if (newObject == this->object)
    {
        return;
    }
    this->object = newObject;

    connect(object, SIGNAL(imageAdded(int)), this, SLOT(addFrame(int)));
    connect(object, SIGNAL(imageAdded(int,int)), this, SLOT(addFrame(int,int)));
    connect(object, SIGNAL(imageRemoved(int)), this, SLOT(removeFrame(int)));

    m_nCurrentLayerIndex = 0; // the default selected layer is the first one
    m_nCurrentFrameIndex = 1;
    frameList.clear();
    frameList << 1;
}

void Editor::updateObject()
{    
    mainWindow->m_colorPalette->selectColorListRow(0);

    getTimeLine()->updateLayerNumber(object->getLayerCount());
    mainWindow->m_colorPalette->refreshColorList();
    clearBackup();
    
    scribbleArea->updateAllFrames();
    updateMaxFrame();
}

void Editor::createExportFramesSizeBox()
{
    int defaultWidth = 720;
    int defaultHeight = 540;
    exportFramesDialog_hBox = new QSpinBox(this);
    exportFramesDialog_hBox->setMinimum(1);
    exportFramesDialog_hBox->setMaximum(10000);
    exportFramesDialog_hBox->setValue(defaultWidth);
    exportFramesDialog_hBox->setFixedWidth(80);
    exportFramesDialog_vBox = new QSpinBox(this);
    exportFramesDialog_vBox->setMinimum(1);
    exportFramesDialog_vBox->setMaximum(10000);
    exportFramesDialog_vBox->setValue(defaultHeight);
    exportFramesDialog_vBox->setFixedWidth(80);
}

void Editor::createExportMovieSizeBox()
{
    int defaultWidth = 720;
    int defaultHeight = 540;
    int defaultFps = 25;
    exportMovieDialog_hBox = new QSpinBox(this);
    exportMovieDialog_hBox->setMinimum(1);
    exportMovieDialog_hBox->setMaximum(10000);
    exportMovieDialog_hBox->setValue(defaultWidth);
    exportMovieDialog_hBox->setFixedWidth(80);
    exportMovieDialog_vBox = new QSpinBox(this);
    exportMovieDialog_vBox->setMinimum(1);
    exportMovieDialog_vBox->setMaximum(10000);
    exportMovieDialog_vBox->setValue(defaultHeight);
    exportMovieDialog_vBox->setFixedWidth(80);

    exportMovieDialog_format = new QComboBox();
    exportMovieDialog_format->addItem("AUTO");
    exportMovieDialog_format->addItem("MOV");
    exportMovieDialog_format->addItem("MPEG2/AVI");
    exportMovieDialog_format->addItem("MPEG4/AVI");
    exportMovieDialog_format->addItem("MPEG4/MP4");
    exportMovieDialog_fpsBox = new QSpinBox(this);
    exportMovieDialog_fpsBox->setMinimum(1);
    exportMovieDialog_fpsBox->setMaximum(60);
    exportMovieDialog_fpsBox->setValue(defaultFps);
    exportMovieDialog_fpsBox->setFixedWidth(40);

}

void Editor::createExportFramesDialog()
{
    exportFramesDialog = new QDialog(this, Qt::Dialog);
    QGridLayout* mainLayout = new QGridLayout;

    QGroupBox* resolutionBox = new QGroupBox(tr("Resolution"));
    if (exportFramesDialog_hBox == NULL || exportFramesDialog_vBox == NULL)
    {
        createExportFramesSizeBox();
    }
    QGridLayout* resolutionLayout = new QGridLayout;
    resolutionLayout->addWidget(exportFramesDialog_hBox,0,0);
    resolutionLayout->addWidget(exportFramesDialog_vBox,0,1);
    resolutionBox->setLayout(resolutionLayout);

    QGroupBox* formatBox = new QGroupBox(tr("Format"));
    exportFramesDialog_format = new QComboBox();
    exportFramesDialog_format->addItem("PNG");
    exportFramesDialog_format->addItem("JPG");
    exportFramesDialog_format->addItem("TIF");
    exportFramesDialog_format->addItem("BMP");
    QGridLayout* formatLayout = new QGridLayout;
    formatLayout->addWidget(exportFramesDialog_format, 0, 0);
    formatBox->setLayout(formatLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), exportFramesDialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), exportFramesDialog, SLOT(reject()));

    mainLayout->addWidget(resolutionBox, 0, 0);
    mainLayout->addWidget(formatBox, 1, 0);
    mainLayout->addWidget(buttonBox, 2, 0);
    exportFramesDialog->setLayout(mainLayout);
    exportFramesDialog->setWindowTitle(tr("Options"));
    exportFramesDialog->setModal(true);
}

void Editor::createExportMovieDialog()
{
    exportMovieDialog = new QDialog(this, Qt::Dialog);
    QGridLayout* mainLayout = new QGridLayout;

    QGroupBox* resolutionBox = new QGroupBox(tr("Resolution"));
    if (!exportMovieDialog_hBox || !exportMovieDialog_vBox)
    {
        createExportMovieSizeBox();
    }
    QGridLayout* resolutionLayout = new QGridLayout;
    resolutionLayout->addWidget(exportMovieDialog_hBox,0,0);
    resolutionLayout->addWidget(exportMovieDialog_vBox,0,1);
    resolutionBox->setLayout(resolutionLayout);

    QGroupBox* formatBox = new QGroupBox(tr("Format"));
    QGridLayout* formatLayout = new QGridLayout;
    QLabel* label1 = new QLabel("Save as");
    formatLayout->addWidget(label1,0,0);
    formatLayout->addWidget(exportMovieDialog_format,0,1);
    QLabel* label2 = new QLabel("Fps");
    formatLayout->addWidget(label2,0,2);
    formatLayout->addWidget(exportMovieDialog_fpsBox,0,3);
    formatBox->setLayout(formatLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), exportMovieDialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), exportMovieDialog, SLOT(reject()));

    mainLayout->addWidget(resolutionBox, 0, 0);
    mainLayout->addWidget(formatBox, 1, 0);
    mainLayout->addWidget(buttonBox, 2, 0);
    exportMovieDialog->setLayout(mainLayout);
    exportMovieDialog->setWindowTitle(tr("Options"));
    exportMovieDialog->setModal(true);
}

void Editor::createExportFlashDialog()
{
    exportFlashDialog = new QDialog(this, Qt::Dialog);
    QGridLayout* mainLayout = new QGridLayout;

    QSettings settings("Pencil","Pencil");

    exportFlashDialog_compression = new QSlider(Qt::Horizontal);
    exportFlashDialog_compression->setTickPosition(QSlider::TicksBelow);
    exportFlashDialog_compression->setMinimum(0);
    exportFlashDialog_compression->setMaximum(10);
    exportFlashDialog_compression->setValue( 10 - settings.value("flashCompressionLevel").toInt() );
    QLabel* label1 = new QLabel("Large file");
    QLabel* label2 = new QLabel("Small file");

    QGroupBox* compressionBox = new QGroupBox(tr("Compression"));
    QGridLayout* compressionLayout = new QGridLayout;
    compressionLayout->addWidget(label1,0,0);
    compressionLayout->addWidget(exportFlashDialog_compression,0,1);
    compressionLayout->addWidget(label2,0,2);
    compressionBox->setLayout(compressionLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), exportFlashDialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), exportFlashDialog, SLOT(reject()));

    mainLayout->addWidget(compressionBox, 0, 0);
    mainLayout->addWidget(buttonBox, 1, 0);
    exportFlashDialog->setLayout(mainLayout);
    exportFlashDialog->setWindowTitle(tr("Export SWF Options"));
    exportFlashDialog->setModal(true);
}

QMatrix Editor::map(QRectF source, QRectF target)   // this method should be put somewhere else...
{
    qreal x1 = source.left();
    qreal y1 = source.top();
    qreal x2 = source.right();
    qreal y2 = source.bottom();
    qreal x1P = target.left();
    qreal y1P = target.top();
    qreal x2P = target.right();
    qreal y2P = target.bottom();
    QMatrix matrix;
    bool mirror = false;
    if ( (x1 != x2) && (y1 != y2) )
    {
        if ( !mirror)
        {
            matrix = QMatrix ( (x2P-x1P)/(x2-x1), 0, 0, (y2P-y1P)/(y2-y1), (x1P*x2-x2P*x1)/(x2-x1), (y1P*y2-y2P*y1)/(y2-y1) );
        }
        else
        {
            matrix = QMatrix ( (x2P-x1P)/(x1-x2), 0, 0, (y2P-y1P)/(y2-y1), (x1P*x1-x2P*x2)/(x1-x2), (y1P*y2-y2P*y1)/(y2-y1) );
        }
    }
    else
    {
        matrix.reset();
    }
    return matrix;
}

bool Editor::exportSeqCLI(QString filePath = "", QString format = "PNG")
{
    int width = scribbleArea->getViewRect().toRect().width();
    int height = scribbleArea->getViewRect().toRect().height();

    QSize exportSize = QSize(width, height);
    QByteArray exportFormat(format.toLatin1());

    QMatrix view = map( scribbleArea->getViewRect(), QRectF(QPointF(0,0), exportSize) );
    view = scribbleArea->getView() * view;

    updateMaxFrame();
    object->exportFrames(1, maxFrame, view, getCurrentLayer(), exportSize, filePath, exportFormat, -1, false, true, 2,NULL,0);
    return true;
}

bool Editor::exportSeq()
{
    QSettings settings("Pencil","Pencil");
    QString initialPath = settings.value("lastExportPath", QVariant(QDir::homePath())).toString();
    if (initialPath.isEmpty())
    {
        QString	initialPath = QDir::homePath() + "/untitled";
    }
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Image Sequence"), initialPath,tr("PNG (*.png);;JPG(*.jpg *.jpeg);;TIFF(*.tiff);;TIF(*.tif);;BMP(*.bmp);;GIF(*.gif)"));
    if (filePath.isEmpty())
    {
        return false;
    }
    else
    {
        settings.setValue("lastExportPath", QVariant(filePath));

        if (!exportFramesDialog) createExportFramesDialog();
        exportFramesDialog_hBox->setValue( scribbleArea->getViewRect().toRect().width() );
        exportFramesDialog_vBox->setValue( scribbleArea->getViewRect().toRect().height() );
        exportFramesDialog->exec();
        if (exportFramesDialog->result() == QDialog::Rejected) return false;

        QSize exportSize = QSize(exportFramesDialog_hBox->value(), exportFramesDialog_vBox->value());
        //QMatrix view = map( QRectF(QPointF(0,0), scribbleArea->size() ), QRectF(QPointF(0,0), exportSize) );
        QMatrix view = map( scribbleArea->getViewRect(), QRectF(QPointF(0,0), exportSize) );
        view = scribbleArea->getView() * view;

        QByteArray exportFormat(exportFramesDialog_format->currentText().toLatin1());
        updateMaxFrame();
        object->exportFrames(1, maxFrame, view, getCurrentLayer(), exportSize, filePath, exportFormat, -1, false, true, 2,NULL,0);
        return true;
    }
}

bool Editor::exportX()
{
    QSettings settings("Pencil","Pencil");
    QString initialPath = settings.value("lastExportPath", QVariant(QDir::homePath())).toString();
    if (initialPath.isEmpty()) initialPath = QDir::homePath() + "/untitled";
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save As"),initialPath);
    if (filePath.isEmpty())
    {
        qDebug() << "empty file";
        return false;
    }
    else
    {
        settings.setValue("lastExportPath", QVariant(filePath));

        QSize exportSize = scribbleArea->getViewRect().toRect().size();
        QMatrix view = map( scribbleArea->getViewRect(), QRectF(QPointF(0,0), exportSize) );
        view = scribbleArea->getView() * view;

        updateMaxFrame();
        if (!object->exportX(1, maxFrame, view, exportSize, filePath, true, 2)) {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("Unable to export image."),
                                 QMessageBox::Ok,
                                 QMessageBox::Ok);
            return false;
        }
        return true;
    }
}

bool Editor::exportImage()
{
    QSettings settings("Pencil","Pencil");
    QString initialPath = settings.value("lastExportPath", QVariant(QDir::homePath())).toString();
    if (initialPath.isEmpty())
    {
        initialPath = QDir::homePath() + "/untitled.png";
    }

    QString filePath=QFileDialog::getSaveFileName(this, tr("Save Image"), initialPath, tr("PNG (*.png);;JPG(*.jpg *.jpeg);;TIFF(*.tiff);;TIF(*.tif);;BMP(*.bmp);;GIF(*.gif)"));
    QFileInfo fi(filePath);

    if (fi.suffix().isEmpty()) {
        // add PNG per default if the name has no suffix
        filePath += ".png";
    }

    if (filePath.isEmpty())
    {
        qDebug() << "empty file";
        return false;
    }
    else
    {
        settings.setValue("lastExportPath", QVariant(filePath));

        QSize exportSize = scribbleArea->getViewRect().toRect().size();
        QMatrix view = map( scribbleArea->getViewRect(), QRectF(QPointF(0,0), exportSize) );
        view = scribbleArea->getView() * view;

        updateMaxFrame();
        if (!object->exportIm(m_nCurrentFrameIndex, maxFrame, view, exportSize, filePath, true, 2)) {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("Unable to export image."),
                                 QMessageBox::Ok,
                                 QMessageBox::Ok);
            return false;
        }

        return true;
    }
}

bool Editor::exportMov()
{
    QSettings settings("Pencil","Pencil");
    QString initialPath = settings.value("lastExportPath", QVariant(QDir::homePath())).toString();
    if (initialPath.isEmpty()) initialPath = QDir::homePath() + "/untitled.avi";
//	QString filePath = QFileDialog::getSaveFileName(this, tr("Export As"),initialPath);
    QString filePath=QFileDialog::getSaveFileName(this, tr("Export Movie As..."),initialPath ,tr("AVI (*.avi);;MOV(*.mov);;WMV(*.wmv)"));
    if (filePath.isEmpty())
    {
        return false;
    }
    else
    {
        settings.setValue("lastExportPath", QVariant(filePath));
        if (!exportMovieDialog) createExportMovieDialog();
        exportMovieDialog_hBox->setValue( scribbleArea->getViewRect().toRect().width() );
        exportMovieDialog_vBox->setValue( scribbleArea->getViewRect().toRect().height() );
        exportMovieDialog->exec();
        if (exportMovieDialog->result() == QDialog::Rejected) return false;

        QSize exportSize = QSize(exportMovieDialog_hBox->value(), exportMovieDialog_vBox->value());
        QMatrix view = map( scribbleArea->getViewRect(), QRectF(QPointF(0,0), exportSize) );
        view = scribbleArea->getView() * view;

        updateMaxFrame();
        object->exportMovie(1, maxFrame, view, getCurrentLayer(), exportSize, filePath, fps, exportMovieDialog_fpsBox->value(),exportMovieDialog_format->currentText());
        return true;
    }
}

bool Editor::exportFlash()
{
    QSettings settings("Pencil","Pencil");
    QString initialPath = settings.value("lastExportPath", QVariant(QDir::homePath())).toString();
    if (initialPath.isEmpty()) initialPath = QDir::homePath() + "/untitled.swf";
//	QString filePath = QFileDialog::getSaveFileName(this, tr("Export SWF As"),initialPath);
    QString filePath=QFileDialog::getSaveFileName(this, tr("Export Movie As..."),initialPath ,tr("SWF (*.swf)"));
    if (filePath.isEmpty())
    {
        return false;
    }
    else
    {
        settings.setValue("lastExportPath", QVariant(filePath));
        if (!exportFlashDialog) createExportFlashDialog();
        exportFlashDialog->exec();
        if (exportFlashDialog->result() == QDialog::Rejected) return false;

        settings.setValue("flashCompressionLevel", 10-exportFlashDialog_compression->value() );

        QSize exportSize = scribbleArea->getViewRect().toRect().size();
        QMatrix view = map( scribbleArea->getViewRect(), QRectF(QPointF(0,0), exportSize) );
        view = scribbleArea->getView() * view;

        updateMaxFrame();
        object->exportFlash(1, maxFrame, view, exportSize, filePath, fps, exportFlashDialog_compression->value());
        return true;
    }
}

void Editor::importImage()
{
    importImage("fromDialog");
}

void Editor::importImage(QString filePath)
{
    Layer* layer = object->getLayer(m_nCurrentLayerIndex);
    if (layer == NULL)
    {
        return;
    }
    if ( layer->type != Layer::BITMAP && layer->type != Layer::VECTOR )
    {
        // create a new Bitmap layer ?
        QMessageBox::warning(this, tr("Warning"),
                             tr("Please select a Bitmap or Vector layer to import images."),
                             QMessageBox::Ok,
                             QMessageBox::Ok);
        return;
    }

    if (filePath == "fromDialog")
    {
        QSettings settings("Pencil","Pencil");
        QString initialPath = settings.value("lastImportPath", QVariant(QDir::homePath())).toString();
        if (initialPath.isEmpty()) initialPath = QDir::homePath();
        filePath = QFileDialog::getOpenFileName(this, tr("Import image..."),initialPath ,tr("PNG (*.png);;JPG(*.jpg *.jpeg);;TIFF(*.tiff);;TIF(*.tif);;BMP(*.bmp);;GIF(*.gif)"));
        if (!filePath.isEmpty()) settings.setValue("lastImportPath", QVariant(filePath));
    }

    if (!filePath.isEmpty())
    {
        backup(tr("ImportImg"));
        // --- option 1
        //((LayerBitmap*)layer)->loadImageAtFrame(filePath, currentFrame);
        // --- option 2
        // TO BE IMPROVED
        if (layer->type == Layer::BITMAP)
        {
            BitmapImage* bitmapImage = ((LayerBitmap*)layer)->getBitmapImageAtFrame(m_nCurrentFrameIndex);
            if (bitmapImage == NULL) 
            { 
                addKey(); 
                bitmapImage = ((LayerBitmap*)layer)->getBitmapImageAtFrame(m_nCurrentFrameIndex); 
            }
            QImage* importedImage = new QImage(filePath);

            if (!importedImage->isNull())
            {
                QRect boundaries = importedImage->rect();
                //boundaries.moveTopLeft( scribbleArea->getView().inverted().map(QPoint(0,0)) );
                boundaries.moveTopLeft( scribbleArea->getCentralPoint().toPoint() - QPoint(boundaries.width()/2, boundaries.height()/2) );
                BitmapImage* importedBitmapImage = new BitmapImage(NULL, boundaries, *importedImage);
                if (scribbleArea->somethingSelected)
                {
                    QRectF selection = scribbleArea->getSelection();
                    if ( importedImage->width() <= selection.width() && importedImage->height() <= selection.height() )
                    {
                        importedBitmapImage->boundaries.moveTopLeft( selection.topLeft().toPoint() );
                    }
                    else
                    {
                        importedBitmapImage->transform( selection.toRect(), true );
                    }
                }
                bitmapImage->paste( importedBitmapImage );
            }
            else
            {
                QMessageBox::warning(this, tr("Warning"),
                                     tr("Unable to load bitmap image.<br><b>TIP:</b> Use Bitmap layer to import bitmaps."),
                                     QMessageBox::Ok,
                                     QMessageBox::Ok);
            }
        }
        if (layer->type == Layer::VECTOR)
        {
            VectorImage* vectorImage = ((LayerVector*)layer)->getVectorImageAtFrame(m_nCurrentFrameIndex);
            if (vectorImage == NULL) { addKey(); vectorImage = ((LayerVector*)layer)->getVectorImageAtFrame(m_nCurrentFrameIndex); }
            VectorImage* importedVectorImage = new VectorImage(NULL);
            bool ok = importedVectorImage->read(filePath);
            if (ok)
            {
                importedVectorImage->selectAll();
                vectorImage->paste( *importedVectorImage );
            }
            else
            {
                QMessageBox::warning(this, tr("Warning"),
                                     tr("Unable to load vector image.<br><b>TIP:</b> Use Vector layer to import vectors."),
                                     QMessageBox::Ok,
                                     QMessageBox::Ok);
            }
        }
        scribbleArea->updateFrame();
        getTimeLine()->updateContent();
    }

}

void Editor::importSound(QString filePath)
{
    Layer* layer = object->getLayer(m_nCurrentLayerIndex);
    if (layer == NULL)
    {
        QMessageBox msg;
        msg.setText("You must select an empty sound layer as the destination for your sound before importing. Please create a new sound layer.");
        msg.setIcon(QMessageBox::Warning);
        msg.exec();
        return;
    }

    if (layer->type != Layer::SOUND)
    {
        QMessageBox msg;
        msg.setText("No sound layer exists as a destination for your import. Create a new sound layer?");
        QAbstractButton* acceptButton = msg.addButton("Create sound layer", QMessageBox::AcceptRole);
        msg.addButton("Don't create layer", QMessageBox::RejectRole);

        msg.exec();
        if (msg.clickedButton() == acceptButton)
        {
            newSoundLayer();
            layer = object->getLayer(m_nCurrentLayerIndex);
        }
        else
        {
            return;
        }
    }

    if (!((LayerSound*)layer)->isEmpty())
    {
        QMessageBox msg;
        msg.setText("The sound layer you have selected already contains a sound item. Please select another.");
        msg.exec();
        return;
    }

    if (filePath.isEmpty() || filePath == "fromDialog")
    {
        QSettings settings("Pencil","Pencil");
        QString initialPath = settings.value("lastImportPath", QVariant(QDir::homePath())).toString();
        if (initialPath.isEmpty()) initialPath = QDir::homePath();
        filePath = QFileDialog::getOpenFileName(this, tr("Import sound..."),initialPath,tr("WAV(*.wav);;MP3(*.mp3)"));
        if (!filePath.isEmpty())
        {
            settings.setValue("lastImportPath", QVariant(filePath));
        }
        else
        {
            return;
        }
    }
    ((LayerSound*)layer)->loadSoundAtFrame(filePath, m_nCurrentFrameIndex);
    getTimeLine()->updateContent();
    modification(m_nCurrentLayerIndex);
}

void Editor::updateFrame(int frameNumber)
{
    scribbleArea->updateFrame(frameNumber);
}

void Editor::updateFrameAndVector(int frameNumber)
{
    scribbleArea->updateAllVectorLayersAt(frameNumber);
}

void Editor::scrubTo(int frameNumber)
{
    if (scribbleArea->shouldUpdateAll())
    {
        scribbleArea->updateAllFrames();
    }
    int oldFrame = m_nCurrentFrameIndex;
    if (frameNumber < 1) frameNumber = 1;
    m_nCurrentFrameIndex = frameNumber;
    //timeLine->setCurrentFrame(currentFrame);
    getTimeLine()->updateFrame(oldFrame);
    getTimeLine()->updateFrame(m_nCurrentFrameIndex);
    getTimeLine()->updateContent();
    scribbleArea->readCanvasFromCache = true;
    scribbleArea->update();
}

void Editor::scrubForward()
{
    scrubTo( m_nCurrentFrameIndex + 1 );
}

void Editor::scrubBackward()
{
    if (m_nCurrentFrameIndex > 1) 
    {
        scrubTo(m_nCurrentFrameIndex - 1);
    }
}

void Editor::previousLayer()
{
    m_nCurrentLayerIndex--;
    if (m_nCurrentLayerIndex < 0)
    {
        m_nCurrentLayerIndex = 0;
    }
    getTimeLine()->updateContent();
    scribbleArea->updateAllFrames();
}

void Editor::nextLayer()
{
    m_nCurrentLayerIndex++;
    if (m_nCurrentLayerIndex == object->getLayerCount())
    {
        m_nCurrentLayerIndex = object->getLayerCount()-1;
    }
    getTimeLine()->updateContent();
    scribbleArea->updateAllFrames();
}

void Editor::addKey()
{
    addKey(m_nCurrentLayerIndex, m_nCurrentFrameIndex);
}

void Editor::duplicateKey()
{
    Layer* layer = object->getLayer(m_nCurrentLayerIndex);
    if (layer != NULL)
    {
        if (layer->type == Layer::VECTOR)
        {
            scribbleArea->selectAll();
            clipboardVectorOk = true;
            clipboardVectorImage = *(  ((LayerVector*)layer)->getLastVectorImageAtFrame(m_nCurrentFrameIndex, 0)  );  // copy the image (that works but I should also provide a copy() method)
            addKey();
            VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(m_nCurrentFrameIndex, 0);
            vectorImage->paste( clipboardVectorImage ); // paste the clipboard
            scribbleArea->setModified(m_nCurrentLayerIndex, m_nCurrentFrameIndex);
            update();
        }
        if (layer->type == Layer::BITMAP)
        {
            scribbleArea->selectAll();
            copy();
            addKey();
            paste();
        }
    }
}

void Editor::addKey(int layerNumber, int& frameNumber)
{
    Layer* layer = object->getLayer(layerNumber);
    if (layer != NULL)
    {
        if (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR || layer->type == Layer::CAMERA)
        {
            bool success = false;
            if (layer->type == Layer::BITMAP) success = ((LayerBitmap*)layer)->addImageAtFrame(frameNumber);
            if (layer->type == Layer::VECTOR) success = ((LayerVector*)layer)->addImageAtFrame(frameNumber);
            if (layer->type == Layer::CAMERA) success = ((LayerCamera*)layer)->addImageAtFrame(frameNumber);
            if (success)
            {
                getTimeLine()->updateContent();
                //scribbleArea->addFrame(frameNumber);
            }
            else
            {
                frameNumber++;
                addKey(); // is this necessary?
                updateMaxFrame();
            }
        }
    }
}

void Editor::removeKey()
{
    Layer* layer = object->getLayer(m_nCurrentLayerIndex);
    if (layer != NULL)
    {
        if (layer->type == Layer::BITMAP) ((LayerBitmap*)layer)->removeImageAtFrame(m_nCurrentFrameIndex);
        if (layer->type == Layer::VECTOR) ((LayerVector*)layer)->removeImageAtFrame(m_nCurrentFrameIndex);
        if (layer->type == Layer::CAMERA) ((LayerCamera*)layer)->removeImageAtFrame(m_nCurrentFrameIndex);
        //if (layer->type == Layer::SOUND)  ((LayerSound*)layer)->removeImageAtFrame(currentFrame);
        scrubBackward();
        getTimeLine()->updateContent();
        scribbleArea->updateFrame();
    }
}

void Editor::addFrame(int frameNumber)   // adding a frame to the cache
{
    frameList << frameNumber;
    qSort(frameList);
    scribbleArea->updateFrame();
    qDebug()<< frameList;
    qDebug()<< frameNumber;
    getTimeLine()->update();
}

void Editor::addFrame(int frameNumber1, int frameNumber2)   // adding a range of frames to the cache
{
    for(int i=frameNumber1; i<=frameNumber2; i++)
    {
        frameList << i;
    }
    qSort(frameList);

    scribbleArea->updateFrame();
    getTimeLine()->update();
}

void Editor::removeFrame(int frameNumber)
{
    frameList.removeAt( getLastIndexAtFrame(frameNumber) );
    scribbleArea->updateFrame();
    getTimeLine()->update();
}

int Editor::getLastIndexAtFrame(int frameNumber)
{
    int position  = -1;
    int index = -1;
    for(int i=0; i < frameList.size(); i++)
    {
        if (frameList.at(i) > position && frameList.at(i) <= frameNumber)
        {
            position = frameList.at(i);
            index = i;
        }
    }
    return index;
}

int Editor::getLastFrameAtFrame(int frameNumber)
{
    return frameList.at( getLastIndexAtFrame(frameNumber) );
}

void Editor::play()
{
    updateMaxFrame();
    if (m_nCurrentFrameIndex > maxFrame)
    {
        scrubTo(maxFrame);
    }
    else
    {
        if (m_nCurrentFrameIndex == maxFrame)
        {
            if ( !playing )
            {
                scrubTo(0);
            }
            else
            {
                if ( looping ) { scrubTo(0); }
                else { startOrStop(); }
            }
        }
        else
        {
            startOrStop();
        }
    }
}

void Editor::startOrStop()
{
    if (!playing)
    {
        playing = true;
        timer->start();
    }
    else
    {
        playing = false;
        timer->stop();
        object->stopSoundIfAny();
    }
}


void Editor::scrubNextKeyframe()
{
    Layer* layer = object->getLayer(m_nCurrentLayerIndex);
    if (layer == NULL)
    {
        return;
    }

    int position = layer->getNextKeyframePosition(m_nCurrentFrameIndex);
    if (position != Layer::NO_KEYFRAME)
    {
        scrubTo(position);
    } else {
        if (looping) {
            // scrubto first key frame
            position = layer->getFirstKeyframePosition();
            if (position != Layer::NO_KEYFRAME) {
                scrubTo(position);
            }
        }
    }

}

void Editor::scrubPreviousKeyframe()
{
    Layer* layer = object->getLayer(m_nCurrentLayerIndex);
    if (layer == NULL)
    {
        return;
    }

    int position = layer->getPreviousKeyframePosition(m_nCurrentFrameIndex);
    if (position != Layer::NO_KEYFRAME)
    {
        scrubTo(position);
    } else {
        if (looping) {
            // scrubto first key frame
            position = layer->getLastKeyframePosition();
            if (position != Layer::NO_KEYFRAME) {
                scrubTo(position);
            }
        }
    }
}

void Editor::playNextFrame()
{
    updateMaxFrame();

    if (m_nCurrentFrameIndex < maxFrame)
    {
        if (sound) object->playSoundIfAny(m_nCurrentFrameIndex,fps);
        scrubForward();
    }
    else
    {
        if ( !playing )
        {
            scrubTo(maxFrame);
        }
        else
        {
            if ( looping ) { scrubTo(0); }
            else { startOrStop(); }
        }
    }
}

void Editor::playPrevFrame()
{
    if (m_nCurrentFrameIndex > 0)
    {
        if (sound) object->playSoundIfAny(m_nCurrentFrameIndex,fps);
        scrubBackward();
    }
}

void Editor::changeFps(int x)
{
    fps=x;
//	QSettings settings("Pencil","Pencil");
//	settings.setValue("fps", x);
    timer->setInterval(1000/fps);
    getTimeLine()->updateContent();
}

int Editor::getFps()
{
    return fps;
}

void Editor::setLoop(bool checked)
{
    looping = checked;
}

void Editor::setSound()
{
    if (sound) sound = false;
    else sound = true;
}

void Editor::setCurrentLayer(int layerNumber)
{
    m_nCurrentLayerIndex = layerNumber;
    getTimeLine()->updateContent();
    scribbleArea->updateAllFrames();
}

void Editor::switchVisibilityOfLayer(int layerNumber)
{
    Layer* layer = object->getLayer(layerNumber);
    if (layer != NULL) layer->switchVisibility();
    scribbleArea->updateAllFrames();
    getTimeLine()->updateContent();
}

void Editor::moveLayer(int i, int j)
{
    object->moveLayer(i, j);
    if (j<i) { m_nCurrentLayerIndex = j; }
    else { m_nCurrentLayerIndex = j-1; }
    getTimeLine()->updateContent();
    scribbleArea->updateAllFrames();
}

void Editor::updateMaxFrame()
{
    maxFrame = -1;
    for(int i=0; i < object->getLayerCount(); i++)
    {
        int frameNumber = object->getLayer(i)->getMaxFrameIndex();
        if ( frameNumber > maxFrame)
        {
            maxFrame = frameNumber;
        }
    }
    getTimeLine()->forceUpdateLength(QString::number(maxFrame));
}

void Editor::restorePalettesSettings(bool restoreFloating, bool restorePosition, bool restoreSize)
{
    QSettings settings("Pencil", "Pencil");

    ColorPaletteWidget* colourPalette = mainWindow->m_colorPalette;
    if (colourPalette != NULL)
    {
        QPoint pos = settings.value("colourPalettePosition", QPoint(100, 100)).toPoint();
        QSize size = settings.value("colourPaletteSize", QSize(400, 300)).toSize();
        bool floating = settings.value("colourPaletteFloating", false).toBool();
        if (restoreFloating) colourPalette->setFloating(floating);
        if (restorePosition) colourPalette->move(pos);
        if (restoreSize) colourPalette->resize(size);
        colourPalette->show();
    }

    TimeLine* timelinePalette = getTimeLine();
    if (timelinePalette != NULL)
    {
        QPoint pos = settings.value("timelinePalettePosition", QPoint(100, 100)).toPoint();
        QSize size = settings.value("timelinePaletteSize", QSize(400, 300)).toSize();
        bool floating = settings.value("timelinePaletteFloating", false).toBool();
        if (restoreFloating) timelinePalette->setFloating(floating);
        if (restorePosition) timelinePalette->move(pos);
        if (restoreSize) timelinePalette->resize(size);
        timelinePalette->show();
    }

    QDockWidget* toolWidget = toolSet;
    if (toolWidget != NULL)
    {
        QPoint pos = settings.value("drawPalettePosition", QPoint(100, 100)).toPoint();
        QSize size = settings.value("drawPaletteSize", QSize(400, 300)).toSize();
        bool floating = settings.value("drawPaletteFloating", false).toBool();
        if (restoreFloating) toolWidget->setFloating(floating);
        if (restorePosition) toolWidget->move(pos);
        if (restoreSize) toolWidget->resize(size);
        toolWidget->show();
    }

    QDockWidget* optionPalette = mainWindow->m_toolOptionWidget;
    if (optionPalette != NULL)
    {
        QPoint pos = settings.value("optionPalettePosition", QPoint(100, 100)).toPoint();
        QSize size = settings.value("optionPaletteSize", QSize(400, 300)).toSize();
        bool floating = settings.value("optionPaletteFloating", false).toBool();
        if (restoreFloating) optionPalette->setFloating(floating);
        if (restorePosition) optionPalette->move(pos);
        if (restoreSize) optionPalette->resize(size);
        optionPalette->show();
    }

    QDockWidget* displayPalette = mainWindow->m_displayOptionWidget;
    if (displayPalette != NULL)
    {
        QPoint pos = settings.value("displayPalettePosition", QPoint(100, 100)).toPoint();
        QSize size = settings.value("displayPaletteSize", QSize(400, 300)).toSize();
        bool floating = settings.value("displayPaletteFloating", false).toBool();
        if (restoreFloating) displayPalette->setFloating(floating);
        if (restorePosition) displayPalette->move(pos);
        if (restoreSize) displayPalette->resize(size);
        displayPalette->show();
    }
}

void Editor::clearCurrentFrame()
{
    scribbleArea->clearImage();
}

void Editor::setzoom()
{
    scribbleArea->zoom();
}

void Editor::setzoom1()
{
    scribbleArea->zoom1();
}
void Editor::rotatecw()
{
    scribbleArea->rotatecw();
}

void Editor::rotateacw()
{
    scribbleArea->rotateacw();
}

void Editor::gridview()
{    
    resetView();

    scribbleArea->grid();
    QMessageBox msgBox;
    msgBox.setText("Would you like to add a camera layer?");
    msgBox.exec();
}

void Editor::print()
{
    QPrinter printer(QPrinter::HighResolution);
    //printer.setOrientation(QPrinter::Landscape);
    //printer.setFullPage(false);
    //printer->setPaperSize(QPrinter::A4);

    QPrintPreviewDialog printPreviewDialog(&printer,this);
    connect(&printPreviewDialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printAndPreview(QPrinter*)));
    if (printPreviewDialog.exec() == QDialog::Accepted)
    {
        if (!printer.isValid())
        {
            QMessageBox msg;
            msg.setText("An invalid printer was selected. The print job will now abort.");
            msg.setIcon(QMessageBox::Warning);
            msg.exec();
            return;
        }

        printAndPreview(&printer);
    }
}

void Editor::printAndPreview(QPrinter* printer)
{
    QRect exportRect = scribbleArea->rect();
    QSize exportSize = exportRect.size();
    if (printer->outputFileName() != "")
    {
        QPrinter pdfPrinter(QPrinter::ScreenResolution);
        pdfPrinter.setOutputFileName(printer->outputFileName());
        pdfPrinter.setOutputFormat(QPrinter::PdfFormat);
        pdfPrinter.setOrientation(printer->orientation());
        QPainter painter(&pdfPrinter);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        QRect pageRect = pdfPrinter.pageRect();
        pageRect.moveTo(0, 0);
        qDebug() << "page:" << pageRect.width() << "x" << pageRect.height();
        qDebug() << "image:" << exportRect.width() << "x" << exportRect.height();
        if (exportSize.width() >= exportSize.height())
        {
            // landscape
        }
        else
        {
            // portrait
        }
        //exportSize.scale(pageRect.size(), Qt::KeepAspectRatio);
        //exportRect.setSize(exportSize);
        painter.setViewport(pageRect);
        painter.setWindow(exportRect);
        scribbleArea->render(&painter);
        painter.end();
    }
    else
    {
        QRect pageRect = printer->pageRect();
        pageRect.moveTo(0, 0);
        exportSize.scale(pageRect.size(), Qt::KeepAspectRatio);
        exportRect.setSize(exportSize);
        QPainter painter(printer);
        painter.setViewport(pageRect);
        painter.setWindow(exportRect);
        scribbleArea->render( &painter );
        painter.end();
    }
}

void Editor::getCameraLayer()
{
    for(int i=0; i < object->getLayerCount(); i++)
    {
        Layer* layer = object->getLayer(i);
        // paints the bitmap images
        if (layer->type == Layer::BITMAP)
        {
            nextLayer();
        }
        if (layer->type == Layer::VECTOR)
        {
            nextLayer();
        }
        if (layer->type == Layer::CAMERA)
        {

        }
    }
}

void Editor::endPlay()
{
    int a =frameList.count();
    QListIterator<int> z( frameList );
    for (int i = 0; i < a; ++i)
    {
        int b = z.next();
        scrubTo(b);
    }


}


void Editor::startPlay()
{
//scrubTo(1);

    QListIterator<int> z( frameList );
    for (int i = 0; i < 1; ++i)
    {
        int b = z.next();
        scrubTo(b);
    }
}

void Editor::saveSvg()
{
    QString newPath = QFileDialog::getSaveFileName(this, tr("Save SVG"),
                      path, tr("SVG files (*.svg)"));

    if (newPath.isEmpty())
        return;

    path = newPath;

    QSvgGenerator generator;
    generator.setFileName(path);
    generator.setSize(QSize(700, 500));
    generator.setViewBox(QRect(0, 0, 700, 500));
    generator.setTitle(tr("SVG Generator Example Drawing"));
    generator.setDescription(tr("An SVG drawing created by the SVG Generator "
                                "Example provided with Qt."));

    QPainter painter;
    painter.begin(&generator);
    Layer* layer = object->getLayer(m_nCurrentLayerIndex);
    if (layer != NULL)
    {
        if (layer->type == Layer::VECTOR)
        {
            scribbleArea->selectAll();
            //VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(currentFrame, 0);
            scribbleArea->render(&painter);
        }
    }
    painter.end();
}

void Editor::resetView()
{
    getScribbleArea()->resetView();
}

void Editor::setTool( ToolType toolType)
{
    getScribbleArea()->setCurrentTool(toolType);

    emit changeTool(toolType);
}

