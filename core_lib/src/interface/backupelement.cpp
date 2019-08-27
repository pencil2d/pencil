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

#include <QCoreApplication>
#include "QProgressDialog"

#include "layermanager.h"
#include "backupmanager.h"
#include "viewmanager.h"
#include "selectionmanager.h"
#include "keyframemanager.h"
#include "canvasmanager.h"

#include "layersound.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layercamera.h"

#include "editor.h"
#include "backupelement.h"

#include "vectorimage.h"
#include "bitmapimage.h"
#include "soundclip.h"
#include "camera.h"

BackupElement::BackupElement(Editor* editor, QUndoCommand* parent) : QUndoCommand(parent)
{
    qDebug() << "backupElement created";
    mEditor = editor;
}

BackupElement::~BackupElement()
{
}

AddBitmapElement::AddBitmapElement(const BitmapImage* backupBitmap,
                                   const int& backupLayerId,
                                   QString description,
                                   Editor *editor,
                                   QUndoCommand *parent) : BackupElement(editor, parent)
{

    oldBitmap = backupBitmap->clone();

    oldFrameIndex = oldBitmap->pos();
    newLayerIndex = editor->currentLayerIndex();
    oldLayerId = backupLayerId;

    Layer* layer = editor->layers()->currentLayer();
    newLayerId = layer->id();

    emptyFrameSettingVal = editor->preference()->
            getInt(SETTING::DRAW_ON_EMPTY_FRAME_ACTION);
    newFrameIndex = editor->currentFrame();
    newFrameIndex = BackupManager::getActiveFrameIndex(layer, newFrameIndex, emptyFrameSettingVal);

    newBitmap = static_cast<LayerBitmap*>(layer)->
            getBitmapImageAtFrame(newFrameIndex)->clone();

    auto selectMan = editor->select();
    if (selectMan->somethingSelected()) {
        BitmapImage selectionBitmap = newBitmap->transformed(selectMan->mySelectionRect().toRect(),
                                                              selectMan->selectionTransform(),
                                                              false);

        newBitmap->clear(selectMan->mySelectionRect().toRect());
        newBitmap->paste(&selectionBitmap, QPainter::CompositionMode_SourceOver);
    }

    setText(description);
}

void AddBitmapElement::undo()
{
    Layer* layer = editor()->layers()->findLayerById(oldLayerId);

    if (editor()->select()->somethingSelected())
    {
        undoTransform();
    }
    else
    {
        static_cast<LayerBitmap*>(layer)->replaceLastBitmapAtFrame(oldBitmap);
    }

    editor()->scrubTo(oldFrameIndex);
}

void AddBitmapElement::redo()
{
    if (isFirstRedo) { isFirstRedo = false; return; }
    if (editor()->select()->somethingSelected())
    {
        redoTransform();
    }
    else
    {
        Layer* layer = editor()->layers()->findLayerById(newLayerId);
        static_cast<LayerBitmap*>(layer)->replaceLastBitmapAtFrame(newBitmap);
    }

    editor()->scrubTo(newFrameIndex);
}

void AddBitmapElement::undoTransform()
{
    const TransformElement* childElem = static_cast<const TransformElement*>(this->child(0));

    BitmapImage* oldBitmapClone = oldBitmap->clone();

    // make the cloned bitmap the new canvas image.
    Layer* layer = editor()->layers()->findLayerById(oldLayerId);
    static_cast<LayerBitmap*>(layer)->replaceLastBitmapAtFrame(oldBitmapClone);

    // set selections so the transform will be correct
    auto selectMan = editor()->select();

    selectMan->setSelectionTransform(childElem->oldTransform);
    selectMan->setSelectionRect(childElem->oldSelectionRect);
    selectMan->setTempTransformedSelectionRect(childElem->oldSelectionRectTemp);
    selectMan->setTransformedSelectionRect(childElem->oldTransformedSelectionRect);
    selectMan->setRotation(childElem->oldRotationAngle);
    selectMan->setSomethingSelected(childElem->oldIsSelected);

    editor()->canvas()->paintTransformedSelection(layer, oldBitmapClone, childElem->oldTransform, childElem->oldSelectionRect);
}

