
#include "displayoptionwidget.h"
#include "ui_displayoption.h"

#include <QFrame>
#include <QToolButton>
#include <QGridLayout>
#include "editor.h"
#include "scribblearea.h"


DisplayOptionWidget::DisplayOptionWidget(QWidget *parent) : QDockWidget( parent )
{
    ui = new Ui::DisplayOption;
    ui->setupUi( this );
    //createUI();
    //setWindowTitle(tr("Display Options"));
    //setMaximumHeight(100);
}

void DisplayOptionWidget::createUI()
{
    // Create Display Option Tool Buttons
    QFrame* displayGroup = new QFrame();

    outlinesButton = new QToolButton(displayGroup);
    outlinesButton->setIcon(QIcon(":icons/outlines5.png"));
    outlinesButton->setToolTip(tr("Show outlines only"));
    outlinesButton->setIconSize( QSize(21,21) );

    mirrorButton = new QToolButton(displayGroup);
    mirrorButton->setIcon(QIcon(":icons/mirror.png"));
    mirrorButton->setToolTip(tr("Horizontal flip"));
    mirrorButton->setIconSize( QSize(21,21) );

    mirrorButtonV = new QToolButton(displayGroup);
    mirrorButtonV->setIcon(QIcon(":icons/mirrorV.png"));
    mirrorButtonV->setToolTip(tr("Vertical flip"));
    mirrorButtonV->setIconSize( QSize(21,21) );

    onionPrevButton = new QToolButton(displayGroup);
    onionPrevButton->setIcon(QIcon(":icons/onionPrev.png"));
    onionPrevButton->setToolTip(tr("Onion skin previous frame"));
    onionPrevButton->setIconSize( QSize(21,21) );

    onionNextButton = new QToolButton(displayGroup);
    onionNextButton->setIcon(QIcon(":icons/onionNext.png"));
    onionNextButton->setToolTip(tr("Onion skin next frame"));
    onionNextButton->setIconSize( QSize(21,21) );

    onionBlueButton = new QToolButton(displayGroup);
    onionBlueButton->setIcon(QIcon(":icons/onion-blue.png"));
    onionBlueButton->setToolTip(tr("Onion skin color: blue"));
    onionBlueButton->setIconSize( QSize(21,21) );

    onionRedButton = new QToolButton(displayGroup);
    onionRedButton->setIcon(QIcon(":icons/onion-red.png"));
    onionRedButton->setToolTip(tr("Onion skin color: red"));
    onionRedButton->setIconSize( QSize(21,21) );

    multiLayerOnionSkinButton = new QToolButton(displayGroup);
    multiLayerOnionSkinButton->setText(QString("M"));
    multiLayerOnionSkinButton->setToolTip(tr("enable onionskin on multiple layers"));
    multiLayerOnionSkinButton->setIconSize( QSize(21,21) );

    gridAButton = new QToolButton(displayGroup);
    gridAButton->setIcon(QIcon(":icons/grid-a.png"));
    gridAButton->setToolTip(tr("Grid A - composition"));
    gridAButton->setIconSize( QSize(21,21) );

    thinLinesButton->setCheckable(true);
    thinLinesButton->setChecked(false);
    outlinesButton->setCheckable(true);
    outlinesButton->setChecked(false);
    mirrorButton->setCheckable(true);
    mirrorButton->setChecked(false);
    mirrorButtonV->setCheckable(true);
    mirrorButtonV->setChecked(false);
    onionPrevButton->setCheckable(true);
    onionPrevButton->setChecked(true);
    onionNextButton->setCheckable(true);
    onionNextButton->setChecked(false);
    onionBlueButton->setCheckable(true);
    onionBlueButton->setChecked(true);
    onionRedButton->setCheckable(true);
    onionRedButton->setChecked(false);
    gridAButton->setCheckable(true);
    gridAButton->setChecked(false);

    multiLayerOnionSkinButton->setCheckable(true);
    multiLayerOnionSkinButton->setChecked(true);


    QGridLayout* layout = new QGridLayout();
    layout->setMargin(4);
    layout->setSpacing(0);
    layout->addWidget(mirrorButton,0,0);
    layout->addWidget(onionPrevButton,1,2);
    layout->addWidget(onionNextButton,0,2);
    layout->addWidget(onionBlueButton,0,3);
    layout->addWidget(mirrorButtonV,1,0);
    layout->addWidget(outlinesButton,1,1);
    layout->addWidget(thinLinesButton,0,1);
    layout->addWidget(onionRedButton,1,3);
    layout->addWidget(gridAButton,2,0);
    layout->addWidget(multiLayerOnionSkinButton,2,2);


    displayGroup->setLayout(layout);

    setWidget(displayGroup);
}

void DisplayOptionWidget::makeConnectionToEditor(Editor* editor)
{
	ScribbleArea* pScriArea = editor->getScribbleArea();

	connect( ui->thinLinesButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleThinLines);
	connect( outlinesButton, &QToolButton::clicked,  pScriArea, &ScribbleArea::toggleOutlines);
    connect(onionPrevButton, &QToolButton::clicked, editor, &Editor::toggleOnionPrev);
	connect(onionNextButton, &QToolButton::clicked, editor, &Editor::toggleOnionNext);
	connect(onionBlueButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleOnionBlue );
	connect(onionRedButton, &QToolButton::clicked,  pScriArea, &ScribbleArea::toggleOnionRed );
	connect(mirrorButton, &QToolButton::clicked, editor, &Editor::toggleMirror);
	connect(mirrorButtonV, &QToolButton::clicked, editor, &Editor::toggleMirrorV);
	connect(gridAButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleGridA);
	connect(multiLayerOnionSkinButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleMultiLayerOnionSkin);

    connect(editor, &Editor::changeOutlinesButton, this, &DisplayOptionWidget::changeOutlinesButton );
    connect(editor, &Editor::changeThinLinesButton, this, &DisplayOptionWidget::changeThinLinesButton );
    connect(editor, &Editor::onionPrevChanged, this, &DisplayOptionWidget::onionPrevChanged );
    connect(editor, &Editor::onionNextChanged, this, &DisplayOptionWidget::onionNextChanged );
}


// ### public slots ###

void DisplayOptionWidget::changeOutlinesButton(bool bIsChecked)
{
    outlinesButton->setChecked(bIsChecked);
}

void DisplayOptionWidget::changeThinLinesButton(bool bIsChecked)
{
    thinLinesButton->setChecked(bIsChecked);
}

void DisplayOptionWidget::onionPrevChanged(bool checked)
{
    onionPrevButton->setChecked(checked);
}

void DisplayOptionWidget::changeMultiLayerOnionSkin(bool bIsChecked)
{
    multiLayerOnionSkinButton->setChecked(bIsChecked);
}

void DisplayOptionWidget::multiLayerOnionSkinButtonChanged(bool checked)
{
    multiLayerOnionSkinButton->setChecked(checked);
}

void DisplayOptionWidget::onionNextChanged(bool checked)
{
    onionNextButton->setChecked(checked);
}