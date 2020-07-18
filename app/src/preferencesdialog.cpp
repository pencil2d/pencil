/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2018 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "preferencesdialog.h"

#include <QComboBox>
#include <QMessageBox>
#include <QSlider>
#include <QDir>
#include <QStandardPaths>
#include <QtMath>
#include <QSettings>

#include "ui_preferencesdialog.h"
#include "ui_generalpage.h"
#include "ui_timelinepage.h"
#include "ui_filespage.h"
#include "ui_toolspage.h"
#include "util.h"
#include "filemanager.h"
#include "presetdialog.h"
#include "errordialog.h"


PreferencesDialog::PreferencesDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::init(PreferenceManager* m)
{
    Q_ASSERT(m != nullptr);
    mPrefManager = m;

    ui->general->setManager(mPrefManager);
    ui->general->updateValues();

    ui->filesPage->setManager(mPrefManager);
    ui->filesPage->updateValues();

    ui->timeline->setManager(mPrefManager);
    ui->timeline->updateValues();

    ui->tools->setManager(mPrefManager);
    ui->tools->updateValues();

    ui->shortcuts->setManager(mPrefManager);

    connect(ui->general, &GeneralPage::windowOpacityChange, this, &PreferencesDialog::windowOpacityChange);
    connect(ui->timeline, &TimelinePage::soundScrubChanged, this, &PreferencesDialog::soundScrubChanged);
    connect(ui->timeline, &TimelinePage::soundScrubMsecChanged, this, &PreferencesDialog::soundScrubMsecChanged);
    connect(ui->filesPage, &FilesPage::clearRecentList, this, &PreferencesDialog::clearRecentList);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &PreferencesDialog::close);

    auto onCurrentItemChanged = static_cast<void (QListWidget::*)(QListWidgetItem*, QListWidgetItem*)>(&QListWidget::currentItemChanged);
    connect(ui->contentsWidget, onCurrentItemChanged, this, &PreferencesDialog::changePage);
}

void PreferencesDialog::closeEvent(QCloseEvent*)
{
    done(QDialog::Accepted);
}

void PreferencesDialog::changePage(QListWidgetItem* current, QListWidgetItem* previous)
{
    if (!current)
        current = previous;

    ui->pagesWidget->setCurrentIndex(ui->contentsWidget->row(current));
}

void PreferencesDialog::updateRecentListBtn(bool isEmpty)
{
    if (isEmpty == true)
    {
        emit updateRecentFileListBtn();
    }
}