void AddBitmapElement::redoTransform()
{
    const TransformElement* childElem = static_cast<const TransformElement*>(this->child(0));
    Layer* layer = editor()->layers()->findLayerById(newLayerId);

    BitmapImage* newBitmapClone = newBitmap->clone();

    static_cast<LayerBitmap*>(layer)->replaceLastBitmapAtFrame(newBitmapClone);

    auto selectMan = editor()->select();
    selectMan->setSelectionTransform(childElem->newTransform);
    selectMan->setSelectionRect(childElem->newSelectionRect);
    selectMan->setTempTransformedSelectionRect(childElem->newSelectionRectTemp);
    selectMan->setTransformedSelectionRect(childElem->newTransformedSelectionRect);
    selectMan->setRotation(childElem->newRotationAngle);
    selectMan->setSomethingSelected(childElem->newIsSelected);

    editor()->canvas()->paintTransformedSelection(layer, newBitmapClone, childElem->oldTransform, childElem->oldSelectionRect);
}

AddVectorElement::AddVectorElement(const VectorImage* backupVector,
                                   const int& backupLayerId,
                                   QString description,
                                   Editor* editor,
                                   QUndoCommand* parent) : BackupElement(editor, parent)
{

    oldVector = backupVector->clone();
    oldFrameIndex = oldVector->pos();

    newLayerIndex = editor->layers()->currentLayerIndex();
    newFrameIndex = editor->currentFrame();

    oldLayerId = backupLayerId;
    Layer* layer = editor->layers()->currentLayer();
    newLayerId = layer->id();

    emptyFrameSettingVal = editor->preference()->
            getInt(SETTING::DRAW_ON_EMPTY_FRAME_ACTION);

    newFrameIndex = BackupManager::getActiveFrameIndex(layer, newFrameIndex, emptyFrameSettingVal);
    newVector = static_cast<LayerVector*>(layer)->
            getVectorImageAtFrame(newFrameIndex)->clone();

    setText(description);
}

void AddVectorElement::undo()
{
    qDebug() << "BackupVectorElement: undo";

    Layer* layer = editor()->layers()->findLayerById(oldLayerId);

    *static_cast<LayerVector*>(layer)->
            getVectorImageAtFrame(oldFrameIndex) = *oldVector;

    editor()->scrubTo(oldFrameIndex);
}

void AddVectorElement::redo()
{
    qDebug() << "BackupVectorElement: redo";

    if (isFirstRedo) { isFirstRedo = false; return; }

    Layer* layer = editor()->layers()->findLayerById(newLayerId);

    *static_cast<LayerVector*>(layer)->
            getVectorImageAtFrame(newFrameIndex) = *newVector;

    editor()->scrubTo(newFrameIndex);
}

AddKeyFrameElement::AddKeyFrameElement(const int& backupFrameIndex,
                                       const int& backupLayerId,
                                       const int& backupKeySpacing,
                                       const bool& backupKeyExisted,
                                       QString description,
                                       Editor *editor,
                                       QUndoCommand *parent) : BackupElement(editor, parent)
{

    Layer* layer = editor->layers()->currentLayer();
    newLayerIndex = editor->currentLayerIndex();
    newFrameIndex = editor->currentFrame();

    oldFrameIndex = backupFrameIndex;

    oldLayerId = backupLayerId;
    newLayerId = layer->id();

    oldKeyExisted = backupKeyExisted;
    oldKeySpacing = backupKeySpacing;

    emptyFrameSettingVal = editor->preference()->
            getInt(SETTING::DRAW_ON_EMPTY_FRAME_ACTION);
    newFrameIndex = editor->currentFrame();
    newFrameIndex = BackupManager::getActiveFrameIndex(layer, newFrameIndex, emptyFrameSettingVal);

    newKey = layer->getLastKeyFrameAtPosition(oldFrameIndex)->clone();
    oldKeyFrames.insert(std::make_pair(oldFrameIndex, newKey));

    bool isSequence = (oldKeySpacing > 1) ? true : false;

    if (description.isEmpty() && !isSequence)
    {
        switch (layer->type())
        {
        case Layer::BITMAP: description = QObject::tr("Bitmap: New key"); break;
        case Layer::VECTOR: description = QObject::tr("Vector: New Key"); break;
        case Layer::SOUND: description = QObject::tr("Sound: New Key"); break;
        case Layer::CAMERA: description = QObject::tr("Camera: New Key"); break;
        default: break;
        }
    }
    setText(description);
}

void AddKeyFrameElement::undo()
{
    qDebug() << "key remove triggered";
    bool isSequence = (oldKeySpacing > 1) ? true : false;
    if (isSequence)
    {
        qDebug() << "oldKeyFrames: " << oldKeyFrames;
        for (auto map : oldKeyFrames)
        {
            qDebug() << "did A key exist before:" << oldKeyExisted;
            if (!oldKeyExisted) {
                editor()->removeKeyAtLayerId(oldLayerId, map.first);
            }
        }
    }
    else
    {
        editor()->removeKeyAtLayerId(oldLayerId, oldFrameIndex);
    }
    editor()->updateCurrentFrame();
}

