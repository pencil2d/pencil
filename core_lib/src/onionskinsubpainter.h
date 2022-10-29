/*

Pencil2D - Traditional Animation Software
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef ONIONSKINSUBPAINTER_H
#define ONIONSKINSUBPAINTER_H

#include <functional>

#include "onionskinpaintstate.h"

class Layer;
struct OnionSkinPainterOptions;
class QPainter;

/// A Sub Painter class is meant to be used in existing painter classes
/// It does not and should not cary the same responsibility as a Painter class
/// It works as a sort of utility class for functionality that makes sense to reuse
/// Eg. OnionSkinSubPainter is used by CanvasPainter and CameraPainter
class OnionSkinSubPainter
{
public:
    explicit OnionSkinSubPainter();
    void paint(QPainter& painter, const Layer* layer, const OnionSkinPainterOptions& options, int frameIndex, const std::function<void(OnionSkinPaintState, int)>& state) const;
};

#endif // ONIONSKINSUBPAINTER_H