GeneralPage::GeneralPage() : ui(new Ui::GeneralPage)
{
    ui->setupUi(this);

    QSettings settings(PENCIL2D, PENCIL2D);

    QString languages [][3]
    {
        // translatable string, endonym, locale code
        { tr("Arabic"), QStringLiteral("العربية"), "ar" },
        { tr("Catalan"), QStringLiteral("Català"), "ca" },
        { tr("Czech"), QStringLiteral("Čeština"), "cs" },
        { tr("Danish"), QStringLiteral("Dansk"), "da" },
        { tr("German"), QStringLiteral("Deutsch"), "de" },
        { tr("Greek"), QStringLiteral("Ελληνικά"), "el" },
        { tr("English"), QStringLiteral("English"), "en" },
        { tr("Spanish"), QStringLiteral("Español"), "es" },
        { tr("Estonian"), QStringLiteral("Eesti"), "et" },
        { tr("French"), QStringLiteral("Français"), "fr" },
        { tr("Hebrew"), QStringLiteral("עברית"), "he" },
        { tr("Hungarian"), QStringLiteral("Magyar"), "hu_HU" },
        { tr("Indonesian"), QStringLiteral("Bahasa Indonesia"), "id" },
        { tr("Italian"), QStringLiteral("Italiano"), "it" },
        { tr("Japanese"), QStringLiteral("日本語"), "ja" },
        { tr("Kabyle"), QStringLiteral("Taqbaylit"), "kab" },
        { tr("Polish"), QStringLiteral("Polski"), "pl" },
        { tr("Portuguese \u2013 Portugal"), QStringLiteral("Português \u2013 Portugal"), "pt_PT" },
        { tr("Portuguese \u2013 Brazil"), QStringLiteral("Português \u2013 Brasil"), "pt_BR" },
        { tr("Russian"), QStringLiteral("Русский"), "ru" },
        { tr("Slovene"), QStringLiteral("Slovenščina"), "sl" },
        { tr("Swedish"), QStringLiteral("Svenska"), "sv" },
        { tr("Turkish"), QStringLiteral("Türkçe"), "tr" },
        { tr("Vietnamese"), QStringLiteral("Tiếng Việt"), "vi" },
        { tr("Chinese \u2013 China"), QStringLiteral("简体中文"), "zh_CN" },
        { tr("Chinese \u2013 Taiwan"), QStringLiteral("繁體中文"), "zh_TW" },
    };

    for (auto& lang : languages)
    {
        const QString itemText = QStringLiteral("%1 (%2)").arg(lang[0]).arg(lang[1]);
        const QString localeCode = lang[2];
        ui->languageCombo->addItem(itemText, localeCode);
    }

    int value = settings.value("windowOpacity").toInt();
    ui->windowOpacityLevel->setValue(100 - value);

    QPixmap previewCheckerboard(":background/checkerboard.png");
    QPixmap previewWhite(32, 32);
    QPixmap previewGrey(32, 32);
    QPixmap previewDots(":background/dots.png");
    QPixmap previewWeave(":background/weave.jpg");

    previewWhite.fill(Qt::white);
    previewGrey.fill(Qt::lightGray);

    ui->checkerBackgroundButton->setIcon(previewCheckerboard.scaled(32, 32));
    ui->whiteBackgroundButton->setIcon(previewWhite);
    ui->greyBackgroundButton->setIcon(previewGrey);
    ui->dotsBackgroundButton->setIcon(previewDots.scaled(32, 32));
    ui->weaveBackgroundButton->setIcon(previewWeave.scaled(32, 32));
    ui->backgroundButtons->setId(ui->checkerBackgroundButton, 1);
    ui->backgroundButtons->setId(ui->whiteBackgroundButton, 2);
    ui->backgroundButtons->setId(ui->greyBackgroundButton, 3);
    ui->backgroundButtons->setId(ui->dotsBackgroundButton, 4);
    ui->backgroundButtons->setId(ui->weaveBackgroundButton, 5);

    auto buttonClicked = static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked);
    auto curIndexChagned = static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
    auto spinValueChanged = static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged);
    connect(ui->languageCombo, curIndexChagned, this, &GeneralPage::languageChanged);
    connect(ui->windowOpacityLevel, &QSlider::valueChanged, this, &GeneralPage::windowOpacityChange);
    connect(ui->backgroundButtons, buttonClicked, this, &GeneralPage::backgroundChanged);
    connect(ui->shadowsBox, &QCheckBox::stateChanged, this, &GeneralPage::shadowsCheckboxStateChanged);
    connect(ui->toolCursorsBox, &QCheckBox::stateChanged, this, &GeneralPage::toolCursorsCheckboxStateChanged);
    connect(ui->antialiasingBox, &QCheckBox::stateChanged, this, &GeneralPage::antiAliasCheckboxStateChanged);
    connect(ui->curveSmoothingLevel, &QSlider::valueChanged, this, &GeneralPage::curveSmoothingChanged);
    connect(ui->highResBox, &QCheckBox::stateChanged, this, &GeneralPage::highResCheckboxStateChanged);
    connect(ui->dottedCursorBox, &QCheckBox::stateChanged, this, &GeneralPage::dottedCursorCheckboxStateChanged);
    connect(ui->gridSizeInputW, spinValueChanged, this, &GeneralPage::gridWidthChanged);
    connect(ui->gridSizeInputH, spinValueChanged, this, &GeneralPage::gridHeightChanged);
    connect(ui->actionSafeCheckBox, &QCheckBox::stateChanged, this, &GeneralPage::actionSafeCheckBoxStateChanged);
    connect(ui->actionSafeInput, spinValueChanged, this, &GeneralPage::actionSafeAreaChanged);
    connect(ui->titleSafeCheckBox, &QCheckBox::stateChanged, this, &GeneralPage::titleSafeCheckBoxStateChanged);
    connect(ui->titleSafeInput, spinValueChanged, this, &GeneralPage::titleSafeAreaChanged);
    connect(ui->safeHelperTextCheckbox, &QCheckBox::stateChanged, this, &GeneralPage::SafeAreaHelperTextCheckBoxStateChanged);
    connect(ui->gridCheckBox, &QCheckBox::stateChanged, this, &GeneralPage::gridCheckBoxStateChanged);
    connect(ui->framePoolSizeSpin, spinValueChanged, this, &GeneralPage::frameCacheNumberChanged);
}

