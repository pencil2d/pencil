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

    oldBitmap = *backupBitmap;
    oldFrameIndex = oldBitmap.pos();
    oldLayerId = backupLayerId;

    Layer* layer = editor->layers()->currentLayer();
    newLayerId = layer->id();
    newFrameIndex = editor->currentFrame();
    newBitmap = *static_cast<LayerBitmap*>(layer)->
            getBitmapImageAtFrame(newFrameIndex);

    setText(description);
}

void BitmapElement::undo()
{
    QUndoCommand::undo();

    Layer* layer = editor()->layers()->findLayerById(oldLayerId);
    static_cast<LayerBitmap*>(layer)->replaceLastBitmapAtFrame(&oldBitmap);

    editor()->scrubTo(/*oldLayerId, */oldFrameIndex);
}

void BitmapElement::redo()
{
    if (isFirstRedo()) { setFirstRedo(false); return; }

    QUndoCommand::redo();

    Layer* layer = editor()->layers()->findLayerById(newLayerId);
    static_cast<LayerBitmap*>(layer)->replaceLastBitmapAtFrame(&newBitmap);

    editor()->scrubTo(/*newLayerId, */newFrameIndex);
}

VectorElement::VectorElement(const VectorImage* backupVector,
                                   const int& backupLayerId,
                                   QString description,
                                   Editor* editor,
                                   QUndoCommand* parent) : BackupElement(editor, parent)
{

    oldVector = *backupVector;
    oldFrameIndex = oldVector.pos();

    newLayerIndex = editor->layers()->currentLayerIndex();
    newFrameIndex = editor->currentFrame();

    oldLayerId = backupLayerId;
    Layer* layer = editor->layers()->currentLayer();
    newLayerId = layer->id();

    newVector = *static_cast<LayerVector*>(layer)->
            getVectorImageAtFrame(newFrameIndex);

    setText(description);
}

void VectorElement::undo()
{
    qDebug() << "BackupVectorElement: undo";

    QUndoCommand::undo();

    Layer* layer = editor()->layers()->findLayerById(oldLayerId);

    static_cast<LayerVector*>(layer)->replaceLastVectorImageAtFrame(&oldVector);

    editor()->scrubTo(/*oldLayerId, */oldFrameIndex);
}

void VectorElement::redo()
{
    qDebug() << "BackupVectorElement: redo";

    if (isFirstRedo()) { setFirstRedo(false); return; }

    QUndoCommand::redo();

    Layer* layer = editor()->layers()->findLayerById(newLayerId);

    static_cast<LayerVector*>(layer)->replaceLastVectorImageAtFrame(&newVector);

    editor()->scrubTo(/*newLayerId, */newFrameIndex);
}

TransformElement::TransformElement(KeyFrame* backupKeyFrame,
                                   const int backupLayerId,
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

    switch(layer->type())
    {
        case Layer::BITMAP:
        {
            oldBitmap = *static_cast<BitmapImage*>(backupKeyFrame);
            newBitmap = *static_cast<LayerBitmap*>(layer)->getBitmapImageAtFrame(newFrameIndex);
            break;
        }
        case Layer::VECTOR:
        {
            oldVector = *static_cast<VectorImage*>(backupKeyFrame);
            newVector = *static_cast<LayerVector*>(layer)->
                    getVectorImageAtFrame(newFrameIndex);
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
    if (isFirstRedo()) { setFirstRedo(false); return; }

    apply(newBitmap,
          newVector,
          newSelectionRect,
          newTranslation,
          newRotationAngle,
          newScaleX,
          newScaleY,
          newAnchor,
          newLayerId);
}

void TransformElement::apply(const BitmapImage& bitmapImage,
                             const VectorImage& vectorImage,
                             const QRectF& selectionRect,
                             const QPointF translation,
                             const qreal rotationAngle,
                             const qreal scaleX,
                             const qreal scaleY,
                             const QPointF selectionAnchor,
                             const int layerId)
{

    Layer* layer = editor()->layers()->findLayerById(layerId);
    Layer* currentLayer = editor()->layers()->currentLayer();

    if (layer->type() != currentLayer->type())
    {
        editor()->layers()->setCurrentLayer(layer);
    }

    int frameNumber = 0;
    bool roundPixels = true;
    switch(layer->type())
    {
        case Layer::BITMAP:
        {
            static_cast<LayerBitmap*>(layer)->replaceLastBitmapAtFrame(&bitmapImage);
            frameNumber = bitmapImage.pos();
            break;
        }
        case Layer::VECTOR:
        {
            LayerVector* vlayer = static_cast<LayerVector*>(layer);
            vlayer->replaceLastVectorImageAtFrame(&vectorImage);
            frameNumber = vectorImage.pos();
            roundPixels = false;
            break;
        }
        default:
            break;
    }

    auto selectMan = editor()->select();
    selectMan->setSelection(selectionRect, roundPixels);
    selectMan->setTransformAnchor(selectionAnchor);
    selectMan->setTranslation(translation);
    selectMan->setRotation(rotationAngle);
    selectMan->setScale(scaleX, scaleY);

    selectMan->calculateSelectionTransformation();

    emit editor()->frameModified(frameNumber);
}
