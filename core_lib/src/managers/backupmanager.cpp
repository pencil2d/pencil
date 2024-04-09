/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2008-2009 Mj Mendoza IV
Copyright (C) 2012-2020 Matthew Chiawen Chang

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

#include <QDebug>
#include <QSettings>


#include "preferencemanager.h"
#include "layermanager.h"
#include "soundmanager.h"
#include "backupmanager.h"
#include "viewmanager.h"
#include "selectionmanager.h"
#include "soundmanager.h"

#include "backupelement.h"
#include "legacybackupelement.h"

#include "layerbitmap.h"
#include "layercamera.h"
#include "layervector.h"
#include "layersound.h"


#include "bitmapimage.h"
#include "vectorimage.h"
#include "soundclip.h"
#include "camera.h"

BackupManager::BackupManager(Editor* editor) : BaseManager(editor, "BackupManager")
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

    mNewBackupSystemEnabled = editor()->preference()->isOn(SETTING::NEW_UNDO_REDO_SYSTEM_ON);

    return true;
}

Status BackupManager::load(Object* /*o*/)
{
    clearStack();
    return Status::OK;
}

Status BackupManager::save(Object* /*o*/)
{
    if (newUndoRedoSystemEnabled()) {
        mBackupAtSave = latestBackupElement();
    } else {
        mLegacyBackupAtSave = mLegacyBackupList[mLegacyBackupIndex];
    }
    return Status::OK;
}

void BackupManager::backup(BackupType type)
{
    switch (type)
    {
        case BackupType::STROKE: {
            Layer* currentLayer = editor()->layers()->currentLayer();
            if (currentLayer->type() == Layer::BITMAP) {
                bitmap(tr("Bitmap stroke"));
            } else if (currentLayer->type() == Layer::VECTOR) {
                vector(tr("Vector stroke"));
            } else {
                Q_ASSERT_X(false, "BackupManager", "A stroke can only be applied to either the Bitmap or Vector layer");
            }
            break;
        }
        case BackupType::POLYLINE: {
            Layer* currentLayer = editor()->layers()->currentLayer();
            if (currentLayer->type() == Layer::BITMAP) {
                bitmap(tr("Bitmap polyline"));
            } else if (currentLayer->type() == Layer::VECTOR) {
                vector(tr("Vector polyline"));
            } else {
                Q_ASSERT_X(false, "BackupManager", "A polyline can only be applied to either the Bitmap or Vector layer");
            }
        break;
        }
        default:
            Q_ASSERT_X(false, "BackupManager", "Tried to make a backup for a case which hasn't been handled yet");
    }
}

const BackupElement* BackupManager::latestBackupElement() const
{
    return static_cast<const BackupElement*>(mUndoStack->command(mUndoStack->index() - 1));
}

bool BackupManager::newUndoRedoSystemEnabled() const
{
    return mNewBackupSystemEnabled;
}

bool BackupManager::hasUnsavedChanges() const
{
    if (newUndoRedoSystemEnabled()) {
        return mBackupAtSave != latestBackupElement();
    } else {
        if (mLegacyBackupIndex >= 0) {
            return mLegacyBackupAtSave != mLegacyBackupList[mLegacyBackupIndex];
        }
        return false;
    }
}

void BackupManager::pushCommand(QUndoCommand* command)
{
    mUndoStack->push(command);
    emit didUpdateUndoStack();
}

void BackupManager::bitmap(const QString& description)
{
    if (mBitmap == nullptr) { return; }
    BitmapElement* element = new BitmapElement(mBitmap,
                                               mLayerId,
                                               editor(),
                                               description);

    // if (mIsSelected)
    // {
    //     new TransformElement(mKeyframe,
    //                          mLayerId,
    //                          mEmptyFrameSettingVal,
    //                          mSelectionRect,
    //                          mSelectionTranslation,
    //                          mSelectionRotationAngle,
    //                          mSelectionScaleX,
    //                          mSelectionScaleY,
    //                          mSelectionAnchor,
    //                          description,
    //                          editor(), element);
    // }
    pushCommand(element);
}

void BackupManager::vector(const QString& description)
{
    if (mVector == nullptr) { return; }
    VectorElement* element = new VectorElement(mVector,
                                                     mLayerId,
                                                     mEmptyFrameSettingVal,
                                                     description,
                                                     editor());
    pushCommand(element);
}