void AddKeyFrameElement::redo()
{
    qDebug() << "undo: new backup frame " << newFrameIndex;
    qDebug() << "undo: newLayer" << newLayerIndex;

    if (isFirstRedo) { isFirstRedo = false; return; }
    bool isSequence = (oldKeySpacing > 1) ? true : false;

    if (newFrameIndex > 0)
    {
        if (isSequence)
        {
            qDebug() << "nnnew:" << newKeyFrames;
            for (auto map : newKeyFrames)
            {
                newFrameIndex = map.first;
                newKey = map.second;
                editor()->backups()->restoreKey(this);
            }
        }
        else
        {
            qDebug() << "restore Addkey triggered";
            editor()->backups()->restoreKey(this);
        }
    }
    editor()->updateCurrentFrame();

}

bool AddKeyFrameElement::mergeWith(const QUndoCommand *other)
{
    qDebug() << "MERGE CHECK!";

    qDebug() << "state of frames: old" << oldKeyFrames;
    qDebug() << "state of frames:: new" << newKeyFrames;
    qDebug() << newKeyFrames;

    bool isSequence = (oldKeySpacing > 1) ? true : false;

    if (newKeyFrames.empty())
    {
        newKeyFrames.insert(std::make_pair(oldFrameIndex, newKey));
    }

    const AddKeyFrameElement* element = static_cast<const AddKeyFrameElement*>(other);

    if (!isSequence || element->oldKeySpacing < 2)
    {
        return false;
    }
    qDebug() << "MERGING!";

    oldFrameIndex = element->oldFrameIndex;
    newFrameIndex = element->newFrameIndex;
    newKey = element->newKey;
    oldKeyExisted = element->oldKeyExisted;

    std::map<int, KeyFrame*>frames = static_cast<const AddKeyFrameElement*>(other)->oldKeyFrames;
    for (auto map : frames)
    {
        oldKeyFrames.insert(std::make_pair(map.first, map.second));
    }

    newKeyFrames.insert(std::make_pair(oldFrameIndex, newKey));
    return true;
}


RemoveKeyFrameElement::RemoveKeyFrameElement(const KeyFrame* backupKey,
                                             const int& backupLayerId,
                                             Editor *editor,
                                             QUndoCommand *parent) : BackupElement(editor, parent)
{
    oldFrameIndex = backupKey->pos();
    oldLayerId = backupLayerId;
    oldKey = backupKey->clone();

    Layer* layer = editor->layers()->findLayerById(oldLayerId);

    switch(layer->type())
    {
        case Layer::BITMAP:
        {
            oldBitmap = static_cast<BitmapImage*>(oldKey);
            setText(QObject::tr("Remove Bitmap Key"));
            break;
        }
        case Layer::VECTOR:
        {
            oldVector = static_cast<VectorImage*>(oldKey);
            setText(QObject::tr("Remove Vector Key"));
            break;
        }
        case Layer::SOUND:
        {
            oldClip = static_cast<SoundClip*>(oldKey);
            setText(QObject::tr("Remove Sound Key"));
            break;
        }
        case Layer::CAMERA:
        {
            oldCamera = static_cast<Camera*>(oldKey);
            setText(QObject::tr("Remove Camera key"));
            break;
        }
        default:
            break;
    }
}

void RemoveKeyFrameElement::undo()
{
    qDebug() << "undo: old frame index" << oldFrameIndex;
    qDebug() << "restore key";
    editor()->backups()->restoreKey(this);
}

void RemoveKeyFrameElement::redo()
{
    qDebug() << "redo: old backup frame: " << oldFrameIndex;

    if (isFirstRedo) { isFirstRedo = false; return; }

    if (oldFrameIndex > 1)
    {
        qDebug() << "RemoveKeyFrame triggered";
        editor()->removeKeyAtLayerId(oldLayerId, oldFrameIndex);
    }

}

SelectionElement::SelectionElement(const int backupLayerId,
                                   const int backupFrameIndex,
                                   const VectorSelection& backupVectorSelection,
                                   const SelectionType& backupSelectionType,
                                   const QRectF& backupSelectionRect,
                                   const qreal& backupRotationAngle,
                                   const bool& backupIsSelected,
                                   Editor* editor,
                                   QUndoCommand* parent) : BackupElement(editor, parent),
    layerId(backupLayerId),
    frameIndex(backupFrameIndex)
{
    oldSelectionRect = backupSelectionRect;
    oldRotationAngle = backupRotationAngle;
    oldIsSelected = backupIsSelected;

    oldVectorSelection = backupVectorSelection;

    newSelectionRect = editor->select()->myTransformedSelectionRect();
    newRotationAngle = editor->select()->myRotation();
    newIsSelected = editor->select()->somethingSelected();

    newVectorSelection = editor->select()->vectorSelection();

    selectionType = backupSelectionType;

    if (selectionType == SelectionType::SELECTION) {
        setText(QObject::tr("New Selection"));
    } else {
        setText(QObject::tr("Deselected"));
    }

}

