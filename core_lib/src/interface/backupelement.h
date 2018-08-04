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
    AddBitmapElement(BitmapImage* backupBitmap,
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

    bool isFirstRedo = true;

    void undo() override;
    void redo() override;
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
                       bool backupIsSequence,
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

    bool oldIsSequence = false;
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

    SelectionElement(bool backupIsSelected,
                     QRectF backupSelection,
                     Editor* editor,
                     QUndoCommand* parent = 0);

    bool oldIsSelected = false;
    QRectF oldSelection = QRectF(0,0,0,0);

    bool newIsSelected = false;
    QRectF newSelection = QRectF(0,0,0,0);

    bool isFirstRedo = true;

    void undo() override;
    void redo() override;
    bool mergeWith(const QUndoCommand *other) override;
    int id() const override { return Id; }
};

class TransformElement : public BackupElement
{
public:

    enum { Id = 2 };
    TransformElement(int backupLayerId,
                KeyFrame* backupKeyFrame,
                QRectF backupTempSelection,
                Editor* editor,
                QUndoCommand* parent = 0);

    QPointF transformOffset = QPointF(0,0);

    QRectF oldTransformApplied = QRectF(0,0,0,0);
    QRectF newTransformApplied = QRectF(0,0,0,0);

    BitmapImage* oldBitmap;
    BitmapImage* newBitmap;

    VectorImage* oldVector;
    VectorImage* newVector;

    int oldLayerId = 0;
    int newLayerId = 0;

    int oldFrameIndex = 0;
    int newFrameIndex = 0;


    bool isFirstRedo = true;
    void undo() override;
    void redo() override;
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