/**
 * @brief BackupManager::saveStates
 * This method should be called prior to a backup taking place.
 * Only the most essential values should be retrieved here.
 */
void BackupManager::saveStates()
{
    if (!mNewBackupSystemEnabled) {
        return;
    }

    mBitmap = nullptr;
    mVector = nullptr;
    mCamera = nullptr;
    mClip = nullptr;
    mKeyframe = nullptr;

    mLayer = editor()->layers()->currentLayer();
    mLayerId = mLayer->id();

    mEmptyFrameSettingVal = static_cast<DrawOnEmptyFrameAction>(editor()->preference()->getInt(SETTING::DRAW_ON_EMPTY_FRAME_ACTION));

    mFrameIndex = editor()->currentFrame();

    auto selectMan = editor()->select();
    mIsSelected = selectMan->somethingSelected();
    mSelectionRect = selectMan->mySelectionRect();
    mSelectionRotationAngle = selectMan->myRotation();
    mSelectionTranslation = selectMan->myTranslation();
    mSelectionAnchor = selectMan->currentTransformAnchor();
    mSelectionScaleX = selectMan->myScaleX();
    mSelectionScaleY = selectMan->myScaleY();
    mMoveMode = selectMan->getMoveMode();
    mVectorSelection = selectMan->vectorSelection;

    mFrameIndexes = mLayer->getSelectedFramesByPos();

    mLayerName = mLayer->name();
    mLayerIndex = editor()->currentLayerIndex();
    mLayerType = mLayer->type();

    ViewManager* viewMgr = editor()->view();
    mViewTranslation = viewMgr->translation();
    mViewScale = viewMgr->scaling();
    mViewRotation = viewMgr->rotation();

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

QAction* BackupManager::createUndoAction(QObject* parent, const QString& description, const QIcon& icon)
{
    QAction* undoAction = nullptr;
    if (newUndoRedoSystemEnabled()) {
        undoAction = mUndoStack->createUndoAction(parent, description);
    } else {
        undoAction = new QAction(parent);
    }
    undoAction->setText(description);
    undoAction->setIcon(icon);

    if (newUndoRedoSystemEnabled()) {
        // The new system takes care of this automatically
    } else {
        connect(undoAction, &QAction::triggered, this, &BackupManager::legacyUndo);
    }
    return undoAction;
}

QAction* BackupManager::createRedoAction(QObject* parent, const QString& description, const QIcon& icon)
{
    QAction* redoAction = nullptr;
    if (newUndoRedoSystemEnabled()) {
        redoAction = mUndoStack->createRedoAction(parent, description);
    } else {
        redoAction = new QAction(parent);
    }
    redoAction->setText(description);
    redoAction->setIcon(icon);

    if (newUndoRedoSystemEnabled()) {
        // The new system takes care of this automatically
    } else {
        connect(redoAction, &QAction::triggered, this, &BackupManager::legacyRedo);
    }
    return redoAction;
}

void BackupManager::updateUndoAction(QAction* undoAction)
{
    if (newUndoRedoSystemEnabled()) {
        // Not used
        // function can be removed when we have replaced the legacy system
    } else {
        if (mLegacyBackupIndex < 0)
        {
            undoAction->setText(tr("Undo", "Menu item text"));
            undoAction->setEnabled(false);
            qDebug() << undoAction->text();
        }
        else
        {
            undoAction->setText(QString("%1   %2 %3").arg(tr("Undo", "Menu item text"))
                                    .arg(mLegacyBackupIndex + 1)
                                    .arg(mLegacyBackupList.at(mLegacyBackupIndex)->undoText));
            undoAction->setIconText(QString("%1   %2 %3").arg(tr("Undo", "Menu item text"))
                                    .arg(mLegacyBackupIndex + 1)
                                    .arg(mLegacyBackupList.at(mLegacyBackupIndex)->undoText));
            undoAction->setEnabled(true);
            qDebug() << undoAction->text();
        }
    }
}

void BackupManager::updateRedoAction(QAction* redoAction)
{
    if (newUndoRedoSystemEnabled()) {
        // Not used
        // function can be removed when we have replaced the legacy system
    } else {
        if (mLegacyBackupIndex + 2 < mLegacyBackupList.size())
        {
            redoAction->setText(QString("%1   %2 %3").arg(tr("Redo", "Menu item text"))
                                    .arg(mLegacyBackupIndex + 2)
                                    .arg(mLegacyBackupList.at(mLegacyBackupIndex + 1)->undoText));
            redoAction->setEnabled(true);
        }
        else
        {
            redoAction->setText(tr("Redo", "Menu item text"));
            redoAction->setEnabled(false);
        }
    }
}

void BackupManager::clearStack()
{
    if (newUndoRedoSystemEnabled()) {
        mUndoStack->clear();
    } else {
        mLegacyBackupIndex = -1;
        while (!mLegacyBackupList.isEmpty())
        {
            delete mLegacyBackupList.takeLast();
        }
        mLegacyLastModifiedLayer = -1;
        mLegacyLastModifiedFrame = -1;
    }
}

// Legacy backup system

void BackupManager::legacyBackup(const QString& undoText)
{

    if (newUndoRedoSystemEnabled()) {
        return;
    }

    KeyFrame* frame = nullptr;
    int currentFrame = editor()->currentFrame();
    if (mLegacyLastModifiedLayer > -1 && mLegacyLastModifiedFrame > 0)
    {
        if (editor()->layers()->currentLayer()->type() == Layer::SOUND)
        {
            frame = editor()->layers()->currentLayer()->getKeyFrameWhichCovers(mLegacyLastModifiedFrame);
            if (frame != nullptr)
            {
                legacyBackup(mLegacyLastModifiedLayer, frame->pos(), undoText);
            }
        }
        else
        {
            legacyBackup(mLegacyLastModifiedLayer, mLegacyLastModifiedFrame, undoText);
        }
    }
    if (mLegacyLastModifiedLayer != editor()->layers()->currentLayerIndex() || mLegacyLastModifiedFrame != currentFrame)
    {
        if (editor()->layers()->currentLayer()->type() == Layer::SOUND)
        {
            frame = editor()->layers()->currentLayer()->getKeyFrameWhichCovers(currentFrame);

            if (frame != nullptr)
            {
                legacyBackup(editor()->layers()->currentLayerIndex(), frame->pos(), undoText);
            }
        }
        else
        {
            legacyBackup(editor()->layers()->currentLayerIndex(), currentFrame, undoText);
        }
    }
}

bool BackupManager::legacyBackup(int backupLayer, int backupFrame, const QString& undoText)
{
    if (newUndoRedoSystemEnabled()) {
        return false;
    }

    while (mLegacyBackupList.size() - 1 > mLegacyBackupIndex && !mLegacyBackupList.empty())
    {
        delete mLegacyBackupList.takeLast();
    }
    while (mLegacyBackupList.size() > 19)   // we authorize only 20 levels of cancellation
    {
        delete mLegacyBackupList.takeFirst();
        mLegacyBackupIndex--;
    }

    Layer* layer = editor()->layers()->getLayer(backupLayer);
    int currentFrame = editor()->currentFrame();
    if (layer != nullptr)
    {
        if (layer->type() == Layer::BITMAP)
        {
            BitmapImage* bitmapImage = static_cast<BitmapImage*>(layer->getLastKeyFrameAtPosition(backupFrame));
            if (currentFrame == 1)
            {
                int previous = layer->getPreviousKeyFramePosition(backupFrame);
                bitmapImage = static_cast<BitmapImage*>(layer->getKeyFrameAt(previous));
            }
            if (bitmapImage != nullptr)
            {
                BackupLegacyBitmapElement* element = new BackupLegacyBitmapElement(bitmapImage);
                element->layerId = layer->id();
                element->layer = backupLayer;
                element->frame = bitmapImage->pos();
                element->undoText = undoText;
                element->somethingSelected = editor()->select()->somethingSelected();
                element->mySelection = editor()->select()->mySelectionRect();
                element->rotationAngle = editor()->select()->myRotation();
                element->scaleX = editor()->select()->myScaleX();
                element->scaleY = editor()->select()->myScaleY();
                element->translation = editor()->select()->myTranslation();
                element->selectionAnchor = editor()->select()->currentTransformAnchor();

                mLegacyBackupList.append(element);
                mLegacyBackupIndex++;
            }
            else
            {
                return false;
            }
        }
        else if (layer->type() == Layer::VECTOR)
        {
            VectorImage* vectorImage = static_cast<VectorImage*>(layer->getLastKeyFrameAtPosition(backupFrame));
            if (vectorImage != nullptr)
            {
                BackupLegacyVectorElement* element = new BackupLegacyVectorElement(vectorImage);
                element->layerId = layer->id();
                element->layer = backupLayer;
                element->frame = vectorImage->pos();
                element->undoText = undoText;
                element->somethingSelected = editor()->select()->somethingSelected();
                element->mySelection = editor()->select()->mySelectionRect();
                element->rotationAngle = editor()->select()->myRotation();
                element->scaleX = editor()->select()->myScaleX();
                element->scaleY = editor()->select()->myScaleY();
                element->translation = editor()->select()->myTranslation();
                element->selectionAnchor = editor()->select()->currentTransformAnchor();
                mLegacyBackupList.append(element);
                mLegacyBackupIndex++;
            }
            else
            {
                return false;
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
                    BackupLegacySoundElement* element = new BackupLegacySoundElement(clip);
                    element->layerId = layer->id();
                    element->layer = backupLayer;
                    element->frame = backupFrame;
                    element->undoText = undoText;
                    element->fileName = clip->fileName();
                    element->originalName = clip->soundClipName();
                    mLegacyBackupList.append(element);
                    mLegacyBackupIndex++;
                }
            }
            else
            {
                return false;
            }
        }
    }

    emit didUpdateUndoStack();

    return true;
}

