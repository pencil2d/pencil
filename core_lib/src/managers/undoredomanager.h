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
#ifndef UNDOREDOMANAGER_H
#define UNDOREDOMANAGER_H

#include "basemanager.h"
#include "layer.h"
#include "keyframe.h"

#include "preferencesdef.h"

#include <QUndoStack>
#include <QRectF>

class QAction;
class QUndoCommand;

class BitmapImage;
class VectorImage;
class Camera;
class SoundClip;
class KeyFrame;
class LegacyBackupElement;
class UndoRedoCommand;

enum class UndoRedoType {
    STROKE,
    POLYLINE,
    SELECTION,
};

struct UndoSaveState {

    int layerId = 0;
    Layer::LAYER_TYPE layerType = Layer::UNDEFINED;
    std::shared_ptr<KeyFrame> keyframe = nullptr;

    QRectF  selectionRect;
    qreal   selectionRotationAngle = 0.0;
    qreal   selectionScaleX = 0.0;
    qreal   selectionScaleY = 0.0;
    QPointF selectionTranslation;
    QPointF selectionAnchor;

    bool invalidated = false;
};

class UndoRedoManager : public BaseManager
{
    Q_OBJECT

public:
    explicit UndoRedoManager(Editor* editor);
    ~UndoRedoManager() override;

    bool init() override;
    Status load(Object*) override;
    Status save(Object*) override;

    /**
     * Adds a undo/redo state of the given UndoRedoType
     * @param undoRedoType The type to add
    */
    void add(UndoSaveState& undoSaveState, UndoRedoType undoRedoType);

    bool hasUnsavedChanges() const;

    /**
     * @brief UndoRedoManager::saveStates
     * This method should be called prior to a backup taking place.
     * Only the most essential values should be retrieved here.
     * @return A struct with the most recent state
     */
    UndoSaveState saveStates() const;

    QAction* createUndoAction(QObject* parent, const QIcon& icon);
    QAction* createRedoAction(QObject* parent, const QIcon& icon);

    void updateUndoAction(QAction* undoAction);
    void updateRedoAction(QAction* redoAction);

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

    void onSettingChanged(SETTING setting);

signals:
    void didUpdateUndoStack();

private:

    // functions

    void invalidateSaveState(UndoSaveState& undoSaveState);

    void bitmap(const UndoSaveState& undoState, const QString& description);
    void vector(const UndoSaveState& undoState, const QString& description);
    void selection(const UndoSaveState& undoState, const QString& description);

    void pushCommand(QUndoCommand* command);

    void legacyUndo();
    void legacyRedo();

    // variables

    QUndoStack mUndoStack;

    // Legacy system
    int mLegacyBackupIndex = -1;
    LegacyBackupElement* mLegacyBackupAtSave = nullptr;
    QList<LegacyBackupElement*> mLegacyBackupList;

    int mLegacyLastModifiedLayer = -1;
    int mLegacyLastModifiedFrame = -1;

    bool mNewBackupSystemEnabled = false;
};

#endif // UNDOREDOMANAGER_H
