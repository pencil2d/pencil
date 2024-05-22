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

#include "generalpage.h"

#include <memory>
#include <QMessageBox>
#include <QSettings>
#include <QTranslator>

#include "pencildef.h"
#include "preferencemanager.h"

#include "ui_generalpage.h"

GeneralPage::GeneralPage() : ui(new Ui::GeneralPage)
{
    ui->setupUi(this);

    QSettings settings(PENCIL2D, PENCIL2D);

    QString languages [][3]
        {
            // translatable string, endonym, locale code
            { tr("Arabic"), QStringLiteral("العربية"), "ar" },
            { tr("Bulgarian"), QStringLiteral("Български"), "bg" },
            { tr("Catalan"), QStringLiteral("Català"), "ca" },
            { tr("Czech"), QStringLiteral("Čeština"), "cs" },
            { tr("Danish"), QStringLiteral("Dansk"), "da" },
            { tr("German"), QStringLiteral("Deutsch"), "de" },
            { tr("Greek"), QStringLiteral("Ελληνικά"), "el" },
            { tr("English"), QStringLiteral("English"), "en" },
            { tr("Spanish"), QStringLiteral("Español"), "es" },
            { tr("Estonian"), QStringLiteral("Eesti"), "et" },
            { tr("Persian"), QStringLiteral("فارسی"), "fa" },
            { tr("French"), QStringLiteral("Français"), "fr" },
            { tr("Hebrew"), QStringLiteral("עברית"), "he" },
            { tr("Hungarian"), QStringLiteral("Magyar"), "hu_HU" },
            { tr("Indonesian"), QStringLiteral("Bahasa Indonesia"), "id" },
            { tr("Italian"), QStringLiteral("Italiano"), "it" },
            { tr("Japanese"), QStringLiteral("日本語"), "ja" },
            { tr("Kabyle"), QStringLiteral("Taqbaylit"), "kab" },
            { tr("Korean"), QStringLiteral("한국어"), "ko" },
            { tr("Norwegian Bokmål"), QStringLiteral("Norsk bokmål"), "nb" },
            { tr("Dutch \u2013 Netherlands"), QStringLiteral("Nederlands \u2013 Nederland"), "nl_NL" },
            { tr("Polish"), QStringLiteral("Polski"), "pl" },
            { tr("Portuguese \u2013 Portugal"), QStringLiteral("Português \u2013 Portugal"), "pt_PT" },
            { tr("Portuguese \u2013 Brazil"), QStringLiteral("Português \u2013 Brasil"), "pt_BR" },
            { tr("Russian"), QStringLiteral("Русский"), "ru" },
            { tr("Slovene"), QStringLiteral("Slovenščina"), "sl" },
            { tr("Swedish"), QStringLiteral("Svenska"), "sv" },
            { tr("Turkish"), QStringLiteral("Türkçe"), "tr" },
            { tr("Vietnamese"), QStringLiteral("Tiếng Việt"), "vi" },
            { tr("Cantonese"), QStringLiteral("粵语"), "yue" },
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

    auto buttonClicked = static_cast<void (QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked);
    auto curIndexChanged = static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
    auto spinValueChanged = static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged);
    connect(ui->languageCombo, curIndexChanged, this, &GeneralPage::languageChanged);
    connect(ui->windowOpacityLevel, &QSlider::valueChanged, this, &GeneralPage::windowOpacityChange);
    connect(ui->backgroundButtons, buttonClicked, this, &GeneralPage::backgroundChanged);
    connect(ui->shadowsBox, &QCheckBox::stateChanged, this, &GeneralPage::shadowsCheckboxStateChanged);
    connect(ui->toolCursorsBox, &QCheckBox::stateChanged, this, &GeneralPage::toolCursorsCheckboxStateChanged);
    connect(ui->antialiasingBox, &QCheckBox::stateChanged, this, &GeneralPage::antiAliasCheckboxStateChanged);
    connect(ui->curveSmoothingLevel, &QSlider::valueChanged, this, &GeneralPage::curveSmoothingChanged);
    connect(ui->highResBox, &QCheckBox::stateChanged, this, &GeneralPage::highResCheckboxStateChanged);
    connect(ui->canvasCursorBox, &QCheckBox::stateChanged, this, &GeneralPage::canvasCursorCheckboxStateChanged);
    connect(ui->gridSizeInputW, spinValueChanged, this, &GeneralPage::gridWidthChanged);
    connect(ui->gridSizeInputH, spinValueChanged, this, &GeneralPage::gridHeightChanged);
    connect(ui->actionSafeCheckBox, &QCheckBox::stateChanged, this, &GeneralPage::actionSafeCheckBoxStateChanged);
    connect(ui->actionSafeInput, spinValueChanged, this, &GeneralPage::actionSafeAreaChanged);
    connect(ui->titleSafeCheckBox, &QCheckBox::stateChanged, this, &GeneralPage::titleSafeCheckBoxStateChanged);
    connect(ui->titleSafeInput, spinValueChanged, this, &GeneralPage::titleSafeAreaChanged);
    connect(ui->safeHelperTextCheckbox, &QCheckBox::stateChanged, this, &GeneralPage::SafeAreaHelperTextCheckBoxStateChanged);
    connect(ui->gridCheckBox, &QCheckBox::stateChanged, this, &GeneralPage::gridCheckBoxStateChanged);
    connect(ui->framePoolSizeSpin, spinValueChanged, this, &GeneralPage::frameCacheNumberChanged);
    connect(ui->invertScrollDirectionBox, &QCheckBox::stateChanged, this, &GeneralPage::invertScrollDirectionBoxStateChanged);
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
    QSignalBlocker b6(ui->canvasCursorBox);
    ui->canvasCursorBox->setChecked(mManager->isOn(SETTING::CANVAS_CURSOR));
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

    ui->invertScrollDirectionBox->setChecked(mManager->isOn(SETTING::INVERT_SCROLL_ZOOM_DIRECTION));
}