GeneralPage::~GeneralPage()
{
    delete ui;
}

void GeneralPage::updateValues()
{
    int index = ui->languageCombo->findData(mManager->getString(SETTING::LANGUAGE));

    if (index >= 0)
    {
        QSignalBlocker b(ui->languageCombo);
        ui->languageCombo->setCurrentIndex(index);
    }

    QSignalBlocker b1(ui->curveSmoothingLevel);
    ui->curveSmoothingLevel->setValue(mManager->getInt(SETTING::CURVE_SMOOTHING));
    QSignalBlocker b2(ui->windowOpacityLevel);
    ui->windowOpacityLevel->setValue(100 - mManager->getInt(SETTING::WINDOW_OPACITY));
    QSignalBlocker b3(ui->shadowsBox);
    ui->shadowsBox->setChecked(mManager->isOn(SETTING::SHADOW));
    QSignalBlocker b4(ui->toolCursorsBox);
    ui->toolCursorsBox->setChecked(mManager->isOn(SETTING::TOOL_CURSOR));
    QSignalBlocker b5(ui->antialiasingBox);
    ui->antialiasingBox->setChecked(mManager->isOn(SETTING::ANTIALIAS));
    QSignalBlocker b6(ui->dottedCursorBox);
    ui->dottedCursorBox->setChecked(mManager->isOn(SETTING::DOTTED_CURSOR));
    QSignalBlocker b7(ui->gridSizeInputW);
    ui->gridSizeInputW->setValue(mManager->getInt(SETTING::GRID_SIZE_W));
    QSignalBlocker b11(ui->gridSizeInputH);
    ui->gridSizeInputH->setValue(mManager->getInt(SETTING::GRID_SIZE_H));
    QSignalBlocker b8(ui->gridCheckBox);
    ui->gridCheckBox->setChecked(mManager->isOn(SETTING::GRID));
    QSignalBlocker b16(ui->actionSafeCheckBox);

    bool actionSafeOn = mManager->isOn(SETTING::ACTION_SAFE_ON);
    ui->actionSafeCheckBox->setChecked(actionSafeOn);
    QSignalBlocker b14(ui->actionSafeInput);
    ui->actionSafeInput->setValue(mManager->getInt(SETTING::ACTION_SAFE));
    QSignalBlocker b17(ui->titleSafeCheckBox);
    bool titleSafeOn = mManager->isOn(SETTING::TITLE_SAFE_ON);
    ui->titleSafeCheckBox->setChecked(titleSafeOn);
    QSignalBlocker b15(ui->titleSafeInput);
    ui->titleSafeInput->setValue(mManager->getInt(SETTING::TITLE_SAFE));

    QSignalBlocker b18(ui->safeHelperTextCheckbox);
    ui->safeHelperTextCheckbox->setChecked(mManager->isOn(SETTING::OVERLAY_SAFE_HELPER_TEXT_ON));

    QSignalBlocker b9(ui->highResBox);
    ui->highResBox->setChecked(mManager->isOn(SETTING::HIGH_RESOLUTION));

    QSignalBlocker b10(ui->backgroundButtons);
    QString bgName = mManager->getString(SETTING::BACKGROUND_STYLE);

    QSignalBlocker b12(ui->framePoolSizeSpin);
    ui->framePoolSizeSpin->setValue(mManager->getInt(SETTING::FRAME_POOL_SIZE));

    int buttonIdx = 1;
    if (bgName == "checkerboard") buttonIdx = 1;
    else if (bgName == "white")   buttonIdx = 2;
    else if (bgName == "grey")    buttonIdx = 3;
    else if (bgName == "dots")    buttonIdx = 4;
    else if (bgName == "weave")   buttonIdx = 5;
    else Q_ASSERT(false);

    ui->backgroundButtons->button(buttonIdx)->setChecked(true);
}

