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
#include "ui_preferencesdialog.h"
#include "ui_generalpage.h"
#include "ui_timelinepage.h"
#include "ui_filespage.h"
#include "ui_toolspage.h"
#include <QComboBox>
#include <QMessageBox>
#include "util.h"

PreferencesDialog::PreferencesDialog( QWidget* parent ) :
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
    connect(this, &PreferencesDialog::updateRecentFileListBtn, ui->filesPage, &FilesPage::updateClearRecentListButton);

    auto onCurrentItemChanged = static_cast<void (QListWidget::*)(QListWidgetItem*, QListWidgetItem*)>(&QListWidget::currentItemChanged);
    connect(ui->contentsWidget, onCurrentItemChanged, this, &PreferencesDialog::changePage);
}

void PreferencesDialog::closeEvent(QCloseEvent *)
{
    done( QDialog::Accepted );
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

GeneralPage::GeneralPage(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::GeneralPage)
{
    ui->setupUi(this);

    QSettings settings( PENCIL2D, PENCIL2D );

    ui->languageCombo->addItem( tr( "Czech" ), "cs" );
    ui->languageCombo->addItem( tr( "Danish" ), "da" );
    ui->languageCombo->addItem( tr( "English" ), "en" );
    ui->languageCombo->addItem( tr( "German" ), "de" );
    ui->languageCombo->addItem( tr( "Spanish" ), "es" );
    ui->languageCombo->addItem( tr( "French" ), "fr" );
    ui->languageCombo->addItem( tr( "Hungarian" ), "hu-HU" );
    ui->languageCombo->addItem( tr( "Italian" ), "it" );
    ui->languageCombo->addItem( tr( "Japanese" ), "ja" );
    ui->languageCombo->addItem( tr( "Portuguese - Brazil" ), "pt-BR" );
    ui->languageCombo->addItem( tr( "Russian" ), "ru" );
    ui->languageCombo->addItem( tr( "Chinese - Taiwan" ), "zh-TW" );

    int value = settings.value("windowOpacity").toInt();
    ui->windowOpacityLevel->setValue( 100 - value );

    QPixmap previewCheckerboard( ":background/checkerboard.png" );
    QPixmap previewWhite(32,32);
    QPixmap previewGrey(32,32);
    QPixmap previewDots( ":background/dots.png" );
    QPixmap previewWeave( ":background/weave.jpg" );

    previewWhite.fill( Qt::white );
    previewGrey.fill( Qt:: lightGray );

    ui->checkerBackgroundButton->setIcon( previewCheckerboard.scaled(32, 32) );
    ui->whiteBackgroundButton->setIcon( previewWhite );
    ui->greyBackgroundButton->setIcon( previewGrey );
    ui->dotsBackgroundButton->setIcon( previewDots.scaled(32, 32) );
    ui->weaveBackgroundButton->setIcon( previewWeave.scaled(32, 32) );
    ui->backgroundButtons->setId(ui->checkerBackgroundButton, 1);
    ui->backgroundButtons->setId(ui->whiteBackgroundButton, 2);
    ui->backgroundButtons->setId(ui->greyBackgroundButton, 3);
    ui->backgroundButtons->setId(ui->dotsBackgroundButton, 4);
    ui->backgroundButtons->setId(ui->weaveBackgroundButton, 5);

    connect(ui->windowOpacityLevel, &QSlider::valueChanged, this, &GeneralPage::windowOpacityChange);
}

GeneralPage::~GeneralPage()
{
    delete ui;
}



void GeneralPage::updateValues()
{
    int index = ui->languageCombo->findData( mManager->getString( SETTING::LANGUAGE ) );

    if ( index >= 0 )
    {
        SignalBlocker b(ui->languageCombo);
        ui->languageCombo->blockSignals( true );
        ui->languageCombo->setCurrentIndex( index );
        ui->languageCombo->blockSignals( false );
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
    SignalBlocker b7(ui->gridSizeInput);
    ui->gridSizeInput->setValue(mManager->getInt(SETTING::GRID_SIZE));
    SignalBlocker b8(ui->gridCheckBox);
    ui->gridCheckBox->setChecked(mManager->isOn(SETTING::GRID));

    SignalBlocker b9(ui->highResBox);
    ui->highResBox->setChecked(mManager->isOn(SETTING::HIGH_RESOLUTION));

    SignalBlocker b10(ui->backgroundButtons);
    QString bgName = mManager->getString(SETTING::BACKGROUND_STYLE);
    if (bgName == "checkerboard")
        ui->backgroundButtons->button(1)->setChecked(true);
    else if (bgName == "white")
        ui->backgroundButtons->button(2)->setChecked(true);
    else if (bgName == "grey")
        ui->backgroundButtons->button(3)->setChecked(true);
    else if (bgName == "dots")
        ui->backgroundButtons->button(4)->setChecked(true);
    else if (bgName == "weave")
        ui->backgroundButtons->button(5)->setChecked(true);
    else
        Q_ASSERT(false);
}

void GeneralPage::languageChanged( int i )
{
    QString strLocale = ui->languageCombo->itemData( i ).toString();
    mManager->set( SETTING::LANGUAGE, strLocale );

    QMessageBox::warning( this,
                          tr( "Restart Required" ),
                          tr( "The language change will take effect after a restart of Pencil2D" ) );
}

void GeneralPage::backgroundChange(int value)
{
    QString brushName = "white";
    switch (value)
	{
    case 1:
        brushName = "checkerboard";
        break;
    case 2:
        brushName = "white";
        break;
    case 3:
        brushName = "grey";
        break;
    case 4:
        brushName = "dots";
        break;
    case 5:
        brushName = "weave";
        break;
    default:
        break;
    }
    mManager->set(SETTING::BACKGROUND_STYLE, brushName);
}

void GeneralPage::curveSmoothingChange(int value)
{
    mManager->set(SETTING::CURVE_SMOOTHING, value);
}

void GeneralPage::highResCheckboxStateChanged( int b )
{
    mManager->set( SETTING::HIGH_RESOLUTION, b != Qt::Unchecked );
}

void GeneralPage::shadowsCheckboxStateChanged( int b )
{
    mManager->set( SETTING::SHADOW, b != Qt::Unchecked );
}

void GeneralPage::antiAliasCheckboxStateChanged( int b )
{
    mManager->set( SETTING::ANTIALIAS, b != Qt::Unchecked );
}

void GeneralPage::toolCursorsCheckboxStateChanged(int b)
{
    mManager->set( SETTING::TOOL_CURSOR, b != Qt::Unchecked );
}

void GeneralPage::dottedCursorCheckboxStateChanged(int b)
{
    mManager->set( SETTING::DOTTED_CURSOR, b != Qt::Unchecked );
}

void GeneralPage::gridSizeChange(int value)
{
    mManager->set(SETTING::GRID_SIZE, value);
}

void GeneralPage::gridCheckBoxStateChanged(int b)
{
    mManager->set(SETTING::GRID, b != Qt::Unchecked);
}

TimelinePage::TimelinePage(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::TimelinePage)
{
    ui->setupUi(this);

    QIntValidator* lengthSizeValidator = new QIntValidator(this);
    lengthSizeValidator->setBottom(2);
    ui->lengthSize->setValidator( lengthSizeValidator );
}

TimelinePage::~TimelinePage()
{
    delete ui;
}

void TimelinePage::updateValues()
{
    ui->scrubBox->setChecked(mManager->isOn(SETTING::SHORT_SCRUB));
    ui->frameSize->setValue(mManager->getInt(SETTING::FRAME_SIZE));
    if (mManager->getString(SETTING::FRAME_SIZE).toInt()==0) ui->frameSize->setValue(6);
    ui->lengthSize->setText(mManager->getString(SETTING::TIMELINE_SIZE));
    if (mManager->getString(SETTING::TIMELINE_SIZE).toInt()==0) ui->lengthSize->setText("240");
}

void TimelinePage::lengthSizeChange(QString value)
{
    int length = value.toInt();
    mManager->set(SETTING::TIMELINE_SIZE, length);
}

void TimelinePage::fontSizeChange(int value)
{
    mManager->set(SETTING::LABEL_FONT_SIZE, value);
}

void TimelinePage::frameSizeChange(int value)
{
    mManager->set(SETTING::FRAME_SIZE, value);
}

void TimelinePage::labelChange(bool value)
{
    mManager->set(SETTING::DRAW_LABEL, value);
}

void TimelinePage::scrubChange(int value)
{
    mManager->set(SETTING::SHORT_SCRUB, value != Qt::Unchecked);
}

FilesPage::FilesPage(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::FilesPage)
{
    ui->setupUi(this);
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

void FilesPage::updateClearRecentListButton()
{
    ui->clearRecentFilesBtn->setEnabled(false);
    ui->clearRecentFilesBtn->setText(tr("List is empty"));
}

void FilesPage::autosaveChange(int b)
{
    mManager->set(SETTING::AUTO_SAVE, b != Qt::Unchecked);
}

void FilesPage::autosaveNumberChange(int number)
{
    mManager->set(SETTING::AUTO_SAVE_NUMBER, number);
}

void FilesPage::clearRecentFilesList()
{
    emit clearRecentList();
}

ToolsPage::ToolsPage(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ToolsPage)
{
    ui->setupUi(this);
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

void ToolsPage::quickSizingChange( int b )
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
