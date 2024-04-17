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

#include "basemanager.h"
#include "layer.h"

#include <QRectF>

class QAction;
class QUndoCommand;
class QUndoStack;

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
    ~BackupManager() override;

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
    /**
     * Restores integrity of the backup elements after a layer has been deleted.
     * Removes backup elements affecting the deleted layer and adjusts the layer
     * index on other backup elements as necessary.
     *
     * @param layerIndex The index of the layer that was deleted
     *
     * @warning This serves as a temporary hack to prevent crashes until #864 is
     *          done (see #1412).
     */
    void sanitizeLegacyBackupElementsAfterLayerDeletion(int layerIndex);
    void restoreLegacyKey();

    void rememberLastModifiedFrame(int layerNumber, int frameNumber);

signals:
    void didUpdateUndoStack();

private:

    // functions

    void bitmap(const QString& description);
    void vector(const QString& description);
    void selection(const QString& description);

    void pushCommand(QUndoCommand* command);

    void legacyUndo();
    void legacyRedo();

    // variables

    QUndoStack* mUndoStack = nullptr;

    int mLayerId = 0;

    Layer::LAYER_TYPE mType = Layer::UNDEFINED;
    KeyFrame* mKeyframe = nullptr;

    QRectF mSelectionRect = QRectF();
    qreal mSelectionRotationAngle = 0.0;
    qreal mSelectionScaleX = 0.0;
    qreal mSelectionScaleY = 0.0;
    QPointF mSelectionTranslation;
    QPointF mSelectionAnchor;

    // Legacy system
    int mLegacyBackupIndex = -1;
    LegacyBackupElement* mLegacyBackupAtSave = nullptr;
    QList<LegacyBackupElement*> mLegacyBackupList;

    int mLegacyLastModifiedLayer = -1;
    int mLegacyLastModifiedFrame = -1;

    bool mNewBackupSystemEnabled = false;

};

#endif // BACKUPMANAGER_H
