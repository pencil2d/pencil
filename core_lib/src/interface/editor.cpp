/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "editor.h"

#include <memory>
#include <iostream>
#include <QApplication>
#include <QClipboard>
#include <QTimer>
#include <QImageReader>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

#include "object.h"
#include "objectdata.h"
#include "vectorimage.h"
#include "bitmapimage.h"
#include "soundclip.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layercamera.h"
#include "backupelement.h"

#include "colormanager.h"
#include "toolmanager.h"
#include "layermanager.h"
#include "playbackmanager.h"
#include "viewmanager.h"
#include "preferencemanager.h"
#include "soundmanager.h"
#include "selectionmanager.h"

#include "scribblearea.h"
#include "timeline.h"
#include "util.h"
#include "movieexporter.h"

#define MIN(a,b) ((a)>(b)?(b):(a))


static BitmapImage g_clipboardBitmapImage;
static VectorImage g_clipboardVectorImage;


Editor::Editor(QObject* parent) : QObject(parent)
{
    mBackupIndex = -1;
    clipboardBitmapOk = false;
    clipboardVectorOk = false;
    clipboardSoundClipOk = false;
}

Editor::~Editor()
{
    // a lot more probably needs to be cleaned here...
    clearUndoStack();
    clearTemporary();
}

bool Editor::init()
{
    // Initialize managers
    mColorManager = new ColorManager(this);
    mLayerManager = new LayerManager(this);
    mToolManager = new ToolManager(this);
    mPlaybackManager = new PlaybackManager(this);
    mViewManager = new ViewManager(this);
    mPreferenceManager = new PreferenceManager(this);
    mSoundManager = new SoundManager(this);
    mSelectionManager = new SelectionManager(this);

    mAllManagers =
    {
        mColorManager,
        mToolManager,
        mLayerManager,
        mPlaybackManager,
        mViewManager,
        mPreferenceManager,
        mSoundManager,
        mSelectionManager
    };

    for (BaseManager* pManager : mAllManagers)
    {
        pManager->init();
    }
    //setAcceptDrops( true ); // TODO: drop event

    makeConnections();

    mIsAutosave = mPreferenceManager->isOn(SETTING::AUTO_SAVE);
    mAutosaveNumber = mPreferenceManager->getInt(SETTING::AUTO_SAVE_NUMBER);

    return true;
}

int Editor::currentFrame()
{
    return mFrame;
}

int Editor::fps()
{
    return mPlaybackManager->fps();
}

void Editor::setFps(int fps)
{
    mPreferenceManager->set(SETTING::FPS, fps);
}

void Editor::makeConnections()
{
    connect(mPreferenceManager, &PreferenceManager::optionChanged, this, &Editor::settingUpdated);
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &Editor::clipboardChanged);
}

void Editor::dragEnterEvent(QDragEnterEvent* event)
{
    event->acceptProposedAction();
}

void Editor::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        for (int i = 0; i < event->mimeData()->urls().size(); i++)
        {
            if (i > 0) scrubForward();
            QUrl url = event->mimeData()->urls()[i];
            QString filePath = url.toLocalFile();
            if (filePath.endsWith(".png") || filePath.endsWith(".jpg") || filePath.endsWith(".jpeg"))
            {
                importImage(filePath);
            }
            //if ( filePath.endsWith( ".aif" ) || filePath.endsWith( ".mp3" ) || filePath.endsWith( ".wav" ) )
                //importSound( filePath );
        }
    }
}

void Editor::settingUpdated(SETTING setting)
{
    switch (setting)
    {
    case SETTING::AUTO_SAVE:
        mIsAutosave = mPreferenceManager->isOn(SETTING::AUTO_SAVE);
        break;
    case SETTING::AUTO_SAVE_NUMBER:
        mAutosaveNumber = mPreferenceManager->getInt(SETTING::AUTO_SAVE_NUMBER);
        break;
    case SETTING::ONION_TYPE:
        mScribbleArea->updateAllFrames();
        emit updateTimeLine();
        break;
    case SETTING::FRAME_POOL_SIZE:
        mObject->setActiveFramePoolSize(mPreferenceManager->getInt(SETTING::FRAME_POOL_SIZE));
        break;
    case SETTING::LAYER_VISIBILITY:
        mScribbleArea->setLayerVisibility(static_cast<LayerVisibility>(mPreferenceManager->getInt(SETTING::LAYER_VISIBILITY)));
        emit updateTimeLine();
        break;
    default:
        break;
    }
}

