#include "keyframemanager.h"

#include "editor.h"

#include "vectorimage.h"
#include "bitmapimage.h"
#include "layer.h"
#include "layerbitmap.h"
#include "layervector.h"

KeyFrameManager::KeyFrameManager(Editor* editor) : BaseManager(editor), mEditor(editor)
{

}

KeyFrameManager::~KeyFrameManager()
{
    // TODO: cleanup stuff here...
}

bool KeyFrameManager::init()
{
    return true;
}

Status KeyFrameManager::load(Object* o)
{
    Q_UNUSED(o);
    return Status::OK;
}

Status KeyFrameManager::save(Object* o)
{
    Q_UNUSED(o);
    return Status::OK;
}

KeyFrame* KeyFrameManager::currentKeyFrame(Layer* layer) const
{
//    Q_ASSERT(layer->type() == Layer::Undefined);
    return layer->getLastKeyFrameAtPosition(mEditor->currentFrame());
}

BitmapImage* KeyFrameManager::currentBitmapImage(Layer* layer) const
{
    Q_ASSERT(layer->type() == Layer::BITMAP);
    auto bitmapLayer = static_cast<LayerBitmap*>(layer);
    return bitmapLayer->getLastBitmapImageAtFrame(mEditor->currentFrame());
}

VectorImage* KeyFrameManager::currentVectorImage(Layer* layer) const
{
    Q_ASSERT(layer->type() == Layer::VECTOR);
    auto vectorLayer = (static_cast<LayerVector*>(layer));
    return vectorLayer->getLastVectorImageAtFrame(mEditor->currentFrame(), 0);
}