void SelectionElement::undo()
{
    editor()->scrubTo(layerId, frameIndex);
    if (selectionType == SelectionType::SELECTION) {
        undoSelection();
    } else {
        undoDeselection();
    }
}

void SelectionElement::undoSelection()
{
    auto selectMan = editor()->select();

    selectMan->setSelection(oldSelectionRect);
    selectMan->setRotation(oldRotationAngle);
    selectMan->setSomethingSelected(oldIsSelected);

    Layer* layer = editor()->layers()->findLayerById(layerId);
    if (layer->type() == Layer::VECTOR) {
        VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getVectorImageAtFrame(frameIndex);
        vectorImage->setSelected(oldVectorSelection.curves, true);
        selectMan->setVectorSelection(oldVectorSelection);
    }

    editor()->deselectAll();

    KeyFrame* cKeyFrame = editor()->keyframes()->currentKeyFrame(layer);
    editor()->canvas()->applyTransformedSelection(layer,
                                                  cKeyFrame,
                                                  selectMan->selectionTransform(),
                                                  oldSelectionRect);
}

void SelectionElement::undoDeselection()
{
    auto selectMan = editor()->select();
    selectMan->resetSelectionTransform();
    selectMan->setSelection(oldSelectionRect);
    selectMan->setRotation(oldRotationAngle);
    selectMan->setSomethingSelected(oldIsSelected);

    Layer* layer = editor()->layers()->findLayerById(layerId);
    if (layer->type() == Layer::VECTOR) {
        VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getVectorImageAtFrame(frameIndex);
        vectorImage->setSelected(oldVectorSelection.curves, true);
        selectMan->setVectorSelection(oldVectorSelection);
    }
}

void SelectionElement::redo()
{
    if (isFirstRedo) { isFirstRedo = false; return; }

    editor()->scrubTo(layerId, frameIndex);
    if (selectionType == SelectionType::SELECTION) {
        redoSelection();
    } else {
        redoDeselection();
    }

}

void SelectionElement::redoSelection()
{
    auto selectMan = editor()->select();

    selectMan->setSelection(newSelectionRect);
    selectMan->setRotation(newRotationAngle);
    selectMan->setSomethingSelected(newIsSelected);
    selectMan->calculateSelectionTransformation();

    Layer* layer = editor()->layers()->findLayerById(layerId);
    if (layer->type() == Layer::VECTOR) {
        VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getVectorImageAtFrame(frameIndex);
        vectorImage->setSelected(newVectorSelection.curves, true);
        selectMan->setVectorSelection(newVectorSelection);
    }
}

void SelectionElement::redoDeselection()
{
    auto selectMan = editor()->select();

    Layer* layer = editor()->layers()->findLayerById(layerId);
    KeyFrame* cKeyFrame = editor()->keyframes()->currentKeyFrame(layer);
    editor()->canvas()->applyTransformedSelection(layer,
                                                  cKeyFrame,
                                                  selectMan->selectionTransform(),
                                                  selectMan->mySelectionRect());

    if (layer->type() == Layer::VECTOR) {
        VectorImage* vectorImage = static_cast<LayerVector*>(layer)->getVectorImageAtFrame(frameIndex);
        vectorImage->setSelected(newVectorSelection.curves, true);
        selectMan->setVectorSelection(newVectorSelection);
    }

    editor()->deselectAll();
}

bool SelectionElement::mergeWith(const QUndoCommand *other)
{
    if (other->id() != id())
    {
        return false;
    }

    auto otherSelectionElement = static_cast<const SelectionElement*>(other);
    SelectionType otherType = otherSelectionElement->selectionType;
    if (selectionType == SelectionType::SELECTION && otherType == selectionType) {
        newSelectionRect = otherSelectionElement->newSelectionRect;
        newIsSelected = otherSelectionElement->newIsSelected;
        newRotationAngle = otherSelectionElement->newRotationAngle;

        auto selectMan = editor()->select();
        selectMan->setSelectionRect(newSelectionRect);
        selectMan->setRotation(newRotationAngle);
        selectMan->setSomethingSelected(newIsSelected);

        return true;
    } else {
        return false;
    }
}