BackupElement* Editor::currentBackup()
{
    if (mBackupIndex >= 0)
    {
        return mBackupList[mBackupIndex];
    }
    else
    {
        return nullptr;
    }
}

void Editor::backup(QString undoText)
{
    KeyFrame* frame = nullptr;
    if (mLastModifiedLayer > -1 && mLastModifiedFrame > 0)
    {
        if (layers()->currentLayer()->type() == Layer::SOUND)
        {
            frame = layers()->currentLayer()->getKeyFrameWhichCovers(mLastModifiedFrame);
            if (frame != nullptr)
            {
                backup(mLastModifiedLayer, frame->pos(), undoText);
            }
        }
        else
        {
            backup(mLastModifiedLayer, mLastModifiedFrame, undoText);
        }
    }
    if (mLastModifiedLayer != layers()->currentLayerIndex() || mLastModifiedFrame != currentFrame())
    {
        if (layers()->currentLayer()->type() == Layer::SOUND)
        {
            frame = layers()->currentLayer()->getKeyFrameWhichCovers(currentFrame());

            if (frame != nullptr)
            {
                backup(layers()->currentLayerIndex(), frame->pos(), undoText);
            }
        }
        else
        {
            backup(layers()->currentLayerIndex(), currentFrame(), undoText);
        }
    }
}

void Editor::backup(int backupLayer, int backupFrame, QString undoText)
{
    while (mBackupList.size() - 1 > mBackupIndex && mBackupList.size() > 0)
    {
        delete mBackupList.takeLast();
    }
    while (mBackupList.size() > 19)   // we authorize only 20 levels of cancellation
    {
        delete mBackupList.takeFirst();
        mBackupIndex--;
    }

    Layer* layer = mObject->getLayer(backupLayer);
    if (layer != nullptr)
    {
        if (layer->type() == Layer::BITMAP)
        {
            BitmapImage* bitmapImage = static_cast<LayerBitmap*>(layer)->getLastBitmapImageAtFrame(backupFrame, 0);
            if (currentFrame() == 1) {
                int previous = layer->getPreviousKeyFramePosition(backupFrame);
                bitmapImage = static_cast<LayerBitmap*>(layer)->getBitmapImageAtFrame(previous);
            }
            if (bitmapImage != nullptr)
            {
                BackupBitmapElement* element = new BackupBitmapElement(bitmapImage);
                element->layer = backupLayer;
                element->frame = backupFrame;
                element->undoText = undoText;
                element->somethingSelected = select()->somethingSelected();
                element->mySelection = select()->mySelectionRect();
                element->myTransformedSelection = select()->myTransformedSelectionRect();
                element->myTempTransformedSelection = select()->myTempTransformedSelectionRect();
                element->rotationAngle = select()->myRotation();
                mBackupList.append(element);
                mBackupIndex++;
            }
        }
        else if (layer->type() == Layer::VECTOR)
        {
            VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(backupFrame, 0);
            if (vectorImage != nullptr)
            {
                BackupVectorElement* element = new BackupVectorElement(vectorImage);
                element->layer = backupLayer;
                element->frame = backupFrame;
                element->undoText = undoText;
                element->somethingSelected = select()->somethingSelected();
                element->mySelection = select()->mySelectionRect();
                element->myTransformedSelection = select()->myTransformedSelectionRect();
                element->myTempTransformedSelection = select()->myTempTransformedSelectionRect();
                element->rotationAngle = select()->myRotation();
                mBackupList.append(element);
                mBackupIndex++;
            }
        }
        else if (layer->type() == Layer::SOUND)
        {
            int previous = layer->getPreviousKeyFramePosition(backupFrame);
            KeyFrame* key = layer->getLastKeyFrameAtPosition(backupFrame);

            // in case tracks overlap, get previous frame
            if (key == nullptr)
            {
                KeyFrame* previousKey = layer->getKeyFrameAt(previous);
                key = previousKey;
            }
            if (key != nullptr) {
                SoundClip* clip = static_cast<SoundClip*>(key);
                if (clip)
                {
                    BackupSoundElement* element = new BackupSoundElement(clip);
                    element->layer = backupLayer;
                    element->frame = backupFrame;
                    element->undoText = undoText;
                    element->fileName = clip->fileName();
                    mBackupList.append(element);
                    mBackupIndex++;
                }
            }
        }
    }

    updateAutoSaveCounter();

    emit updateBackup();
}

