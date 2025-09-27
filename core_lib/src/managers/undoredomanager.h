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
#include <QMap>

class QAction;
class QUndoCommand;

class BitmapImage;
class VectorImage;
class Camera;
class SoundClip;
class KeyFrame;
class LegacyBackupElement;
class UndoRedoCommand;

typedef int SAVESTATE_ID;

/// The undo/redo type which correspond to what is being recorded
enum class UndoRedoRecordType {
    KEYFRAME_MODIFY, // Any modification that involve a keyframe
    KEYFRAME_REMOVE, // Removing a keyframe
    KEYFRAME_ADD, // Adding a keyframe
    KEYFRAME_MOVE,
    // SCRUB_LAYER, // Scrubbing layer
    // SCRUB_KEYFRAME, // Scrubbing keyframe
    INVALID
};

struct SelectionSaveState {

    SelectionSaveState() { }
    SelectionSaveState(const QRectF& rect,
                       const qreal rotationAngle,
                       const qreal scaleX,
                       const qreal scaleY,
                       const QPointF& translation,
                       const QPointF& anchor)
    {
        this->bounds = rect;
        this->rotationAngle = rotationAngle;
        this->scaleX = scaleX;
        this->scaleY = scaleY;
        this->translation = translation;
        this->anchor = anchor;
    }

    QRectF  bounds;
    qreal   rotationAngle = 0.0;
    qreal   scaleX = 0.0;
    qreal   scaleY = 0.0;
    QPointF translation;
    QPointF anchor;
};

struct MoveFramesSaveState {

    MoveFramesSaveState() { }
    MoveFramesSaveState(int offset,
                        const QList<int>& positions)
    {
        this->offset = offset;
        this->positions = positions;
    }

    int offset = 0;
    QList<int> positions;
};

/// Use this struct to store user related data that will later be added to the backup
/// This struct is meant to be safely shared and stored temporarily,
/// as such don't store ptrs here...
/// All data stored in here should be based on ZII (zero is initialization) principle
/// Only store what you need.
struct UserSaveState {
    MoveFramesSaveState moveFramesState = {};
};

/// This is the main undo/redo state structure which is meant to populate
/// whatever states that needs to be stored temporarily.
struct UndoSaveState {

    ~UndoSaveState()
    {
        if (recordType != UndoRedoRecordType::INVALID) {
            keyframe.reset();
        }
    }

    // Common data
    UndoRedoRecordType recordType = UndoRedoRecordType::INVALID;
    int layerId = 0;
    int currentFrameIndex = 0;
    Layer::LAYER_TYPE layerType = Layer::UNDEFINED;
    std::unique_ptr<KeyFrame> keyframe;
    SelectionSaveState selectionState = {};

    UserSaveState userState = {};
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

    /** Records the given save state.
     *  The input save state is cleaned up and set to nullptr after use.
    * @param SaveStateId The state that will be fetched and recorded based on the input SaveStateId.
    * @param description The description that will bound to the undo/redo action.
    */
    void record(SAVESTATE_ID SaveStateId, const QString& description);


    /** Checks whether there are unsaved changes.
     *  @return true if there are unsaved changes, otherwise false */
    bool hasUnsavedChanges() const;

    /** Prepares and returns an save state with common data
     * @return A UndoSaveState struct with common keyframe data */
    SAVESTATE_ID createState(UndoRedoRecordType recordType);

    /** Adds userState to the saveState found at SaveStateId
     *  If no record is found matching the id, nothing happens.
     *  @param SaveStateId The id used to fetch the saveState
     *  @param userState The data to be inserted onto on the saveState
     */
    void addUserState(SAVESTATE_ID SaveStateId, UserSaveState userState);

    QAction* createUndoAction(QObject* parent, const QIcon& icon);
    QAction* createRedoAction(QObject* parent, const QIcon& icon);

    void updateUndoAction(QAction* undoAction);
    void updateRedoAction(QAction* redoAction);

    /** Clears the undo stack */
    void clearStack();

    // Developer note:
    // Our legacy undo/redo system is not meant to be build upon anymore.
    // The implementation should however be kept until the new undo/redo system takes over.

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

    void replaceKeyFrame(const UndoSaveState& undoState, const QString& description);
    void replaceBitmap(const UndoSaveState& undoState, const QString& description);
    void replaceVector(const UndoSaveState& undoState, const QString& description);

    void addKeyFrame(const UndoSaveState& undoState, const QString& description);
    void removeKeyFrame(const UndoSaveState& undoState, const QString& description);
    void moveKeyFrames(const UndoSaveState& undoState, const QString& description);

    void initCommonKeyFrameState(UndoSaveState* undoSaveState) const;

    void pushCommand(QUndoCommand* command);

    void clearState(UndoSaveState*& state);
    void clearSaveStates();

    void legacyUndo();
    void legacyRedo();

    QUndoStack mUndoStack;

    QMap<SAVESTATE_ID, UndoSaveState*> mSaveStates;

    // Legacy system
    int mLegacyBackupIndex = -1;
    LegacyBackupElement* mLegacyBackupAtSave = nullptr;
    QList<LegacyBackupElement*> mLegacyBackupList;

    int mLegacyLastModifiedLayer = -1;
    int mLegacyLastModifiedFrame = -1;

    SAVESTATE_ID mSaveStateId = 1;

    bool mNewBackupSystemEnabled = false;
};

#endif // UNDOREDOMANAGER_H
