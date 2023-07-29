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

#ifndef ONIONSKINPAINTEROPTIONS_H
#define ONIONSKINPAINTEROPTIONS_H

struct OnionSkinPainterOptions {
    bool enabledWhilePlaying = false;
    bool isPlaying = false;
    float minOpacity = 0;
    float maxOpacity = 100;
    bool skinPrevFrames = false;
    bool skinNextFrames = false;
    bool colorizePrevFrames = false;
    bool colorizeNextFrames = false;
    int framesToSkinPrev = 0;
    int framesToSkinNext = 0;
    bool isAbsolute = false;
};
#endif // ONIONSKINPAINTEROPTIONS_H