TransformElement::TransformElement(const KeyFrame* backupKeyFrame,
                                   const int backupLayerId,
                                   const QRectF& backupSelectionRect,
                                   const QRectF& backupTempSelectionRect,
                                   const QRectF& backupTransformedSelectionRect,
                                   const qreal backupRotationAngle,
                                   const qreal backupScaleX,
                                   const qreal backupScaleY,
                                   const bool backupIsSelected,
                                   const QTransform& backupTransform,
                                   const QString& description,
                                   Editor *editor,
                                   QUndoCommand *parent) : BackupElement(editor, parent)
{


    oldLayerId = backupLayerId;
    oldFrameIndex = backupKeyFrame->pos();
    oldSelectionRect = backupSelectionRect;
    oldSelectionRectTemp = backupTempSelectionRect;
    oldTransformedSelectionRect = backupTransformedSelectionRect;
    oldRotationAngle = backupRotationAngle;
    oldIsSelected = backupIsSelected;

    oldTransform = backupTransform;
    oldScaleX = backupScaleX;
    oldScaleY = backupScaleY;

    Layer* newLayer = editor->layers()->currentLayer();
    newLayerId = newLayer->id();
    newFrameIndex = editor->currentFrame();

    auto selectMan = editor->select();
    newSelectionRect = selectMan->mySelectionRect();
    newSelectionRectTemp = selectMan->myTempTransformedSelectionRect();
    newTransformedSelectionRect = selectMan->myTransformedSelectionRect();
    newRotationAngle = selectMan->myRotation();
    newIsSelected = selectMan->somethingSelected();
    newTransform = selectMan->selectionTransform();
    newScaleX = selectMan->myScaleX();
    newScaleY = selectMan->myScaleY();

    Layer* layer = editor->layers()->findLayerById(backupLayerId);
    int emptyFrameSettingVal = editor->preference()->getInt(SETTING::DRAW_ON_EMPTY_FRAME_ACTION);

    newFrameIndex = BackupManager::getActiveFrameIndex(layer, newFrameIndex, emptyFrameSettingVal);
    KeyFrame* oldKeyFrame = backupKeyFrame->clone();

    switch(layer->type())
    {
        case Layer::BITMAP:
        {
            oldBitmap = static_cast<BitmapImage*>(oldKeyFrame);
            newBitmap = static_cast<LayerBitmap*>(layer)->getBitmapImageAtFrame(newFrameIndex);
            break;
        }
        case Layer::VECTOR:
        {
            oldVector = static_cast<VectorImage*>(oldKeyFrame);
            newVector = static_cast<LayerVector*>(layer)->
                    getVectorImageAtFrame(newFrameIndex)->clone();
            break;
        }
        default:
            break;
    }

    setText(description);
}

void TransformElement::undo()
{
    apply(oldSelectionRectTemp,
          oldBitmap,
          oldVector,
          oldSelectionRect,
          oldTransformedSelectionRect,
          oldRotationAngle,
          oldScaleX,
          oldScaleY,
          oldIsSelected,
          oldTransform,
          oldLayerId);
}

void TransformElement::redo()
{
    if (isFirstRedo) {
        isFirstRedo = false; return;
    }

    apply(newSelectionRectTemp,
          newBitmap,
          newVector,
          newSelectionRect,
          newTransformedSelectionRect,
          newRotationAngle,
          newScaleX,
          newScaleY,
          newIsSelected,
          newTransform,
          newLayerId);
}

void TransformElement::apply(const QRectF& tempRect,
                             const BitmapImage* bitmapImage,
                             const VectorImage* vectorImage,
                             const QRectF& selectionRect,
                             const QRectF& transformedRect,
                             const qreal rotationAngle,
                             const qreal scaleX,
                             const qreal scaleY,
                             const bool isSelected,
                             const QTransform& transform,
                             const int layerId)
{

    Layer* layer = editor()->layers()->findLayerById(layerId);
    Layer* currentLayer = editor()->layers()->currentLayer();

    if (layer->type() != currentLayer->type())
    {
        editor()->layers()->setCurrentLayer(layer);
    }

    auto selectMan = editor()->select();
    selectMan->setSelectionTransform(transform);
    selectMan->setSelectionRect(selectionRect);
    selectMan->setTempTransformedSelectionRect(tempRect);
    selectMan->setTransformedSelectionRect(transformedRect);
    selectMan->setRotation(rotationAngle);
    selectMan->setSomethingSelected(isSelected);
    selectMan->setScale(scaleX, scaleY);

    switch(layer->type())
    {
        case Layer::BITMAP:
        {
            if (bitmapImage->isMinimallyBounded()) {
                static_cast<LayerBitmap*>(layer)->replaceLastBitmapAtFrame(bitmapImage);
            }
            break;
        }
        case Layer::VECTOR:
        {
            static_cast<LayerVector*>(layer)->replaceLastVectorAtFrame(vectorImage);
            break;
        }
        default:
            break;

    }

    KeyFrame* cKeyFrame = editor()->keyframes()->currentKeyFrame(layer);
    editor()->canvas()->paintTransformedSelection(layer,
                                                  cKeyFrame,
                                                  transform,
                                                  selectionRect);
}

