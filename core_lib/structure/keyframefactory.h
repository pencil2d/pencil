#ifndef KEYFRAMEFACTORY_H
#define KEYFRAMEFACTORY_H

#include "layer.h"
class Object;

class KeyFrameFactory
{
public:
    static KeyFrame* create( Layer::LAYER_TYPE, Object* );
};

#endif // KEYFRAMEFACTORY_H
