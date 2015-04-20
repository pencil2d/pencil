/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2015 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include <QListWidget>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QGroupBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include "preferences.h"
#include "scribblearea.h"
#include "shortcutspage.h"


Preferences::Preferences( QWidget* parent ) : QDialog(parent)
{
    contentsWidget = new QListWidget;
    contentsWidget->setViewMode(QListView::IconMode);
    contentsWidget->setIconSize(QSize(96, 84));
    contentsWidget->setMovement(QListView::Static);
    contentsWidget->setMaximumWidth(128);
    contentsWidget->setSpacing(12);

    pagesWidget = new QStackedWidget;
    pagesWidget->addWidget(new GeneralPage(this));
    pagesWidget->addWidget(new FilesPage(this));
    pagesWidget->addWidget(new TimelinePage(this));
    pagesWidget->addWidget(new ToolsPage(this));
    pagesWidget->addWidget(new ShortcutsPage(this));

    QPushButton* closeButton = new QPushButton(tr("Close"));
    connect( closeButton, &QPushButton::clicked, this, &Preferences::close );

    createIcons();
    contentsWidget->setCurrentRow(0);
    
    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(contentsWidget);
    horizontalLayout->addWidget(pagesWidget, 1);

    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(closeButton);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Preferences"));
}

Preferences::~Preferences()
{
}

void Preferences::createIcons()
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

    connect(contentsWidget,
            SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this,
            SLOT(changePage(QListWidgetItem*, QListWidgetItem*)));
}

void Preferences::closeEvent(QCloseEvent *)
{

    this->deleteLater();
}

void Preferences::changePage(QListWidgetItem* current, QListWidgetItem* previous)
{
    if (!current)
        current = previous;

    pagesWidget->setCurrentIndex(contentsWidget->row(current));
}

