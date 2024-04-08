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

#include <QCoreApplication>
#include "QProgressDialog"
#include <QDebug>

#include "layermanager.h"
#include "backupmanager.h"
#include "viewmanager.h"
#include "selectionmanager.h"

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

BitmapElement::BitmapElement(const BitmapImage* backupBitmap,
                             const int backupLayerId,
                             Editor *editor,
                             QString description,
                             QUndoCommand *parent) : BackupElement(editor, parent)
{

    oldBitmap = backupBitmap->clone();
    oldFrameIndex = oldBitmap->pos();
    oldLayerId = backupLayerId;

    Layer* layer = editor->layers()->currentLayer();
    newLayerId = layer->id();
    newFrameIndex = editor->currentFrame();
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

void BitmapElement::undo()
{
    Layer* layer = editor()->layers()->findLayerById(oldLayerId);

    const TransformElement* childElem = static_cast<const TransformElement*>(this->child(0));
    if (childElem)
    {
        undoTransform(childElem);
    }
    else
    {
        static_cast<LayerBitmap*>(layer)->replaceLastBitmapAtFrame(oldBitmap);
    }

    editor()->scrubTo(/*oldLayerId, */oldFrameIndex);
}

void BitmapElement::redo()
{
    if (isFirstRedo()) { setFirstRedo(false); return; }

    const TransformElement* childElem = static_cast<const TransformElement*>(this->child(0));
    if (childElem)
    {
        redoTransform(childElem);
    }
    else
    {
        Layer* layer = editor()->layers()->findLayerById(newLayerId);
        static_cast<LayerBitmap*>(layer)->replaceLastBitmapAtFrame(newBitmap);
    }

    editor()->scrubTo(/*newLayerId, */newFrameIndex);
}

void BitmapElement::undoTransform(const TransformElement* childElem)
{

    BitmapImage* oldBitmapClone = oldBitmap->clone();

    // make the cloned bitmap the new canvas image.
    Layer* layer = editor()->layers()->findLayerById(oldLayerId);
    static_cast<LayerBitmap*>(layer)->replaceLastBitmapAtFrame(oldBitmapClone);

    // set selections so the transform will be correct
    auto selectMan = editor()->select();

    selectMan->setSelection(childElem->oldSelectionRect);
    selectMan->setTransformAnchor(childElem->oldAnchor);
    selectMan->setTranslation(childElem->oldTranslation);
    selectMan->setScale(childElem->oldScaleX, childElem->oldScaleY);
    selectMan->setRotation(childElem->oldRotationAngle);

    // editor()->canvas()->paintTransformedSelection(layer, oldBitmapClone, childElem->oldTransform, childElem->oldSelectionRect);
}

void BitmapElement::redoTransform(const TransformElement* childElem)
{
    Layer* layer = editor()->layers()->findLayerById(newLayerId);

    BitmapImage* newBitmapClone = newBitmap->clone();

    static_cast<LayerBitmap*>(layer)->replaceLastBitmapAtFrame(newBitmapClone);

    auto selectMan = editor()->select();
    selectMan->setSelection(childElem->newSelectionRect);
    selectMan->setTransformAnchor(childElem->newAnchor);
    selectMan->setTranslation(childElem->newTranslation);
    selectMan->setScale(childElem->newScaleX, childElem->newScaleY);
    selectMan->setRotation(childElem->newRotationAngle);

    // editor()->canvas()->paintTransformedSelection(layer, newBitmapClone, childElem->oldTransform, childElem->oldSelectionRect);
}

VectorElement::VectorElement(const VectorImage* backupVector,
                                   const int& backupLayerId,
                                   const DrawOnEmptyFrameAction& backupFrameAction,
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

    newVector = static_cast<LayerVector*>(layer)->
            getVectorImageAtFrame(newFrameIndex)->clone();

    setText(description);
}

void VectorElement::undo()
{
    qDebug() << "BackupVectorElement: undo";

    Layer* layer = editor()->layers()->findLayerById(oldLayerId);

    static_cast<LayerVector*>(layer)->replaceLastVectorImageAtFrame(oldVector);

    editor()->scrubTo(/*oldLayerId, */oldFrameIndex);
}

void VectorElement::redo()
{
    qDebug() << "BackupVectorElement: redo";

    if (isFirstRedo()) { setFirstRedo(false); return; }

    Layer* layer = editor()->layers()->findLayerById(newLayerId);

    static_cast<LayerVector*>(layer)->replaceLastVectorImageAtFrame(newVector);

    editor()->scrubTo(/*newLayerId, */newFrameIndex);
}

TransformElement::TransformElement(const KeyFrame* backupKeyFrame,
                                   const int backupLayerId,
                                   const DrawOnEmptyFrameAction& backupFrameAction,
                                   const QRectF& backupSelectionRect,
                                   const QPointF backupTranslation,
                                   const qreal backupRotationAngle,
                                   const qreal backupScaleX,
                                   const qreal backupScaleY,
                                   const QPointF backupTransformAnchor,
                                   const QString& description,
                                   Editor* editor,
                                   QUndoCommand *parent) : BackupElement(editor, parent)
{


    oldLayerId = backupLayerId;
    oldFrameIndex = backupKeyFrame->pos();
    oldSelectionRect = backupSelectionRect;
    oldAnchor = backupTransformAnchor;
    oldTranslation = backupTranslation;
    oldRotationAngle = backupRotationAngle;
    oldScaleX = backupScaleX;
    oldScaleY = backupScaleY;

    Layer* newLayer = editor->layers()->currentLayer();
    newLayerId = newLayer->id();
    newFrameIndex = editor->currentFrame();

    auto selectMan = editor->select();
    newSelectionRect = selectMan->mySelectionRect();
    newAnchor = selectMan->currentTransformAnchor();
    newTranslation = selectMan->myTranslation();
    newRotationAngle = selectMan->myRotation();
    newScaleX = selectMan->myScaleX();
    newScaleY = selectMan->myScaleY();

    Layer* layer = editor->layers()->findLayerById(backupLayerId);

    KeyFrame* oldKeyFrame = backupKeyFrame->clone();

    switch(layer->type())
    {
        case Layer::BITMAP:
        {
            oldBitmap = static_cast<BitmapImage*>(oldKeyFrame)->clone();
            newBitmap = static_cast<LayerBitmap*>(layer)->getBitmapImageAtFrame(newFrameIndex)->clone();
            break;
        }
        case Layer::VECTOR:
        {
            oldVector = static_cast<VectorImage*>(oldKeyFrame)->clone();
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
    apply(oldBitmap,
          oldVector,
          oldSelectionRect,
          oldTranslation,
          oldRotationAngle,
          oldScaleX,
          oldScaleY,
          oldAnchor,
          oldLayerId);
}

void TransformElement::redo()
{
    // if (isFirstRedo()) { setFirstRedo(false); return; }

    // apply(newBitmap,
    //       newVector,
    //       newSelectionRect,
    //       newSelectionRectTemp,
    //       newTransformedSelectionRect,
    //       newRotationAngle,
    //       newScaleX,
    //       newScaleY,
    //       newIsSelected,
    //       newTransform,
    //       newLayerId);
}

void TransformElement::apply(const BitmapImage* bitmapImage,
                             const VectorImage* vectorImage,
                             const QRectF& selectionRect,
                             const QPointF translation,
                             const qreal rotationAngle,
                             const qreal scaleX,
                             const qreal scaleY,
                             const QPointF selectionAnchor,
                             const int layerId)
{

    // Layer* layer = editor()->layers()->findLayerById(layerId);
    // Layer* currentLayer = editor()->layers()->currentLayer();

    // if (layer->type() != currentLayer->type())
    // {
    //     editor()->layers()->setCurrentLayer(layer);
    // }

    // auto selectMan = editor()->select();
    // selectMan->setSelection(selectionRect);
    // selectMan->setRotation(rotationAngle);
    // selectMan->setScale(scaleX, scaleY);
    // selectMan->setTranslation(translation);
    // selectMan->setTransformAnchor(selectionAnchor);

    // switch(layer->type())
    // {
    //     case Layer::BITMAP:
    //     {
    //         if (bitmapImage->isMinimallyBounded()) {
    //             static_cast<LayerBitmap*>(layer)->replaceLastBitmapAtFrame(bitmapImage);
    //             KeyFrame* cKeyFrame = layer->getLastKeyFrameAtPosition(editor()->currentFrame());
    //             // editor()->canvas()->paintTransformedSelection(layer,
    //             //                                               cKeyFrame,
    //             //                                               transform,
    //             //                                               selectionRect);
    //         }
    //         break;
    //     }
    //     case Layer::VECTOR:
    //     {
    //         LayerVector* vlayer = static_cast<LayerVector*>(layer);
    //         vlayer->replaceLastVectorImageAtFrame(vectorImage);
    //         VectorImage* vecImage = vlayer->getLastVectorImageAtFrame(editor()->currentFrame(), 0);
    //         // vecImage->setSelectionTransformation(transform); // Maybe not needed
    //         editor()->updateFrame();
    //         break;
    //     }
    //     default:
    //         break;

    // }
}