void Editor::restoreKey()
{
    BackupElement* lastBackupElement = mBackupList[mBackupIndex];

    Layer* layer = nullptr;
    int frame = 0;
    int layerIndex = 0;
    if (lastBackupElement->type() == BackupElement::BITMAP_MODIF)
    {
        BackupBitmapElement* lastBackupBitmapElement = static_cast<BackupBitmapElement*>(lastBackupElement);
        layerIndex = lastBackupBitmapElement->layer;
        frame = lastBackupBitmapElement->frame;
        layer = object()->getLayer(layerIndex);
        addKeyFrame(layerIndex, frame);
        dynamic_cast<LayerBitmap*>(layer)->getBitmapImageAtFrame(frame)->paste(&lastBackupBitmapElement->bitmapImage);
    }
    if (lastBackupElement->type() == BackupElement::VECTOR_MODIF)
    {
        BackupVectorElement* lastBackupVectorElement = static_cast<BackupVectorElement*>(lastBackupElement);
        layerIndex = lastBackupVectorElement->layer;
        frame = lastBackupVectorElement->frame;
        layer = object()->getLayer(layerIndex);
        addKeyFrame(layerIndex, frame);
        dynamic_cast<LayerVector*>(layer)->getVectorImageAtFrame(frame)->paste(lastBackupVectorElement->vectorImage);
    }
    if (lastBackupElement->type() == BackupElement::SOUND_MODIF)
    {
        QString strSoundFile;
        BackupSoundElement* lastBackupSoundElement = static_cast<BackupSoundElement*>(lastBackupElement);
        layerIndex = lastBackupSoundElement->layer;
        frame = lastBackupSoundElement->frame;

        strSoundFile = lastBackupSoundElement->fileName;
        KeyFrame* key = addKeyFrame(layerIndex, frame);
        SoundClip* clip = dynamic_cast<SoundClip*>(key);
        if (clip)
        {
            if (strSoundFile.isEmpty())
            {
                return;
            }
            else
            {
                //Status st = sound()->pasteSound(clip, strSoundFile);
                //Q_ASSERT(st.ok());
            }
        }
    }
}

void BackupBitmapElement::restore(Editor* editor)
{
    Layer* layer = editor->object()->getLayer(this->layer);
    auto selectMan = editor->select();
    selectMan->setSelection(mySelection);
    selectMan->setTransformedSelectionRect(myTransformedSelection);
    selectMan->setTempTransformedSelectionRect(myTempTransformedSelection);
    selectMan->setRotation(rotationAngle);
    selectMan->setSomethingSelected(somethingSelected);

    editor->updateFrame(this->frame);
    editor->scrubTo(this->frame);

    if (this->frame > 0 && layer->getKeyFrameAt(this->frame) == nullptr)
    {
        editor->restoreKey();
    }
    else
    {
        if (layer != nullptr)
        {
            if (layer->type() == Layer::BITMAP)
            {
                auto pLayerBitmap = static_cast<LayerBitmap*>(layer);
                *pLayerBitmap->getLastBitmapImageAtFrame(this->frame, 0) = this->bitmapImage;  // restore the image
            }
        }
    }
}

void BackupVectorElement::restore(Editor* editor)
{
    Layer* layer = editor->object()->getLayer(this->layer);
    auto selectMan = editor->select();
    selectMan->setSelection(mySelection);
    selectMan->setTransformedSelectionRect(myTransformedSelection);
    selectMan->setTempTransformedSelectionRect(myTempTransformedSelection);
    selectMan->setRotation(rotationAngle);
    selectMan->setSomethingSelected(somethingSelected);

    editor->updateFrameAndVector(this->frame);
    editor->scrubTo(this->frame);
    if (this->frame > 0 && layer->getKeyFrameAt(this->frame) == nullptr)
    {
        editor->restoreKey();
    }
    else
    {
        if (layer != nullptr)
        {
            if (layer->type() == Layer::VECTOR)
            {
                auto pVectorImage = static_cast<LayerVector*>(layer);
                *pVectorImage->getLastVectorImageAtFrame(this->frame, 0) = this->vectorImage;  // restore the image
            }
        }
    }
}

void BackupSoundElement::restore(Editor* editor)
{
    Layer* layer = editor->object()->getLayer(this->layer);
    editor->updateFrame(this->frame);
    editor->scrubTo(this->frame);

    // TODO: soundclip won't restore if overlapping on first frame
    if (this->frame > 0 && layer->getKeyFrameAt(this->frame) == nullptr)
    {
        editor->restoreKey();
    }
}

