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
#include "app_util.h"
#include <QAction>
#include <QDialog>
#include "preferencemanager.h"

void hideQuestionMark(QDialog& dlg)
{
    dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void setMenuActionChecked(QAction* action, bool checked)
{
    QSignalBlocker b(action);
    action->setChecked(checked);
}

void bindPreferenceSetting(QAction* action, PreferenceManager* prefs, const SETTING& setting)
{
    Q_ASSERT(action->isCheckable());
    Q_ASSERT(prefs);

    QSignalBlocker b(action);
    action->setChecked(prefs->isOn(setting)); // set initial state

    // 2-way binding
    QObject::connect(action, &QAction::triggered, [=](bool b) { prefs->set(setting, b); });
    QObject::connect(prefs, &PreferenceManager::optionChanged, [=](SETTING s)
    {
        if (s == setting)
        {
            action->setChecked(prefs->isOn(setting));
        }
    });
}
