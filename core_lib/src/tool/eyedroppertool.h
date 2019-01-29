/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef EYEDROPPERTOOL_H
#define EYEDROPPERTOOL_H

#include "basetool.h"


class EyedropperTool : public BaseTool
{
    Q_OBJECT
public:
    explicit EyedropperTool( QObject* parent = 0 );
    ToolType type() override { return EYEDROPPER; }
    void loadSettings() override;
    QCursor cursor() override;
    QCursor cursor( const QColor colour );

    void pointerPressEvent( PointerEvent* ) override;
    void pointerReleaseEvent( PointerEvent* event ) override;
    void pointerMoveEvent( PointerEvent* ) override;

    /** Updates front color for bitmap and color index for vector */
    void updateFrontColor();
};

#endif // EYEDROPPERTOOL_H