GeneralPage::GeneralPage(QWidget* parent) : QWidget(parent)
{
    QSettings settings("Pencil","Pencil");
    QVBoxLayout* lay = new QVBoxLayout();

    QGroupBox* windowOpacityBox = new QGroupBox(tr("Window opacity"));
    QGroupBox* backgroundBox = new QGroupBox(tr("Background"));
    QGroupBox* appearanceBox = new QGroupBox(tr("Appearance"));
    QGroupBox* displayBox = new QGroupBox(tr("Rendering"));
    QGroupBox* editingBox = new QGroupBox(tr("Editing"));

    QLabel* windowOpacityLabel = new QLabel(tr("Opacity"));
    QSlider* windowOpacityLevel = new QSlider(Qt::Horizontal);
    windowOpacityLevel->setMinimum(30);
    windowOpacityLevel->setMaximum(100);
    int value = settings.value("windowOpacity").toInt();
    windowOpacityLevel->setValue( 100 - value );

    QButtonGroup* backgroundButtons = new QButtonGroup();
    QRadioButton* checkerBackgroundButton = new QRadioButton();
    QRadioButton* whiteBackgroundButton = new QRadioButton();
    QRadioButton* greyBackgroundButton = new QRadioButton();
    QRadioButton* dotsBackgroundButton = new QRadioButton();
    QRadioButton* weaveBackgroundButton = new QRadioButton();
    QPixmap previewCheckerboard(32,32);
    QPixmap previewWhite(32,32);
    QPixmap previewGrey(32,32);
    QPixmap previewDots(32,32);
    QPixmap previewWeave(32,32);
    QPainter painter(&previewCheckerboard);
    painter.fillRect( QRect(0,0,32,32), ScribbleArea::getBackgroundBrush("checkerboard") );
    painter.end();
    painter.begin(&previewDots);
    painter.fillRect( QRect(0,0,32,32), ScribbleArea::getBackgroundBrush("dots") );
    painter.end();
    painter.begin(&previewWeave);
    painter.fillRect( QRect(0,0,32,32), ScribbleArea::getBackgroundBrush("weave") );
    painter.end();
    previewWhite.fill( Qt::white );
    previewGrey.fill( Qt:: lightGray );
    checkerBackgroundButton->setIcon( previewCheckerboard );
    whiteBackgroundButton->setIcon( previewWhite );
    greyBackgroundButton->setIcon( previewGrey );
    dotsBackgroundButton->setIcon( previewDots );
    dotsBackgroundButton->setIcon( previewWeave );
    backgroundButtons->addButton(checkerBackgroundButton);
    backgroundButtons->addButton(whiteBackgroundButton);
    backgroundButtons->addButton(greyBackgroundButton);
    backgroundButtons->addButton(dotsBackgroundButton);
    backgroundButtons->addButton(weaveBackgroundButton);
    backgroundButtons->setId(checkerBackgroundButton, 1);
    backgroundButtons->setId(whiteBackgroundButton, 2);
    backgroundButtons->setId(greyBackgroundButton, 3);
    backgroundButtons->setId(dotsBackgroundButton, 4);
    backgroundButtons->setId(weaveBackgroundButton, 5);
    QHBoxLayout* backgroundLayout = new QHBoxLayout();
    backgroundBox->setLayout(backgroundLayout);
    backgroundLayout->addWidget(checkerBackgroundButton);
    backgroundLayout->addWidget(whiteBackgroundButton);
    backgroundLayout->addWidget(greyBackgroundButton);
    backgroundLayout->addWidget(dotsBackgroundButton);
    backgroundLayout->addWidget(weaveBackgroundButton);
    if ( settings.value("background").toString() == "checkerboard" ) checkerBackgroundButton->setChecked(true);
    if ( settings.value("background").toString() == "white" ) whiteBackgroundButton->setChecked(true);
    if ( settings.value("background").toString() == "grey" ) greyBackgroundButton->setChecked(true);
    if ( settings.value("background").toString() == "dots" ) dotsBackgroundButton->setChecked(true);
    if ( settings.value("background").toString() == "weave" ) weaveBackgroundButton->setChecked(true);

    QCheckBox* shadowsBox = new QCheckBox(tr("Shadows"));
    shadowsBox->setChecked(false); // default
    if (settings.value("shadows").toString()=="true") shadowsBox->setChecked(true);

    QCheckBox* toolCursorsBox = new QCheckBox(tr("Tool Cursors"));
    toolCursorsBox->setChecked(true); // default
    if (settings.value("toolCursors").toString()=="false") toolCursorsBox->setChecked(false);

    QCheckBox* antialiasingBox = new QCheckBox(tr("Antialiasing"));
    antialiasingBox->setChecked(true); // default
    if (settings.value("antialiasing").toString()=="false") antialiasingBox->setChecked(false);

    QGridLayout* windowOpacityLayout = new QGridLayout();
    windowOpacityBox->setLayout(windowOpacityLayout);
    windowOpacityLayout->addWidget(windowOpacityLabel, 0, 0);
    windowOpacityLayout->addWidget(windowOpacityLevel, 0, 1);

    QVBoxLayout* appearanceLayout = new QVBoxLayout();
    appearanceBox->setLayout(appearanceLayout);
    appearanceLayout->addWidget(shadowsBox);
    appearanceLayout->addWidget(toolCursorsBox);

    QGridLayout* displayLayout = new QGridLayout();
    displayBox->setLayout(displayLayout);
    displayLayout->addWidget(antialiasingBox, 0, 0);

    QLabel* curveSmoothingLabel = new QLabel(tr("Vector curve smoothing"));
    QSlider* curveSmoothingLevel = new QSlider(Qt::Horizontal);
    curveSmoothingLevel->setMinimum(1);
    curveSmoothingLevel->setMaximum(100);
    value = settings.value("curveSmoothing").toInt();
    curveSmoothingLevel->setValue( value );

    QCheckBox* highResBox = new QCheckBox(tr("Tablet high-resolution position"));
    if (settings.value(SETTING_HIGH_RESOLUTION) == "true")
    {
        highResBox->setChecked(true);
    }
    else
    {
        highResBox->setChecked(false);
    }

    QGridLayout* editingLayout = new QGridLayout();
    editingBox->setLayout(editingLayout);
    editingLayout->addWidget(curveSmoothingLabel, 0, 0);
    editingLayout->addWidget(curveSmoothingLevel, 1, 0);
    editingLayout->addWidget(highResBox, 2, 0);

    lay->addWidget(windowOpacityBox);
    lay->addWidget(appearanceBox);
    lay->addWidget(backgroundBox);
    lay->addWidget(displayBox);
    lay->addWidget(editingBox);

    Preferences* preference = qobject_cast< Preferences* >( parent );

    auto kButtonClicked = static_cast< void (QButtonGroup::* )( int ) >( &QButtonGroup::buttonClicked );
    connect( windowOpacityLevel, &QSlider::valueChanged, preference, &Preferences::windowOpacityChange );
    connect( backgroundButtons,  kButtonClicked,         preference, &Preferences::backgroundChange );
    connect( shadowsBox,         &QCheckBox::stateChanged, preference, &Preferences::shadowsChange );
    connect( toolCursorsBox,     &QCheckBox::stateChanged, preference, &Preferences::toolCursorsChange );
    connect( antialiasingBox,    &QCheckBox::stateChanged, preference, &Preferences::antialiasingChange );
    connect( curveSmoothingLevel, &QSlider::valueChanged, preference, &Preferences::curveSmoothingChange );
    connect( highResBox,         &QCheckBox::stateChanged, preference, &Preferences::highResPositionChange );

    setLayout(lay);
}