void GeneralPage::languageChanged(int i)
{
    QString strLocale = ui->languageCombo->itemData(i).toString();
    mManager->set(SETTING::LANGUAGE, strLocale);

    QMessageBox::warning(this,
                         tr("Restart Required"),
                         tr("The language change will take effect after a restart of Pencil2D"));
}

void GeneralPage::backgroundChanged(int value)
{
    QString brushName = "white";
    switch (value)
    {
    case 1: brushName = "checkerboard"; break;
    case 2: brushName = "white"; break;
    case 3: brushName = "grey"; break;
    case 4: brushName = "dots"; break;
    case 5: brushName = "weave"; break;
    default:
        break;
    }
    mManager->set(SETTING::BACKGROUND_STYLE, brushName);
}

void GeneralPage::curveSmoothingChanged(int value)
{
    mManager->set(SETTING::CURVE_SMOOTHING, value);
}

void GeneralPage::highResCheckboxStateChanged(int b)
{
    mManager->set(SETTING::HIGH_RESOLUTION, b != Qt::Unchecked);
}

void GeneralPage::shadowsCheckboxStateChanged(int b)
{
    mManager->set(SETTING::SHADOW, b != Qt::Unchecked);
}

void GeneralPage::antiAliasCheckboxStateChanged(int b)
{
    mManager->set(SETTING::ANTIALIAS, b != Qt::Unchecked);
}

void GeneralPage::toolCursorsCheckboxStateChanged(int b)
{
    mManager->set(SETTING::TOOL_CURSOR, b != Qt::Unchecked);
}

void GeneralPage::dottedCursorCheckboxStateChanged(int b)
{
    mManager->set(SETTING::DOTTED_CURSOR, b != Qt::Unchecked);
}

void GeneralPage::gridWidthChanged(int value)
{
    mManager->set(SETTING::GRID_SIZE_W, value);
}

void GeneralPage::gridHeightChanged(int value)
{
    mManager->set(SETTING::GRID_SIZE_H, value);
}

void GeneralPage::actionSafeCheckBoxStateChanged(int b)
{
    mManager->set(SETTING::ACTION_SAFE_ON, b != Qt::Unchecked);
    updateSafeHelperTextEnabledState();
}

void GeneralPage::actionSafeAreaChanged(int value)
{
    mManager->set(SETTING::ACTION_SAFE, value);
}

void GeneralPage::titleSafeCheckBoxStateChanged(int b)
{
    mManager->set(SETTING::TITLE_SAFE_ON, b != Qt::Unchecked);
    updateSafeHelperTextEnabledState();
}

void GeneralPage::updateSafeHelperTextEnabledState()
{
    if (ui->actionSafeCheckBox->isChecked() == false && ui->titleSafeCheckBox->isChecked() == false) {
        ui->safeHelperTextCheckbox->setEnabled(false);
        ui->labSafeHelperText->setEnabled(false);
    } else {
        ui->safeHelperTextCheckbox->setEnabled(true);
        ui->labSafeHelperText->setEnabled(true);
    }
}

void GeneralPage::SafeAreaHelperTextCheckBoxStateChanged(int b)
{
    mManager->set(SETTING::OVERLAY_SAFE_HELPER_TEXT_ON, b != Qt::Unchecked);
}

void GeneralPage::titleSafeAreaChanged(int value)
{
    mManager->set(SETTING::TITLE_SAFE, value);
}

void GeneralPage::gridCheckBoxStateChanged(int b)
{
    mManager->set(SETTING::GRID, b != Qt::Unchecked);
}

void GeneralPage::frameCacheNumberChanged(int value)
{
    mManager->set(SETTING::FRAME_POOL_SIZE, value);
}

