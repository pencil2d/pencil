/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2017 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "keyframefactory.h"
#include "bitmapimage.h"
#include "vectorimage.h"
#include "soundclip.h"
#include "camera.h"

KeyFrame *KeyFrameFactory::create( Layer::LAYER_TYPE eType, Object* obj )
{
    switch( eType )
    {
    case Layer::BITMAP:
        return new BitmapImage;
    case Layer::VECTOR:
    {
        VectorImage* v = new VectorImage;
        v->setObject( obj );
        return v;
    }
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