void Editor::undo()
{
    if (mBackupList.size() > 0 && mBackupIndex > -1)
    {
        if (mBackupIndex == mBackupList.size() - 1)
        {
            BackupElement* lastBackupElement = mBackupList[mBackupIndex];
            if (lastBackupElement->type() == BackupElement::BITMAP_MODIF)
            {
                BackupBitmapElement* lastBackupBitmapElement = static_cast<BackupBitmapElement*>(lastBackupElement);
                backup(lastBackupBitmapElement->layer, lastBackupBitmapElement->frame, "NoOp");
                mBackupIndex--;
            }
            if (lastBackupElement->type() == BackupElement::VECTOR_MODIF)
            {
                BackupVectorElement* lastBackupVectorElement = static_cast<BackupVectorElement*>(lastBackupElement);
                backup(lastBackupVectorElement->layer, lastBackupVectorElement->frame, "NoOp");
                mBackupIndex--;
            }
            if (lastBackupElement->type() == BackupElement::SOUND_MODIF)
            {
                BackupSoundElement* lastBackupSoundElement = static_cast<BackupSoundElement*>(lastBackupElement);
                backup(lastBackupSoundElement->layer, lastBackupSoundElement->frame, "NoOp");
                mBackupIndex--;
            }
        }

        mBackupList[mBackupIndex]->restore(this);
        mBackupIndex--;
        mScribbleArea->cancelTransformedSelection();

        Layer* layer = layers()->currentLayer();
        if (layer == nullptr) { return; }

        select()->resetSelectionTransform();
        if (layer->type() == Layer::VECTOR) {
            VectorImage *vectorImage = static_cast<LayerVector*>(layer)->getVectorImageAtFrame(mFrame);
            vectorImage->calculateSelectionRect();
            select()->setSelection(vectorImage->getSelectionRect());
        }
        emit updateBackup();
    }
}

void Editor::redo()
{
    if (mBackupList.size() > 0 && mBackupIndex < mBackupList.size() - 2)
    {
        mBackupIndex++;

        mBackupList[mBackupIndex + 1]->restore(this);
        emit updateBackup();
    }
}

void Editor::clearUndoStack()
{
    mBackupIndex = -1;
    while (!mBackupList.isEmpty())
    {
        delete mBackupList.takeLast();
    }
    mLastModifiedLayer = -1;
    mLastModifiedFrame = -1;
}

void Editor::updateAutoSaveCounter()
{
    if (mIsAutosave == false)
        return;

    mAutosaveCounter++;
    if (mAutosaveCounter >= mAutosaveNumber)
    {
        resetAutoSaveCounter();
        emit needSave();
    }
}

void Editor::resetAutoSaveCounter()
{
    mAutosaveCounter = 0;
}

void Editor::cut()
{
    copy();
    mScribbleArea->deleteSelection();
    deselectAll();
}

void Editor::copy()
{
    Layer* layer = mObject->getLayer(layers()->currentLayerIndex());
    if (layer == nullptr)
    {
        return;
    }

    if (layer->type() == Layer::BITMAP)
    {
        LayerBitmap* layerBitmap = static_cast<LayerBitmap*>(layer);
        if (select()->somethingSelected())
        {
            g_clipboardBitmapImage = layerBitmap->getLastBitmapImageAtFrame(currentFrame(), 0)->copy(select()->mySelectionRect().toRect());  // copy part of the image
        }
        else
        {
            g_clipboardBitmapImage = layerBitmap->getLastBitmapImageAtFrame(currentFrame(), 0)->copy();  // copy the whole image
        }
        clipboardBitmapOk = true;
        if (g_clipboardBitmapImage.image() != nullptr)
            QApplication::clipboard()->setImage(*g_clipboardBitmapImage.image());
    }
    if (layer->type() == Layer::VECTOR)
    {
        clipboardVectorOk = true;
        g_clipboardVectorImage = *((static_cast<LayerVector*>(layer))->getLastVectorImageAtFrame(currentFrame(), 0));  // copy the image
    }
}

