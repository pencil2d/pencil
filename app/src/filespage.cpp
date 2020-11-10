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

#include "filespage.h"

#include <QSettings>
#include <QStandardPaths>

#include "errordialog.h"
#include "filemanager.h"
#include "pencilerror.h"
#include "presetdialog.h"

#include "ui_filespage.h"

FilesPage::FilesPage()
    : ui(new Ui::FilesPage)
{
    ui->setupUi(this);

    initPreset();

    connect(ui->addPreset, &QPushButton::clicked, this, &FilesPage::addPreset);
    connect(ui->removePreset, &QPushButton::clicked, this, &FilesPage::removePreset);
    connect(ui->setDefaultPreset, &QPushButton::clicked, this, &FilesPage::setDefaultPreset);
    connect(ui->askPresetRbtn, &QRadioButton::toggled, this, &FilesPage::askForPresetChange);
    connect(ui->loadLastActiveRbtn, &QRadioButton::toggled, this, &FilesPage::loadMostRecentChange);
    connect(ui->loadDefaultPresetRbtn, &QRadioButton::toggled, this, &FilesPage::loadDefaultPreset);
    connect(ui->presetListWidget, &QListWidget::itemChanged, this, &FilesPage::presetNameChanged);

    auto spinBoxValueChange = static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged);
    connect(ui->autosaveCheckBox, &QCheckBox::stateChanged, this, &FilesPage::autoSaveChange);
    connect(ui->autosaveNumberBox, spinBoxValueChange, this, &FilesPage::autoSaveNumberChange);
}

FilesPage::~FilesPage()
{
    delete ui;
}

void FilesPage::initPreset()
{
    mPresetDir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    mPresetDir.mkpath("presets");
    mPresetDir.cd("presets");

    mPresetSettings = new QSettings(mPresetDir.filePath("presets.ini"), QSettings::IniFormat, this);

    QListWidgetItem* defaultItem = new QListWidgetItem("Blank");
    defaultItem->setData(Qt::UserRole, 0);
    ui->presetListWidget->addItem(defaultItem);

    bool ok = true;
    for (const QString& key : mPresetSettings->allKeys())
    {
        int index = key.toInt(&ok);
        if (!ok || index == 0 || !mPresetDir.exists(QString("%1.pclx").arg(index))) continue;

        mMaxPresetIndex = qMax(index, mMaxPresetIndex);

        QString name = mPresetSettings->value(key).toString();
        if (name.isEmpty())
            continue;

        QListWidgetItem* item = new QListWidgetItem(name);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setData(Qt::UserRole, index);
        ui->presetListWidget->addItem(item);
    }
}

void FilesPage::addPreset()
{
    int newPresetIndex = mMaxPresetIndex + 1;

    // 1. save the current object to the preset folder
    FileManager fm(this);
    Status st = fm.save(mManager->object(), PresetDialog::getPresetPath(newPresetIndex));
    if (!st.ok())
    {
        ErrorDialog errorDialog(st.title(),
                                st.description().append(tr("<br><br>Error: your file may not have saved successfully."
                                                           "If you believe that this error is an issue with Pencil2D, please create a new issue at:"
                                                           "<br><a href='https://github.com/pencil2d/pencil/issues'>https://github.com/pencil2d/pencil/issues</a><br>"
                                                           "Please include the following details in your issue:")), st.details().html());
        errorDialog.exec();
        return;
    }

    // 2. update the preset ini
    QString presetName = QString("Preset %1").arg(newPresetIndex);
    mPresetSettings->setValue(QString::number(newPresetIndex), presetName);
    mMaxPresetIndex = newPresetIndex;

    // 3. update the list widget
    QListWidgetItem* newItem = new QListWidgetItem(presetName);
    newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
    newItem->setData(Qt::UserRole, newPresetIndex);
    ui->presetListWidget->addItem(newItem);

    ui->presetListWidget->scrollToBottom();
    ui->presetListWidget->editItem(newItem);
}