TimelinePage::TimelinePage(QWidget* parent) : QWidget(parent)
{
    QSettings settings("Pencil","Pencil");

    QVBoxLayout* lay = new QVBoxLayout();

    QGroupBox* timeLineBox = new QGroupBox(tr("Timeline"));
    QCheckBox* drawLabel = new QCheckBox(tr("Draw timeline labels"));
    QSpinBox* fontSize = new QSpinBox();
    QLabel* frameSizeLabel = new QLabel(tr("Frame size in Pixels"));
    QSpinBox* frameSize = new QSpinBox(this);
    QLabel* lengthSizeLabel = new QLabel(tr("Timeline size in Frames"));
    QLineEdit* lengthSize = new QLineEdit(this);
    lengthSize->setInputMask("0009");

    QCheckBox* scrubBox = new QCheckBox(tr("Short scrub"));
    scrubBox->setChecked(false); // default
    if (settings.value("shortScrub").toBool()) scrubBox->setChecked(true);

    fontSize->setMinimum(4);
    fontSize->setMaximum(20);
    frameSize->setMinimum(4);
    frameSize->setMaximum(20);

    fontSize->setFixedWidth(50);
    frameSize->setFixedWidth(50);
    lengthSize->setFixedWidth(50);

    if (settings.value("drawLabel")=="false") drawLabel->setChecked(false);
    else drawLabel->setChecked(true);
    fontSize->setValue(settings.value("labelFontSize").toInt());
    frameSize->setValue(settings.value("frameSize").toInt());
    if (settings.value("labelFontSize").toInt()==0) fontSize->setValue(12);
    if (settings.value("frameSize").toInt()==0) frameSize->setValue(6);
    lengthSize->setText(settings.value("length").toString());
    if (settings.value("length").toInt()==0) lengthSize->setText("240");

    connect(fontSize, SIGNAL(valueChanged(int)), parent, SIGNAL(fontSizeChange(int)));
    connect(frameSize, SIGNAL(valueChanged(int)), parent, SIGNAL(frameSizeChange(int)));
    connect(lengthSize, SIGNAL(textChanged(QString)), parent, SIGNAL(lengthSizeChange(QString)));
    connect(drawLabel, SIGNAL(stateChanged(int)), parent, SIGNAL(labelChange(int)));
    connect(scrubBox, SIGNAL(stateChanged(int)), parent, SIGNAL(scrubChange(int)));

    lay->addWidget(frameSizeLabel);
    lay->addWidget(frameSize);
    lay->addWidget(lengthSizeLabel);
    lay->addWidget(lengthSize);
    lay->addWidget(scrubBox);
    timeLineBox->setLayout(lay);

    QVBoxLayout* lay2 = new QVBoxLayout();
    lay2->addWidget(timeLineBox);
    lay2->addStretch(1);
    setLayout(lay2);
}