ImportBitmapElement::ImportBitmapElement(const std::map<int, KeyFrame*, std::greater<int>>& backupCanvasKeyFrames,
                                         const std::map<int, KeyFrame*, std::less<int>>& backupImportedKeyFrames,
                                         const int& backupLayerId,
                                         Editor *editor,
                                         QUndoCommand *parent) : BackupElement(editor, parent)
{

    oldLayerId = backupLayerId;
    newLayerId = editor->layers()->currentLayer()->id();

    importedKeyFrames = backupImportedKeyFrames;
    oldKeyFrames = backupCanvasKeyFrames;

    setText(QObject::tr("Import images/s"));
}

void ImportBitmapElement::undo()
{
    for (auto key : importedKeyFrames)
    {
        editor()->removeKeyAtLayerId(oldLayerId,key.second->pos());
    }

    Layer* layer = editor()->layers()->currentLayer();

    // we've removed all keyframes + those that were overwritten
    // now re-add the old ones
    for (auto key : oldKeyFrames)
    {
        editor()->addKeyFrameToLayerId(oldLayerId, key.first, true);
        static_cast<LayerBitmap*>(layer)->putBitmapIntoFrame(key.second, key.second->pos());
    }
    editor()->updateCurrentFrame();
}

void ImportBitmapElement::redo()
{
    if (isFirstRedo)
    {
        isFirstRedo = false;
        return;
    }

    Layer* layer = editor()->layers()->currentLayer();

    for (auto key : importedKeyFrames)
    {
        editor()->addKeyFrameToLayerId(newLayerId, key.first, true);
        static_cast<LayerBitmap*>(layer)->putBitmapIntoFrame(key.second, key.second->pos());
    }
    editor()->updateCurrentFrame();
}

CameraMotionElement::CameraMotionElement(const int backupFrameIndex,
                                         const int backupLayerId,
                                         const QPointF& backupTranslation,
                                         const float backupRotation,
                                         const float backupScale,
                                         Editor* editor,
                                         QUndoCommand* parent) : BackupElement(editor, parent)
{

    frameIndex = backupFrameIndex;
    layerId = backupLayerId;

    oldTranslation = backupTranslation;
    oldRotation = backupRotation;
    oldScale = backupScale;

    ViewManager* viewMgr = editor->view();
    newTranslation = viewMgr->translation();
    newRotation = viewMgr->rotation();
    newScale = viewMgr->scaling();

    setText(QObject::tr("Camera: New motion"));

}

void CameraMotionElement::undo()
{
    ViewManager* viewMgr = editor()->view();

    Layer* layer = editor()->layers()->findLayerById(layerId);
    editor()->scrubTo(layer, frameIndex);

    viewMgr->translate(oldTranslation);
    viewMgr->rotate(oldRotation);
    viewMgr->scale(oldScale);
}

void CameraMotionElement::redo()
{
    if (isFirstRedo) { isFirstRedo = false; return; }

    Layer* layer = editor()->layers()->findLayerById(layerId);
    editor()->scrubTo(layer, frameIndex);

    ViewManager* viewMgr = editor()->view();
    viewMgr->translate(newTranslation);
    viewMgr->rotate(newRotation);
    viewMgr->scale(newScale);
}

bool CameraMotionElement::mergeWith(const QUndoCommand *other)
{
    if (other->id() != id())
    {
        return false;
    }
    newTranslation = static_cast<const CameraMotionElement*>(other)->newTranslation;
    newRotation = static_cast<const CameraMotionElement*>(other)->newRotation;
    newScale = static_cast<const CameraMotionElement*>(other)->newScale;
    return true;
}

