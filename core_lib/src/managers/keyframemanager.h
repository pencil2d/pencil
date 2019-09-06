#ifndef KEYFRAMEMANAGER_H
#define KEYFRAMEMANAGER_H

#include "basemanager.h"

class Layer;
class BitmapImage;
class VectorImage;
class KeyFrame;

class KeyFrameManager : public BaseManager
{
    Q_OBJECT
public:
    explicit KeyFrameManager(Editor* editor);
    ~KeyFrameManager() override;

    bool init() override;
    Status load(Object* o) override;
    Status save(Object* o) override;

    KeyFrame* currentKeyFrame(Layer* layer) const;
    BitmapImage* currentBitmapImage(Layer* layer) const;
    VectorImage* currentVectorImage(Layer* layer) const;

private:
    Editor* mEditor;
};

#endif // KEYFRAMEMANAGER_H
