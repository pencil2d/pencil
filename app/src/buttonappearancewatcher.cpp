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
#include "buttonappearancewatcher.h"

#include <QAbstractButton>
#include <QEvent>

#include "platformhandler.h"

ButtonAppearanceWatcher::ButtonAppearanceWatcher(IconResource normalIconResource,
                                                 IconResource hoverIconResource,
                                                 QObject* parent) :
    QObject(parent),
    mNormalIconResource(normalIconResource),
    mHoverIconResource(hoverIconResource)
{}

bool ButtonAppearanceWatcher::eventFilter(QObject* watched, QEvent* event)
{
    QAbstractButton* button = qobject_cast<QAbstractButton*>(watched);
    if (!button) {
        return false;
    }

    IconResource res = mNormalIconResource;
    AppearanceEventType apType = determineAppearanceEvent(event);

    if (shouldUpdateResource(event, apType)) {
        if (event->type() == QEvent::ApplicationPaletteChange) {
            res = mNormalIconResource;
        }
        else if (event->type() == QEvent::Enter) {
            res = mHoverIconResource;
        }
        else if (event->type() == QEvent::Leave) {
            res = mNormalIconResource;
        }
        mOldAppearanceType = apType;

        bool isDarkmode = PlatformHandler::isDarkMode();
        button->setIcon(res.iconForMode(isDarkmode));
        return true;
    }

    return false;
}

AppearanceEventType ButtonAppearanceWatcher::determineAppearanceEvent(QEvent *event) const
{
    if (event->type() == QEvent::ApplicationPaletteChange) {
        bool isDarkmode = PlatformHandler::isDarkMode();
        if (isDarkmode) {
            return AppearanceEventType::DARK_APPEARANCE;
        } else {
            return AppearanceEventType::LIGHT_APPEARANCE;
        }
    } else if (event->type() == QEvent::Enter) {
        return AppearanceEventType::ICON_ACTIVE;
    } else if (event->type() == QEvent::Leave) {
        return AppearanceEventType::ICON_NORMAL;
    }

    return AppearanceEventType::NONE;
}

bool ButtonAppearanceWatcher::shouldUpdateResource(QEvent* event, AppearanceEventType appearanceType) const
{
    if (appearanceType == mOldAppearanceType) { return false; }

    return determineAppearanceEvent(event) != AppearanceEventType::NONE;
}
