#ifndef BACKUPMANAGER_H
#define BACKUPMANAGER_H

#include <QUndoStack>
#include <QUndoView>
#include "basemanager.h"
#include "preferencemanager.h"
#include "layer.h"
#include "direction.h"

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

    void keyAdded(bool isSequence, bool keyExisted, QString description);
    void keyAdded();
    void keyRemoved();
    void bitmap(QString description);
    void vector(QString description);
    void cameraMotion();
    void layerAdded();
    void layerDeleted(std::map<int, KeyFrame*, std::greater<int>> oldKeys);
    void layerRenamed();
    void layerMoved(int backupNewLayerIndex);

    void selection();
    void transform();
    void cameraProperties(QRect backupViewRect);
    void frameDragged(int backupFrameOffset);
    void flipView(bool backupIsFlipped, DIRECTION backupFlipDirection);
    void toggleSetting(bool backupToggleState, SETTING backupType);
    void prepareBackup();

    void restoreKey(int layerId, int frame, KeyFrame* keyFrame);

    const BackupElement* currentBackup();

    QUndoStack* undoStack() { return mUndoStack; }

Q_SIGNALS:
    void updateBackup();

private:
    void restoreKey(BackupElement* element);
    void restoreLayerKeys(BackupElement* element);
    void handleDrawingOnEmptyFrame();

    QUndoStack* mUndoStack;

    int mLayerId = 0;
    int mFrameIndex = 0;
    int mLayerIndex = 0;

    bool mIsSelected = false;

    float mRotation = 0;
    float mScale = 0;

    QString mLayerName;

    Layer* mLayer = nullptr;
    BitmapImage* mBitmap = nullptr;
    VectorImage* mVector = nullptr;
    SoundClip* mClip = nullptr;
    Camera* mCamera = nullptr;
    KeyFrame* mKeyframe = nullptr;

    QRectF mSelectionRect = QRectF();
    QRectF mTempSelection = QRectF();

    QPointF mTranslation = QPointF(0,0);

    Layer::LAYER_TYPE mLayerType;

};

#endif // BACKUPMANAGER_H
