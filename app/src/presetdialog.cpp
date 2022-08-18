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
#include "presetdialog.h"
#include "ui_presetdialog.h"
#include "app_util.h"

#include <QStandardPaths>
#include <QDir>
#include <QSettings>


PresetDialog::PresetDialog(PreferenceManager* preferences, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::PresetDialog),
    mPrefs(preferences)
{
    ui->setupUi(this);

    initPresets();
    hideQuestionMark(*this);
}

PresetDialog::~PresetDialog()
{
    delete ui;
}

int PresetDialog::getPresetIndex()
{
    bool ok = true;
    int index = ui->presetComboBox->currentData().toInt(&ok);
    Q_ASSERT(ok);
    return index;
}

bool PresetDialog::shouldAlwaysUse()
{
    return ui->alwaysUse->isChecked();
}

QString PresetDialog::getPresetPath(int index)
{
    if (index == 0)
    {
        return QString();
    }

    const QString filename = QString("%1.pclx").arg(index);
    QDir dataDir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if (dataDir.cd("presets"))
    {
        return dataDir.filePath(filename);
    }
    return QString();
}

void PresetDialog::initPresets()
{
    // Make sure the presets directory in the data directory exists and navigate to it
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dataDir(dataPath);
    dataDir.mkdir("presets");
    if (dataDir.cd("presets") == false)
    {
        reject(); // the presets folder doesn't exist and cannot be created
        return;
    }

    // Find all presets in the preferences and add them to the combo box
    int defaultIndex = mPrefs->getInt(SETTING::DEFAULT_PRESET);
    ui->presetComboBox->addItem("Default", 0);
    ui->presetComboBox->setCurrentIndex(0);

    if (!dataDir.exists("presets.ini"))
    {
        reject();
        return;
    }
    QSettings presets(dataDir.filePath("presets.ini"), QSettings::IniFormat, this);

    bool ok = true;
    for (const QString& key : presets.allKeys())
    {
        int index = key.toInt(&ok);
        if (!ok || index == 0 || !dataDir.exists(QString("%1.pclx").arg(index))) continue;

        QString name = presets.value(key, QString()).toString();
        if (name.isEmpty()) continue;
        ui->presetComboBox->addItem(name, index);
        if (index == defaultIndex)
        {
            ui->presetComboBox->setCurrentIndex(ui->presetComboBox->count()-1);
        }
    }

    ui->presetComboBox->model()->sort(0);
}