TimelinePage::TimelinePage()
    : ui(new Ui::TimelinePage)
{
    ui->setupUi(this);

    ui->timelineLength->setMaximum(MaxFramesBound);

    auto spinBoxValueChange = static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged);
    auto sliderChanged = static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged);
    auto comboChanged = static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
    connect(ui->timelineLength, spinBoxValueChange, this, &TimelinePage::timelineLengthChanged);
    connect(ui->scrubBox, &QCheckBox::stateChanged, this, &TimelinePage::scrubChanged);
    connect(ui->radioButtonAddNewKey, &QRadioButton::toggled, this, &TimelinePage::drawEmptyKeyRadioButtonToggled);
    connect(ui->radioButtonDuplicate, &QRadioButton::toggled, this, &TimelinePage::drawEmptyKeyRadioButtonToggled);
    connect(ui->radioButtonDrawOnPrev, &QRadioButton::toggled, this, &TimelinePage::drawEmptyKeyRadioButtonToggled);
    connect(ui->flipRollMsecsSlider, sliderChanged, this, &TimelinePage::flipRollMsecSliderChanged);
    connect(ui->flipRollMsecsSpinBox, spinBoxValueChange, this, &TimelinePage::flipRollMsecSpinboxChanged);
    connect(ui->flipRollNumDrawingsSlider, sliderChanged, this, &TimelinePage::flipRollNumDrawingdSliderChanged);
    connect(ui->flipRollNumDrawingsSpinBox, spinBoxValueChange, this, &TimelinePage::flipRollNumDrawingdSpinboxChanged);
    connect(ui->flipInBtwnMsecSlider, sliderChanged, this, &TimelinePage::flipInbetweenMsecSliderChanged);
    connect(ui->flipInBtwnMsecSpinBox, spinBoxValueChange, this, &TimelinePage::flipInbetweenMsecSpinboxChanged);
    connect(ui->soundScrubSlider, sliderChanged, this, &TimelinePage::soundScrubMsecSliderChanged);
    connect(ui->soundScrubSpinBox, spinBoxValueChange, this, &TimelinePage::soundScrubMsecSpinboxChanged);
    connect(ui->layerVisibilityComboBox, comboChanged, this, &TimelinePage::layerVisibilityChanged);
    connect(ui->visibilitySlider, &QSlider::valueChanged, this, &TimelinePage::layerVisibilityThresholdChanged);
    connect(ui->visibilitySpinbox, spinBoxValueChange, this, &TimelinePage::layerVisibilityThresholdChanged);
    ui->visibilitySpinbox->setSuffix("%");
}

TimelinePage::~TimelinePage()
{
    delete ui;
}

void TimelinePage::updateValues()
{
    QSignalBlocker b1(ui->scrubBox);
    ui->scrubBox->setChecked(mManager->isOn(SETTING::SHORT_SCRUB));

    QSignalBlocker b3(ui->timelineLength);
    ui->timelineLength->setValue(mManager->getInt(SETTING::TIMELINE_SIZE));
    if (mManager->getString(SETTING::TIMELINE_SIZE).toInt() <= 0)
        ui->timelineLength->setValue(240);

    QSignalBlocker b4(ui->radioButtonAddNewKey);
    QSignalBlocker b5(ui->radioButtonDuplicate);
    QSignalBlocker b6(ui->radioButtonDrawOnPrev);
    int action = mManager->getInt(SETTING::DRAW_ON_EMPTY_FRAME_ACTION);
    switch (action)
    {
    case CREATE_NEW_KEY:
        ui->radioButtonAddNewKey->setChecked(true);
        break;
    case DUPLICATE_PREVIOUS_KEY:
        ui->radioButtonDuplicate->setChecked(true);
        break;
    case KEEP_DRAWING_ON_PREVIOUS_KEY:
        ui->radioButtonDrawOnPrev->setChecked(true);
        break;
    default:
        break;
    }

    // to secure that you have a relevant minimum setting for sound scrub
    int fps = mManager->getInt(SETTING::FPS);
    int minMsec = 1000 / fps;
    if (minMsec > 100) { minMsec = 100; }
    ui->soundScrubSpinBox->setMinimum(minMsec);
    ui->soundScrubSlider->setMinimum(minMsec);

    ui->flipRollMsecsSlider->setValue(mManager->getInt(SETTING::FLIP_ROLL_MSEC));
    ui->flipRollNumDrawingsSlider->setValue(mManager->getInt(SETTING::FLIP_ROLL_DRAWINGS));
    ui->flipInBtwnMsecSlider->setValue(mManager->getInt(SETTING::FLIP_INBETWEEN_MSEC));
    ui->flipRollMsecsSpinBox->setValue(mManager->getInt(SETTING::FLIP_ROLL_MSEC));
    ui->flipRollNumDrawingsSpinBox->setValue(mManager->getInt(SETTING::FLIP_ROLL_DRAWINGS));
    ui->flipInBtwnMsecSpinBox->setValue(mManager->getInt(SETTING::FLIP_INBETWEEN_MSEC));
    ui->soundScrubSpinBox->setValue(mManager->getInt(SETTING::SOUND_SCRUB_MSEC));
    ui->soundScrubSlider->setValue(mManager->getInt(SETTING::SOUND_SCRUB_MSEC));

    int convertedVisibilityThreshold = static_cast<int>(mManager->getFloat(SETTING::LAYER_VISIBILITY_THRESHOLD)*100);

    ui->visibilitySlider->setValue(convertedVisibilityThreshold);
    ui->visibilitySpinbox->setValue(convertedVisibilityThreshold);

    int visibilityType = mManager->getInt(SETTING::LAYER_VISIBILITY);
    ui->layerVisibilityComboBox->setCurrentIndex(visibilityType);
    layerVisibilityChanged(visibilityType);
}

