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

#include "object.h"
#include "editor.h"

#include "layermanager.h"
#include "soundmanager.h"
#include "backupmanager.h"
#include "viewmanager.h"

#include "scribblearea.h"
#include "backupelement.h"

#include "layerbitmap.h"
#include "layercamera.h"
#include "layervector.h"
#include "layersound.h"

#include "bitmapimage.h"
#include "vectorimage.h"
#include "soundclip.h"
#include "camera.h"

BackupManager::BackupManager(Editor* editor) : BaseManager(editor)
{
    qDebug() << "BackupManager: created";
}

BackupManager::~BackupManager()
{
    qDebug() << "BackupManager: destroyed";
}

bool BackupManager::init()
{
    mUndoStack = new QUndoStack(this);
    qDebug() << "BackupManager: init";

    return true;
}

Status BackupManager::load(Object* /*o*/)
{
    return Status::OK;
}

Status BackupManager::save(Object* /*o*/)
{
    return Status::OK;
}

const BackupElement* BackupManager::currentBackup()
{
    if (mUndoStack->count())
    {
        return static_cast<const BackupElement*>(mUndoStack->command(mUndoStack->index()-1));
    }
    else
    {
        return nullptr;
    }
}

void BackupManager::keyAdded(int keySpacing, bool keyExisted, QString description)
{
    if (mLayer == NULL) { return; }

    AddKeyFrameElement* element = new AddKeyFrameElement(mFrameIndex,
                                                         mLayerId,
                                                         keySpacing,
                                                         keyExisted,
                                                         description,
                                                         editor());
    mUndoStack->push(element);

    emit updateBackup();
}

void BackupManager::keyAdded()
{
    if (mLayer == NULL) { return; }

    AddKeyFrameElement* element = new AddKeyFrameElement(mFrameIndex,
                                                         mLayerId,
                                                         false,
                                                         false,
                                                         "",
                                                         editor());
    mUndoStack->push(element);
    emit updateBackup();
}

void BackupManager::importBitmap(std::map<int, KeyFrame*, std::greater<int>> canvasKeys,
                                 std::map<int, KeyFrame*, std::less<int>> importedKeys)
{
    if (mLayer->type() != Layer::BITMAP) { return; }

    ImportBitmapElement* element = new ImportBitmapElement(canvasKeys,
                                                         importedKeys,
                                                         mLayerId,
                                                         editor());

    mUndoStack->push(element);
    emit updateBackup();
}

void BackupManager::keyRemoved()
{
    if (mLayer == NULL) { return; }

    RemoveKeyFrameElement* element = new RemoveKeyFrameElement(mKeyframe,
                                                               mLayerId,
                                                               editor());
    mUndoStack->push(element);
    emit updateBackup();

}

void BackupManager::bitmap(QString description)
{
    if (!mBitmap) { return; }
    AddBitmapElement* element = new AddBitmapElement(mBitmap,
                                                     mBufferImage->clone(),
                                                     mLayerId,
                                                     mFrameIndex,
                                                     description,
                                                     editor());

    if (mIsSelected)
    {
        new TransformElement(mKeyframe,
                             mLayerId,
                             mFrameIndex,
                             mSelectionRect,
                             mTempSelectionRect,
                             mSelectionTransform,
                             editor(), element);
    }
    mUndoStack->push(element);
    emit updateBackup();
}

void BackupManager::vector(QString description)
{
    if (!mVector) { return; }
    AddVectorElement* element = new AddVectorElement(mVector,
                                                     mFrameIndex,
                                                     mLayerId,
                                                     description,
                                                     editor());
    mUndoStack->push(element);
    emit updateBackup();
}

void BackupManager::selection()
{
    SelectionElement* element = new SelectionElement(SelectionType::SELECTION,
                                                     mTempSelectionRect,
                                                     mSelectionRect,
                                                     editor());
    mUndoStack->push(element);
    emit updateBackup();
}

