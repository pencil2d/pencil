/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2017 Matt Chiawen Chang

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

PreferencesDialog::PreferencesDialog( QWidget* parent ) : QDialog(parent)
{
    setWindowTitle(tr("Preferences"));
    setMaximumWidth(600);
    setMaximumHeight(680);
}

PreferencesDialog::~PreferencesDialog()
{
}

void PreferencesDialog::init( PreferenceManager* m )
{
    Q_ASSERT( m != nullptr );
    mPrefManager = m;

    contentsWidget = new QListWidget;
    contentsWidget->setViewMode( QListView::IconMode );
    contentsWidget->setIconSize( QSize( 96, 84 ) );
    contentsWidget->setMovement( QListView::Static );
    contentsWidget->setMaximumWidth( 128 );
    contentsWidget->setSpacing( 5 );

    GeneralPage* general = new GeneralPage( this );
    general->setManager( mPrefManager );
    general->updateValues();
    //connect(mPrefManager, &PreferenceManager::effectChanged, general, &GeneralPage::updateValues);

    mFilesPage = new FilesPage( this );
    mFilesPage->setManager( mPrefManager );
    mFilesPage->updateValues();

    connect(mFilesPage, &FilesPage::clearRecentList, this, &PreferencesDialog::clearRecentList);
    connect(this, &PreferencesDialog::updateRecentFileListBtn, mFilesPage, &FilesPage::updateClearRecentListButton);

    TimelinePage* timeline = new TimelinePage( this );
    timeline->setManager( mPrefManager );
    timeline->updateValues();

    ToolsPage* tools = new ToolsPage( this );
    tools->setManager( mPrefManager );
    tools->updateValues();

    ShortcutsPage* shortcuts = new ShortcutsPage( this );
    shortcuts->setManager( mPrefManager );

    pagesWidget = new QStackedWidget;
    pagesWidget->setMinimumHeight(40);

    pagesWidget->addWidget( general );
    pagesWidget->addWidget( mFilesPage );
    pagesWidget->addWidget( timeline );
    pagesWidget->addWidget( tools );
    pagesWidget->addWidget( shortcuts );

    QPushButton* closeButton = new QPushButton( tr( "Close", "Close button of preference dialog." ) );
    connect( closeButton, &QPushButton::clicked, this, &PreferencesDialog::close );

    createIcons();
    contentsWidget->setCurrentRow( 0 );

    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget( contentsWidget );
    horizontalLayout->addWidget( pagesWidget, 1 );

    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch( 1 );
    buttonsLayout->addWidget( closeButton );

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout( horizontalLayout );
    mainLayout->addStretch( 1 );
    mainLayout->addSpacing( 5 );
    mainLayout->addLayout( buttonsLayout );
    setLayout( mainLayout );
}

void PreferencesDialog::createIcons()
{
    QListWidgetItem* generalButton = new QListWidgetItem(contentsWidget);
    generalButton->setIcon(QIcon(":icons/prefspencil.png"));
    generalButton->setText(tr("General"));
    generalButton->setTextAlignment(Qt::AlignHCenter);
    generalButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem* filesButton = new QListWidgetItem(contentsWidget);
    filesButton->setIcon(QIcon(":icons/prefs-files.png"));
    filesButton->setText(tr("Files"));
    filesButton->setTextAlignment(Qt::AlignHCenter);
    filesButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem* timelineButton = new QListWidgetItem(contentsWidget);
    timelineButton->setIcon(QIcon(":icons/prefstimeline.png"));
    timelineButton->setText(tr("Timeline"));
    timelineButton->setTextAlignment(Qt::AlignHCenter);
    timelineButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem* toolsButton = new QListWidgetItem(contentsWidget);
    toolsButton->setIcon(QIcon(":/icons/prefs-files.png"));
    toolsButton->setText(tr("Tools"));
    toolsButton->setTextAlignment(Qt::AlignHCenter);
    toolsButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem* shortcutsButton = new QListWidgetItem(contentsWidget);
    shortcutsButton->setIcon(QIcon(":/icons/prefs-shortcuts.png"));
    shortcutsButton->setText(tr("Shortcuts"));
    shortcutsButton->setTextAlignment(Qt::AlignHCenter);
    shortcutsButton->setFlags((Qt::ItemIsSelectable | Qt::ItemIsEnabled));

    auto onCurrentItemChanged = static_cast< void ( QListWidget::* )( QListWidgetItem*, QListWidgetItem* ) >( &QListWidget::currentItemChanged );
    connect(contentsWidget, onCurrentItemChanged, this, &PreferencesDialog::changePage);
}

