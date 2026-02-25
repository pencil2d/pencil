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

#include <QAction>
#include <QDebug>
#include <QSettings>


#include "preferencemanager.h"
#include "layermanager.h"
#include "soundmanager.h"
#include "undoredomanager.h"
#include "selectionmanager.h"

#include "undoredocommand.h"
#include "legacybackupelement.h"

#include "layerbitmap.h"
#include "layervector.h"
#include "layersound.h"


#include "bitmapimage.h"
#include "vectorimage.h"
#include "soundclip.h"

UndoRedoManager::UndoRedoManager(Editor* editor) : BaseManager(editor, "UndoRedoManager")
{
    qDebug() << "UndoRedoManager: created";
}

UndoRedoManager::~UndoRedoManager()
{
    if (!mNewBackupSystemEnabled)
    {
        clearStack();
    }
    clearSaveStates();
    qDebug() << "UndoRedoManager: destroyed";
}

bool UndoRedoManager::init()
{
    qDebug() << "UndoRedoManager: init";

    mUndoStack.setUndoLimit(editor()->preference()->getInt(SETTING::UNDO_REDO_MAX_STEPS));
    mNewBackupSystemEnabled = editor()->preference()->isOn(SETTING::NEW_UNDO_REDO_SYSTEM_ON);

    return true;
}

void UndoRedoManager::onSettingChanged(SETTING setting)
{
    if (setting == SETTING::UNDO_REDO_MAX_STEPS) {
        // The stack needs to be cleared in order to change the undo redo limit
        clearStack();
        qDebug() << "updated undo stack limit";
        mUndoStack.setUndoLimit(editor()->preference()->getInt(SETTING::UNDO_REDO_MAX_STEPS));
    }
}

Status UndoRedoManager::load(Object* /*o*/)
{
    clearStack();
    return Status::OK;
}

Status UndoRedoManager::save(Object* /*o*/)
{
    if (mNewBackupSystemEnabled) {
        mUndoStack.setClean();
    } else if (!mLegacyBackupList.isEmpty() && mLegacyBackupIndex < mLegacyBackupList.count()) {
        mLegacyBackupAtSave = mLegacyBackupList[mLegacyBackupIndex];
    }
    return Status::OK;
}

void UndoRedoManager::record(SAVESTATE_ID saveStateId, const QString& description)
{
    if (!mSaveStates.contains(saveStateId)) {
        return;
    }

    UndoSaveState* saveState = mSaveStates.take(saveStateId);
    if (!saveState) { return; }

    if (!mNewBackupSystemEnabled) {
        clearState(saveState);
        return;
    }

    switch (saveState->recordType)
    {
        case UndoRedoRecordType::KEYFRAME_MODIFY: {
            replaceKeyFrame(*saveState, description);
            break;
        }
        case UndoRedoRecordType::KEYFRAME_REMOVE: {
            removeKeyFrame(*saveState, description);
            break;
        }
        case UndoRedoRecordType::KEYFRAME_ADD: {
            addKeyFrame(*saveState, description);
            break;
        }
        case UndoRedoRecordType::KEYFRAME_MOVE: {
            moveKeyFrames(*saveState, description);
            break;
        }
        default: {
            QString reason("Unhandled case for: ");
            reason.append(description);
            Q_ASSERT_X(false, "UndoRedoManager::record", qPrintable(reason));
            break;
        }
    }

    // The save state has now been used and should be invalidated so we can't use it again.
    clearState(saveState);
}

void UndoRedoManager::clearState(UndoSaveState*& state)
{
    if (state) {
        delete state;
        state = nullptr;
    }
}

void UndoRedoManager::clearSaveStates()
{
    for (UndoSaveState* saveState : mSaveStates) {
        if (saveState) {
            delete saveState;
            saveState = nullptr;
        }
    }
    mSaveStates.clear();
}

bool UndoRedoManager::hasUnsavedChanges() const
{
    if (mNewBackupSystemEnabled) {
        return !mUndoStack.isClean();
    } else {
        if (mLegacyBackupIndex >= 0) {
            return mLegacyBackupAtSave != mLegacyBackupList[mLegacyBackupIndex];
        }
        return false;
    }
}