void Editor::paste()
{
    Layer* layer = mObject->getLayer(layers()->currentLayerIndex());
    if (layer != nullptr)
    {
        if (layer->type() == Layer::BITMAP && g_clipboardBitmapImage.image() != nullptr)
        {
            backup(tr("Paste"));

            BitmapImage tobePasted = g_clipboardBitmapImage.copy();
            qDebug() << "to be pasted --->" << tobePasted.image()->size();
            if (select()->somethingSelected())
            {
                QRectF selection = select()->mySelectionRect();
                if (g_clipboardBitmapImage.width() <= selection.width() && g_clipboardBitmapImage.height() <= selection.height())
                {
                    tobePasted.moveTopLeft(selection.topLeft());
                }
                else
                {
                    tobePasted.transform(selection, true);
                }
            }
            auto pLayerBitmap = static_cast<LayerBitmap*>(layer);
            pLayerBitmap->getLastBitmapImageAtFrame(currentFrame(), 0)->paste(&tobePasted); // paste the clipboard
        }
        else if (layer->type() == Layer::VECTOR && clipboardVectorOk)
        {
            backup(tr("Paste"));
            deselectAll();
            VectorImage* vectorImage = (static_cast<LayerVector*>(layer))->getLastVectorImageAtFrame(currentFrame(), 0);
            vectorImage->paste(g_clipboardVectorImage);  // paste the clipboard
            select()->setSelection(vectorImage->getSelectionRect());
        }
    }
    mScribbleArea->updateCurrentFrame();
}

void Editor::flipSelection(bool flipVertical)
{
    mScribbleArea->flipSelection(flipVertical);
}

void Editor::clipboardChanged()
{
    if (clipboardBitmapOk == false)
    {
        g_clipboardBitmapImage.setImage(new QImage(QApplication::clipboard()->image()));
        g_clipboardBitmapImage.bounds() = QRect(g_clipboardBitmapImage.topLeft(), g_clipboardBitmapImage.image()->size());
        //qDebug() << "New clipboard image" << g_clipboardBitmapImage.image()->size();
    }
    else
    {
        clipboardBitmapOk = false;
        //qDebug() << "The image has been saved in the clipboard";
    }
}

void Editor::setLayerVisibility(LayerVisibility visibility) {
    mScribbleArea->setLayerVisibility(visibility);
    emit updateTimeLine();
}

void Editor::notifyAnimationLengthChanged()
{
    layers()->notifyAnimationLengthChanged();
}

LayerVisibility Editor::layerVisibility()
{
    return mScribbleArea->getLayerVisibility();
}

void Editor::increaseLayerVisibilityIndex()
{
    mScribbleArea->increaseLayerVisibilityIndex();
    emit updateTimeLine();
}

void Editor::decreaseLayerVisibilityIndex()
{
    mScribbleArea->decreaseLayerVisibilityIndex();
    emit updateTimeLine();
}

void Editor::toogleOnionSkinType()
{
    QString onionSkinState = mPreferenceManager->getString(SETTING::ONION_TYPE);
    QString newState;
    if (onionSkinState == "relative")
    {
        newState = "absolute";
    }
    else
    {
        newState = "relative";
    }

    mPreferenceManager->set(SETTING::ONION_TYPE, newState);
}

void Editor::addTemporaryDir(QTemporaryDir* const dir)
{
    mTemporaryDirs.append(dir);
}

void Editor::clearTemporary()
{
    while(!mTemporaryDirs.isEmpty()) {
        mTemporaryDirs.takeFirst()->remove();
    }
}

Status Editor::setObject(Object* newObject)
{
    if (newObject == nullptr)
    {
        Q_ASSERT(false);
        return Status::INVALID_ARGUMENT;
    }

    if (newObject == mObject.get())
    {
        return Status::SAFE;
    }

    clearUndoStack();
    mObject.reset(newObject);

    for (BaseManager* m : mAllManagers)
    {
        m->load(mObject.get());
    }

    g_clipboardVectorImage.setObject(newObject);

    updateObject();

    if (mViewManager)
    {
        connect(newObject, &Object::layerViewChanged, mViewManager, &ViewManager::viewChanged);
    }

    emit objectLoaded();

    return Status::OK;
}

void Editor::updateObject()
{
    setCurrentLayerIndex(mObject->data()->getCurrentLayer());
    scrubTo(mObject->data()->getCurrentFrame());

    mAutosaveCounter = 0;
    mAutosaveNeverAskAgain = false;

    if (mScribbleArea)
    {
        mScribbleArea->updateAllFrames();
    }
    
    if (mPreferenceManager)
    {
        mObject->setActiveFramePoolSize(mPreferenceManager->getInt(SETTING::FRAME_POOL_SIZE));
    }

    emit updateLayerCount();
}

/* TODO: Export absolutely does not belong here, but due to the messed up project structure
 * there isn't really any better place atm. Once we do have a proper structure in place, this
 * should go somewhere else */
