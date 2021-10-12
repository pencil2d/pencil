/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef CAMERAFIELDOPTION_H
#define CAMERAFIELDOPTION_H

enum class CameraFieldOption {
    RESET_FIELD,
    RESET_TRANSLATION,
    RESET_SCALING,
    RESET_ROTATION,
    ALIGN_HORIZONTAL,
    ALIGN_VERTICAL,
    HOLD_FRAME,
    SHOW_CAMERA_PATH
};

#endif // CAMERAFIELDOPTION_H
