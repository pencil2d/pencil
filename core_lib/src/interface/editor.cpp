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

#include <QApplication>
#include <QClipboard>
#include <QTimer>
#include <QImageReader>
#include <QDropEvent>
#include <QMimeData>
#include <QTemporaryDir>

#include "object.h"
#include "vectorimage.h"
#include "bitmapimage.h"
#include "camera.h"
#include "soundclip.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layercamera.h"
#include "activeframepool.h"

#include "colormanager.h"
#include "filemanager.h"
#include "toolmanager.h"
#include "layermanager.h"
#include "playbackmanager.h"
#include "viewmanager.h"
#include "preferencemanager.h"
#include "soundmanager.h"
#include "selectionmanager.h"
#include "backupmanager.h"
#include "keyframemanager.h"
#include "canvasmanager.h"

#include "scribblearea.h"
#include "timeline.h"
#include "util.h"


static BitmapImage g_clipboardBitmapImage;
static VectorImage g_clipboardVectorImage;


Editor::Editor(QObject* parent) : QObject(parent)
{
    clipboardBitmapOk = false;
    clipboardVectorOk = false;
}

Editor::~Editor()
{
    // a lot more probably needs to be cleaned here...
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
    mBackupManager = new BackupManager(this);
    mKeyFrameManager = new KeyFrameManager(this);
    mCanvasManager = new CanvasManager(this);

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
        mBackupManager,
        mKeyFrameManager,
        mCanvasManager
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
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &Editor::clipboardChanged);
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

void Editor::cut()
{
    backups()->saveStates();
    copy();

    Layer* layer = layers()->currentLayer();
    if (layer->type() == Layer::VECTOR) {
        keyframes()->currentVectorImage(layer)->deleteSelection();
        deselectAll();
        backups()->vector("Vector: Cut");
    }
    if (layer->type() == Layer::BITMAP) {
        keyframes()->currentBitmapImage(layer)->clear(select()->mySelectionRect());
        deselectAll();
        backups()->bitmap("Bitmap: Cut");
    }
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
        BitmapImage* bitmapImage = layerBitmap->getLastBitmapImageAtFrame(currentFrame(), 0);
        if (bitmapImage == nullptr) { return; }
        if (select()->somethingSelected())
        {
            g_clipboardBitmapImage = bitmapImage->copy(select()->mySelectionRect().toRect());  // copy part of the image
        }
        else
        {
            g_clipboardBitmapImage = *bitmapImage;  // copy the whole image
        }
        clipboardBitmapOk = true;
        if (g_clipboardBitmapImage.image() != nullptr)
            QApplication::clipboard()->setImage(*g_clipboardBitmapImage.image());
    }
    if (layer->type() == Layer::VECTOR)
    {
        clipboardVectorOk = true;
        VectorImage *vectorImage = static_cast<VectorImage*>(layer->getLastKeyFrameAtPosition(currentFrame()));
        if (vectorImage == nullptr) { return; }
        g_clipboardVectorImage = *vectorImage;  // copy the image
    }
}

void Editor::paste()
{
    Layer* layer = mObject->getLayer(layers()->currentLayerIndex());
    if (layer != nullptr)
    {
        backups()->saveStates();
        if (layer->type() == Layer::BITMAP && g_clipboardBitmapImage.image() != nullptr)
        {
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
            mScribbleArea->handleDrawingOnEmptyFrame();
            BitmapImage *bitmapImage = static_cast<BitmapImage*>(layer->getLastKeyFrameAtPosition(currentFrame()));
            Q_CHECK_PTR(bitmapImage);
            bitmapImage->paste(&tobePasted); // paste the clipboard

            backups()->bitmap(tr("Bitmap: Paste"));
        }
        else if (layer->type() == Layer::VECTOR && clipboardVectorOk)
        {
            deselectAll();
            mScribbleArea->handleDrawingOnEmptyFrame();
            VectorImage* vectorImage = static_cast<VectorImage*>(layer->getLastKeyFrameAtPosition(currentFrame()));
            Q_CHECK_PTR(vectorImage);
            vectorImage->paste(g_clipboardVectorImage);  // paste the clipboard
            select()->setSelection(vectorImage->getSelectionRect());
            backups()->vector(tr("Vector: Paste"));
        }
    }
    emit frameModified(mFrame);
}

