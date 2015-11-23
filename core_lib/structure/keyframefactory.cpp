#include "keyframefactory.h"
#include "bitmapimage.h"
#include "vectorimage.h"
#include "soundclip.h"
#include "camera.h"

KeyFrame *KeyFrameFactory::create( Layer::LAYER_TYPE eType )
{
    switch( eType )
    {
    case Layer::BITMAP:
        return new BitmapImage;
    case Layer::VECTOR:
        return new VectorImage;
    case Layer::SOUND:
        return new SoundClip;
    case Layer::CAMERA:
        return new Camera;
    case Layer::MOVIE:
    case Layer::UNDEFINED:
        Q_ASSERT( false && "Not supported layer type." );
        return nullptr;
    }
    Q_ASSERT( false );
    return nullptr;
}
