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

#ifndef UNDOREDOCOMMAND_H
#define UNDOREDOCOMMAND_H

#include <QUndoCommand>
#include <QRectF>

#include "bitmapimage.h"
#include "vectorimage.h"

class Editor;
class UndoRedoManager;
class PreferenceManager;
class SoundClip;
class Camera;
class Layer;
class KeyFrame;
class TransformCommand;

class UndoRedoCommand : public QUndoCommand
{
public:
    explicit UndoRedoCommand(Editor* editor, QUndoCommand* parent = nullptr);
    ~UndoRedoCommand() override;

protected:
    Editor* editor() { return mEditor; }

    bool isFirstRedo() const { return mIsFirstRedo; }
    void setFirstRedo(const bool state) { mIsFirstRedo = state; }

private:
    Editor* mEditor = nullptr;
    bool mIsFirstRedo = true;
};

class BitmapCommand : public UndoRedoCommand
{

public:
    BitmapCommand(const BitmapImage* backupBitmap,
                  int backupLayerId,
                  const QString& description,
                  Editor* editor,
                  QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    int undoLayerId = 0;
    int redoLayerId = 0;

    BitmapImage undoBitmap;
    BitmapImage redoBitmap;
};

class VectorCommand : public UndoRedoCommand
{
public:
    VectorCommand(const VectorImage* undoVector,
                     const int& undoLayerId,
                     const QString& description,
                     Editor* editor,
                     QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    int undoLayerId = 0;
    int redoLayerId = 0;

    VectorImage undoVector;
    VectorImage redoVector;
};

class TransformCommand : public UndoRedoCommand

{
public:
    TransformCommand(KeyFrame* undoKeyFrame,
                     int undoLayerId,
                     const QRectF& undoSelectionRect,
                     QPointF undoTranslation,
                     qreal undoRotationAngle,
                     qreal undoScaleX,
                     qreal undoScaleY,
                     QPointF undoTransformAnchor,
                     const QString& description,
                     Editor* editor,
                     QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    void apply(const BitmapImage& bitmapImage,
               const VectorImage& vectorImage,
               const QRectF& selectionRect,
               QPointF translation,
               qreal rotationAngle,
               qreal scaleX,
               qreal scaleY,
               QPointF selectionAnchor,
               int layerId);

    QRectF undoSelectionRect;
    QRectF redoSelectionRect;

    QPointF undoAnchor;
    QPointF redoAnchor;

    QPointF undoTranslation;
    QPointF redoTranslation;

    qreal undoScaleX;
    qreal undoScaleY;

    qreal redoScaleX;
    qreal redoScaleY;

    qreal undoRotationAngle;
    qreal redoRotationAngle;

    BitmapImage undoBitmap;
    BitmapImage redoBitmap;

    VectorImage undoVector;
    VectorImage redoVector;

    int undoLayerId = 0;
    int redoLayerId = 0;
};

#endif // UNDOREDOCOMMAND_H
