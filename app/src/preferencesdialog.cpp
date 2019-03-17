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
#include "ui_preferencesdialog.h"
#include "ui_generalpage.h"
#include "ui_timelinepage.h"
#include "ui_filespage.h"
#include "ui_toolspage.h"
#include "util.h"


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

    ui->languageCombo->addItem(tr("Catalan ") + " (Catalan)", "ca");
    ui->languageCombo->addItem(tr("Czech") + " (Czech)", "cs");
    ui->languageCombo->addItem(tr("Danish") + " (Danish)", "da");
    ui->languageCombo->addItem(tr("German") + " (German)", "de");
    ui->languageCombo->addItem(tr("Greek") + " (Greek)", "el");
    ui->languageCombo->addItem(tr("English") + " (English)", "en");
    ui->languageCombo->addItem(tr("Spanish") + " (Spanish)", "es");
    ui->languageCombo->addItem(tr("Estonian") + " (Estonian)", "et");
    ui->languageCombo->addItem(tr("French") + " (French)", "fr");
    ui->languageCombo->addItem(tr("Hebrew") + " (Hebrew)", "he");
    ui->languageCombo->addItem(tr("Hungarian") + " (Hungarian)", "hu_HU");
    ui->languageCombo->addItem(tr("Indonesian") + " (Indonesian)", "id");
    ui->languageCombo->addItem(tr("Italian") + " (Italian)", "it");
    ui->languageCombo->addItem(tr("Japanese") + " (Japanese)", "ja");
    ui->languageCombo->addItem(tr("Kabyle") + " (Kabyle)", "kab");
    ui->languageCombo->addItem(tr("Polish") + " (Polish)", "pl");
    ui->languageCombo->addItem(tr("Portuguese - Portugal") + "(Portuguese - Portugal)", "pt");
    ui->languageCombo->addItem(tr("Portuguese - Brazil") + "(Portuguese - Brazil)", "pt_BR");
    ui->languageCombo->addItem(tr("Russian") + " (Russian)", "ru");
    ui->languageCombo->addItem(tr("Slovenian") + " (Slovenian)", "sl");
    ui->languageCombo->addItem(tr("Vietnamese") + " (Vietnamese)", "vi");
    ui->languageCombo->addItem(tr("Chinese - China") + " (Chinese - China)", "zh_CN");
    ui->languageCombo->addItem(tr("Chinese - Taiwan") + " (Chinese - Taiwan)", "zh_TW");

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
        SignalBlocker b(ui->languageCombo);
        ui->languageCombo->setCurrentIndex(index);
    }

    SignalBlocker b1(ui->curveSmoothingLevel);
    ui->curveSmoothingLevel->setValue(mManager->getInt(SETTING::CURVE_SMOOTHING));
    SignalBlocker b2(ui->windowOpacityLevel);
    ui->windowOpacityLevel->setValue(100 - mManager->getInt(SETTING::WINDOW_OPACITY));
    SignalBlocker b3(ui->shadowsBox);
    ui->shadowsBox->setChecked(mManager->isOn(SETTING::SHADOW));
    SignalBlocker b4(ui->toolCursorsBox);
    ui->toolCursorsBox->setChecked(mManager->isOn(SETTING::TOOL_CURSOR));
    SignalBlocker b5(ui->antialiasingBox);
    ui->antialiasingBox->setChecked(mManager->isOn(SETTING::ANTIALIAS));
    SignalBlocker b6(ui->dottedCursorBox);
    ui->dottedCursorBox->setChecked(mManager->isOn(SETTING::DOTTED_CURSOR));
    SignalBlocker b7(ui->gridSizeInputW);
    ui->gridSizeInputW->setValue(mManager->getInt(SETTING::GRID_SIZE_W));
    SignalBlocker b11(ui->gridSizeInputH);
    ui->gridSizeInputH->setValue(mManager->getInt(SETTING::GRID_SIZE_H));
    SignalBlocker b8(ui->gridCheckBox);
    ui->gridCheckBox->setChecked(mManager->isOn(SETTING::GRID));

    SignalBlocker b9(ui->highResBox);
    ui->highResBox->setChecked(mManager->isOn(SETTING::HIGH_RESOLUTION));

    SignalBlocker b10(ui->backgroundButtons);
    QString bgName = mManager->getString(SETTING::BACKGROUND_STYLE);

    SignalBlocker b12(ui->framePoolSizeSpin);
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

    auto spinBoxValueChange = static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged);
    auto sliderChanged = static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged);
    connect(ui->timelineLength, spinBoxValueChange, this, &TimelinePage::timelineLengthChanged);
    connect(ui->scrubBox, &QCheckBox::stateChanged, this, &TimelinePage::scrubChanged);
    connect(ui->radioButtonAddNewKey, &QRadioButton::toggled, this, &TimelinePage::drawEmptyKeyRadioButtonToggled);
    connect(ui->radioButtonDuplicate, &QRadioButton::toggled, this, &TimelinePage::drawEmptyKeyRadioButtonToggled);
    connect(ui->radioButtonDrawOnPrev, &QRadioButton::toggled, this, &TimelinePage::drawEmptyKeyRadioButtonToggled);
    connect(ui->onionWhilePlayback, &QCheckBox::stateChanged, this, &TimelinePage::playbackStateChanged);
    connect(ui->flipRollMsecsSlider, sliderChanged, this, &TimelinePage::flipRollMsecSliderChanged);
    connect(ui->flipRollMsecsSpinBox, spinBoxValueChange, this, &TimelinePage::flipRollMsecSpinboxChanged);
    connect(ui->flipRollNumDrawingsSlider, sliderChanged, this, &TimelinePage::flipRollNumDrawingdSliderChanged);
    connect(ui->flipRollNumDrawingsSpinBox, spinBoxValueChange, this, &TimelinePage::flipRollNumDrawingdSpinboxChanged);
    connect(ui->flipInBtwnMsecSlider, sliderChanged, this, &TimelinePage::flipInbetweenMsecSliderChanged);
    connect(ui->flipInBtwnMsecSpinBox, spinBoxValueChange, this, &TimelinePage::flipInbetweenMsecSpinboxChanged);
}