FilesPage::FilesPage(QWidget* parent) : QWidget(parent)
{
    QSettings settings("Pencil","Pencil");

    QVBoxLayout* lay = new QVBoxLayout();

    QGroupBox* autosaveBox = new QGroupBox(tr("Autosave documents"));
    QCheckBox* autosaveCheckBox = new QCheckBox(tr("Enable autosave"));
    QLabel* autosaveNumberLabel = new QLabel(tr("Number of modifications before autosaving:"));
    QSpinBox* autosaveNumberBox = new QSpinBox();

    autosaveNumberBox->setMinimum(5);
    autosaveNumberBox->setMaximum(200);
    autosaveNumberBox->setFixedWidth(50);

    autosaveCheckBox->setChecked(false);
    if (settings.value("autosave")=="true") autosaveCheckBox->setChecked(true);

    autosaveNumberBox->setValue(settings.value("autosaveNumber").toInt());
    if (settings.value("autosaveNumber").toInt()==0) autosaveNumberBox->setValue(20);

    connect(autosaveNumberBox, SIGNAL(valueChanged(int)), parent, SIGNAL(autosaveNumberChange(int)));
    connect(autosaveCheckBox, SIGNAL(stateChanged(int)), parent, SIGNAL(autosaveChange(int)));

    lay->addWidget(autosaveCheckBox);
    lay->addWidget(autosaveNumberLabel);
    lay->addWidget(autosaveNumberBox);
    autosaveBox->setLayout(lay);

    QVBoxLayout* lay2 = new QVBoxLayout();
    lay2->addWidget(autosaveBox);
    lay2->addStretch(1);
    setLayout(lay2);
}

ToolsPage::ToolsPage(QWidget* parent) : QWidget(parent)
{
    QSettings settings("Pencil","Pencil");

    QVBoxLayout* lay = new QVBoxLayout();

    QGroupBox* onionSkinBox = new QGroupBox(tr("Onion skin"));
    
    QLabel* onionMaxOpacityLabel = new QLabel(tr("Maximum onion opacity %"));
    QSpinBox* onionMaxOpacityBox = new QSpinBox();
    QLabel* onionMinOpacityLabel = new QLabel(tr("Minimum onion opacity %"));
    QSpinBox* onionMinOpacityBox = new QSpinBox();
    QLabel* onionPrevFramesNumLabel = new QLabel(tr("Number of previous onion frames shown"));
    QSpinBox* onionPrevFramesNumBox = new QSpinBox();
    QLabel* onionNextFramesNumLabel = new QLabel(tr("Number of next onion frames shown"));
    QSpinBox* onionNextFramesNumBox = new QSpinBox();

    onionMaxOpacityBox->setMinimum(0);
    onionMaxOpacityBox->setMaximum(100);
    onionMaxOpacityBox->setFixedWidth(50);
    onionMinOpacityBox->setMinimum(0);
    onionMinOpacityBox->setMaximum(100);
    onionMinOpacityBox->setFixedWidth(50);
    onionPrevFramesNumBox->setMinimum(1);
    onionPrevFramesNumBox->setMaximum(60);
    onionPrevFramesNumBox->setFixedWidth(50);
    onionNextFramesNumBox->setMinimum(1);
    onionNextFramesNumBox->setMaximum(60);
    onionNextFramesNumBox->setFixedWidth(50);

    onionMaxOpacityBox->setValue(settings.value( SETTING_ONION_MAX_OPACITY ).toInt());
    onionMinOpacityBox->setValue(settings.value( SETTING_ONION_MIN_OPACITY ).toInt());
    onionPrevFramesNumBox->setValue(settings.value( SETTING_ONION_PREV_FRAMES_NUM).toInt());
    onionNextFramesNumBox->setValue(settings.value( SETTING_ONION_NEXT_FRAMES_NUM ).toInt());

    connect(onionMaxOpacityBox, SIGNAL(valueChanged(int)), parent, SIGNAL(onionMaxOpacityChange(int)));
    connect(onionMinOpacityBox, SIGNAL(valueChanged(int)), parent, SIGNAL(onionMinOpacityChange(int)));
    connect(onionPrevFramesNumBox, SIGNAL(valueChanged(int)), parent, SIGNAL(onionPrevFramesNumChange(int)));
    connect(onionNextFramesNumBox, SIGNAL(valueChanged(int)), parent, SIGNAL(onionNextFramesNumChange(int)));

    lay->addWidget(onionMaxOpacityLabel);
    lay->addWidget(onionMaxOpacityBox);
    lay->addWidget(onionMinOpacityLabel);
    lay->addWidget(onionMinOpacityBox);
    lay->addWidget(onionPrevFramesNumLabel);
    lay->addWidget(onionPrevFramesNumBox);
    lay->addWidget(onionNextFramesNumLabel);
    lay->addWidget(onionNextFramesNumBox);
    onionSkinBox->setLayout(lay);

    QVBoxLayout* lay2 = new QVBoxLayout();
    lay2->addWidget(onionSkinBox);
    lay2->addStretch(1);
    setLayout(lay2);
}