void Editor::flipSelection(bool flipVertical)
{
    mScribbleArea->flipSelection(flipVertical);
}

void Editor::deselectAllSelections()
{
    backups()->deselect();
    emit deselectAll();
}

void Editor::deselectAllAndCancelTransform()
{
    backups()->deselect();
    emit deselectAll();
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

    g_clipboardVectorImage.setObject(newObject);

    updateObject();

    // Make sure that object is fully loaded before calling managers.
    for (BaseManager* m : mAllManagers)
    {
        m->load(mObject.get());
    }

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

    if (mPreferenceManager)
    {
        mObject->setActiveFramePoolSize(mPreferenceManager->getInt(SETTING::FRAME_POOL_SIZE));
    }

    emit updateLayerCount();
}

bool Editor::importBitmapImage(const QString& filePath, int space)
{
    QImageReader reader(filePath);

    Q_ASSERT(layers()->currentLayer()->type() == Layer::BITMAP);
    auto layer = static_cast<LayerBitmap*>(layers()->currentLayer());

    QImage img(reader.size(), QImage::Format_ARGB32_Premultiplied);
    if (img.isNull())
    {
        return false;
    }

    backups()->saveStates();

    std::map<int, KeyFrame*, std::greater<int>> canvasKeyFrames;

    layer->foreachKeyFrame([&canvasKeyFrames] (KeyFrame* key) {
        // make sure file is loaded when trying to get bitmap from layer...
        key->loadFile();
        canvasKeyFrames.insert(std::make_pair(key->pos(), key->clone()));
    });

    const QPoint pos(qRound(view()->getImportView().dx() - (img.width() / 2)),
                     qRound(view()->getImportView().dy() - (img.height() / 2)));

    std::map<int, KeyFrame*, std::less<int>> importedKeyFrames;

    while (reader.read(&img))
    {
        int frameNumber = mFrame;
        bool keyExisted = layer->keyExists(currentFrame());
        bool keyAdded = false;
        KeyFrame* newKey = nullptr;
        if (!keyExisted)
        {
            newKey = addNewKey();
            keyAdded = true;
        }
        BitmapImage* bitmapImage = layer->getBitmapImageAtFrame(frameNumber);
        BitmapImage importedBitmapImage(pos, img);
        bitmapImage->paste(&importedBitmapImage);
        emit frameModified(bitmapImage->pos());

        newKey = bitmapImage;
        if (newKey != nullptr) {
            newKey = newKey->clone();
        }
        importedKeyFrames.insert(std::make_pair(newKey->pos(), newKey));

        if (space > 1) {
            frameNumber += space;
        } else {
            frameNumber += 1;
        }
        scrubTo(frameNumber);


        // Workaround for tiff import getting stuck in this loop
        if (!reader.supportsAnimation())
        {
            break;
        }
    }
    backups()->importBitmap(canvasKeyFrames, importedKeyFrames);

    return true;
}

bool Editor::importVectorImage(const QString& filePath, bool isSequence)
{
    Q_UNUSED(isSequence)
    Q_ASSERT(layers()->currentLayer()->type() == Layer::VECTOR);

    auto layer = static_cast<LayerVector*>(layers()->currentLayer());

    backups()->saveStates();
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

        backups()->vector(tr("Vector: Import"));
    }

    return ok;
}

bool Editor::importImage(const QString& filePath, bool isSequence)
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
        return importBitmapImage(filePath, isSequence);

    case Layer::VECTOR:
        return importVectorImage(filePath, isSequence);

    default:
    {
        //mLastError = Status::ERROR_INVALID_LAYER_TYPE;
        return false;
    }
    }
}