void TimelinePage::timelineLengthChanged(int value)
{
    mManager->set(SETTING::TIMELINE_SIZE, value);
}

void TimelinePage::fontSizeChanged(int value)
{
    mManager->set(SETTING::LABEL_FONT_SIZE, value);
}

void TimelinePage::scrubChanged(int value)
{
    mManager->set(SETTING::SHORT_SCRUB, value != Qt::Unchecked);
}

void TimelinePage::layerVisibilityChanged(int value)
{
    mManager->set(SETTING::LAYER_VISIBILITY, value);
    ui->visibilitySlider->setEnabled(value == 1);
    ui->visibilitySpinbox->setEnabled(value == 1);
}

void TimelinePage::layerVisibilityThresholdChanged(int value)
{
    float percentage = static_cast<float>(value/100.0f);
    mManager->set(SETTING::LAYER_VISIBILITY_THRESHOLD, percentage);

    QSignalBlocker b8(ui->visibilitySlider);
    ui->visibilitySlider->setValue(value);

    QSignalBlocker b9(ui->visibilitySpinbox);
    ui->visibilitySpinbox->setValue(value);
}

void TimelinePage::drawEmptyKeyRadioButtonToggled(bool)
{
    if (ui->radioButtonAddNewKey->isChecked())
    {
        mManager->set(SETTING::DRAW_ON_EMPTY_FRAME_ACTION, CREATE_NEW_KEY);
    }
    else if (ui->radioButtonDuplicate->isChecked())
    {
        mManager->set(SETTING::DRAW_ON_EMPTY_FRAME_ACTION, DUPLICATE_PREVIOUS_KEY);
    }
    else if (ui->radioButtonDrawOnPrev->isChecked())
    {
        mManager->set(SETTING::DRAW_ON_EMPTY_FRAME_ACTION, KEEP_DRAWING_ON_PREVIOUS_KEY);
    }
}

void TimelinePage::flipRollMsecSliderChanged(int value)
{
    ui->flipRollMsecsSpinBox->setValue(value);
    mManager->set(SETTING::FLIP_ROLL_MSEC, value);
}

void TimelinePage::flipRollMsecSpinboxChanged(int value)
{
    ui->flipRollMsecsSlider->setValue(value);
    mManager->set(SETTING::FLIP_ROLL_MSEC, value);
}

void TimelinePage::flipRollNumDrawingdSliderChanged(int value)
{
    ui->flipRollNumDrawingsSpinBox->setValue(value);
    mManager->set(SETTING::FLIP_ROLL_DRAWINGS, value);
}

void TimelinePage::flipRollNumDrawingdSpinboxChanged(int value)
{
    ui->flipRollNumDrawingsSlider->setValue(value);
    mManager->set(SETTING::FLIP_ROLL_DRAWINGS, value);
}