bool Editor::exportSeqCLI(QString filePath, LayerCamera *cameraLayer, QString format, int width, int height, int startFrame, int endFrame, bool transparency, bool antialias)
{
    if (width < 0)
    {
        width = cameraLayer->getViewRect().width();
    }
    if (height < 0)
    {
        height = cameraLayer->getViewRect().height();
    }
    if (startFrame < 1)
    {
        startFrame = 1;
    }
    if (endFrame < -1)
    {
        endFrame = mLayerManager->animationLength();
    }
    if (endFrame < 0)
    {
        endFrame = mLayerManager->animationLength(false);
    }

    QSize exportSize = QSize(width, height);
    mObject->exportFrames(startFrame,
                          endFrame,
                          cameraLayer,
                          exportSize,
                          filePath,
                          format,
                          transparency,
                          false,
                          "",
                          antialias,
                          nullptr,
                          0);
    return true;
}

bool Editor::exportMovieCLI(QString filePath, LayerCamera *cameraLayer, int width, int height, int startFrame, int endFrame)
{
    if (width < 0)
    {
        width = cameraLayer->getViewRect().width();
    }
    if (height < 0)
    {
        height = cameraLayer->getViewRect().height();
    }
    if (startFrame < 1)
    {
        startFrame = 1;
    }
    if (endFrame < -1)
    {
        endFrame = mLayerManager->animationLength();
    }
    if (endFrame < 0)
    {
        endFrame = mLayerManager->animationLength(false);
    }

    QSize exportSize = QSize(width, height);

    ExportMovieDesc desc;
    desc.strFileName = filePath;
    desc.startFrame = startFrame;
    desc.endFrame = endFrame;
    desc.fps = playback()->fps();
    desc.exportSize = exportSize;
    desc.strCameraName = cameraLayer->name();

    MovieExporter ex;
    ex.run(object(), desc, [](float,float){}, [](float){}, [](QString){});
    return true;
}

QString Editor::workingDir() const
{
    return mObject->workingDir();
}

bool Editor::importBitmapImage(QString filePath, int space)
{
    QImageReader reader(filePath);

    Q_ASSERT(layers()->currentLayer()->type() == Layer::BITMAP);
    auto layer = static_cast<LayerBitmap*>(layers()->currentLayer());

    QImage img(reader.size(), QImage::Format_ARGB32_Premultiplied);
    if (img.isNull())
    {
        return false;
    }

    const QPoint pos = QPoint(static_cast<int>(view()->getImportView().dx()),
                              static_cast<int>(view()->getImportView().dy())) - QPoint(img.width() / 2, img.height() / 2);
    while (reader.read(&img))
    {
        if (!layer->keyExists(currentFrame()))
        {
            addNewKey();
        }
        BitmapImage* bitmapImage = layer->getBitmapImageAtFrame(currentFrame());
        BitmapImage importedBitmapImage(pos, img);
        bitmapImage->paste(&importedBitmapImage);

        if (space > 1) {
            scrubTo(currentFrame() + space);
        } else {
            scrubTo(currentFrame() + 1);
        }

        backup(tr("Import Image"));

        // Workaround for tiff import getting stuck in this loop
        if (!reader.supportsAnimation())
        {
            break;
        }
    }

    return true;
}

bool Editor::importVectorImage(QString filePath)
{
    Q_ASSERT(layers()->currentLayer()->type() == Layer::VECTOR);

    auto layer = static_cast<LayerVector*>(layers()->currentLayer());

    VectorImage* vectorImage = (static_cast<LayerVector*>(layer))->getVectorImageAtFrame(currentFrame());
    if (vectorImage == nullptr)
    {
        addNewKey();
        vectorImage = (static_cast<LayerVector*>(layer))->getVectorImageAtFrame(currentFrame());
    }

    VectorImage importedVectorImage;
    bool ok = importedVectorImage.read(filePath);
    if (ok)
    {
        importedVectorImage.selectAll();
        vectorImage->paste(importedVectorImage);

        backup(tr("Import Image"));
    }

    return ok;
}

void Editor::createNewBitmapLayer(const QString& name)
{
    Layer* layer = layers()->createBitmapLayer(name);
    layers()->setCurrentLayer(layer);
}

void Editor::createNewVectorLayer(const QString& name)
{
    Layer* layer = layers()->createVectorLayer(name);
    layers()->setCurrentLayer(layer);
}

void Editor::createNewSoundLayer(const QString& name)
{
    Layer* layer = layers()->createVectorLayer(name);
    layers()->setCurrentLayer(layer);
}

void Editor::createNewCameraLayer(const QString& name)
{
    Layer* layer = layers()->createCameraLayer(name);
    layers()->setCurrentLayer(layer);
}