void BackupManager::sanitizeLegacyBackupElementsAfterLayerDeletion(int layerIndex)
{
    if (newUndoRedoSystemEnabled()) {
        return;
    }

    for (int i = 0; i < mLegacyBackupList.size(); i++)
    {
        LegacyBackupElement *backupElement = mLegacyBackupList[i];
        BackupLegacyBitmapElement *bitmapElement;
        BackupLegacyVectorElement *vectorElement;
        BackupLegacySoundElement *soundElement;
        switch (backupElement->type())
        {
        case LegacyBackupElement::BITMAP_MODIF:
            bitmapElement = qobject_cast<BackupLegacyBitmapElement*>(backupElement);
            Q_ASSERT(bitmapElement);
            if (bitmapElement->layer > layerIndex)
            {
                bitmapElement->layer--;
                continue;
            }
            else if (bitmapElement->layer != layerIndex)
            {
                continue;
            }
            break;
        case LegacyBackupElement::VECTOR_MODIF:
            vectorElement = qobject_cast<BackupLegacyVectorElement*>(backupElement);
            Q_ASSERT(vectorElement);
            if (vectorElement->layer > layerIndex)
            {
                vectorElement->layer--;
                continue;
            }
            else if (vectorElement->layer != layerIndex)
            {
                continue;
            }
            break;
        case LegacyBackupElement::SOUND_MODIF:
            soundElement = qobject_cast<BackupLegacySoundElement*>(backupElement);
            Q_ASSERT(soundElement);
            if (soundElement->layer > layerIndex)
            {
                soundElement->layer--;
                continue;
            }
            else if (soundElement->layer != layerIndex)
            {
                continue;
            }
            break;
        default:
            Q_UNREACHABLE();
        }
        if (i <= mLegacyBackupIndex)
        {
            mLegacyBackupIndex--;
        }
        delete mLegacyBackupList.takeAt(i);
        i--;
    }
}