void UndoRedoManager::pushCommand(QUndoCommand* command)
{
    mUndoStack.push(command);

    emit didUpdateUndoStack();
}

void UndoRedoManager::removeKeyFrame(const UndoSaveState& undoState, const QString& description)
{
    KeyFrameRemoveCommand* element = new KeyFrameRemoveCommand(undoState.keyframe.get(),
                                                           undoState.layerId,
                                                           description,
                                                           editor());
    pushCommand(element);
}

void UndoRedoManager::addKeyFrame(const UndoSaveState& undoState, const QString& description)
{
    KeyFrameAddCommand* element = new KeyFrameAddCommand(undoState.currentFrameIndex,
                                                           undoState.layerId,
                                                           description,
                                                           editor());
    pushCommand(element);
}

void UndoRedoManager::replaceKeyFrame(const UndoSaveState& undoState, const QString& description)
{
    if (undoState.layerType == Layer::BITMAP) {
        replaceBitmap(undoState, description);
    } else if (undoState.layerType == Layer::VECTOR) {
        replaceVector(undoState, description);
    } else {
        // Implement other cases
    }
}

void UndoRedoManager::moveKeyFrames(const UndoSaveState& undoState, const QString& description)
{
    const MoveFramesSaveState& state = undoState.userState.moveFramesState;
    MoveKeyFramesCommand* element = new MoveKeyFramesCommand(state.offset,
                                                             state.positions,
                                                             undoState.layerId,
                                                             description,
                                                             editor());
    pushCommand(element);
}

void UndoRedoManager::replaceBitmap(const UndoSaveState& undoState, const QString& description)
{
    if (undoState.keyframe == nullptr || undoState.layerType != Layer::BITMAP) { return; }
    BitmapReplaceCommand* element = new BitmapReplaceCommand(static_cast<BitmapImage*>(undoState.keyframe.get()),
                                               undoState.layerId,
                                               description,
                                               editor());

    const SelectionSaveState& selectionState = undoState.selectionState;
    new TransformCommand(selectionState.bounds,
                         selectionState.translation,
                         selectionState.rotationAngle,
                         selectionState.scaleX,
                         selectionState.scaleY,
                         selectionState.anchor,
                         true, // roundPixels
                         description,
                         editor(), element);

    pushCommand(element);
}

void UndoRedoManager::replaceVector(const UndoSaveState& undoState, const QString& description)
{
    if (undoState.keyframe == nullptr || undoState.layerType != Layer::VECTOR) { return; }
    VectorReplaceCommand* element = new VectorReplaceCommand(static_cast<VectorImage*>(undoState.keyframe.get()),
                                                 undoState.layerId,
                                                 description,
                                                 editor());

    const SelectionSaveState& selectionState = undoState.selectionState;
    new TransformCommand(selectionState.bounds,
                         selectionState.translation,
                         selectionState.rotationAngle,
                         selectionState.scaleX,
                         selectionState.scaleY,
                         selectionState.anchor,
                         false, // Round pixels
                         description,
                         editor(), element);
    pushCommand(element);
}

SAVESTATE_ID UndoRedoManager::createState(UndoRedoRecordType recordType)
{
    int saveStateId = mSaveStateId;
    UndoSaveState* state = new UndoSaveState();
    state->recordType = recordType;
    initCommonKeyFrameState(state);

    mSaveStates[saveStateId] = state;
    mSaveStateId += 1;

    return saveStateId;
}

void UndoRedoManager::addUserState(SAVESTATE_ID saveStateId, const UserSaveState& userState)
{
    if (!mSaveStates.contains(saveStateId)) { return; }
    mSaveStates[saveStateId]->userState = userState;
}