bool Editor::importImage(QString filePath)
{
    Layer* layer = layers()->currentLayer();

    if (view()->getImportFollowsCamera())
    {
        LayerCamera* camera = static_cast<LayerCamera*>(layers()->getLastCameraLayer());
        QTransform transform = camera->getViewAtFrame(currentFrame());
        view()->setImportView(transform);
    }
    switch (layer->type())
    {
    case Layer::BITMAP:
        return importBitmapImage(filePath);

    case Layer::VECTOR:
        return importVectorImage(filePath);

    default:
    {
        //mLastError = Status::ERROR_INVALID_LAYER_TYPE;
        return false;
    }
    }
}

bool Editor::importGIF(QString filePath, int numOfImages)
{
    Layer* layer = layers()->currentLayer();
    if (layer->type() == Layer::BITMAP)
    {
        return importBitmapImage(filePath, numOfImages);
    }
    return false;
}

qreal Editor::viewScaleInversed()
{
    return view()->getViewInverse().m11();
}

void Editor::selectAll()
{
    Layer* layer = layers()->currentLayer();

    QRectF rect;
    if (layer->type() == Layer::BITMAP)
    {
        // Selects the drawn area (bigger or smaller than the screen). It may be more accurate to select all this way
        // as the drawing area is not limited
        BitmapImage *bitmapImage = static_cast<LayerBitmap*>(layer)->getLastBitmapImageAtFrame(mFrame);
        rect = bitmapImage->bounds();
    }
    else if (layer->type() == Layer::VECTOR)
    {
        VectorImage *vectorImage = static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mFrame,0);
        vectorImage->selectAll();
        rect = vectorImage->getSelectionRect();
    }
    select()->setSelection(rect);
    emit updateCurrentFrame();
}

void Editor::deselectAll()
{
    Layer* layer = layers()->currentLayer();
    if (layer == nullptr) { return; }

    if (layer->type() == Layer::VECTOR)
    {
        static_cast<LayerVector*>(layer)->getLastVectorImageAtFrame(mFrame, 0)->deselectAll();
    }

    select()->resetSelectionProperties();
}

void Editor::updateFrame(int frameNumber)
{
    mScribbleArea->updateFrame(frameNumber);
}

void Editor::updateFrameAndVector(int frameNumber)
{
    mScribbleArea->updateAllVectorLayersAt(frameNumber);
}

void Editor::updateCurrentFrame()
{
    mScribbleArea->updateCurrentFrame();
}

void Editor::setCurrentLayerIndex(int i)
{
    mCurrentLayerIndex = i;

    Layer* layer = mObject->getLayer(i);
    for (auto mgr : mAllManagers)
    {
        mgr->workingLayerChanged(layer);
    }
}

void Editor::scrubTo(int frame)
{
    if (frame < 1) { frame = 1; }
    int oldFrame = mFrame;
    mFrame = frame;

    Q_EMIT currentFrameChanged(oldFrame);
    Q_EMIT currentFrameChanged(frame);

    // FIXME: should not emit Timeline update here.
    // Editor must be an individual class.
    // Will remove all Timeline related code in Editor class.
    if (mPlaybackManager && !mPlaybackManager->isPlaying())
    {
        emit updateTimeLine(); // needs to update the timeline to update onion skin positions
    }
    mObject->updateActiveFrames(frame);
}

void Editor::scrubForward()
{
    int nextFrame = mFrame + 1;
    if (!playback()->isPlaying()) {
        playback()->playScrub(nextFrame);
    }
    scrubTo(nextFrame);
}

void Editor::scrubBackward()
{
    if (currentFrame() > 1)
    {
        int previousFrame = mFrame - 1;
        if (!playback()->isPlaying()) {
            playback()->playScrub(previousFrame);
        }
        scrubTo(previousFrame);
    }
}

KeyFrame* Editor::addNewKey()
{
    return addKeyFrame(layers()->currentLayerIndex(), currentFrame());
}

KeyFrame* Editor::addKeyFrame(int layerNumber, int frameIndex)
{
    Layer* layer = mObject->getLayer(layerNumber);
    if (layer == nullptr)
    {
        Q_ASSERT(false);
        return nullptr;
    }

    if (!layer->visible())
    {
        mScribbleArea->showLayerNotVisibleWarning();
        return nullptr;
    }

    // Find next available space for a keyframe (where either no key exists or there is an empty sound key)
    while (layer->keyExists(frameIndex))
    {
        if (layer->type() == Layer::SOUND && static_cast<SoundClip*>(layer->getKeyFrameAt(frameIndex))->fileName().isEmpty()
                && layer->removeKeyFrame(frameIndex))
        {
            break;
        }
        else
        {
            frameIndex += 1;
        }
    }

    bool ok = layer->addNewKeyFrameAt(frameIndex);
    if (ok)
    {
        scrubTo(frameIndex); // currentFrameChanged() emit inside.
        layers()->notifyAnimationLengthChanged();
    }
    return layer->getKeyFrameAt(frameIndex);
}

