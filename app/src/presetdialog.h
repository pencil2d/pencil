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
#ifndef PRESETDIALOG_H
#define PRESETDIALOG_H

#include <QDialog>
#include "preferencemanager.h"

namespace Ui
{
class PresetDialog;
}

class PresetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PresetDialog(PreferenceManager* preferences, QWidget* parent = nullptr);
    ~PresetDialog() override;

    static QString getPresetPath(int index);

    QString getPreset();
    int getPresetIndex();
    bool shouldAlwaysUse();

private:
    void initPresets();

    Ui::PresetDialog* ui;
    PreferenceManager* mPrefs = nullptr;
};

#endif // PRESETDIALOG_H
