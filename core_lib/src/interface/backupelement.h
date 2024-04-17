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

#ifndef BACKUPELEMENT_H
#define BACKUPELEMENT_H

#include <QUndoCommand>
#include <QRectF>

#include "bitmapimage.h"
#include "vectorimage.h"

class Editor;
class BackupManager;
class PreferenceManager;
class SoundClip;
class Camera;
class Layer;
class KeyFrame;
class TransformElement;

class BackupElement : public QUndoCommand
{
public:
    explicit BackupElement(Editor* editor, QUndoCommand* parent = nullptr);
    ~BackupElement() override;

protected:
    Editor* editor() { return mEditor; }

    bool isFirstRedo() const { return mIsFirstRedo; }
    void setFirstRedo(const bool state) { mIsFirstRedo = state; }

private:
    Editor* mEditor = nullptr;
    bool mIsFirstRedo = true;
};

class BitmapElement : public BackupElement
{

public:
    BitmapElement(const BitmapImage* backupBitmap,
                  int backupLayerId,
                  const QString& description,
                  Editor* editor,
                  QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    int oldLayerId = 0;
    int newLayerId = 0;

    BitmapImage oldBitmap;
    BitmapImage newBitmap;
};

class VectorElement : public BackupElement
{
public:
    VectorElement(const VectorImage* backupVector,
                     const int& backupLayerId,
                     const QString& description,
                     Editor* editor,
                     QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:
    int oldLayerId = 0;
    int newLayerId = 0;

    VectorImage oldVector;
    VectorImage newVector;
};

class TransformElement : public BackupElement

{
public:
    TransformElement(KeyFrame* backupKeyFrame,
                     int backupLayerId,
                     const QRectF& backupSelectionRect,
                     QPointF backupTranslation,
                     qreal backupRotationAngle,
                     qreal backupScaleX,
                     qreal backupScaleY,
                     QPointF backupTransformAnchor,
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

    QRectF oldSelectionRect;
    QRectF newSelectionRect;

    QPointF oldAnchor;
    QPointF newAnchor;

    QPointF oldTranslation;
    QPointF newTranslation;

    qreal oldScaleX;
    qreal oldScaleY;

    qreal newScaleX;
    qreal newScaleY;

    qreal oldRotationAngle;
    qreal newRotationAngle;

    BitmapImage oldBitmap;
    BitmapImage newBitmap;

    VectorImage oldVector;
    VectorImage newVector;

    int oldLayerId = 0;
    int newLayerId = 0;
};

#endif // BACKUPELEMENT_H