void GeneralPage::languageChanged(int i)
{
    QString strLocale = ui->languageCombo->itemData(i).toString();
    mManager->set(SETTING::LANGUAGE, strLocale);

    QLocale locale = strLocale.isEmpty() ? QLocale::system() : QLocale(strLocale);
    std::unique_ptr<QTranslator> newlangTr(new QTranslator(this));
    if (newlangTr->load(locale, "pencil", "_", ":/i18n/"))
    {
        QMessageBox::warning(this,
                             newlangTr->translate(staticMetaObject.className(), "Restart Required"),
                             newlangTr->translate(staticMetaObject.className(), "The language change will take effect after a restart of Pencil2D"));
    } else {
        Q_ASSERT(false);
        QMessageBox::warning(this,
                             tr("Restart Required"),
                             tr("The language change will take effect after a restart of Pencil2D"));
    }
}

void GeneralPage::backgroundChanged(QAbstractButton* button)
{
    QString brushName = "white";
    if (button == ui->checkerBackgroundButton)    brushName = "checkerboard";
    else if (button == ui->whiteBackgroundButton) brushName = "white";
    else if (button == ui->greyBackgroundButton)  brushName = "grey";
    else if (button == ui->dotsBackgroundButton)  brushName = "dots";
    else if (button == ui->weaveBackgroundButton) brushName = "weave";
    else Q_UNREACHABLE();
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

void GeneralPage::canvasCursorCheckboxStateChanged(int b)
{
    mManager->set(SETTING::CANVAS_CURSOR, b != Qt::Unchecked);
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
    if (!ui->actionSafeCheckBox->isChecked() && !ui->titleSafeCheckBox->isChecked()) {
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

void GeneralPage::invertScrollDirectionBoxStateChanged(int b)
{
    mManager->set(SETTING::INVERT_SCROLL_ZOOM_DIRECTION, b != Qt::Unchecked);
}
