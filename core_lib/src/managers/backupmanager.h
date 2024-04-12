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
#ifndef BACKUPMANAGER_H
#define BACKUPMANAGER_H

#include <QUndoStack>
#include <QUndoView>
#include "basemanager.h"
#include "preferencemanager.h"
#include "layer.h"
#include "direction.h"
#include "movemode.h"
#include "vectorselection.h"

#include <QAction>

class BitmapImage;
class VectorImage;
class Camera;
class SoundClip;
class KeyFrame;
class LegacyBackupElement;
class BackupElement;

enum class BackupType {
    STROKE,
    POLYLINE,
    SELECTION,
};

class BackupManager : public BaseManager
{
    Q_OBJECT

public:
    explicit BackupManager(Editor* editor);
    ~BackupManager();

    bool init() override;
    Status load(Object*) override;
    Status save(Object*) override;

    void backup(BackupType backupType);

    bool hasUnsavedChanges() const;
    void saveStates();

    QAction* createUndoAction(QObject* parent, const QString& description, const QIcon& icon);
    QAction* createRedoAction(QObject* parent, const QString& description, const QIcon& icon);

    void updateUndoAction(QAction* undoAction);
    void updateRedoAction(QAction* redoAction);

    const BackupElement* latestBackupElement() const;

    void clearStack();

    // Legacy System

    void legacyBackup(const QString& undoText);
    bool legacyBackup(int backupLayer, int backupFrame, const QString& undoText);
    void sanitizeLegacyBackupElementsAfterLayerDeletion(int layerIndex);
    void restoreLegacyKey();

    void rememberLastModifiedFrame(int layerNumber, int frameNumber);

    void pushCommand(QUndoCommand* command);

Q_SIGNALS:
    void didUpdateUndoStack();

private: // functions

    void bitmap(const QString& description);
    void vector(const QString& description);
    void selection(const QString& description);

    void legacyUndo();
    void legacyRedo();

    bool newUndoRedoSystemEnabled() const;

    void restoreKey(const BackupElement* element);
    void restoreLayerKeys(const BackupElement* element);

private: // variables

    QUndoStack* mUndoStack = nullptr;

    int mLayerId = 0;
    int mFrameIndex = 0;
    int mLayerIndex = 0;

    qreal mViewRotation = 0;
    qreal mViewScale = 0;
    QPointF mViewTranslation;

    QString mLayerName;

    Layer* mLayer = nullptr;
    BitmapImage* mBitmap = nullptr;
    VectorImage* mVector = nullptr;
    SoundClip* mClip = nullptr;
    Camera* mCamera = nullptr;
    KeyFrame* mKeyframe = nullptr;

    QList<int> mFrameIndexes;

    bool mIsSelected = false;

    VectorSelection mVectorSelection;

    QRectF mSelectionRect = QRectF();
    qreal mSelectionRotationAngle = 0.0;
    qreal mSelectionScaleX = 0.0;
    qreal mSelectionScaleY = 0.0;
    QPointF mSelectionTranslation;
    QPointF mSelectionAnchor;

    MoveMode mMoveMode;

    Layer::LAYER_TYPE mLayerType;

    DrawOnEmptyFrameAction mEmptyFrameSettingVal;
    const BackupElement* mBackupAtSave = nullptr;

    // Legacy system
    int mLegacyBackupIndex;
    LegacyBackupElement* mLegacyBackupAtSave = nullptr;
    QList<LegacyBackupElement*> mLegacyBackupList;

    int mLegacyLastModifiedLayer = -1;
    int mLegacyLastModifiedFrame = -1;

    bool mNewBackupSystemEnabled = false;

};

#endif // BACKUPMANAGER_H