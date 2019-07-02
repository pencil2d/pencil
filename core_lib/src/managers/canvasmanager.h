#ifndef CANVASMANAGER_H
#define CANVASMANAGER_H

#include "basemanager.h"

class Layer;
class KeyFrame;
class CanvasPainter;

class CanvasManager : public BaseManager
{
public:
    explicit CanvasManager(Editor* editor);
    ~CanvasManager() override;

    bool init() override;
    Status load(Object* o) override;
    Status save(Object* o) override;

    void paintTransformedSelection(Layer* layer,
                                   KeyFrame* keyframe,
                                   const QTransform& selectionTransform,
                                   const QRectF& selectionRect);
    void applyTransformedSelection(Layer* layer,
                                   KeyFrame* keyframe,
                                   const QTransform& selectionTransform,
                                   const QRectF& selectionRect);

    void cancelTransformedSelection(Layer* layer,
                                    KeyFrame* keyframe);

    void ignoreTransformedSelection();

    CanvasPainter* canvasPainter() { return mCanvasPainter; }

private:
    Editor* mEditor = nullptr;
    CanvasPainter* mCanvasPainter = nullptr;
};

#endif // CANVASMANAGER_H
