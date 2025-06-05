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
#ifndef BUTTONAPPEARANCEWATCHER_H
#define BUTTONAPPEARANCEWATCHER_H

enum class AppearanceEventType
{
    NONE,
    LIGHT_APPEARANCE,
    DARK_APPEARANCE,
    ICON_NORMAL,
    ICON_ACTIVE
};

#include "appearance.h"

class ButtonAppearanceWatcher : public QObject
{
    Q_OBJECT

public:
    explicit ButtonAppearanceWatcher(IconResource normalIconResource,
                                     IconResource hoverIconResource,
                                     QObject * parent = nullptr);
    virtual bool eventFilter(QObject * watched, QEvent * event) override;

private:
    bool shouldUpdateResource(QEvent* event, AppearanceEventType appearanceType) const;
    AppearanceEventType determineAppearanceEvent(QEvent* event) const;

    const IconResource mNormalIconResource;
    const IconResource mHoverIconResource;

    AppearanceEventType mOldAppearanceType = AppearanceEventType::NONE;
};

#endif // BUTTONAPPEARANCEWATCHER_H
