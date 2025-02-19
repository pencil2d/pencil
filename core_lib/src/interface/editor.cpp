/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "editor.h"

#include <QTimer>
#include <QImageReader>
#include <QDropEvent>
#include <QTemporaryDir>

#include "object.h"
#include "vectorimage.h"
#include "bitmapimage.h"
#include "soundclip.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layercamera.h"
#include "undoredocommand.h"

#include "colormanager.h"
#include "filemanager.h"
#include "toolmanager.h"
#include "layermanager.h"
#include "playbackmanager.h"
#include "viewmanager.h"
#include "preferencemanager.h"
#include "soundmanager.h"
#include "selectionmanager.h"
#include "overlaymanager.h"
#include "clipboardmanager.h"
#include "undoredomanager.h"

#include "scribblearea.h"

Editor::Editor(QObject* parent) : QObject(parent)
{
}

Editor::~Editor()
{
    // a lot more probably needs to be cleaned here...
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
    mOverlayManager = new OverlayManager(this);
    mClipboardManager = new ClipboardManager(this);
    mUndoRedoManager = new UndoRedoManager(this);

    mAllManagers =
    {
        mColorManager,
        mToolManager,
        mLayerManager,
        mPlaybackManager,
        mViewManager,
        mPreferenceManager,
        mSoundManager,
        mSelectionManager,
        mOverlayManager,
        mClipboardManager,
        mUndoRedoManager
    };

    for (BaseManager* pManager : mAllManagers)
    {
        pManager->init();
    }

    makeConnections();

    mIsAutosave = mPreferenceManager->isOn(SETTING::AUTO_SAVE);
    mAutosaveNumber = mPreferenceManager->getInt(SETTING::AUTO_SAVE_NUMBER);

    return true;
}

int Editor::currentFrame() const
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
    emit fpsChanged(fps);
}