void BackupManager::deselect()
{
    SelectionElement* element = new SelectionElement(SelectionType::DESELECT,
                                                     mTempSelectionRect,
                                                     mSelectionRect,
                                                     editor());
   new TransformElement(mKeyframe,
                         mLayerId,
                         mFrameIndex,
                         mSelectionRect,
                         mTempSelectionRect,
                         mSelectionTransform,
                         editor(), element);
    mUndoStack->push(element);

    emit updateBackup();
}

void BackupManager::clearSelection()
{
    SelectionElement* element = new SelectionElement(SelectionType::DESELECT,
                                                     mTempSelectionRect,
                                                     mSelectionRect,
                                                     editor());
    new TransformElement(mKeyframe,
                          mLayerId,
                          mFrameIndex,
                          mSelectionRect,
                          mTempSelectionRect,
                          mSelectionTransform,
                          editor(), element);



    if (mLayer->type() == Layer::BITMAP) {
        element->setText(tr("Bitmap Clear Selection"));
    } else if (mLayer->type() == Layer::VECTOR) {
        element->setText(tr("Vector: Clear Selection"));
    } else {
        return;
    }
    mUndoStack->push(element);
    emit updateBackup();
}

void BackupManager::transform()
{
    if (!mIsSelected) { return; }
    TransformElement* element = new TransformElement(mKeyframe,
                                                     mLayerId,
                                                     mFrameIndex,
                                                     mSelectionRect,
                                                     mTempSelectionRect,
                                                     mSelectionTransform,
                                                     editor());
    mUndoStack->push(element);
    emit updateBackup();
}


/**
 * @brief Get the frame index for the keyframe which is being painted to
 *
 * @param layer
 * @param frameIndex <- current frame
 * @param usingPreviousFrameAction <- This is whether DRAW_ON_EMPTY_FRAME_ACTION is active
 * @return frameindex
 */
int BackupManager::getActiveFrameIndex(Layer* layer, int frameIndex, bool usingPreviousFrameAction) {
    if (!layer->keyExists(frameIndex)) {
        if (usingPreviousFrameAction)
        {
            frameIndex = layer->getPreviousKeyFramePosition(frameIndex);
        }
    }
    return frameIndex;
}

void BackupManager::restoreLayerKeys(BackupElement* backupElement)
{

    DeleteLayerElement* lastBackupLayerElem = (DeleteLayerElement*)backupElement;
    LayerManager* layerMgr = editor()->layers();
    Layer* layer = nullptr;

    int oldFrameIndex = lastBackupLayerElem->oldFrameIndex;
    int layerIndex = lastBackupLayerElem->oldLayerIndex;
    int layerId = lastBackupLayerElem->oldLayerId;
    QString layerName = lastBackupLayerElem->oldLayerName;

    switch(lastBackupLayerElem->oldLayerType)
    {
        case Layer::BITMAP:
        {
            layerMgr->createBitmapLayerContainingKeyFrames(lastBackupLayerElem->oldLayerKeys,
                                                                     layerId,
                                                                     layerIndex,
                                                                     layerName);
            break;
        }
        case Layer::VECTOR:
        {

           layerMgr->createVectorLayerContainingKeyFrames(lastBackupLayerElem->oldLayerKeys,
                                                                    layerId,
                                                                    layerIndex,
                                                                    layerName);
            break;
        }
        case Layer::SOUND:
        {
            layer = layerMgr->createSoundLayerContaining(layerId,
                                                         layerIndex,
                                                          layerName);
            for (auto map : lastBackupLayerElem->oldLayerKeys)
            {
                int frameIndex = map.second->pos();
                editor()->sound()->loadSound(layer, frameIndex, map.second->fileName());
            }
            break;
        }
        case Layer::CAMERA:
        {
            layerMgr->createCameraLayerContainingKeyFrames(lastBackupLayerElem->oldLayerKeys,
                                                           layerId,
                                                           layerIndex,
                                                           layerName);
            break;
        }
        default:
            break;
    }
    editor()->scrubTo(oldFrameIndex);
}