void Editor::removeKey()
{
    Layer* layer = layers()->currentLayer();
    Q_ASSERT(layer != nullptr);

    if (!layer->visible())
    {
        mScribbleArea->showLayerNotVisibleWarning();
        return;
    }

    if (!layer->keyExistsWhichCovers(currentFrame()))
    {
        return;
    }

    backup(tr("Remove frame"));

    deselectAll();
    layer->removeKeyFrame(currentFrame());

    scrubBackward();
    layers()->notifyAnimationLengthChanged();
    Q_EMIT layers()->currentLayerChanged(layers()->currentLayerIndex()); // trigger timeline repaint.
}

void Editor::scrubNextKeyFrame()
{
    Layer* layer = layers()->currentLayer();
    Q_ASSERT(layer);

    int nextPosition = layer->getNextKeyFramePosition(currentFrame());
    scrubTo(nextPosition);
}

void Editor::scrubPreviousKeyFrame()
{
    Layer* layer = mObject->getLayer(layers()->currentLayerIndex());
    Q_ASSERT(layer);

    int prevPosition = layer->getPreviousKeyFramePosition(currentFrame());
    scrubTo(prevPosition);
}

void Editor::switchVisibilityOfLayer(int layerNumber)
{
    Layer* layer = mObject->getLayer(layerNumber);
    if (layer != nullptr) layer->switchVisibility();
    mScribbleArea->updateAllFrames();

    emit updateTimeLine();
}

void Editor::showLayerNotVisibleWarning()
{
    return mScribbleArea->showLayerNotVisibleWarning();
}

void Editor::swapLayers(int i, int j)
{
    mObject->swapLayers(i, j);
    if (j < i)
    {
        layers()->setCurrentLayer(j + 1);
    }
    else
    {
        layers()->setCurrentLayer(j - 1);
    }
    emit updateTimeLine();
    mScribbleArea->updateAllFrames();
}

Status::StatusInt Editor::pegBarAlignment(QStringList layers)
{
    Status::StatusInt retLeft;
    Status::StatusInt retRight;

    LayerBitmap* layerbitmap = static_cast<LayerBitmap*>(mLayerManager->currentLayer());
    BitmapImage* img = layerbitmap->getBitmapImageAtFrame(currentFrame());
    QRectF rect = select()->mySelectionRect();
    retLeft = img->findLeft(rect, 121);
    retRight = img->findTop(rect, 121);
    if (retLeft.errorcode == Status::FAIL || retRight.errorcode == Status::FAIL)
    {
        retLeft.errorcode = Status::FAIL;
        return retLeft;
    }
    int peg_x = retLeft.value;
    int peg_y = retRight.value;

    // move other layers
    for (int i = 0; i < layers.count(); i++)
    {
        layerbitmap = static_cast<LayerBitmap*>(mLayerManager->findLayerByName(layers.at(i)));
        for (int k = layerbitmap->firstKeyFramePosition(); k <= layerbitmap->getMaxKeyFramePosition(); k++)
        {
            if (layerbitmap->keyExists(k))
            {
                img = layerbitmap->getBitmapImageAtFrame(k);
                retLeft = img->findLeft(rect, 121);
                const QString body = tr("Peg bar not found at %1, %2").arg(layerbitmap->name()).arg(k);
                if (retLeft.errorcode == Status::FAIL)
                {
                    emit needDisplayInfoNoTitle(body);
                    return retLeft;
                }
                retRight = img->findTop(rect, 121);
                if (retRight.errorcode == Status::FAIL)
                {
                    emit needDisplayInfoNoTitle(body);
                    return retRight;
                }
                img->moveTopLeft(QPoint(img->left() + (peg_x - retLeft.value), img->top() + (peg_y - retRight.value)));
            }
        }
    }
    deselectAll();

    return retLeft;
}

void Editor::prepareSave()
{
    for (auto mgr : mAllManagers)
    {
        mgr->save(mObject.get());
    }
}

void Editor::clearCurrentFrame()
{
    mScribbleArea->clearImage();
}
