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

#ifndef BACKUPELEMENT_H
#define BACKUPELEMENT_H

#include <QObject>
#include <QUndoCommand>
#include <QRectF>
#include <QTransform>

#include "direction.h"
#include "movemode.h"
#include "pencildef.h"
#include "layer.h"
#include "vectorselection.h"
#include "preferencemanager.h"

class Editor;
class BackupManager;
class PreferenceManager;
class BitmapImage;
class VectorImage;
class SoundClip;
class Camera;
class Layer;
class KeyFrame;
class TransformElement;

enum types { UNDEFINED,
             ADD_KEY_MODIF,
             REMOVE_KEY_MODIF
           };

class BackupElement : public QUndoCommand
{
public:
    explicit BackupElement(Editor* editor, QUndoCommand* parent = nullptr);
    virtual ~BackupElement();

    Editor* editor() { return mEditor; }

    virtual int type() const { return UNDEFINED; }
    virtual void undo() { Q_ASSUME(true); } // should never end here
    virtual void redo() { Q_ASSUME(true); } // should never end here
private:
    Editor* mEditor = nullptr;
};

class BitmapElement : public BackupElement
{
public:
    BitmapElement(const BitmapImage* backupBitmap,
                     const int& backupLayerId,
                     const DrawOnEmptyFrameAction& frameAction,
                     QString description,
                     Editor* editor,
                     QUndoCommand* parent = nullptr);

    int oldLayerIndex = 0;
    int newLayerIndex = 0;

    int oldFrameIndex = 0;
    int newFrameIndex = 0;
    int previousFrameIndex = 0;

    int otherFrameIndex = 0;

    int oldLayerId = 0;
    int newLayerId = 0;

    int emptyFrameSettingVal = -1;

    BitmapImage* oldBitmap = nullptr;
    BitmapImage* newBitmap = nullptr;

    bool isFirstRedo = true;

    void undo() override;
    void redo() override;

    void redoTransform(const TransformElement* childElem);
    void undoTransform(const TransformElement* childElem);
};

class VectorElement : public BackupElement
{
public:
    VectorElement(const VectorImage* backupVector,
                     const int& backupLayerId,
                     const DrawOnEmptyFrameAction& backupFrameAction,
                     QString description,
                     Editor* editor,
                     QUndoCommand* parent = nullptr);

    int newLayerIndex = 0;
    int oldFrameIndex = 0;
    int newFrameIndex = 0;

    int newLayerId = 0;
    int oldLayerId = 0;
    int emptyFrameSettingVal = -1;

    VectorImage* oldVector = nullptr;
    VectorImage* newVector = nullptr;

    bool isFirstRedo = true;

    void undo() override;
    void redo() override;

};

class TransformElement : public BackupElement

{
public:

    enum { Id = 2 };
    TransformElement(const KeyFrame* backupKeyFrame,
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
                     QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;
    void apply(const BitmapImage* bitmapImage,
               const VectorImage* vectorImage,
               const QRectF& selectionRect,
               const QPointF translation,
               const qreal rotationAngle,
               const qreal scaleX,
               const qreal scaleY,
               const QPointF selectionAnchor,
               const int layerId);

    int id() const override { return Id; }

    int oldFrameIndex = 0;
    int newFrameIndex = 0;

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


    BitmapImage* oldBitmap = nullptr;
    BitmapImage* newBitmap = nullptr;

    VectorImage* oldVector = nullptr;
    VectorImage* newVector = nullptr;

    int oldLayerId = 0;
    int newLayerId = 0;

    bool isFirstRedo = true;
};

#endif // BACKUPELEMENT_H