void BackupManager::restoreLegacyKey()
{
    if (newUndoRedoSystemEnabled()) {
        return;
    }

    LegacyBackupElement* lastBackupElement = mLegacyBackupList[mLegacyBackupIndex];

    Layer* layer = nullptr;
    int frame = 0;
    int layerIndex = 0;
    if (lastBackupElement->type() == LegacyBackupElement::BITMAP_MODIF)
    {
        BackupLegacyBitmapElement* lastBackupBitmapElement = static_cast<BackupLegacyBitmapElement*>(lastBackupElement);
        layerIndex = lastBackupBitmapElement->layer;
        frame = lastBackupBitmapElement->frame;
        layer = object()->findLayerById(lastBackupBitmapElement->layerId);
        editor()->addKeyFrame(layerIndex, frame);
        dynamic_cast<LayerBitmap*>(layer)->getBitmapImageAtFrame(frame)->paste(&lastBackupBitmapElement->bitmapImage);
        editor()->setModified(layerIndex, frame);
    }
    if (lastBackupElement->type() == LegacyBackupElement::VECTOR_MODIF)
    {
        BackupLegacyVectorElement* lastBackupVectorElement = static_cast<BackupLegacyVectorElement*>(lastBackupElement);
        layerIndex = lastBackupVectorElement->layer;
        frame = lastBackupVectorElement->frame;
        layer = object()->findLayerById(layerIndex);
        editor()->addKeyFrame(layerIndex, frame);
        dynamic_cast<LayerVector*>(layer)->getVectorImageAtFrame(frame)->paste(lastBackupVectorElement->vectorImage);
        editor()->setModified(layerIndex, frame);
    }
    if (lastBackupElement->type() == LegacyBackupElement::SOUND_MODIF)
    {
        QString strSoundFile;
        BackupLegacySoundElement* lastBackupSoundElement = static_cast<BackupLegacySoundElement*>(lastBackupElement);
        layerIndex = lastBackupSoundElement->layer;
        frame = lastBackupSoundElement->frame;

        strSoundFile = lastBackupSoundElement->fileName;
        if (strSoundFile.isEmpty()) return;
        KeyFrame* key = editor()->addKeyFrame(layerIndex, frame);
        SoundClip* clip = dynamic_cast<SoundClip*>(key);
        if (clip)
        {
            Status st = editor()->sound()->loadSound(clip, lastBackupSoundElement->fileName);
            clip->setSoundClipName(lastBackupSoundElement->originalName);
            if (!st.ok())
            {
                editor()->removeKey();
                emit editor()->layers()->currentLayerChanged(editor()->layers()->currentLayerIndex()); // trigger timeline repaint.
            }
        }
    }
}

