#include "layermultiplanecamera.h"

LayerMultiPlaneCamera::LayerMultiPlaneCamera(Object *object) :Layer(object, LAYER_TYPE::MULTIPLANCAMERA)
{
    setName(tr("Multiplane Camera"));
}