void FilesPage::removePreset()
{
    if (ui->presetListWidget->count() <= 1) { return; }
    if (ui->presetListWidget->selectedItems().empty()) { return; }

    // 1. Remove the items from list widget
    QList<QListWidgetItem*> itemsToRemove = ui->presetListWidget->selectedItems();
    for (QListWidgetItem* item : itemsToRemove)
    {
        ui->presetListWidget->removeItemWidget(item);
    }

    // 2. Delete preset pclx files
    for (QListWidgetItem* item : itemsToRemove)
    {
        int index = item->data(Qt::UserRole).toInt();
        QFile presetFile(PresetDialog::getPresetPath(index));
        presetFile.remove();
    }

    // 3. Delete items from the ini settings
    for (QListWidgetItem* item : itemsToRemove)
    {
        int index = item->data(Qt::UserRole).toInt();
        mPresetSettings->remove(QString::number(index));
    }

    // 4. check if the default preset has been deleted
    int prevDefaultIndex = mManager->getInt(SETTING::DEFAULT_PRESET);
    for (QListWidgetItem* item : itemsToRemove)
    {
        int index = item->data(Qt::UserRole).toInt();
        if (index == prevDefaultIndex)
        {
            mManager->set(SETTING::DEFAULT_PRESET, 0);
        }
    }

    // 5. delete items
    for (QListWidgetItem* item : itemsToRemove)
    {
        delete item;
    }
    updateValues();
}

void FilesPage::setDefaultPreset()
{
    bool ok = true;

    QListWidgetItem* newDefaultPresetItem = ui->presetListWidget->currentItem();
    if (newDefaultPresetItem)
    {
        int newDefaultIndex = newDefaultPresetItem->data(Qt::UserRole).toInt(&ok);
        Q_ASSERT(ok);

        mManager->set(SETTING::DEFAULT_PRESET, newDefaultIndex);
        updateValues();
    }
}

void FilesPage::presetNameChanged(QListWidgetItem* item)
{
    // Remove characters that may be problematic for ini files
    item->setText(item->text().remove(QChar('@')).remove(QChar('/')).remove(QChar('\\')));

    bool ok = true;
    int index = item->data(Qt::UserRole).toInt(&ok);
    Q_ASSERT(ok);
    mPresetSettings->setValue(QString::number(index), item->text());
}

void FilesPage::updateValues()
{
    bool ok = true;
    int defaultPresetIndex = mManager->getInt(SETTING::DEFAULT_PRESET);

    for (int i = 0; i < ui->presetListWidget->count(); i++)
    {
        QListWidgetItem* item = ui->presetListWidget->item(i);
        int presetIndex = item->data(Qt::UserRole).toInt(&ok);

        bool isDefault = presetIndex == defaultPresetIndex;

        QFont font = item->font();
        font.setBold(isDefault); // Bold text for the default item
        item->setFont(font);

        QBrush backgroundBrush = (isDefault) ? palette().light() : palette().window();
        item->setBackground(backgroundBrush);
    }
    ui->autosaveCheckBox->setChecked(mManager->isOn(SETTING::AUTO_SAVE));
    ui->autosaveNumberBox->setValue(mManager->getInt(SETTING::AUTO_SAVE_NUMBER));
    ui->askPresetRbtn->setChecked(mManager->isOn(SETTING::ASK_FOR_PRESET));
    ui->loadDefaultPresetRbtn->setChecked(mManager->isOn(SETTING::LOAD_DEFAULT_PRESET));
    ui->loadLastActiveRbtn->setChecked(mManager->isOn(SETTING::LOAD_MOST_RECENT));
}

void FilesPage::askForPresetChange(int b)
{
    mManager->set(SETTING::ASK_FOR_PRESET, b != Qt::Unchecked);
}

void FilesPage::loadMostRecentChange(int b)
{
    mManager->set(SETTING::LOAD_MOST_RECENT, b != Qt::Unchecked);
}

void FilesPage::loadDefaultPreset(int b)
{
    mManager->set(SETTING::LOAD_DEFAULT_PRESET, b != Qt::Unchecked);
}

void FilesPage::autoSaveChange(int b)
{
    mManager->set(SETTING::AUTO_SAVE, b != Qt::Unchecked);
}

void FilesPage::autoSaveNumberChange(int number)
{
    mManager->set(SETTING::AUTO_SAVE_NUMBER, number);
}