void BackupManager::legacyUndo()
{
    if (!mLegacyBackupList.empty() && mLegacyBackupIndex > -1)
    {
        if (mLegacyBackupIndex == mLegacyBackupList.size() - 1)
        {
            LegacyBackupElement* lastBackupElement = mLegacyBackupList[mLegacyBackupIndex];
            if (lastBackupElement->type() == LegacyBackupElement::BITMAP_MODIF)
            {
                BackupLegacyBitmapElement* lastBackupBitmapElement = static_cast<BackupLegacyBitmapElement*>(lastBackupElement);
                if (legacyBackup(lastBackupBitmapElement->layer, lastBackupBitmapElement->frame, "NoOp"))
                {
                    mLegacyBackupIndex--;
                }
            }
            if (lastBackupElement->type() == LegacyBackupElement::VECTOR_MODIF)
            {
                BackupLegacyVectorElement* lastBackupVectorElement = static_cast<BackupLegacyVectorElement*>(lastBackupElement);
                if (legacyBackup(lastBackupVectorElement->layer, lastBackupVectorElement->frame, "NoOp"))
                {
                    mLegacyBackupIndex--;
                }
            }
            if (lastBackupElement->type() == LegacyBackupElement::SOUND_MODIF)
            {
                BackupLegacySoundElement* lastBackupSoundElement = static_cast<BackupLegacySoundElement*>(lastBackupElement);
                if (legacyBackup(lastBackupSoundElement->layer, lastBackupSoundElement->frame, "NoOp"))
                {
                    mLegacyBackupIndex--;
                }
            }
        }

        qDebug() << "Undo" << mLegacyBackupIndex;
        mLegacyBackupList[mLegacyBackupIndex]->restore(editor());
        mLegacyBackupIndex--;

        emit didUpdateUndoStack();
    }
}

void BackupManager::legacyRedo()
{
    if (!mLegacyBackupList.empty() && mLegacyBackupIndex < mLegacyBackupList.size() - 2)
    {
        mLegacyBackupIndex++;

        mLegacyBackupList[mLegacyBackupIndex + 1]->restore(editor());
        emit didUpdateUndoStack();
    }
}

void BackupManager::rememberLastModifiedFrame(int layerNumber, int frameNumber)
{
    if (newUndoRedoSystemEnabled()) {
        // not required
    } else {
        mLegacyLastModifiedLayer = layerNumber;
        mLegacyLastModifiedFrame = frameNumber;
    }
}