TimelinePage::~TimelinePage()
{
    delete ui;
}

void TimelinePage::updateValues()
{
    SignalBlocker b1(ui->scrubBox);
    ui->scrubBox->setChecked(mManager->isOn(SETTING::SHORT_SCRUB));

    SignalBlocker b3(ui->timelineLength);
    ui->timelineLength->setValue(mManager->getInt(SETTING::TIMELINE_SIZE));
    if (mManager->getString(SETTING::TIMELINE_SIZE).toInt() <= 0)
        ui->timelineLength->setValue(240);

    SignalBlocker b4(ui->radioButtonAddNewKey);
    SignalBlocker b5(ui->radioButtonDuplicate);
    SignalBlocker b6(ui->radioButtonDrawOnPrev);
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

    SignalBlocker b7(ui->onionWhilePlayback);
    ui->onionWhilePlayback->setChecked(mManager->getInt(SETTING::ONION_WHILE_PLAYBACK));
    ui->flipRollMsecsSlider->setValue(mManager->getInt(SETTING::FLIP_ROLL_MSEC));
    ui->flipRollNumDrawingsSlider->setValue(mManager->getInt(SETTING::FLIP_ROLL_DRAWINGS));
    ui->flipInBtwnMsecSlider->setValue(mManager->getInt(SETTING::FLIP_INBETWEEN_MSEC));
    ui->flipRollMsecsSpinBox->setValue(mManager->getInt(SETTING::FLIP_ROLL_MSEC));
    ui->flipRollNumDrawingsSpinBox->setValue(mManager->getInt(SETTING::FLIP_ROLL_DRAWINGS));
    ui->flipInBtwnMsecSpinBox->setValue(mManager->getInt(SETTING::FLIP_INBETWEEN_MSEC));
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

void TimelinePage::playbackStateChanged(int value)
{
    mManager->set(SETTING::ONION_WHILE_PLAYBACK, value);
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


FilesPage::FilesPage()
    : ui(new Ui::FilesPage)
{
    ui->setupUi(this);

    auto spinBoxValueChange = static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged);
    connect(ui->autosaveCheckBox, &QCheckBox::stateChanged, this, &FilesPage::autosaveChange);
    connect(ui->autosaveNumberBox, spinBoxValueChange, this, &FilesPage::autosaveNumberChange);
}

FilesPage::~FilesPage()
{
    delete ui;
}

void FilesPage::updateValues()
{
    ui->autosaveCheckBox->setChecked(mManager->isOn(SETTING::AUTO_SAVE));
    ui->autosaveNumberBox->setValue(mManager->getInt(SETTING::AUTO_SAVE_NUMBER));
}

void FilesPage::autosaveChange(int b)
{
    mManager->set(SETTING::AUTO_SAVE, b != Qt::Unchecked);
}

void FilesPage::autosaveNumberChange(int number)
{
    mManager->set(SETTING::AUTO_SAVE_NUMBER, number);
}

ToolsPage::ToolsPage()
    : ui(new Ui::ToolsPage)
{
    ui->setupUi(this);

    auto spinBoxChanged = static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged);
    connect(ui->onionMaxOpacityBox, spinBoxChanged, this, &ToolsPage::onionMaxOpacityChange);
    connect(ui->onionMinOpacityBox, spinBoxChanged, this, &ToolsPage::onionMinOpacityChange);
    connect(ui->onionPrevFramesNumBox, spinBoxChanged, this, &ToolsPage::onionPrevFramesNumChange);
    connect(ui->onionNextFramesNumBox, spinBoxChanged, this, &ToolsPage::onionNextFramesNumChange);
    connect(ui->useQuickSizingBox, &QCheckBox::stateChanged, this, &ToolsPage::quickSizingChange);
}

ToolsPage::~ToolsPage()
{
    delete ui;
}

void ToolsPage::updateValues()
{
    ui->onionMaxOpacityBox->setValue(mManager->getInt(SETTING::ONION_MAX_OPACITY));
    ui->onionMinOpacityBox->setValue(mManager->getInt(SETTING::ONION_MIN_OPACITY));
    ui->onionPrevFramesNumBox->setValue(mManager->getInt(SETTING::ONION_PREV_FRAMES_NUM));
    ui->onionNextFramesNumBox->setValue(mManager->getInt(SETTING::ONION_NEXT_FRAMES_NUM));
    ui->useQuickSizingBox->setChecked(mManager->isOn(SETTING::QUICK_SIZING));
}

void ToolsPage::onionMaxOpacityChange(int value)
{
    mManager->set(SETTING::ONION_MAX_OPACITY, value);
}

void ToolsPage::quickSizingChange(int b)
{
    mManager->set(SETTING::QUICK_SIZING, b != Qt::Unchecked);
}

void ToolsPage::onionMinOpacityChange(int value)
{
    mManager->set(SETTING::ONION_MIN_OPACITY, value);
}

void ToolsPage::onionPrevFramesNumChange(int value)
{
    mManager->set(SETTING::ONION_PREV_FRAMES_NUM, value);
}

void ToolsPage::onionNextFramesNumChange(int value)
{
    mManager->set(SETTING::ONION_NEXT_FRAMES_NUM, value);
}


