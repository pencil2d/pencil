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
#include "direction.h"
#include "movemode.h"
#include "pencildef.h"

class Editor;
class BackupManager;
class PreferenceManager;
class BitmapImage;
class VectorImage;
class SoundClip;
class Camera;
class Layer;
class KeyFrame;

class BackupElement : public QUndoCommand
{
public:
    explicit BackupElement(Editor* editor, QUndoCommand* parent = 0);
    virtual ~BackupElement();
    enum types { UNDEFINED,
                 ADD_KEY_MODIF,
                 REMOVE_KEY_MODIF
               };

    Editor* editor() { return mEditor; }

    virtual int type() { return UNDEFINED; }
    virtual void undo() { qDebug() << "shouldn't be here"; }
    virtual void redo() { qDebug() << "shouldn't be here"; }
private:
    Editor* mEditor = nullptr;
};

class AddBitmapElement : public BackupElement
{
public:
    AddBitmapElement(BitmapImage* backupBitmap, BitmapImage* bufferImage,
                     int backupLayerId,
                     int backupFrameIndex,
                     QString description,
                     Editor* editor,
                     QUndoCommand* parent = 0);

    int oldLayerIndex = 0;
    int newLayerIndex = 0;
    int frameIndex = 0;
    int previousFrameIndex = 0;

    int otherFrameIndex = 0;

    int oldLayerId = 0;
    int newLayerId = 0;

    int emptyFrameSettingVal = -1;

    BitmapImage* oldBitmap = nullptr;
    BitmapImage* newBitmap = nullptr;

    BitmapImage* oldBufferImage = nullptr;

    bool isFirstRedo = true;

    void undo() override;
    void redo() override;

    void applyToLastTransformedImage();
};


class AddVectorElement : public BackupElement
{
public:
    AddVectorElement(VectorImage* backupVector,
                     int backupLayerId,
                     QString description,
                     Editor* editor,
                     QUndoCommand* parent = 0);

    int newLayerIndex = 0;
    int frameIndex = 0;
    int otherFrameIndex = 0;

    int newLayerId = 0;
    int oldLayerId = 0;
    int previousFrameIndex = 0;
    int emptyFrameSettingVal = -1;

    VectorImage* oldVector;
    VectorImage* newVector;

    bool isFirstRedo = true;

    void undo() override;
    void redo() override;

};

class AddKeyFrameElement : public BackupElement
{
public:
    enum { Id = 5 };
    AddKeyFrameElement(int backupFrameIndex,
                       int backupLayerId,
                       int backupKeySpacing,
                       bool backupKeyExisted,
                       QString description,
                       Editor* editor,
                       QUndoCommand* parent = 0);


    int newLayerIndex = 0;
    int newFrameIndex = 0;
    int oldFrameIndex = 0;
    int oldLayerId = 0;
    int newLayerId = 0;

    std::map<int, KeyFrame*>oldKeyFrames;
    std::map<int, KeyFrame*>newKeyFrames;

    int oldKeySpacing = 0;
    bool oldKeyExisted = false;

    KeyFrame* newKey = nullptr;

    bool isFirstRedo = true;

    int type() override { return BackupElement::ADD_KEY_MODIF; }
    void undo() override;
    void redo() override;
    int id() const override { return Id; }
    bool mergeWith(const QUndoCommand *other) override;
};

class RemoveKeyFrameElement : public BackupElement
{
public:
    enum { Id = 4 };
    RemoveKeyFrameElement(KeyFrame* backupBitmap,
                          int backupLayerId,
                          Editor* editor,
                          QUndoCommand* parent = 0);

    int oldLayerIndex = 0;
    int newLayerIndex = 0;

    int oldFrameIndex = 0;
    int newFrameIndex = 0;

    int oldLayerId = 0;
    int newLayerId = 0;

    KeyFrame* oldKey;

    BitmapImage* oldBitmap;
    VectorImage* oldVector;
    SoundClip* oldClip;
    Camera* oldCamera;

    bool isFirstRedo = true;

    int type() override { return BackupElement::REMOVE_KEY_MODIF; }
    void undo() override;
    void redo() override;
    bool mergeWith(const QUndoCommand *other) override;
    int id() const override { return Id; }

};

class SelectionElement : public BackupElement
{
public:

    enum { Id = 1 };

    SelectionElement(SelectionType backupSelectionType,
                     QRectF backupTempSelection,
                     QRectF backupSelection,
                     Editor* editor,
                     QUndoCommand* parent = 0);

    QRectF oldSelection = QRectF();
    QRectF newSelection = QRectF();

    QRectF oldTempSelection = QRectF();
    QRectF newTempSelection = QRectF();

    SelectionType selectionType;

    bool isFirstRedo = true;

    void undo() override;
    void redo() override;
    bool mergeWith(const QUndoCommand *other) override;
    int id() const override { return Id; }

    void redoDeselection();
    void redoSelection();
    void undoDeselection();
    void undoSelection();
    void apply(int layerId,
               int frameIndex,
               BitmapImage* bitmap,
               VectorImage* vector);
};

class TransformElement : public BackupElement

{
public:

    enum { Id = 2 };
    TransformElement(KeyFrame* backupKeyFrame,
                     BitmapImage* backupBufferImage,
                     int backupLayerId,
                     int backupFramePos, QRectF backupSelection,
                     QRectF backupTempSelection, QTransform backupTransform,
                     Editor* editor,
                     QUndoCommand* parent = 0);