void PreferencesDialog::closeEvent(QCloseEvent *)
{
    done( QDialog::Accepted );
}

void PreferencesDialog::changePage(QListWidgetItem* current, QListWidgetItem* previous)
{
    if (!current)
        current = previous;

    pagesWidget->setCurrentIndex(contentsWidget->row(current));
}

void PreferencesDialog::updateRecentListBtn(bool isEmpty)
{
    if (isEmpty == true)
    {
        emit updateRecentFileListBtn();
    }
}

GeneralPage::GeneralPage(QWidget* parent) : QWidget(parent)
{
    QSettings settings( PENCIL2D, PENCIL2D );
    contents = new QWidget();
    QVBoxLayout* outerLay = new QVBoxLayout(this);
    QVBoxLayout* lay = new QVBoxLayout(contents);
    scrollArea = new QScrollArea;

    QGroupBox* languageBox = new QGroupBox( tr("Language", "GroupBox title in Preference") );
    QGroupBox* windowOpacityBox = new QGroupBox( tr( "Window opacity", "GroupBox title in Preference" ) );
    QGroupBox* backgroundBox = new QGroupBox( tr( "Background", "GroupBox title in Preference" ) );
    QGroupBox* appearanceBox = new QGroupBox( tr( "Appearance", "GroupBox title in Preference" ) );
    QGroupBox* displayBox = new QGroupBox( tr( "Canvas", "GroupBox title in Preference" ) );
    QGroupBox* editingBox = new QGroupBox( tr( "Editing", "GroupBox title in Preference" ) );
    QGroupBox* gridBox = new QGroupBox(tr("Grid", "groupBox title in Preference") );

    mLanguageCombo = new QComboBox;
    mLanguageCombo->addItem( tr( "<System-Language>" ), "" );
    mLanguageCombo->addItem( tr( "Czech" ), "cs" );
    mLanguageCombo->addItem( tr( "Danish" ), "da" );
    mLanguageCombo->addItem( tr( "English" ), "en" );
    mLanguageCombo->addItem( tr( "German" ), "de" );
    mLanguageCombo->addItem( tr( "Spanish" ), "es" );
    mLanguageCombo->addItem( tr( "French" ), "fr" );
    mLanguageCombo->addItem( tr( "Hungarian" ), "hu-HU" );
    mLanguageCombo->addItem( tr( "Italian" ), "it" );
    mLanguageCombo->addItem( tr( "Japanese" ), "ja" );
    mLanguageCombo->addItem( tr( "Portuguese - Brazil" ), "pt-BR" );
    mLanguageCombo->addItem( tr( "Russian" ), "ru" );
    mLanguageCombo->addItem( tr( "Chinese - Taiwan" ), "zh-TW" );

    QLabel* windowOpacityLabel = new QLabel(tr("Opacity"));
    mWindowOpacityLevel = new QSlider(Qt::Horizontal);
    mWindowOpacityLevel->setMinimum(30);
    mWindowOpacityLevel->setMaximum(100);
    int value = settings.value("windowOpacity").toInt();
    mWindowOpacityLevel->setValue( 100 - value );

    mBackgroundButtons = new QButtonGroup();
    QRadioButton* checkerBackgroundButton = new QRadioButton();
    QRadioButton* whiteBackgroundButton = new QRadioButton();
    QRadioButton* greyBackgroundButton = new QRadioButton();
    QRadioButton* dotsBackgroundButton = new QRadioButton();
    QRadioButton* weaveBackgroundButton = new QRadioButton();

    QPixmap previewCheckerboard( ":background/checkerboard.png" );
    QPixmap previewWhite(32,32);
    QPixmap previewGrey(32,32);
    QPixmap previewDots( ":background/dots.png" );
    QPixmap previewWeave( ":background/weave.jpg" );

    previewWhite.fill( Qt::white );

    previewGrey.fill( Qt:: lightGray );

    checkerBackgroundButton->setIcon( previewCheckerboard.scaled(32, 32) );
    whiteBackgroundButton->setIcon( previewWhite );
    greyBackgroundButton->setIcon( previewGrey );
    dotsBackgroundButton->setIcon( previewDots.scaled(32, 32) );
    weaveBackgroundButton->setIcon( previewWeave.scaled(32, 32) );
    mBackgroundButtons->addButton(checkerBackgroundButton);
    mBackgroundButtons->addButton(whiteBackgroundButton);
    mBackgroundButtons->addButton(greyBackgroundButton);
    mBackgroundButtons->addButton(dotsBackgroundButton);
    mBackgroundButtons->addButton(weaveBackgroundButton);
    mBackgroundButtons->setId(checkerBackgroundButton, 1);
    mBackgroundButtons->setId(whiteBackgroundButton, 2);
    mBackgroundButtons->setId(greyBackgroundButton, 3);
    mBackgroundButtons->setId(dotsBackgroundButton, 4);
    mBackgroundButtons->setId(weaveBackgroundButton, 5);

    QHBoxLayout* backgroundLayout = new QHBoxLayout();
    backgroundBox->setLayout(backgroundLayout);
    backgroundLayout->addWidget(checkerBackgroundButton);
    backgroundLayout->addWidget(whiteBackgroundButton);
    backgroundLayout->addWidget(greyBackgroundButton);
    backgroundLayout->addWidget(dotsBackgroundButton);
    backgroundLayout->addWidget(weaveBackgroundButton);

    mShadowsBox = new QCheckBox(tr("Shadows"));
    mToolCursorsBox = new QCheckBox(tr("Tool Cursors"));
    mAntialiasingBox = new QCheckBox(tr("Antialiasing"));
    mDottedCursorBox = new QCheckBox(tr("Dotted Cursor"));

    QGridLayout* langLayout = new QGridLayout;
    languageBox->setLayout( langLayout );
    langLayout->addWidget( mLanguageCombo );


    mGridCheckBox = new QCheckBox(tr("Enable Grid"));

    mGridSizeInput = new QSpinBox();
    mGridSizeInput->setMinimum(1);
    mGridSizeInput->setMaximum(512);
    mGridSizeInput->setFixedWidth(80);
    gridSize = settings.value("gridSize").toInt();
    mGridSizeInput->setValue( gridSize );

    connect( mGridSizeInput, SIGNAL(valueChanged(int)), this, SLOT(gridSizeChange(int)));

    QGridLayout* windowOpacityLayout = new QGridLayout();
    windowOpacityBox->setLayout(windowOpacityLayout);
    windowOpacityLayout->addWidget(windowOpacityLabel, 0, 0);
    windowOpacityLayout->addWidget(mWindowOpacityLevel, 0, 1);

    QVBoxLayout* appearanceLayout = new QVBoxLayout();
    appearanceBox->setLayout(appearanceLayout);
    appearanceLayout->addWidget(mShadowsBox);
    appearanceLayout->addWidget(mToolCursorsBox);
    appearanceLayout->addWidget(mDottedCursorBox);

    QGridLayout* displayLayout = new QGridLayout();
    displayBox->setLayout(displayLayout);
    displayLayout->addWidget(mAntialiasingBox, 0, 0);

    QGridLayout* gridLayout = new QGridLayout();
    gridBox->setLayout(gridLayout);
    gridLayout->addWidget(mGridSizeInput, 0, 0);
    gridLayout->addWidget(mGridCheckBox, 0, 10);

    QLabel* curveSmoothingLabel = new QLabel(tr("Vector curve smoothing"));
    mCurveSmoothingLevel = new QSlider(Qt::Horizontal);
    mCurveSmoothingLevel->setMinimum(1);
    mCurveSmoothingLevel->setMaximum(100);
    value = settings.value("curveSmoothing").toInt();
    mCurveSmoothingLevel->setValue( value );

    mHighResBox = new QCheckBox(tr("Tablet high-resolution position"));

    QGridLayout* editingLayout = new QGridLayout();
    editingBox->setLayout(editingLayout);
    editingLayout->addWidget(curveSmoothingLabel, 0, 0);
    editingLayout->addWidget(mCurveSmoothingLevel, 1, 0);
    editingLayout->addWidget(mHighResBox, 2, 0);

    outerLay->addWidget(scrollArea);
    outerLay->addStretch(1);

    lay->addWidget( languageBox );
    lay->addWidget( windowOpacityBox );
    lay->addWidget( appearanceBox );
    lay->addWidget( backgroundBox );
    lay->addWidget( displayBox );
    lay->addWidget( editingBox );
    lay->addWidget( gridBox );

    scrollArea->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOn);
    scrollArea->setMaximumWidth(400);
    scrollArea->setMinimumWidth(128);
    scrollArea->setWidgetResizable (true);
    scrollArea->setWidget(contents);

    PreferencesDialog* preference = qobject_cast< PreferencesDialog* >( parent );

    auto kButtonClicked = static_cast< void (QButtonGroup::* )( int ) >( &QButtonGroup::buttonClicked );
    auto kCurIndexChagned = static_cast< void( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged );
    connect( mLanguageCombo,      kCurIndexChagned,         this, &GeneralPage::languageChanged );
    connect( mWindowOpacityLevel, &QSlider::valueChanged,   preference, &PreferencesDialog::windowOpacityChange );
    connect( mBackgroundButtons,  kButtonClicked,           this, &GeneralPage::backgroundChange );
    connect( mShadowsBox,         &QCheckBox::stateChanged, this, &GeneralPage::shadowsCheckboxStateChanged );
    connect( mToolCursorsBox,     &QCheckBox::stateChanged, this, &GeneralPage::toolCursorsCheckboxStateChanged );
    connect( mAntialiasingBox,    &QCheckBox::stateChanged, this, &GeneralPage::antiAliasCheckboxStateChanged );
    connect( mCurveSmoothingLevel, &QSlider::valueChanged,  this, &GeneralPage::curveSmoothingChange );
    connect( mHighResBox,         &QCheckBox::stateChanged, this, &GeneralPage::highResCheckboxStateChanged );
    connect( mDottedCursorBox,    &QCheckBox::stateChanged, this, &GeneralPage::dottedCursorCheckboxStateChanged );
    connect( mGridSizeInput, SIGNAL(valueChanged(int)), this, SLOT(gridSizeChange(int)));
    connect( mGridCheckBox,    &QCheckBox::stateChanged, this, &GeneralPage::gridCheckBoxStateChanged );

    setLayout(lay);
}