AddLayerElement::AddLayerElement(Layer* backupLayer,
                                 Editor* editor,
                                 QUndoCommand* parent) : BackupElement(editor, parent)
{


    oldLayer = backupLayer;
    oldLayerId = backupLayer->id();

    Layer* layer = editor->layers()->currentLayer();
    newLayerType = layer->type();
    newLayerId = layer->id();

    switch(layer->type())
    {
        case Layer::BITMAP:
        {
            newLayer = new LayerBitmap(layer->object());
            break;
        }
        case Layer::VECTOR:
        {
            newLayer = new LayerVector(layer->object());
            break;
        }
        case Layer::SOUND:
        {
            newLayer = new LayerSound(layer->object());
            break;
        }
        case Layer::CAMERA:
        {
            newLayer = new LayerCamera(layer->object());
            break;
        }
        default:
            Q_ASSERT(false);
    }
    newLayerName = layer->name();


    setText(QObject::tr("New Layer"));
}

void AddLayerElement::undo()
{
    qDebug() << "undo";
    qDebug() << "oldLayerId:" << oldLayerId;
    qDebug() << "newLayerId:" << newLayerId;
    editor()->layers()->deleteLayerWithId(newLayerId, newLayerType);

}

void AddLayerElement::redo()
{
    if (isFirstRedo) { isFirstRedo = false; return; }

    switch(newLayer->type())
    {
        case Layer::BITMAP:
        {
            editor()->layers()->createBitmapLayer(newLayerName);
            break;
        }
        case Layer::VECTOR:
        {
            editor()->layers()->createVectorLayer(newLayerName);
            break;
        }
        case Layer::SOUND:
        {
            editor()->layers()->createSoundLayer(newLayerName);
            break;
        }
        case Layer::CAMERA:
        {
            editor()->layers()->createCameraLayer(newLayerName);
            break;
        }
        default:
            break;
    }

}

DeleteLayerElement::DeleteLayerElement(const QString& backupLayerName,
                                       const Layer::LAYER_TYPE& backupType,
                                       const std::map<int, KeyFrame*, std::greater<int> >& backupLayerKeys,
                                       const int& backupFrameIndex,
                                       const int& backupLayerIndex,
                                       const int& backupLayerId,
                                       Editor* editor,
                                       QUndoCommand* parent) : BackupElement(editor, parent)
{


    oldFrameIndex = backupFrameIndex;
    oldLayerIndex = backupLayerIndex;
    oldLayerName = backupLayerName;
    oldLayerKeys = backupLayerKeys;
    oldLayerType = backupType;
    oldLayerId = backupLayerId;

    switch(oldLayerType)
    {
        case Layer::BITMAP:
        {
            setText(QObject::tr("Delete Bitmap Layer"));
            break;
        }
        case Layer::VECTOR:
        {
            setText(QObject::tr("Delete Vector Layer"));
            break;
        }
        case Layer::SOUND:
        {
            setText(QObject::tr("Delete Sound Layer"));
            break;
        }
        case Layer::CAMERA:
        {
            setText(QObject::tr("Delete Camera Layer"));
            break;
        }
        default:
            break;
    }
}

void DeleteLayerElement::undo()
{
    editor()->backups()->restoreLayerKeys(this);
}

void DeleteLayerElement::redo()
{
    if (isFirstRedo) { isFirstRedo = false; return; }

    editor()->layers()->deleteLayerWithId(oldLayerId, oldLayerType);

}

RenameLayerElement::RenameLayerElement(const QString& backupLayerName,
                                       const int& backupLayerId,
                                       Editor *editor,
                                       QUndoCommand *parent) : BackupElement(editor, parent)
{

    oldLayerName = backupLayerName;
    oldLayerId = backupLayerId;

    Layer* layer = editor->layers()->currentLayer();
    newLayerId = layer->id();
    newLayerName = layer->name();

    setText(QObject::tr("Rename layer"));
}

void RenameLayerElement::undo()
{
    Layer* layer = editor()->layers()->findLayerById(oldLayerId);
    editor()->layers()->renameLayer(layer, oldLayerName);
}

void RenameLayerElement::redo()
{
    if (isFirstRedo) { isFirstRedo = false; return; }
    Layer* layer = editor()->layers()->findLayerById(newLayerId);
    editor()->layers()->renameLayer(layer, newLayerName);

}

CameraPropertiesElement::CameraPropertiesElement(const QString& backupLayerName,
                                                 const QRect& backupViewRect,
                                                 const int& backupLayerId,
                                                 Editor *editor,
                                                 QUndoCommand *parent) : BackupElement(editor, parent)
{

    oldLayerId = backupLayerId;
    oldViewRect = backupViewRect;
    oldLayerName = backupLayerName;

    LayerCamera* layer = static_cast<LayerCamera*>(editor->layers()->currentLayer());

    newLayerId = layer->id();
    newLayerName = layer->name();
    newViewRect = layer->getViewRect();

    if (oldViewRect != newViewRect)
    {
        setText(QObject::tr("Edit Camera Properties"));
    }
    else
    {
        setText(QObject::tr("Rename Layer"));
    }

}