    QRectF oldSelectionRect = QRectF();
    QRectF newSelectionRect = QRectF();

    QRectF oldSelectionRectTemp = QRectF();
    QRectF newSelectionRectTemp= QRectF();

    QTransform oldTransform;
    QTransform newTransform;

    BitmapImage* oldBitmap;
    BitmapImage* newBitmap;

    BitmapImage* bufferImg;

    VectorImage* oldVector;
    VectorImage* newVector;

    int oldLayerId = 0;
    int newLayerId = 0;

    int oldFrameIndex = 0;
    int newFrameIndex = 0;

    bool isFirstRedo = true;
    void undo() override;
    void redo() override;
    void apply(QRectF tempRect,
               BitmapImage* bitmapImage,
               VectorImage* vectorImage,
               QRectF selectionRect,
               QTransform transform,
               int frameIndex,
               int layerId);

    bool mergeWith(const QUndoCommand *other) override;
    int id() const override { return Id; }
};

class CameraMotionElement : public BackupElement
{
public:

    enum { Id = 3 };
    CameraMotionElement(QPointF backupTranslation,
                        float backupRotation,
                        float backupScale,
                        Editor* editor,
                        QUndoCommand* parent = 0);


    QPointF oldTranslation = QPointF(0,0);
    float  oldRotation = 0.0f;
    float oldScale = 0.0f;

    QPointF newTranslation = QPointF(0,0);
    float  newRotation = 0.0f;
    float newScale = 0.0f;

    bool isFirstRedo = true;

    void undo() override;
    void redo() override;
    bool mergeWith(const QUndoCommand *other) override;
    int id() const override { return Id; }
};

class AddLayerElement : public BackupElement
{
public:
    AddLayerElement(Layer* backupLayer,
                    Editor* editor,
                    QUndoCommand* parent = 0);

    Layer* oldLayer;
    Layer* newLayer;

    QString newLayerName;

    Layer::LAYER_TYPE newLayerType = Layer::UNDEFINED;

    int newLayerId = 0;
    int oldLayerId = 0;

    bool isFirstRedo = true;

    void undo() override;
    void redo() override;
};

class DeleteLayerElement : public BackupElement
{
public:
    DeleteLayerElement(QString backupLayerName,
                       Layer::LAYER_TYPE backupType,
                       std::map<int, KeyFrame*, std::greater<int> >,
                       int backupLayerIndex,
                       int backupLayerId,
                       Editor* editor,
                       QUndoCommand* parent = 0);

    Layer* oldLayer;

    std::map<int, KeyFrame*, std::greater<int> >oldLayerKeys;

    QString oldLayerName;

    Layer::LAYER_TYPE oldLayerType;

    int oldLayerIndex = 0;
    int newLayerIndex = 0;

    int oldFrameIndex = 0;

    int oldLayerId = 0;
    int newLayerId = 0;

    bool isFirstRedo = true;

    void undo() override;
    void redo() override;
};

class RenameLayerElement : public BackupElement
{
public:
    RenameLayerElement(QString backupLayerName,
                       int backupLayerId,
                       Editor* editor,
                       QUndoCommand* parent = 0);

    QString oldLayerName;
    QString newLayerName;

    int oldLayerIndex = 0;
    int newLayerIndex = 0;

    int oldLayerId = 0;
    int newLayerId = 0;

    bool isFirstRedo = true;

    void undo() override;
    void redo() override;
};

class CameraPropertiesElement : public BackupElement
{
public:
    CameraPropertiesElement(QString backupLayerName,
                            QRect backupViewRect,
                            int backupLayerId,
                            Editor* editor,
                            QUndoCommand* parent = 0);

    QString oldLayerName;
    QString newLayerName;

    int oldLayerIndex = 0;
    int newLayerIndex = 0;

    QRect oldViewRect;
    QRect newViewRect;

    int oldLayerId = 0;
    int newLayerId = 0;

    bool isFirstRedo = true;

    void undo() override;
    void redo() override;
};

class DragFrameElement : public BackupElement
{
public:
    DragFrameElement(int backupLayerIndex,
                     int backupFrameOffset,
                     Editor* editor,
                     QUndoCommand* parent = 0);

    int layerId = 0;

    int frameOffset = 0;
    int endFrame = 0;
    int startFrame = 0;

    bool isFirstRedo = true;

    void undo() override;
    void redo() override;
};

class FlipViewElement : public BackupElement
{
public:
    FlipViewElement(bool backupFlipEnabled,
                    DIRECTION backupFlipDirection,
                    Editor* editor,
                    QUndoCommand* parent = 0);

    bool isFlipped = false;

    DIRECTION direction;

    bool isFirstRedo = true;

    void undo() override;
    void redo() override;
};

class MoveLayerElement : public BackupElement
{

public:
    MoveLayerElement(int backupOldLayerIndex,
                     int backupNewLayerIndex,
                     Editor* editor,
                     QUndoCommand* parent = 0);

    int oldLayerIndex = 0;
    int newLayerIndex = 0;

    bool isFirstRedo = true;

    void undo() override;
    void redo() override;
};

#endif // BACKUPELEMENT_H