void GeneralPage::resizeEvent(QResizeEvent* event)
{
    int size = 0;
    if (this->height() < 560 ) {
       size = this->height();
    } else if (this->height() >= 560) {
        size = 560;
    }
    scrollArea->setMinimumHeight(size);
    QWidget::resizeEvent(event);
}


void GeneralPage::updateValues()
{
    int index = mLanguageCombo->findData( mManager->getString( SETTING::LANGUAGE ) );

    if ( index >= 0 )
    {
        mLanguageCombo->blockSignals( true );
        mLanguageCombo->setCurrentIndex( index );
        mLanguageCombo->blockSignals( false );
    }

    mCurveSmoothingLevel->setValue(mManager->getInt(SETTING::CURVE_SMOOTHING));
    mWindowOpacityLevel->setValue(100 - mManager->getInt(SETTING::WINDOW_OPACITY));
    mShadowsBox->setChecked(mManager->isOn(SETTING::SHADOW));
    mToolCursorsBox->setChecked(mManager->isOn(SETTING::TOOL_CURSOR));
    mAntialiasingBox->setChecked(mManager->isOn(SETTING::ANTIALIAS));
    mDottedCursorBox->setChecked(mManager->isOn(SETTING::DOTTED_CURSOR));
    mGridSizeInput->setValue(mManager->getInt(SETTING::GRID_SIZE));
    mGridCheckBox->setChecked(mManager->isOn(SETTING::GRID));

    mHighResBox->setChecked(mManager->isOn(SETTING::HIGH_RESOLUTION));

    QString bgName = mManager->getString(SETTING::BACKGROUND_STYLE);
    if (bgName == "checkerboard") {
        mBackgroundButtons->button(1)->setChecked(true);
    }
    if (bgName == "white") {
        mBackgroundButtons->button(2)->setChecked(true);
    }
    if (bgName == "grey") {
        mBackgroundButtons->button(3)->setChecked(true);
    }
    if (bgName == "dots") {
        mBackgroundButtons->button(4)->setChecked(true);
    }
    if (bgName == "weave") {
        mBackgroundButtons->button(5)->setChecked(true);
    }
}

