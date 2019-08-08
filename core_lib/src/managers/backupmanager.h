#ifndef BACKUPMANAGER_H
#define BACKUPMANAGER_H

#include <QUndoStack>
#include <QUndoView>
#include "basemanager.h"
#include "preferencemanager.h"
#include "layer.h"
#include "direction.h"
#include "movemode.h"

class BitmapImage;
class VectorImage;
class Camera;
class SoundClip;
class KeyFrame;
class BackupElement;

class BackupManager : public BaseManager
{
    Q_OBJECT

    friend class RemoveKeyFrameElement;
    friend class AddKeyFrameElement;
    friend class DeleteLayerElement;

public:
    explicit BackupManager(Editor* editor);
    ~BackupManager();

    bool init() override;
    Status load(Object*) override;
    Status save(Object*) override;

    void keyAdded(const int& keySpacing, const bool& keyExisted, const QString& description);
    void keyAdded(const QString& description = "");
    void keyRemoved();
    void bitmap(const QString& description);
    void vector(const QString& description);
    void cameraMotion();
    void layerAdded();
    void layerDeleted(const std::map<int, KeyFrame*, std::greater<int>>& oldKeys);
    void layerRenamed();
    void layerMoved(const int& backupNewLayerIndex);

    void importBitmap(const std::map<int, KeyFrame*, std::greater<int>>& canvasKeys,
                      const std::map<int, KeyFrame*, std::less<int>>& importedKeys);
    void selection();
    void deselect();
    void transform();
    void cameraProperties(const QRect& backupViewRect);
    void frameDragged(const int& backupFrameOffset);
    void flipView(const bool& backupIsFlipped, const DIRECTION& backupFlipDirection);
    void toggleSetting(bool backupToggleState, const SETTING& backupType);
    void saveStates();

    void restoreKey(const int& layerId, const int& frame, KeyFrame* keyFrame);

    static int getActiveFrameIndex(Layer* layer, const int& frameIndex, const bool& usingPreviousFrameAction);

    const BackupElement* currentBackup();

    QUndoStack* undoStack() { return mUndoStack; }

Q_SIGNALS:
    void updateBackup();

private:
    void restoreKey(const BackupElement* element);
    void restoreLayerKeys(const BackupElement* element);

    QUndoStack* mUndoStack;

    int mLayerId = 0;
    int mFrameIndex = 0;
    int mLayerIndex = 0;

    float mRotation = 0;
    float mScale = 0;

    QString mLayerName;

    Layer* mLayer = nullptr;
    BitmapImage* mBitmap = nullptr;
    VectorImage* mVector = nullptr;
    SoundClip* mClip = nullptr;
    Camera* mCamera = nullptr;
    KeyFrame* mKeyframe = nullptr;

    bool mIsSelected = false;

    QRectF mSelectionRect = QRectF();
    QRectF mTempSelectionRect = QRectF();
    QRectF mTransformedSelectionRect = QRectF();

    qreal mSelectionRotationAngle = 0.0;

    QPointF mTranslation = QPointF(0,0);
    QPointF mMoveOffset = QPointF(0,0);
    QTransform mSelectionTransform;

    MoveMode mMoveMode;

    Layer::LAYER_TYPE mLayerType;

};

#endif // BACKUPMANAGER_H