void CameraPropertiesElement::undo()
{
    LayerManager* lmgr = editor()->layers();
    LayerCamera* layer = static_cast<LayerCamera*>(lmgr->findLayerById(oldLayerId));

    lmgr->renameLayer(layer, oldLayerName);
    layer->setViewRect(oldViewRect);
    editor()->updateCurrentFrame();

}

void CameraPropertiesElement::redo()
{

    if (isFirstRedo) { isFirstRedo = false; return; }

    LayerManager* lmgr = editor()->layers();
    LayerCamera* layer = static_cast<LayerCamera*>(lmgr->findLayerById(newLayerId));

    if (layer->name() != newLayerName)
    {
        lmgr->renameLayer(layer, newLayerName);
    }
    if (layer->getViewRect() != newViewRect)
    {
        layer->setViewRect(newViewRect);
    }
    editor()->updateCurrentFrame();
}

DragFrameElement::DragFrameElement(const int& backupLayerId,
                                   const int& backupFrameOffset,
                                   Editor* editor,
                                   QUndoCommand* parent) : BackupElement(editor, parent)
{

    layerId = backupLayerId;
    frameOffset = backupFrameOffset;
    startFrame = editor->layers()->findLayerById(backupLayerId)->getFirstFrameInSelection();
    endFrame = editor->layers()->findLayerById(backupLayerId)->getLastFrameInSelection();

    if (startFrame == endFrame)
    {
        setText(QObject::tr("Move Frame"));
    }
    else
    {
        setText(QObject::tr("Move Frames"));
    }
}

void DragFrameElement::undo()
{

    qDebug() << "UNDO";
    qDebug() << "start: " << startFrame;
    qDebug() << "end: " << endFrame;
    Layer* layer = editor()->layers()->findLayerById(layerId);

    layer->deselectAll();
    layer->setFrameSelected(endFrame, true);
    layer->extendSelectionTo(startFrame);
    layer->moveSelectedFrames(-frameOffset);

    editor()->updateTimeLine();
    editor()->updateCurrentFrame();
}

void DragFrameElement::redo()
{

    if (isFirstRedo) { isFirstRedo = false; return; }

    Layer* layer = editor()->layers()->findLayerById(layerId);
    qDebug() << "REDO";
    qDebug() << "start: " << startFrame;
    qDebug() << "end: " << endFrame;

    layer->deselectAll();
    layer->setFrameSelected(endFrame-frameOffset, true);
    layer->extendSelectionTo(startFrame-frameOffset);
    layer->moveSelectedFrames(frameOffset);

    editor()->updateTimeLine();
    editor()->updateCurrentFrame();
}

FlipViewElement::FlipViewElement(const bool& backupFlipState,
                                 const DIRECTION& backupFlipDirection,
                                 Editor *editor,
                                 QUndoCommand *parent) : BackupElement(editor, parent)
{


    isFlipped = backupFlipState;
    direction = backupFlipDirection;

    if (direction == DIRECTION::HORIZONTAL)
    {
        setText(QObject::tr("Flip View X"));
    }
    else
    {
        setText(QObject::tr("Flip View Y"));
    }
}

void FlipViewElement::undo()
{
    if (direction == DIRECTION::VERTICAL)
    {
        editor()->view()->flipVertical(!isFlipped);
    }
    else
    {
        editor()->view()->flipHorizontal(!isFlipped);
    }
}

void FlipViewElement::redo()
{

    if (isFirstRedo) { isFirstRedo = false; return; }

    if (direction == DIRECTION::VERTICAL)
    {
        editor()->view()->flipVertical(isFlipped);
    }
    else
    {
        editor()->view()->flipHorizontal(isFlipped);
    }
}

MoveLayerElement::MoveLayerElement(const int& backupOldLayerIndex,
                                   const int& backupNewLayerIndex,
                                   Editor* editor,
                                   QUndoCommand* parent) : BackupElement(editor, parent)
{
    oldLayerIndex = backupOldLayerIndex;
    newLayerIndex = backupNewLayerIndex;

    setText(QObject::tr("Move layer"));
}

void MoveLayerElement::undo()
{
    editor()->moveLayers(newLayerIndex, oldLayerIndex);
    editor()->layers()->setCurrentLayer(oldLayerIndex);
}

void MoveLayerElement::redo()
{
    if (isFirstRedo) { isFirstRedo = false; return; }

    editor()->moveLayers(oldLayerIndex, newLayerIndex);
    editor()->layers()->setCurrentLayer(newLayerIndex);

}
