#ifndef KEYFRAMEFACTORY_H
#define KEYFRAMEFACTORY_H

#include "layer.h"


class KeyFrameFactory
{
public:
    static KeyFrame* create( Layer::LAYER_TYPE );
};

#endif // KEYFRAMEFACTORY_H