void UndoRedoManager::initCommonKeyFrameState(UndoSaveState* undoSaveState) const
{
    const Layer* layer = editor()->layers()->currentLayer();
    undoSaveState->layerType = layer->type();
    undoSaveState->layerId = layer->id();
    undoSaveState->currentFrameIndex = editor()->currentFrame();

    if (layer->type() == Layer::BITMAP || layer->type() == Layer::VECTOR) {
        auto selectMan = editor()->select();
        undoSaveState->selectionState = SelectionSaveState(
            selectMan->mySelectionRect(),
            selectMan->myRotation(),
            selectMan->myScaleX(),
            selectMan->myScaleY(),
            selectMan->myTranslation(),
            selectMan->currentTransformAnchor());
    }

    const int frameIndex = editor()->currentFrame();
    if (layer->keyExists(frameIndex))
    {
        undoSaveState->keyframe = std::unique_ptr<KeyFrame>(layer->getLastKeyFrameAtPosition(frameIndex)->clone());
    }
    else if (layer->getKeyFrameWhichCovers(frameIndex) != nullptr)
    {
        undoSaveState->keyframe = std::unique_ptr<KeyFrame>(layer->getKeyFrameWhichCovers(frameIndex)->clone());
    }
}

QAction* UndoRedoManager::createUndoAction(QObject* parent, const QIcon& icon)
{
    QAction* undoAction = nullptr;
    if (mNewBackupSystemEnabled) {
        undoAction = mUndoStack.createUndoAction(parent);
    } else {
        undoAction = new QAction(parent);
        undoAction->setText(tr("Undo"));
        undoAction->setDisabled(true);
    }
    undoAction->setIcon(icon);

    if (mNewBackupSystemEnabled) {
        // The new system takes care of this automatically
    } else {
        connect(undoAction, &QAction::triggered, this, &UndoRedoManager::legacyUndo);
    }
    return undoAction;
}

QAction* UndoRedoManager::createRedoAction(QObject* parent, const QIcon& icon)
{
    QAction* redoAction = nullptr;
    if (mNewBackupSystemEnabled) {
        redoAction = mUndoStack.createRedoAction(parent);
    } else {
        redoAction = new QAction(parent);
        redoAction->setText(tr("Redo"));
        redoAction->setDisabled(true);
    }
    redoAction->setIcon(icon);

    if (mNewBackupSystemEnabled) {
        // The new system takes care of this automatically
    } else {
        connect(redoAction, &QAction::triggered, this, &UndoRedoManager::legacyRedo);
    }
    return redoAction;
}

void UndoRedoManager::updateUndoAction(QAction* undoAction)
{
    if (mNewBackupSystemEnabled) {
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

void UndoRedoManager::updateRedoAction(QAction* redoAction)
{
    if (mNewBackupSystemEnabled) {
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

void UndoRedoManager::clearStack()
{
    if (mNewBackupSystemEnabled) {
        mUndoStack.clear();
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

void UndoRedoManager::legacyBackup(const QString& undoText)
{

    if (mNewBackupSystemEnabled) {
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

bool UndoRedoManager::legacyBackup(int backupLayer, int backupFrame, const QString& undoText)
{
    if (mNewBackupSystemEnabled) {
        return false;
    }

    while (mLegacyBackupList.size() - 1 > mLegacyBackupIndex && !mLegacyBackupList.empty())
    {
        delete mLegacyBackupList.takeLast();
    }
    while (mLegacyBackupList.size() >= editor()->preference()->getInt(SETTING::UNDO_REDO_MAX_STEPS))
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

void UndoRedoManager::sanitizeLegacyBackupElementsAfterLayerDeletion(int layerIndex)
{
    if (mNewBackupSystemEnabled) {
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

void UndoRedoManager::restoreLegacyKey()
{
    if (mNewBackupSystemEnabled) {
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

void UndoRedoManager::legacyUndo()
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

void UndoRedoManager::legacyRedo()
{
    if (!mLegacyBackupList.empty() && mLegacyBackupIndex < mLegacyBackupList.size() - 2)
    {
        mLegacyBackupIndex++;

        mLegacyBackupList[mLegacyBackupIndex + 1]->restore(editor());
        emit didUpdateUndoStack();
    }
}

void UndoRedoManager::rememberLastModifiedFrame(int layerNumber, int frameNumber)
{
    if (mNewBackupSystemEnabled) {
        // not required
    } else {
        mLegacyLastModifiedLayer = layerNumber;
        mLegacyLastModifiedFrame = frameNumber;
    }
}
