
#include "displayoptiondockwidget.h"

#include <QFrame>
#include <QToolButton>
#include <QGridLayout>
#include "editor.h"
#include "scribblearea.h"


DisplayOptionDockWidget::DisplayOptionDockWidget(QWidget *parent) :
    QDockWidget(parent)
{
    createUI();
    setWindowTitle(tr("Display Options"));
    setMaximumHeight(100);
}

void DisplayOptionDockWidget::createUI()
{
    // Create Display Option Tool Buttons
    QFrame* displayGroup = new QFrame();

    thinLinesButton = new QToolButton(displayGroup);
    thinLinesButton->setIcon(QIcon(":icons/thinlines5.png"));
    thinLinesButton->setToolTip(tr("Show invisible lines"));
    thinLinesButton->setIconSize( QSize(21,21) );

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

    gridBButton = new QToolButton(displayGroup);
    gridBButton->setIcon(QIcon(":icons/grid-b.png"));
    gridBButton->setToolTip(tr("Grid B - perspective"));
    gridBButton->setIconSize( QSize(21,21) );

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
    gridBButton->setCheckable(true);
    gridBButton->setChecked(false);
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
    //layout->addWidget(onionBlueNextButton,0,4);hide until separate onionskin colors works
    //layout->addWidget(onionRedNextButton,1,4);
    layout->addWidget(gridAButton,2,0);
    layout->addWidget(gridBButton,2,1);
    layout->addWidget(multiLayerOnionSkinButton,2,2);


    displayGroup->setLayout(layout);

    setWidget(displayGroup);
}

void DisplayOptionDockWidget::makeConnectionToEditor(Editor* editor)
{
    connect(thinLinesButton, &QToolButton::clicked, editor->getScribbleArea(), &ScribbleArea::toggleThinLines );
    connect(outlinesButton, &QToolButton::clicked, editor->getScribbleArea(), &ScribbleArea::toggleOutlines);
    connect(onionPrevButton, &QToolButton::clicked, editor, &Editor::toggleOnionPrev);
    connect(onionNextButton, SIGNAL(clicked(bool)), editor, SIGNAL(toggleOnionNext(bool)));
    connect(onionBlueButton, SIGNAL(clicked(bool)), editor->getScribbleArea(), SLOT(toggleOnionBlue(bool)));
    connect(onionRedButton, SIGNAL(clicked(bool)), editor->getScribbleArea(), SLOT(toggleOnionRed(bool)));
    connect(mirrorButton, SIGNAL(clicked()), editor, SLOT(toggleMirror()));
    connect(mirrorButtonV, SIGNAL(clicked()), editor, SLOT(toggleMirrorV()));
    connect(gridAButton, SIGNAL(clicked(bool)), editor->getScribbleArea(), SLOT(toggleGridA(bool)));
    connect(gridBButton, SIGNAL(clicked(bool)), editor->getScribbleArea(), SLOT(toggleGridB(bool)));
    connect(multiLayerOnionSkinButton,SIGNAL(clicked(bool)), editor->getScribbleArea(),SLOT(toggleMultiLayerOnionSkin(bool)));

    connect(editor, SIGNAL(changeOutlinesButton(bool)), this, SLOT(changeOutlinesButton(bool)));
    connect(editor, SIGNAL(changeThinLinesButton(bool)), this, SLOT(changeThinLinesButton(bool)));
    connect(editor, SIGNAL(onionPrevChanged(bool)), this, SLOT(onionPrevChanged(bool)));
    connect(editor, SIGNAL(onionNextChanged(bool)), this, SLOT(onionNextChanged(bool)));
}


// ### public slots ###

void DisplayOptionDockWidget::changeOutlinesButton(bool bIsChecked)
{
    outlinesButton->setChecked(bIsChecked);
}

void DisplayOptionDockWidget::changeThinLinesButton(bool bIsChecked)
{
    thinLinesButton->setChecked(bIsChecked);
}

void DisplayOptionDockWidget::onionPrevChanged(bool checked)
{
    onionPrevButton->setChecked(checked);
}

void DisplayOptionDockWidget::changeMultiLayerOnionSkin(bool bIsChecked)
{
    multiLayerOnionSkinButton->setChecked(bIsChecked);
}

void DisplayOptionDockWidget::multiLayerOnionSkinButtonChanged(bool checked)
{
    multiLayerOnionSkinButton->setChecked(checked);
}

void DisplayOptionDockWidget::onionNextChanged(bool checked)
{
    onionNextButton->setChecked(checked);
}
