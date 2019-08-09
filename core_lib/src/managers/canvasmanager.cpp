#include "canvasmanager.h"

#include "layer.h"

#include "bitmapimage.h"
#include "vectorimage.h"
#include "canvaspainter.h"

CanvasManager::CanvasManager(Editor* editor) : BaseManager(editor), mEditor(editor)
{
    mCanvasPainter = new CanvasPainter();
}

CanvasManager::~CanvasManager()
{
    // TODO: cleanup stuff..
}

bool CanvasManager::init()
{
    return true;
}

Status CanvasManager::load(Object* o)
{
    Q_UNUSED(o);
    return Status::OK;
}

Status CanvasManager::save(Object* o)
{
    Q_UNUSED(o);
    return Status::OK;
}

void CanvasManager::paintTransformedSelection(Layer* layer, KeyFrame* keyframe,
                                              const QTransform& selectionTransform,
                                              const QRectF& selectionRect)
{
    if (layer->type() == Layer::BITMAP)
    {
        mCanvasPainter->setTransformedSelection(selectionRect.toRect(), selectionTransform);
    }
    else if (layer->type() == Layer::VECTOR)
    {
        // vector transformation
        VectorImage* vectorImage = static_cast<VectorImage*>(keyframe);
        vectorImage->setSelectionRect(selectionRect);
        vectorImage->setSelectionTransformation(selectionTransform);

    }

    emit needPaint();
}

void CanvasManager::applyTransformedSelection(Layer* layer,
                                              KeyFrame* keyframe,
                                              const QTransform& selectionTransform,
                                              const QRectF& selectionRect)
{
    mCanvasPainter->ignoreTransformedSelection();

    if (selectionRect.isEmpty()) { return; }

    if (layer->type() == Layer::BITMAP)
    {
        QRect selectionRectAligned = selectionRect.toRect();
        BitmapImage* bitmapImage = static_cast<BitmapImage*>(keyframe);
        BitmapImage transformedImage = bitmapImage->transformed(selectionRectAligned, selectionTransform, true);

        bitmapImage->clear(selectionRectAligned);
        bitmapImage->paste(&transformedImage, QPainter::CompositionMode_SourceOver);
    }
    else if (layer->type() == Layer::VECTOR)
    {
        VectorImage* vectorImage = static_cast<VectorImage*>(keyframe);
        vectorImage->applySelectionTransformation();

    }

    emit needPaint();
}

void CanvasManager::ignoreTransformedSelection()
{
    mCanvasPainter->ignoreTransformedSelection();
}

void CanvasManager::cancelTransformedSelection(Layer* layer, KeyFrame* keyframe)
{
    if (layer == nullptr) { return; }

    if (layer->type() == Layer::VECTOR) {

        VectorImage* vectorImage = static_cast<VectorImage*>(keyframe);
        vectorImage->setSelectionTransformation(QTransform());
    }
}