void Editor::makeConnections()
{
    connect(mPreferenceManager, &PreferenceManager::optionChanged, this, &Editor::settingUpdated);
    connect(mUndoRedoManager, &UndoRedoManager::didUpdateUndoStack, this, &Editor::updateAutoSaveCounter);
    connect(mPreferenceManager, &PreferenceManager::optionChanged, mUndoRedoManager, &UndoRedoManager::onSettingChanged);

    // XXX: This is a hack to prevent crashes until #864 is done (see #1412)
    connect(mLayerManager, &LayerManager::layerDeleted, mUndoRedoManager, &UndoRedoManager::sanitizeLegacyBackupElementsAfterLayerDeletion);
    connect(mLayerManager, &LayerManager::currentLayerWillChange, this, &Editor::onCurrentLayerWillChange);
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
        mScribbleArea->onOnionSkinTypeChanged();
        emit updateTimeLineCached();
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

void Editor::onCurrentLayerWillChange(int index)
{
    Layer* newLayer = layers()->getLayer(index);
    Layer* currentLayer = layers()->currentLayer();
    Q_ASSERT(newLayer && currentLayer);
    if (currentLayer->type() != newLayer->type()) {
        // We apply transform changes upon leaving a layer and deselect all
        mScribbleArea->applyTransformedSelection();

        if (currentLayer->type() == Layer::VECTOR) {
            auto keyFrame = static_cast<VectorImage*>(currentLayer->getLastKeyFrameAtPosition(mFrame));
            if (keyFrame)
            {
                keyFrame->deselectAll();
            }
        }

        select()->resetSelectionProperties();
    }
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

void Editor::copy()
{
    Layer* currentLayer = layers()->currentLayer();

    Q_ASSERT(currentLayer != nullptr);

    if (!canCopy()) { return; }

    backup(tr("Copy"));

    if (currentLayer->hasAnySelectedFrames() && !select()->somethingSelected()) {
        clipboards()->copySelectedFrames(currentLayer);
    } else if (currentLayer->type() == Layer::BITMAP) {
        BitmapImage* bitmapImage = static_cast<BitmapImage*>(currentLayer->getLastKeyFrameAtPosition(currentFrame()));
        clipboards()->copyBitmapImage(bitmapImage, select()->mySelectionRect());
    } else if (currentLayer->type() == Layer::VECTOR) {
        VectorImage* vectorImage = static_cast<VectorImage*>(currentLayer->getLastKeyFrameAtPosition(currentFrame()));
        clipboards()->copyVectorImage(vectorImage);
    }
}

void Editor::copyAndCut()
{
    copy();

    Layer* currentLayer = layers()->currentLayer();

    if (currentLayer->hasAnySelectedFrames() && !select()->somethingSelected()) {
        for (int pos : currentLayer->selectedKeyFramesPositions()) {
            currentLayer->removeKeyFrame(pos);
        }
        emit layers()->currentLayerChanged(currentLayerIndex());
        emit updateTimeLine();
        return;
    }

    if (currentLayer->type() == Layer::BITMAP || currentLayer->type() == Layer::VECTOR) {
        mScribbleArea->deleteSelection();
        deselectAll();
    }
}

void Editor::pasteFromPreviousFrame()
{
    Layer* currentLayer = layers()->currentLayer();
    int prevFrame = currentLayer->getPreviousKeyFramePosition(mFrame);
    if (!currentLayer->keyExists(mFrame) || prevFrame == mFrame)
    {
        return;
    }

    if (currentLayer->type() == Layer::BITMAP)
    {
        backup(tr("Paste from Previous Keyframe"));
        BitmapImage* bitmapImage = static_cast<BitmapImage*>(currentLayer->getKeyFrameAt(prevFrame));
        if (select()->somethingSelected())
        {
            BitmapImage copy = bitmapImage->copy(select()->mySelectionRect().toRect());
            pasteToCanvas(&copy, mFrame);
        }
        else
        {
            pasteToCanvas(bitmapImage, mFrame);
        }
    }
    else if (currentLayer->type() == Layer::VECTOR)
    {
        backup(tr("Paste from Previous Keyframe"));
        VectorImage* vectorImage = static_cast<VectorImage*>(currentLayer->getKeyFrameAt(prevFrame));
        pasteToCanvas(vectorImage, mFrame);
    }
}

void Editor::pasteToCanvas(BitmapImage* bitmapImage, int frameNumber)
{
    Layer* currentLayer = layers()->currentLayer();

    Q_ASSERT(currentLayer->type() == Layer::BITMAP);

    if (select()->somethingSelected())
    {
       QRectF selection = select()->mySelectionRect();
       if (bitmapImage->width() <= selection.width() && bitmapImage->height() <= selection.height())
       {
           bitmapImage->moveTopLeft(selection.topLeft());
       }
       else
       {
           bitmapImage->transform(selection, true);
       }
    }
    mScribbleArea->handleDrawingOnEmptyFrame();
    BitmapImage *canvasImage = static_cast<BitmapImage*>(currentLayer->getLastKeyFrameAtPosition(frameNumber));

    // Paste clipboard onto current shown image
    canvasImage->paste(bitmapImage);

    // TODO: currently we don't support placing an image without already pasting it on an already existing
    // image, this should be reworked such that a hovering selection could be shown, before applying it...
    select()->setSelection(bitmapImage->bounds());
    emit frameModified(frameNumber);
}

void Editor::pasteToCanvas(VectorImage* vectorImage, int frameNumber)
{
    Layer* currentLayer = layers()->currentLayer();

    Q_ASSERT(currentLayer->type() == Layer::VECTOR);

    deselectAll();
    mScribbleArea->handleDrawingOnEmptyFrame();
    VectorImage* canvasImage = static_cast<VectorImage*>(currentLayer->getLastKeyFrameAtPosition(frameNumber));
    canvasImage->paste(*vectorImage);
    select()->setSelection(vectorImage->getSelectionRect());
    emit frameModified(frameNumber);
}

void Editor::pasteToFrames()
{
    auto clipboardFrames = clipboards()->getClipboardFrames();
    Q_ASSERT(!clipboardFrames.empty());
    Layer* currentLayer = layers()->currentLayer();

    currentLayer->deselectAll();

    int newPositionOffset = mFrame - clipboardFrames.cbegin()->first;
    for (auto it = clipboardFrames.cbegin(); it != clipboardFrames.cend(); ++it)
    {
        int newPosition = it->first + newPositionOffset;

        KeyFrame* keyFrameNewPos = currentLayer->getKeyFrameWhichCovers(newPosition);

        if (keyFrameNewPos != nullptr) {

            // Select and move any frames that may come into contact with the new position
            currentLayer->newSelectionOfConnectedFrames(newPosition);
            currentLayer->moveSelectedFrames(1);
        }

        KeyFrame* key = it->second;
        // It's a bug if the keyframe is nullptr at this point...
        Q_ASSERT(key != nullptr);

        // TODO: undo/redo implementation
        currentLayer->addKeyFrame(newPosition, key);
        if (currentLayer->type() == Layer::SOUND)
        {
            auto soundClip = static_cast<SoundClip*>(key);
            sound()->loadSound(soundClip, soundClip->fileName());
        }

        currentLayer->setFrameSelected(key->pos(), true);
    }
}

void Editor::paste()
{
    Layer* currentLayer = layers()->currentLayer();

    Q_ASSERT(currentLayer != nullptr);

    if (!canPaste()) { return; }

    if (clipboards()->framesIsEmpty()) {

        backup(tr("Paste"));

        clipboards()->setFromSystemClipboard(mScribbleArea->getCentralPoint(), currentLayer);

        BitmapImage clipboardImage = clipboards()->getBitmapClipboard();
        VectorImage clipboardVectorImage = clipboards()->getVectorClipboard();
        if (currentLayer->type() == Layer::BITMAP && clipboardImage.isLoaded()) {
            pasteToCanvas(&clipboardImage, mFrame);
        } else if (currentLayer->type() == Layer::VECTOR && !clipboardVectorImage.isEmpty()) {
            pasteToCanvas(&clipboardVectorImage, mFrame);
        }
    } else {
        // TODO: implement undo/redo
        pasteToFrames();
    }

    emit frameModified(mFrame);
}

void Editor::flipSelection(bool flipVertical)
{
    if (flipVertical) {
        backup(tr("Flip selection vertically"));
    } else {
        backup(tr("Flip selection horizontally"));
    }
    mScribbleArea->flipSelection(flipVertical);
}

void Editor::repositionImage(QPoint transform, int frame)
{
    if (layers()->currentLayer()->type() == Layer::BITMAP)
    {
        scrubTo(frame);
        LayerBitmap* layer = static_cast<LayerBitmap*>(layers()->currentLayer());
        QRect reposRect = layer->getFrameBounds(frame);
        select()->setSelection(reposRect);
        QPoint point = reposRect.topLeft();
        point += transform;
        layer->repositionFrame(point, frame);
        backup(layer->id(), frame, tr("Reposition frame")); // TOOD: backup multiple reposition operations.
    }
}

void Editor::setModified(int layerNumber, int frameNumber)
{
    Layer* layer = object()->getLayer(layerNumber);
    if (layer == nullptr) { return; }

    layer->setModified(frameNumber, true);
    undoRedo()->rememberLastModifiedFrame(layerNumber, frameNumber);

    emit frameModified(frameNumber);
}

void Editor::clipboardChanged()
{
    Layer* layer = layers()->currentLayer();

    clipboards()->setFromSystemClipboard(mScribbleArea->getCentralPoint(), layer);

    bool canCopyState = canCopy();
    bool canPasteState = canPaste();

    emit canCopyChanged(canCopyState);
    emit canPasteChanged(canPasteState);
}

void Editor::setLayerVisibility(LayerVisibility visibility) {
    mScribbleArea->setLayerVisibility(visibility);
    emit updateTimeLine();
}

LayerVisibility Editor::layerVisibility()
{
    return mScribbleArea->getLayerVisibility();
}

qreal Editor::viewScaleInversed()
{
    return view()->getScaleInversed();
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

void Editor::addTemporaryDir(QTemporaryDir* const dir)
{
    mTemporaryDirs.append(dir);
}

void Editor::clearTemporary()
{
    while(!mTemporaryDirs.isEmpty())
    {
        QTemporaryDir* t = mTemporaryDirs.takeLast();
        t->remove();
        delete t;
    }
}

Status Editor::openObject(const QString& strFilePath, const std::function<void(int)>& progressChanged, const std::function<void(int)>& progressRangeChanged)
{
    // Check for potential issues with the file
    Q_ASSERT(!strFilePath.isEmpty());
    QFileInfo fileInfo(strFilePath);
    DebugDetails dd;
    dd << QString("Raw file path: %1").arg(strFilePath);
    dd << QString("Resolved file path: %1").arg(fileInfo.absoluteFilePath());
    if (fileInfo.isDir())
    {
        return Status(Status::ERROR_FILE_CANNOT_OPEN,
                      dd,
                      tr("Could not open file"),
                      tr("The file you have selected is a directory, so we are unable to open it. "
                         "If you are are trying to open a project that uses the old structure, "
                         "please open the file ending with .pcl, not the data folder."));
    }
    if (!fileInfo.exists())
    {
        return Status(Status::FILE_NOT_FOUND,
                      dd,
                      tr("Could not open file"),
                      tr("The file you have selected does not exist, so we are unable to open it. "
                         "Please make sure that you've entered the correct path and that the file is accessible and try again."));
    }
    if (!fileInfo.isReadable())
    {
        dd << QString("Permissions: 0x%1").arg(fileInfo.permissions(), 0, 16);
        return Status(Status::ERROR_FILE_CANNOT_OPEN,
                      dd,
                      tr("Could not open file"),
                      tr("This program does not have permission to read the file you have selected. "
                         "Please check that you have read permissions for this file and try again."));
    }

    int progress = 0;
    FileManager fm(this);
    connect(&fm, &FileManager::progressChanged, [&progress, &progressChanged](int p)
    {
        progressChanged(progress = p);
    });
    connect(&fm, &FileManager::progressRangeChanged, [&progressRangeChanged](int max)
    {
        progressRangeChanged(max + 3);
    });

    QString fullPath = fileInfo.absoluteFilePath();

    Object* object = fm.load(fullPath);

    Status fmStatus = fm.error();
    if (!fmStatus.ok())
    {
        dd.collect(fmStatus.details());
        fmStatus.setDetails(dd);
        return fmStatus;
    }

    if (object == nullptr)
    {
        return Status(Status::ERROR_FILE_CANNOT_OPEN,
                      dd,
                      tr("Could not open file"),
                      tr("An unknown error occurred while trying to load the file and we are not able to load your file."));
    }

    setObject(object);

    progressChanged(progress + 1);

    layers()->notifyAnimationLengthChanged();
    setFps(playback()->fps());

    return Status::OK;
}

Status Editor::setObject(Object* newObject)
{
    Q_ASSERT(newObject);

    if (newObject == mObject.get())
    {
        return Status::SAFE;
    }

    mObject.reset(newObject);

    updateObject();

    // Make sure that object is fully loaded before calling managers.
    for (BaseManager* m : mAllManagers)
    {
        m->load(mObject.get());
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

    if (mPreferenceManager)
    {
        mObject->setActiveFramePoolSize(mPreferenceManager->getInt(SETTING::FRAME_POOL_SIZE));
    }

    emit updateLayerCount();
}

Status Editor::importBitmapImage(const QString& filePath)
{
    QImageReader reader(filePath);

    Q_ASSERT(layers()->currentLayer()->type() == Layer::BITMAP);
    const auto layer = static_cast<LayerBitmap*>(layers()->currentLayer());

    if (!layer->visible())
    {
        mScribbleArea->showLayerNotVisibleWarning();
        return Status::SAFE;
    }

    Status status = Status::OK;
    DebugDetails dd;
    dd << QString("Raw file path: %1").arg(filePath);

    QImage img(reader.size(), QImage::Format_ARGB32_Premultiplied);
    if (!reader.read(&img)) {
        QString format = reader.format();
        if (!format.isEmpty())
        {
            dd << QString("QImageReader format: %1").arg(format);
        }
        dd << QString("QImageReader ImageReaderError type: %1").arg(reader.errorString());

        QString errorDesc;
        switch(reader.error())
        {
        case QImageReader::ImageReaderError::FileNotFoundError:
            errorDesc = tr("File not found at path \"%1\". Please check the image is present at the specified location and try again.").arg(filePath);
            break;
        case QImageReader::UnsupportedFormatError:
            errorDesc = tr("Image format is not supported. Please convert the image file to one of the following formats and try again:\n%1")
                        .arg(QString::fromUtf8(reader.supportedImageFormats().join(", ")));
            break;
        default:
            errorDesc = tr("An error has occurred while reading the image. Please check that the file is a valid image and try again.");
        }

        status = Status(Status::FAIL, dd, tr("Import failed"), errorDesc);
    }

    const QPoint pos(view()->getImportView().dx() - (img.width() / 2),
                     view()->getImportView().dy() - (img.height() / 2));

    if (!layer->keyExists(mFrame))
    {
        const bool ok = addNewKey();
        Q_ASSERT(ok);
    }
    BitmapImage* bitmapImage = layer->getBitmapImageAtFrame(mFrame);
    BitmapImage importedBitmapImage(pos, img);
    bitmapImage->paste(&importedBitmapImage);
    emit frameModified(bitmapImage->pos());

    scrubTo(mFrame+1);

    backup(tr("Import Image"));

    return status;
}

Status Editor::importVectorImage(const QString& filePath)
{
    Q_ASSERT(layers()->currentLayer()->type() == Layer::VECTOR);

    auto layer = static_cast<LayerVector*>(layers()->currentLayer());

    Status status = Status::OK;
    DebugDetails dd;
    dd << QString("Raw file path: %1").arg(filePath);

    VectorImage* vectorImage = layer->getVectorImageAtFrame(currentFrame());
    if (vectorImage == nullptr)
    {
        addNewKey();
        vectorImage = layer->getVectorImageAtFrame(currentFrame());
    }

    VectorImage importedVectorImage;
    bool ok = importedVectorImage.read(filePath);
    if (ok)
    {
        importedVectorImage.selectAll();
        vectorImage->paste(importedVectorImage);
        emit frameModified(importedVectorImage.pos());

        backup(tr("Import Image"));
    }
    else {
        status = Status(Status::FAIL, dd, tr("Import failed"), tr("You cannot import images into a vector layer."));
    }

    return status;
}

Status Editor::importImage(const QString& filePath)
{
    Layer* layer = layers()->currentLayer();

    DebugDetails dd;
    dd << QString("Raw file path: %1").arg(filePath);

    if (view()->getImportFollowsCamera())
    {
        LayerCamera* camera = static_cast<LayerCamera*>(layers()->getLastCameraLayer());
        Q_ASSERT(camera);
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
        dd << QString("Current layer: %1").arg(layer->type());
        return Status(Status::ERROR_INVALID_LAYER_TYPE, dd, tr("Import failed"), tr("You can only import images to a bitmap layer."));
    }
}

Status Editor::importAnimatedImage(const QString& filePath, int frameSpacing, const std::function<void(int)>& progressChanged, const std::function<bool()>& wasCanceled)
{
    frameSpacing = qMax(1, frameSpacing);

    DebugDetails dd;
    dd << QString("Raw file path: %1").arg(filePath);

    Layer* layer = layers()->currentLayer();
    if (layer->type() != Layer::BITMAP)
    {
        dd << QString("Current layer: %1").arg(layer->type());
        return Status(Status::ERROR_INVALID_LAYER_TYPE, dd, tr("Import failed"), tr("You can only import images to a bitmap layer."));
    }
    LayerBitmap* bitmapLayer = static_cast<LayerBitmap*>(layers()->currentLayer());

    QImageReader reader(filePath);
    dd << QString("QImageReader format: %1").arg(QString(reader.format()));
    if (!reader.supportsAnimation()) {
        return Status(Status::ERROR_INVALID_LAYER_TYPE, dd, tr("Import failed"), tr("The selected image has a format that does not support animation."));
    }

    QImage img(reader.size(), QImage::Format_ARGB32_Premultiplied);
    const QPoint pos(view()->getImportView().dx() - (img.width() / 2),
                     view()->getImportView().dy() - (img.height() / 2));
    int totalFrames = reader.imageCount();
    while (reader.read(&img))
    {
        if (reader.error())
        {
            dd << QString("QImageReader ImageReaderError type: %1").arg(reader.errorString());

            QString errorDesc;
            switch(reader.error())
            {
            case QImageReader::ImageReaderError::FileNotFoundError:
                errorDesc = tr("File not found at path \"%1\". Please check the image is present at the specified location and try again.").arg(filePath);
                break;
            case QImageReader::UnsupportedFormatError:
                errorDesc = tr("Image format is not supported. Please convert the image file to one of the following formats and try again:\n%1")
                            .arg((QString)reader.supportedImageFormats().join(", "));
                break;
            default:
                errorDesc = tr("An error has occurred while reading the image. Please check that the file is a valid image and try again.");
            }

            return Status(Status::FAIL, dd, tr("Import failed"), errorDesc);
        }

        if (!bitmapLayer->keyExists(mFrame))
        {
            addNewKey();
        }
        BitmapImage* bitmapImage = bitmapLayer->getBitmapImageAtFrame(mFrame);
        BitmapImage importedBitmapImage(pos, img);
        bitmapImage->paste(&importedBitmapImage);
        emit frameModified(bitmapImage->pos());

        if (wasCanceled())
        {
            break;
        }

        scrubTo(mFrame + frameSpacing);

        backup(tr("Import Image"));

        progressChanged(qFloor(qMin(static_cast<double>(reader.currentImageNumber()) / totalFrames, 1.0) * 100));
    }

    return Status::OK;
}

void Editor::selectAll() const
{
    Layer* layer = layers()->currentLayer();

    QRectF rect;
    if (layer->type() == Layer::BITMAP)
    {
        // Selects the drawn area (bigger or smaller than the screen). It may be more accurate to select all this way
        // as the drawing area is not limited
        BitmapImage *bitmapImage = static_cast<BitmapImage*>(layer->getLastKeyFrameAtPosition(mFrame));
        if (bitmapImage == nullptr) { return; }

        rect = bitmapImage->bounds();
    }
    else if (layer->type() == Layer::VECTOR)
    {
        VectorImage *vectorImage = static_cast<VectorImage*>(layer->getLastKeyFrameAtPosition(mFrame));
        if (vectorImage != nullptr)
        {
            vectorImage->selectAll();
            rect = vectorImage->getSelectionRect();
        }
    }
    select()->setSelection(rect, false);
}

void Editor::deselectAll() const
{
    select()->resetSelectionProperties();

    Layer* layer = layers()->currentLayer();
    if (layer == nullptr) { return; }

    if (layer->type() == Layer::VECTOR)
    {
        VectorImage *vectorImage = static_cast<VectorImage*>(layer->getLastKeyFrameAtPosition(mFrame));
        if (vectorImage != nullptr)
        {
            vectorImage->deselectAll();
        }
    }

    if (layer->hasAnySelectedFrames()) {
        layer->deselectAll();
        emit updateTimeLine();
    }
}

void Editor::updateFrame()
{
    mScribbleArea->updateFrame();
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
    mFrame = frame;

    // FIXME: should not emit Timeline update here.
    // Editor must be an individual class.
    // Will remove all Timeline related code in Editor class.
    if (mPlaybackManager && !mPlaybackManager->isPlaying())
    {
        emit updateTimeLineCached(); // needs to update the timeline to update onion skin positions
    }
    mObject->updateActiveFrames(frame);
    emit scrubbed(frame);
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

KeyFrame* Editor::addKeyFrame(const int layerNumber, int frameIndex)
{
    Layer* layer = mObject->getLayer(layerNumber);
    Q_ASSERT(layer);

    if (!layer->visible())
    {
        mScribbleArea->showLayerNotVisibleWarning();
        return nullptr;
    }

    // Find next available space for a keyframe (where either no key exists or there is an empty sound key)
    while (layer->keyExists(frameIndex))
    {
        if (layer->type() == Layer::SOUND
            && layer->getKeyFrameAt(frameIndex)->fileName().isEmpty()
            && layer->removeKeyFrame(frameIndex))
        {
            break;
        }
        else
        {
            frameIndex += 1;
        }
    }

    const bool ok = layer->addNewKeyFrameAt(frameIndex);
    Q_ASSERT(ok); // We already ensured that there is no keyframe at frameIndex, so this should always succeed
    scrubTo(frameIndex); // currentFrameChanged() emit inside.
    emit frameModified(frameIndex);
    layers()->notifyAnimationLengthChanged();
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
        scrubBackward();
        return;
    }

    backup(tr("Remove frame"));

    deselectAll();
    layer->removeKeyFrame(currentFrame());
    layers()->notifyAnimationLengthChanged();
    emit layers()->currentLayerChanged(layers()->currentLayerIndex()); // trigger timeline repaint.
}

void Editor::scrubNextKeyFrame()
{
    Layer* currentLayer = layers()->currentLayer();
    Q_ASSERT(currentLayer);

    int nextPosition = currentLayer->getNextKeyFramePosition(currentFrame());
    if (currentFrame() >= currentLayer->getMaxKeyFramePosition()) nextPosition = currentFrame() + 1;
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
    mScribbleArea->onLayerChanged();

    emit updateTimeLine();
}

void Editor::swapLayers(int i, int j)
{
    bool didSwapLayer = mObject->swapLayers(i, j);
    if (!didSwapLayer) { return; }

    if (j < i)
    {
        layers()->setCurrentLayer(j + 1);
    }
    else
    {
        layers()->setCurrentLayer(j - 1);
    }
    emit updateTimeLine();
    mScribbleArea->onLayerChanged();
}

bool Editor::canSwapLayers(int layerIndexLeft, int layerIndexRight) const
{
    return mObject->canSwapLayers(layerIndexLeft, layerIndexRight);
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

bool Editor::canCopy() const
{
    Layer* layer = layers()->currentLayer();
    KeyFrame* keyframe = layer->getLastKeyFrameAtPosition(mFrame);

    switch (layer->type())
    {
    case Layer::SOUND:
    case Layer::CAMERA:
        return canCopyFrames(layer);
    case Layer::BITMAP:
        return canCopyBitmapImage(static_cast<BitmapImage*>(keyframe)) || canCopyFrames(layer);
    case Layer::VECTOR:
        return canCopyVectorImage(static_cast<VectorImage*>(keyframe)) || canCopyFrames(layer);
    default:
        Q_UNREACHABLE();
    }
}

bool Editor::canPaste() const
{
    Layer* layer = layers()->currentLayer();
    auto clipboardMan = clipboards();
    auto layerType = layer->type();

    return (layerType == clipboardMan->framesLayerType() && !clipboardMan->framesIsEmpty()) ||
           (layerType == Layer::BITMAP && clipboardMan->getBitmapClipboard().isLoaded()) ||
           (layerType == Layer::VECTOR && !clipboardMan->getVectorClipboard().isEmpty());
}

bool Editor::canCopyFrames(const Layer* layer) const
{
    Q_ASSERT(layer != nullptr);
    return layer->hasAnySelectedFrames();
}

bool Editor::canCopyBitmapImage(BitmapImage* bitmapImage) const
{
    return bitmapImage != nullptr && bitmapImage->isLoaded() && !bitmapImage->bounds().isEmpty();
}

bool Editor::canCopyVectorImage(const VectorImage* vectorImage) const
{
    return vectorImage != nullptr && !vectorImage->isEmpty();
}

void Editor::backup(const QString &undoText)
{
    undoRedo()->legacyBackup(undoText);
    updateAutoSaveCounter();
}

bool Editor::backup(int layerNumber, int frameNumber, const QString &undoText)
{
    bool didBackup = undoRedo()->legacyBackup(layerNumber, frameNumber, undoText);

    updateAutoSaveCounter();
    return didBackup;
}