void TimelinePage::flipInbetweenMsecSliderChanged(int value)
{
    ui->flipInBtwnMsecSpinBox->setValue(value);
    mManager->set(SETTING::FLIP_INBETWEEN_MSEC, value);
}

void TimelinePage::flipInbetweenMsecSpinboxChanged(int value)
{
    ui->flipInBtwnMsecSlider->setValue(value);
    mManager->set(SETTING::FLIP_INBETWEEN_MSEC, value);
}

void TimelinePage::soundScrubActiveChanged(int i)
{
    bool b = true;
    if (i == 0)
        b = false;
    mManager->set(SETTING::SOUND_SCRUB_ACTIVE, b);
    emit soundScrubChanged(b);
}

void TimelinePage::soundScrubMsecSliderChanged(int value)
{
    ui->soundScrubSpinBox->setValue(value);
    mManager->set(SETTING::SOUND_SCRUB_MSEC, value);
    emit soundScrubMsecChanged(value);
}

void TimelinePage::soundScrubMsecSpinboxChanged(int value)
{
    ui->soundScrubSlider->setValue(value);
    mManager->set(SETTING::SOUND_SCRUB_MSEC, value);
    emit soundScrubMsecChanged(value);
}

FilesPage::FilesPage()
    : ui(new Ui::FilesPage)
{
    ui->setupUi(this);

    initPreset();

    connect(ui->addPreset, &QPushButton::clicked, this, &FilesPage::addPreset);
    connect(ui->removePreset, &QPushButton::clicked, this, &FilesPage::removePreset);
    connect(ui->setDefaultPreset, &QPushButton::clicked, this, &FilesPage::setDefaultPreset);
    connect(ui->askPresetCheckBox, &QCheckBox::stateChanged, this, &FilesPage::askForPresetChange);
    connect(ui->presetListWidget, &QListWidget::itemChanged, this, &FilesPage::presetNameChanged);

    auto spinBoxValueChange = static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged);
    connect(ui->autosaveCheckBox, &QCheckBox::stateChanged, this, &FilesPage::autosaveChange);
    connect(ui->autosaveNumberBox, spinBoxValueChange, this, &FilesPage::autosaveNumberChange);
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
    ui->askPresetCheckBox->setChecked(mManager->isOn(SETTING::ASK_FOR_PRESET));

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
}

void FilesPage::askForPresetChange(int b)
{
    mManager->set(SETTING::ASK_FOR_PRESET, b != Qt::Unchecked);
}

void FilesPage::autosaveChange(int b)
{
    mManager->set(SETTING::AUTO_SAVE, b != Qt::Unchecked);
}

void FilesPage::autosaveNumberChange(int number)
{
    mManager->set(SETTING::AUTO_SAVE_NUMBER, number);
}

ToolsPage::ToolsPage() : ui(new Ui::ToolsPage)
{
    ui->setupUi(this);

    connect(ui->useQuickSizingBox, &QCheckBox::stateChanged, this, &ToolsPage::quickSizingChange);
    connect(ui->rotationIncrementSlider, &QSlider::valueChanged, this, &ToolsPage::rotationIncrementChange);
}

ToolsPage::~ToolsPage()
{
    delete ui;
}

void ToolsPage::updateValues()
{
    ui->useQuickSizingBox->setChecked(mManager->isOn(SETTING::QUICK_SIZING));
    setRotationIncrement(mManager->getInt(SETTING::ROTATION_INCREMENT));
}

void ToolsPage::quickSizingChange(int b)
{
    mManager->set(SETTING::QUICK_SIZING, b != Qt::Unchecked);
}

void ToolsPage::setRotationIncrement(int angle)
{
    int value = qSqrt((angle - 1) / 359.0) * 359;
    ui->rotationIncrementSlider->setValue(value);
}

void ToolsPage::rotationIncrementChange(int value)
{
    // Use log scale
    int angle = qPow(value / 359.0, 2) * 359 + 1;
    // Basically round up to the nearest number that is a divisor of 360
    while (360 % angle != 0) {
        angle++;
    }
    ui->rotationIncrementDisplay->setText(tr("%1 degrees").arg(angle)); // don't use tr()'s plural settings, it breaks Transifex.
    mManager->set(SETTING::ROTATION_INCREMENT, angle);
}