void GeneralPage::languageChanged( int i )
{
    QString strLocale = mLanguageCombo->itemData( i ).toString();
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

void GeneralPage::highResCheckboxStateChanged( bool b )
{
    mManager->set( SETTING::HIGH_RESOLUTION, b );
}

void GeneralPage::shadowsCheckboxStateChanged( bool b )
{
    mManager->set( SETTING::SHADOW, b );
}

void GeneralPage::antiAliasCheckboxStateChanged( bool b )
{
    mManager->set( SETTING::ANTIALIAS, b );
}

void GeneralPage::toolCursorsCheckboxStateChanged(bool b)
{
    mManager->set( SETTING::TOOL_CURSOR, b );
}

void GeneralPage::dottedCursorCheckboxStateChanged(bool b)
{
    mManager->set( SETTING::DOTTED_CURSOR, b );
}

void GeneralPage::gridSizeChange(int value)
{
    mManager->set(SETTING::GRID_SIZE, value);
}

void GeneralPage::gridCheckBoxStateChanged(bool b)
{
    mManager->set(SETTING::GRID, b);
}

TimelinePage::TimelinePage(QWidget* parent) : QWidget(parent)
{
    QSettings settings( PENCIL2D, PENCIL2D );

    QVBoxLayout* lay = new QVBoxLayout();

    QGroupBox* timeLineBox = new QGroupBox(tr("Timeline"));
    mDrawLabel = new QCheckBox(tr("Draw timeline labels"));
    mFontSize = new QSpinBox();
    QLabel* frameSizeLabel = new QLabel(tr("Frame size"));
    mFrameSize = new QSlider(Qt::Horizontal, this);
    QLabel* lengthSizeLabel = new QLabel(tr("Timeline size in Frames"));
    mLengthSize = new QLineEdit(this);
    QIntValidator* lengthSizeValidator = new QIntValidator(this);
    lengthSizeValidator->setBottom(2);
    mLengthSize->setValidator( lengthSizeValidator );

    mScrubBox = new QCheckBox(tr("Short scrub"));

    mFontSize->setRange(4, 20);
    mFrameSize->setRange(4, 20);

    mFontSize->setFixedWidth(50);
    mLengthSize->setFixedWidth(50);


    mFrameSize->setValue(settings.value("frameSize").toInt());
    if (settings.value("labelFontSize").toInt()==0) mFontSize->setValue(12);
    if (settings.value("frameSize").toInt()==0) mFrameSize->setValue(6);
    mLengthSize->setText(settings.value("length").toString());
    if (settings.value("length").toInt()==0) mLengthSize->setText("240");

    connect(mFontSize, SIGNAL(valueChanged(int)), this, SLOT(fontSizeChange(int)));
    connect(mFrameSize, SIGNAL(valueChanged(int)), this, SLOT(frameSizeChange(int)));
    connect(mLengthSize, SIGNAL(textChanged(QString)), this, SLOT(lengthSizeChange(QString)));
    connect( mDrawLabel, &QCheckBox::stateChanged, this, &TimelinePage::labelChange );
    connect( mScrubBox, &QCheckBox::stateChanged, this, &TimelinePage::scrubChange );

    lay->addWidget(frameSizeLabel);
    lay->addWidget(mFrameSize);
    lay->addWidget(lengthSizeLabel);
    lay->addWidget(mLengthSize);
    lay->addWidget(mScrubBox);
    timeLineBox->setLayout(lay);

    QVBoxLayout* lay2 = new QVBoxLayout();
    lay2->addWidget(timeLineBox);
    lay2->addStretch(1);
    setLayout(lay2);
}

void TimelinePage::updateValues()
{
    mScrubBox->setChecked(mManager->isOn(SETTING::SHORT_SCRUB));
    mDrawLabel->setChecked(mManager->isOn(SETTING::DRAW_LABEL));
    mFontSize->setValue(mManager->getInt(SETTING::LABEL_FONT_SIZE));
    mFrameSize->setValue(mManager->getInt(SETTING::FRAME_SIZE));
    mLengthSize->setText(mManager->getString(SETTING::TIMELINE_SIZE));
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

void TimelinePage::scrubChange(bool value)
{
    mManager->set(SETTING::SHORT_SCRUB, value);
}

FilesPage::FilesPage(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout *lay = new QVBoxLayout();

    QVBoxLayout *clearRecentChangesLay = new QVBoxLayout();

	QGroupBox *autosaveBox = new QGroupBox( tr( "Autosave documents", "Preference" ) );
    mAutosaveCheckBox = new QCheckBox(tr("Enable autosave", "Preference" ));
	QLabel *autosaveNumberLabel = new QLabel( tr( "Number of modifications before autosaving:", "Preference" ) );
    mAutosaveNumberBox = new QSpinBox();

    QGroupBox *clearRecentFilesBox = new QGroupBox(tr("Clear recent files list", "Clear Recent Files (Preference)" ));
    QLabel *clearRecentFilesLbl = new QLabel(tr("This will clear your list of recently opened files", "Clear Recent Files (Preference)" ));
	mClearRecentFilesBtn = new QPushButton( tr( "Clear", "Clear Recent Files (Preference)" ) );

    mAutosaveNumberBox = new QSpinBox();

    mAutosaveNumberBox->setMinimum(5);
    mAutosaveNumberBox->setMaximum(200);
    mAutosaveNumberBox->setFixedWidth(50);

    connect(mAutosaveCheckBox, &QCheckBox::stateChanged, this, &FilesPage::autosaveChange);
    connect(mAutosaveNumberBox, SIGNAL(valueChanged(int)), this, SLOT(autosaveNumberChange(int)));
    connect(mClearRecentFilesBtn, SIGNAL(clicked(bool)), this, SLOT(clearRecentFilesList()));

    lay->addWidget(mAutosaveCheckBox);
    lay->addWidget(autosaveNumberLabel);
    lay->addWidget(mAutosaveNumberBox);
    autosaveBox->setLayout(lay);

    clearRecentChangesLay->addWidget(clearRecentFilesLbl);
    clearRecentChangesLay->addWidget(mClearRecentFilesBtn);
    clearRecentFilesBox->setLayout(clearRecentChangesLay);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(autosaveBox);
    mainLayout->addWidget(clearRecentFilesBox);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

void FilesPage::updateValues()
{
    mAutosaveCheckBox->setChecked(mManager->isOn(SETTING::AUTO_SAVE));
    mAutosaveNumberBox->setValue(mManager->getInt(SETTING::AUTO_SAVE_NUMBER));
}

void FilesPage::updateClearRecentListButton()
{
    mClearRecentFilesBtn->setEnabled(false);
    mClearRecentFilesBtn->setText("List is empty");
}

void FilesPage::autosaveChange(bool b)
{
    mManager->set(SETTING::AUTO_SAVE, b);
}

void FilesPage::autosaveNumberChange(int number)
{
    mManager->set(SETTING::AUTO_SAVE_NUMBER, number);
}

void FilesPage::clearRecentFilesList()
{
    emit clearRecentList();
}

ToolsPage::ToolsPage(QWidget* parent) : QWidget(parent)
{
    QSettings settings( PENCIL2D, PENCIL2D );

    QVBoxLayout* lay = new QVBoxLayout();

    QGroupBox* onionSkinBox = new QGroupBox(tr("Onion skin"));

    QLabel* onionMaxOpacityLabel = new QLabel(tr("Maximum onion opacity %"));
    mOnionMaxOpacityBox = new QSpinBox();
    QLabel* onionMinOpacityLabel = new QLabel(tr("Minimum onion opacity %"));
    mOnionMinOpacityBox = new QSpinBox();
    QLabel* onionPrevFramesNumLabel = new QLabel(tr("Number of previous onion frames shown"));
    mOnionPrevFramesNumBox = new QSpinBox();
    QLabel* onionNextFramesNumLabel = new QLabel(tr("Number of next onion frames shown"));
    mOnionNextFramesNumBox = new QSpinBox();

    mOnionMaxOpacityBox->setMinimum(0);
    mOnionMaxOpacityBox->setMaximum(100);
    mOnionMaxOpacityBox->setFixedWidth(50);
    mOnionMinOpacityBox->setMinimum(0);
    mOnionMinOpacityBox->setMaximum(100);
    mOnionMinOpacityBox->setFixedWidth(50);
    mOnionPrevFramesNumBox->setMinimum(1);
    mOnionPrevFramesNumBox->setMaximum(60);
    mOnionPrevFramesNumBox->setFixedWidth(50);
    mOnionNextFramesNumBox->setMinimum(1);
    mOnionNextFramesNumBox->setMaximum(60);
    mOnionNextFramesNumBox->setFixedWidth(50);

    mOnionMaxOpacityBox->setValue(settings.value( SETTING_ONION_MAX_OPACITY ).toInt());
    mOnionMinOpacityBox->setValue(settings.value( SETTING_ONION_MIN_OPACITY ).toInt());
    mOnionPrevFramesNumBox->setValue(settings.value( SETTING_ONION_PREV_FRAMES_NUM).toInt());
    mOnionNextFramesNumBox->setValue(settings.value( SETTING_ONION_NEXT_FRAMES_NUM ).toInt());

    connect(mOnionMaxOpacityBox, SIGNAL(valueChanged(int)), this, SLOT(onionMaxOpacityChange(int)));
    connect(mOnionMinOpacityBox, SIGNAL(valueChanged(int)), this, SLOT(onionMinOpacityChange(int)));
    connect(mOnionPrevFramesNumBox, SIGNAL(valueChanged(int)), this, SLOT(onionPrevFramesNumChange(int)));
    connect(mOnionNextFramesNumBox, SIGNAL(valueChanged(int)), this, SLOT(onionNextFramesNumChange(int)));

    lay->addWidget(onionMaxOpacityLabel);
    lay->addWidget(mOnionMaxOpacityBox);
    lay->addWidget(onionMinOpacityLabel);
    lay->addWidget(mOnionMinOpacityBox);
    lay->addWidget(onionPrevFramesNumLabel);
    lay->addWidget(mOnionPrevFramesNumBox);
    lay->addWidget(onionNextFramesNumLabel);
    lay->addWidget(mOnionNextFramesNumBox);
    onionSkinBox->setLayout(lay);

    QGroupBox* brushBox = new QGroupBox(tr("Brush Tools"));
    mUseQuickSizingBox = new QCheckBox(tr("Use Quick Sizing"));
    QVBoxLayout* brushBoxLayout = new QVBoxLayout();
    brushBoxLayout->addWidget(mUseQuickSizingBox);

    connect( mUseQuickSizingBox, &QCheckBox::stateChanged, this, &ToolsPage::quickSizingChange );

    brushBox->setLayout(brushBoxLayout);


    QVBoxLayout* lay2 = new QVBoxLayout();
    lay2->addWidget(onionSkinBox);
    lay2->addWidget(brushBox);
    lay2->addStretch(1);
    setLayout(lay2);
}

void ToolsPage::updateValues()
{
    mOnionMaxOpacityBox->setValue(mManager->getInt(SETTING::ONION_MAX_OPACITY));
    mOnionMinOpacityBox->setValue(mManager->getInt(SETTING::ONION_MIN_OPACITY));
    mOnionPrevFramesNumBox->setValue(mManager->getInt(SETTING::ONION_PREV_FRAMES_NUM));
    mOnionNextFramesNumBox->setValue(mManager->getInt(SETTING::ONION_NEXT_FRAMES_NUM));
    mUseQuickSizingBox->setChecked(mManager->isOn(SETTING::QUICK_SIZING));
}

void ToolsPage::onionMaxOpacityChange(int value)
{
    mManager->set(SETTING::ONION_MAX_OPACITY, value);
}

void ToolsPage::quickSizingChange( bool b )
{
    mManager->set(SETTING::QUICK_SIZING, b);
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