bool Editor::importGIF(const QString& filePath, int numOfImages)
{
    Layer* layer = layers()->currentLayer();
    if (layer->type() == Layer::BITMAP)
    {
        return importBitmapImage(filePath, numOfImages);
    }
    return false;
}

void Editor::updateView()
{
    view()->updateViewTransforms();
    emit needPaint();
}

void Editor::selectAll() const
{
    Layer* layer = layers()->currentLayer();

    QRectF rect;

    canvas()->applyTransformedSelection(layer,
                                        keyframes()->currentKeyFrame(layer),
                                        select()->selectionTransform(),
                                        select()->mySelectionRect());
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

    select()->resetSelectionProperties();
    emit needPaint();
}

void Editor::updateFrame(int frameNumber)
{
    emit needPaintAtFrame(frameNumber);
}

void Editor::updateCurrentFrame()
{
    emit needPaint();
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

void Editor::scrubTo(const int layerId, const int frameIndex)
{
    layers()->setCurrentLayerFromId(layerId);
    scrubTo(frameIndex);
}

void Editor::scrubTo(Layer* layer, const int frameIndex)
{
    layers()->setCurrentLayer(layer);
    scrubTo(frameIndex);
}

void Editor::scrubTo(int frame)
{
    if (frame < 1) { frame = 1; }
    mFrame = frame;

    emit scrubbed(frame);

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

KeyFrame* Editor::addKeyFrame(int layerIndex, int frameIndex)
{
    return addKeyFrame(layerIndex, frameIndex, false);
}

/**
 * @brief Editor::addKeyFrame
 * @param layerIndex
 * @param frameIndex
 * @param isLastFrame Set true if no more frames will be added, otherwise false.
 * @return KeyFrame*
 */
KeyFrame* Editor::addKeyFrame(int layerIndex, int frameIndex, bool ignoreKeyExists)
{
    Layer* layer = mObject->getLayer(layerIndex);
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

    bool ok = layer->addNewKeyFrameAt(frameIndex);
    if (ok)
    {
        scrubTo(frameIndex); // currentFrameChanged() emit inside.
        emit frameModified(frameIndex);
        layers()->notifyAnimationLengthChanged();
    }

    if (layerIndex != currentLayerIndex())
    {
        setCurrentLayerIndex(layerIndex);
    }

    return layer->getKeyFrameAt(frameIndex);
}

KeyFrame* Editor::addKeyFrameToLayerId(int layerId, int frameIndex)
{
    return addKeyFrameToLayerId(layerId,frameIndex, false);
}

KeyFrame* Editor::addKeyFrameToLayer(Layer* layer, const int layerIndex, int frameIndex, const bool ignoreKeyExists)
{
    if (layer == NULL)
    {
        Q_ASSERT(false);
        return nullptr;
    }

    if (!ignoreKeyExists)
    {
        while (layer->keyExists(frameIndex) && frameIndex > 1)
        {
            frameIndex += 1;
        }
    }

    bool ok = layer->addNewKeyFrameAt(frameIndex);
    if (ok)
    {
        scrubTo(frameIndex); // currentFrameChanged() emit inside.
    }

    if (layerIndex != currentLayerIndex())
    {
        setCurrentLayerIndex(layerIndex);
    }

    return layer->getKeyFrameAt(frameIndex);
}

KeyFrame* Editor::addKeyFrameToLayerId(int layerId, int frameIndex, bool ignoreKeyExists)
{
    Layer* layer = layers()->findLayerById(layerId);
    int layerIndex = layers()->getLayerIndex(layer);
    if (layer == NULL)
    {
        Q_ASSERT(false);
        return nullptr;
    }

    if (!ignoreKeyExists)
    {
        while (layer->keyExists(frameIndex) && frameIndex > 1)
        {
            frameIndex += 1;
        }
    }

    bool ok = layer->addNewKeyFrameAt(frameIndex);
    if (ok)
    {
        scrubTo(frameIndex); // currentFrameChanged() emit inside.
    }

    if (layerIndex != currentLayerIndex())
    {
        setCurrentLayerIndex(layerIndex);
    }

    return layer->getKeyFrameAt(frameIndex);
}


/**
 * @brief Editor::addKeyContaining
 * Add a keyframe to the timeline which contains a keyframe
 * @param layerIndex
 * @param frameIndex
 * @param key
 *
 */
void Editor::addKeyContaining(int layerId, int frameIndex, KeyFrame* key)
{
    Layer* layer = layers()->findLayerById(layerId);
    int layerIndex = layers()->getLayerIndex(layer);
    while (layer->keyExistsWhichCovers(frameIndex))
    {
        frameIndex += 1;
    }

    bool ok = layer->addKeyFrame(frameIndex, key);
    if (ok)
    {
        scrubTo(frameIndex); // currentFrameChanged() emit inside.
    }

    if (layerIndex != currentLayerIndex())
    {
        setCurrentLayerIndex(layerIndex);
    }

}

void Editor::removeKeyAt(int layerIndex, int frameIndex)
{
    Layer* layer = layers()->getLayer(layerIndex);

    if (!layer->visible())
    {
        mScribbleArea->showLayerNotVisibleWarning();
        return;
    }

    if (!layer->keyExistsWhichCovers(frameIndex))
    {
        return;
    }

    layer->removeKeyFrame(frameIndex);

    if (!layer->keyExists(frameIndex) && frameIndex > 1)
    {
        frameIndex -= 1;
    }

    scrubTo(frameIndex);

    if (layerIndex != currentLayerIndex())
    {
        setCurrentLayerIndex(layerIndex);
    }
}

void Editor::removeKeyAtLayerId(int layerId, int frameIndex)
{
    Layer* layer = layers()->findLayerById(layerId);
    int layerIndex = layers()->getLayerIndex(layer);
    if (!layer->keyExistsWhichCovers(frameIndex))
    {
        return;
    }

    layer->removeKeyFrame(frameIndex);

    while (!layer->keyExists(frameIndex) && frameIndex > 1)
    {
        frameIndex -= 1;
    }

    scrubTo(frameIndex);

    if (layerIndex != currentLayerIndex())
    {
        setCurrentLayerIndex(layerIndex);
    }
}

void Editor::removeCurrentKey()
{
    removeKeyAt(currentLayerIndex(), currentFrame());
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
    mScribbleArea->onLayerChanged();

    emit updateTimeLine();
}

void Editor::showLayerNotVisibleWarning()
{
    return mScribbleArea->showLayerNotVisibleWarning();
}

void Editor::moveLayers(const int& fromIndex, const int& toIndex)
{
    if (toIndex < fromIndex) // bubble up
    {
        for (int i = fromIndex - 1; i >= toIndex; i--)
            mObject->swapLayers(i, i + 1);
    }
    else // bubble down
    {
        for (int i = fromIndex + 1; i <= toIndex; i++)
            mObject->swapLayers(i, i - 1);
    }
    emit updateTimeLine();
    mScribbleArea->onLayerChanged();
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
    mScribbleArea->onLayerChanged();
}

void Editor::prepareSave()
{
    for (auto mgr : mAllManagers)
    {
        mgr->save(mObject.get());
    }
}

/**
 * @brief Editor::clearCurrentFrame
 * Depending no the context, this will clear the keyframe..
 * for bitmap and vector that means wiping the canvas
 * for camera it will reset the view
 */
void Editor::clearCurrentFrame()
{
    Layer* layer = layers()->currentLayer();
    switch(layer->type()) {
    case Layer::BITMAP:
    case Layer::VECTOR: {
        mScribbleArea->clearImage();
        break;
    }
    case Layer::CAMERA: {
        Camera* camera = static_cast<LayerCamera*>(layer)->getCameraAtFrame(currentFrame());
        camera->reset();
        backups()->cameraMotion(tr("Camera: reset view"));
        updateView();
        break;
    }
    default:
        break;
    }
}