void BackupManager::restoreKey(BackupElement* backupElement)
{
    Layer* layer = nullptr;
    int frame = 0;
    int layerIndex = 0;
    int layerId = 0;
    KeyFrame* keyFrame = nullptr;

    if (backupElement->type() == ADD_KEY_MODIF)
    {
        AddKeyFrameElement* lastBackupKeyFrameElement = (AddKeyFrameElement*)backupElement;
        layerIndex = lastBackupKeyFrameElement->newLayerIndex;
        frame = lastBackupKeyFrameElement->newFrameIndex;
        layerId = lastBackupKeyFrameElement->newLayerId;
        layer = object()->findLayerById(layerId);
        keyFrame = lastBackupKeyFrameElement->newKey;

        restoreKey(layerId, frame, keyFrame);

    }
    else // REMOVE_KEY_MODIF
    {
        RemoveKeyFrameElement* lastBackupKeyFrameElement = (RemoveKeyFrameElement*)backupElement;
        layerIndex = lastBackupKeyFrameElement->oldLayerIndex;
        frame = lastBackupKeyFrameElement->oldFrameIndex;
        layerId = lastBackupKeyFrameElement->oldLayerId;
        layer = editor()->layers()->findLayerById(layerId);
        keyFrame = lastBackupKeyFrameElement->oldKey;

        restoreKey(layerId, frame, keyFrame);
    }
}

void BackupManager::restoreKey(int layerId, int frame, KeyFrame *keyFrame)
{
    Layer* layer = editor()->layers()->findLayerById(layerId);

    switch(layer->type())
    {
        case Layer::BITMAP:
        case Layer::VECTOR:
        case Layer::CAMERA:
        {
            if (!layer->keyExists(frame))
            {
                editor()->addKeyFrameToLayerId(layerId, frame);
            }
            break;
        }
        default:
            break;
    }

    switch(layer->type())
    {
        case Layer::BITMAP:
        {
            BitmapImage* bitmap = static_cast<BitmapImage*>(keyFrame);
            dynamic_cast<LayerBitmap*>(layer)->getBitmapImageAtFrame(frame)->paste(bitmap);
            break;
        }
        case Layer::VECTOR:
        {
            VectorImage* vector = static_cast<VectorImage*>(keyFrame);
            dynamic_cast<LayerVector*>(layer)->getVectorImageAtFrame(frame)->paste(*vector);
            break;
        }
        case Layer::SOUND:
        {
            SoundClip* clip = static_cast<SoundClip*>(keyFrame);
            clip = new SoundClip(*clip);
            editor()->addKeyContaining(layerId, frame, clip);

            if (clip)
            {
                Status st = editor()->sound()->processSound(clip);
                if (!st.ok())
                {
                      Q_ASSERT(st.ok());
                }
            }
            break;
        }
        case Layer::CAMERA:
        {
            Camera* cam = static_cast<Camera*>(keyFrame);
            cam = new Camera(*cam);
            dynamic_cast<LayerCamera*>(layer)->getCameraAtFrame(frame)->assign(*cam);
            break;
        }
        default:
            break;
    }
}

void BackupManager::cameraMotion()
{
    if (mLayer == NULL) { return; }

    CameraMotionElement* element = new CameraMotionElement(mTranslation,
                                                           mRotation,
                                                           mScale,
                                                           editor());
    mUndoStack->push(element);

    emit updateBackup();
}

void BackupManager::layerAdded()
{
    AddLayerElement* element = new AddLayerElement(mLayer, editor());
    mUndoStack->push(element);

    emit updateBackup();
}

void BackupManager::layerDeleted(std::map<int, KeyFrame*, std::greater<int> > oldKeys)
{

    DeleteLayerElement* element = new DeleteLayerElement(mLayerName,
                                                         mLayerType,
                                                         oldKeys,
                                                         mFrameIndex,
                                                         mLayerIndex,
                                                         mLayerId,
                                                         editor());
    mUndoStack->push(element);

    emit updateBackup();
}

void BackupManager::layerRenamed()
{
    RenameLayerElement* element = new RenameLayerElement(mLayerName,
                                                         mLayerId,
                                                         editor());
    mUndoStack->push(element);
    emit updateBackup();
}

