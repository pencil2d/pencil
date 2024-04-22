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

#include <QDebug>

#include "layermanager.h"
#include "selectionmanager.h"

#include "layersound.h"
#include "layerbitmap.h"
#include "layervector.h"

#include "editor.h"
#include "undoredocommand.h"

UndoRedoCommand::UndoRedoCommand(Editor* editor, QUndoCommand* parent) : QUndoCommand(parent)
{
    qDebug() << "backupElement created";
    mEditor = editor;
}

UndoRedoCommand::~UndoRedoCommand()
{
}

BitmapCommand::BitmapCommand(const BitmapImage* undoBitmap,
                             const int undoLayerId,
                             const QString& description,
                             Editor *editor,
                             QUndoCommand *parent) : UndoRedoCommand(editor, parent)
{

    this->undoBitmap = *undoBitmap;
    this->undoLayerId = undoLayerId;

    Layer* layer = editor->layers()->currentLayer();
    redoLayerId = layer->id();
    redoBitmap = *static_cast<LayerBitmap*>(layer)->
            getBitmapImageAtFrame(editor->currentFrame());

    setText(description);
}

void BitmapCommand::undo()
{
    QUndoCommand::undo();

    Layer* layer = editor()->layers()->findLayerById(undoLayerId);
    static_cast<LayerBitmap*>(layer)->replaceKeyFrame(&undoBitmap);

    editor()->scrubTo(undoBitmap.pos());
}

void BitmapCommand::redo()
{
    // Ignore automatic redo when added to undo stack
    if (isFirstRedo()) { setFirstRedo(false); return; }

    QUndoCommand::redo();

    Layer* layer = editor()->layers()->findLayerById(redoLayerId);
    static_cast<LayerBitmap*>(layer)->replaceKeyFrame(&redoBitmap);

    editor()->scrubTo(redoBitmap.pos());
}

VectorCommand::VectorCommand(const VectorImage* undoVector,
                                   const int& undoLayerId,
                                   const QString& description,
                                   Editor* editor,
                                   QUndoCommand* parent) : UndoRedoCommand(editor, parent)
{

    this->undoVector = *undoVector;
    this->undoLayerId = undoLayerId;
    Layer* layer = editor->layers()->currentLayer();
    redoLayerId = layer->id();
    redoVector = *static_cast<LayerVector*>(layer)->
            getVectorImageAtFrame(editor->currentFrame());

    setText(description);
}

void VectorCommand::undo()
{
    qDebug() << "BackupVectorElement: undo";

    QUndoCommand::undo();

    Layer* layer = editor()->layers()->findLayerById(undoLayerId);

    static_cast<LayerVector*>(layer)->replaceKeyFrame(&undoVector);

    editor()->scrubTo(undoVector.pos());
}

void VectorCommand::redo()
{
    qDebug() << "BackupVectorElement: redo";

    // Ignore automatic redo when added to undo stack
    if (isFirstRedo()) { setFirstRedo(false); return; }

    QUndoCommand::redo();

    Layer* layer = editor()->layers()->findLayerById(redoLayerId);

    static_cast<LayerVector*>(layer)->replaceKeyFrame(&redoVector);

    editor()->scrubTo(redoVector.pos());
}

TransformCommand::TransformCommand(KeyFrame* undoKeyFrame,
                                   const int undoLayerId,
                                   const QRectF& undoSelectionRect,
                                   const QPointF undoTranslation,
                                   const qreal undoRotationAngle,
                                   const qreal undoScaleX,
                                   const qreal undoScaleY,
                                   const QPointF undoTransformAnchor,
                                   const QString& description,
                                   Editor* editor,
                                   QUndoCommand *parent) : UndoRedoCommand(editor, parent)
{


    this->undoLayerId = undoLayerId;
    this->undoSelectionRect = undoSelectionRect;
    this->undoAnchor = undoTransformAnchor;
    this->undoTranslation = undoTranslation;
    this->undoRotationAngle = undoRotationAngle;
    this->undoScaleX = undoScaleX;
    this->undoScaleY = undoScaleY;

    Layer* redoLayer = editor->layers()->currentLayer();
    redoLayerId = redoLayer->id();

    auto selectMan = editor->select();
    redoSelectionRect = selectMan->mySelectionRect();
    redoAnchor = selectMan->currentTransformAnchor();
    redoTranslation = selectMan->myTranslation();
    redoRotationAngle = selectMan->myRotation();
    redoScaleX = selectMan->myScaleX();
    redoScaleY = selectMan->myScaleY();

    Layer* layer = editor->layers()->findLayerById(undoLayerId);

    // TODO: this could become a bug.. should we check layer type for undo and redo layer respectively?
    const int currentFrame = editor->currentFrame();
    switch(layer->type())
    {
        case Layer::BITMAP:
        {
            undoBitmap = *static_cast<BitmapImage*>(undoKeyFrame);
            redoBitmap = *static_cast<LayerBitmap*>(layer)->getBitmapImageAtFrame(currentFrame);
            break;
        }
        case Layer::VECTOR:
        {
            undoVector = *static_cast<VectorImage*>(undoKeyFrame);
            redoVector = *static_cast<LayerVector*>(layer)->
                    getVectorImageAtFrame(currentFrame);
            break;
        }
        default:
            break;
    }

    setText(description);
}

void TransformCommand::undo()
{
    apply(undoBitmap,
          undoVector,
          undoSelectionRect,
          undoTranslation,
          undoRotationAngle,
          undoScaleX,
          undoScaleY,
          undoAnchor,
          undoLayerId);
}

void TransformCommand::redo()
{
    // Ignore automatic redo when added to undo stack
    if (isFirstRedo()) { setFirstRedo(false); return; }

    apply(redoBitmap,
          redoVector,
          redoSelectionRect,
          redoTranslation,
          redoRotationAngle,
          redoScaleX,
          redoScaleY,
          redoAnchor,
          redoLayerId);
}

void TransformCommand::apply(const BitmapImage& bitmapImage,
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

    int frameNumber = 0;
    bool roundPixels = true;
    switch(layer->type())
    {
        case Layer::BITMAP:
        {
            static_cast<LayerBitmap*>(layer)->replaceKeyFrame(&bitmapImage);
            frameNumber = bitmapImage.pos();
            break;
        }
        case Layer::VECTOR:
        {
            LayerVector* vlayer = static_cast<LayerVector*>(layer);
            vlayer->replaceKeyFrame(&vectorImage);
            frameNumber = vectorImage.pos();
            roundPixels = false;
            break;
        }
        default:
            // Only canvas related KeyFrame types are relevant for transforms.
            Q_UNREACHABLE();
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
