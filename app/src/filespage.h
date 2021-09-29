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

#ifndef FILESPAGE_H
#define FILESPAGE_H

#include <QDir>

class PreferenceManager;
class QListWidgetItem;
class QSettings;

namespace Ui {
class FilesPage;
}

class FilesPage : public QWidget
{
Q_OBJECT

public:
    FilesPage();
    ~FilesPage() override;
    void setManager(PreferenceManager* p) { mManager = p; }

public slots:
    void initPreset();
    void addPreset();
    void removePreset();
    void setDefaultPreset();
    void presetNameChanged(QListWidgetItem* item);

    void updateValues();
    void askForPresetChange(int b);
    void loadMostRecentChange(int b);
    void loadDefaultPreset(int b);
    void vectorChanged(int b);
    void autoSaveChange(int b);
    void autoSaveNumberChange(int number);

signals:
    void clearRecentList();

private:
    Ui::FilesPage* ui = nullptr;
    PreferenceManager* mManager = nullptr;
    QSettings* mPresetSettings = nullptr;
    QDir mPresetDir;
    int mMaxPresetIndex = 0;
};

#endif // FILESPAGE_H