void BackupManager::cameraProperties(QRect backupViewRect)
{
    CameraPropertiesElement* element = new CameraPropertiesElement(mLayerName,
                                                                   backupViewRect,
                                                                   mLayerId,
                                                                   editor());
    mUndoStack->push(element);
    emit updateBackup();
}

void BackupManager::frameDragged(int backupFrameOffset)
{
    DragFrameElement* element = new DragFrameElement(mLayerId,
                                                     backupFrameOffset,
                                                     editor());

    mUndoStack->push(element);
    emit updateBackup();
}

void BackupManager::flipView(bool backupIsFlipped, DIRECTION backupFlipDirection)
{
    FlipViewElement* element = new FlipViewElement(backupIsFlipped,
                                                   backupFlipDirection,
                                                   editor());

    mUndoStack->push(element);
    emit updateBackup();
}

void BackupManager::toggleSetting(bool /*backupToggleState*/, SETTING /*backupType*/)
{
//    ToggleSettingElement* element = new ToggleSettingElement(backupToggleState,
//                                                             backupType,
//                                                             editor());

//    mUndoStack->push(element);
//    emit updateBackup();
}

void BackupManager::layerMoved(int backupNewLayerIndex)
{
    MoveLayerElement* element = new MoveLayerElement(mLayerIndex,
                                                     backupNewLayerIndex,
                                                     editor());
    mUndoStack->push(element);
    emit updateBackup();
}

/**
 * @brief BackupManager::saveStates
 * This method should be called prior to a backup taking place.
 */
void BackupManager::saveStates()
{
    mBitmap = nullptr;
    mVector = nullptr;
    mCamera = nullptr;
    mClip = nullptr;
    mKeyframe = nullptr;

    mBufferImage = editor()->getScribbleArea()->mBufferImg->clone();
    mLayer = editor()->layers()->currentLayer();
    mLayerId = mLayer->id();

    int emptyFrameSettingVal = editor()->preference()->getInt(SETTING::DRAW_ON_EMPTY_FRAME_ACTION);

    mFrameIndex = editor()->currentFrame();
    mFrameIndex = BackupManager::getActiveFrameIndex(mLayer, mFrameIndex, emptyFrameSettingVal);

    mIsSelected = editor()->getScribbleArea()->isSomethingSelected();
    mSelectionRect = editor()->getScribbleArea()->mySelection;
    mTempSelectionRect = editor()->getScribbleArea()->myTempTransformedSelection;
    mLayerName = mLayer->name();
    mLayerIndex = editor()->currentLayerIndex();
    mLayerType = mLayer->type();
    mMoveOffset = editor()->getScribbleArea()->getTransformOffset();
    mSelectionTransform = editor()->getScribbleArea()->getSelectionTransformation();
    mMoveMode = editor()->getScribbleArea()->getMoveMode();

    ViewManager* viewMgr = editor()->view();
    mTranslation = viewMgr->translation();
    mScale = viewMgr->scaling();
    mRotation = viewMgr->rotation();

    if (mLayer->keyExists(mFrameIndex))
    {
        mKeyframe = mLayer->getLastKeyFrameAtPosition(mFrameIndex)->clone();
    }
    else if (mLayer->getKeyFrameWhichCovers(mFrameIndex) != nullptr)
    {
        mKeyframe = mLayer->getKeyFrameWhichCovers(mFrameIndex)->clone();
    }

    switch(mLayer->type())
    {
        case Layer::BITMAP:
        {
            mBitmap = static_cast<BitmapImage*>(mKeyframe);
            break;
        }
        case Layer::VECTOR:
        {
            mVector = static_cast<VectorImage*>(mKeyframe);
            break;
        }
        case Layer::SOUND:
        {
            mClip = static_cast<SoundClip*>(mKeyframe);
            break;
        }
        case Layer::CAMERA:
        {
            mCamera = static_cast<Camera*>(mKeyframe);
            break;
        }
        default:
            break;
    }
}
