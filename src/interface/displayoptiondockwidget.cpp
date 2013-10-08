
#include <QFrame>
#include <QToolButton>
#include <QGridLayout>
#include "editor.h"

#include "displayoptiondockwidget.h"


DisplayOptionDockWidget::DisplayOptionDockWidget(QWidget *parent) :
    QDockWidget(parent)
{
    createUI();
    setWindowTitle(tr("Display Options"));
    setMaximumHeight(60);
}

void DisplayOptionDockWidget::createUI()
{
    // Create Display Option Tool Buttons
    QFrame* displayGroup = new QFrame();

    thinLinesButton = new QToolButton(displayGroup);
    thinLinesButton->setIcon(QIcon(":icons/thinlines5.png"));
    thinLinesButton->setToolTip("Show invisible lines");
    thinLinesButton->setIconSize( QSize(21,21) );

    outlinesButton = new QToolButton(displayGroup);
    outlinesButton->setIcon(QIcon(":icons/outlines5.png"));
    outlinesButton->setToolTip("Show outlines only");
    outlinesButton->setIconSize( QSize(21,21) );

    mirrorButton = new QToolButton(displayGroup);
    mirrorButton->setIcon(QIcon(":icons/mirror.png"));
    mirrorButton->setToolTip("Horizontal flip");
    mirrorButton->setIconSize( QSize(21,21) );

    mirrorButtonV = new QToolButton(displayGroup);
    mirrorButtonV->setIcon(QIcon(":icons/mirrorV.png"));
    mirrorButtonV->setToolTip("Vertical flip");
    mirrorButtonV->setIconSize( QSize(21,21) );

    onionPrevButton = new QToolButton(displayGroup);
    onionPrevButton->setIcon(QIcon(":icons/onionPrev.png"));
    onionPrevButton->setToolTip("Onion skin previous frame");
    onionPrevButton->setIconSize( QSize(21,21) );

    onionNextButton = new QToolButton(displayGroup);
    onionNextButton->setIcon(QIcon(":icons/onionNext.png"));
    onionNextButton->setToolTip("Onion skin next frame");
    onionNextButton->setIconSize( QSize(21,21) );

    gridAButton = new QToolButton(displayGroup);
    gridAButton->setIcon(QIcon(":icons/grid-a.png"));
    gridAButton->setToolTip("Grid A - composition");
    gridAButton->setIconSize( QSize(21,21) );

    gridBButton = new QToolButton(displayGroup);
    gridBButton->setIcon(QIcon(":icons/grid-b.png"));
    gridBButton->setToolTip("Grid B - perspective");
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
    gridAButton->setCheckable(true);
    gridAButton->setChecked(false);
    gridBButton->setCheckable(true);
    gridBButton->setChecked(false);

    QGridLayout* layout = new QGridLayout();
    layout->setMargin(4);
    layout->setSpacing(0);
    layout->addWidget(mirrorButton,0,0);
    layout->addWidget(mirrorButtonV,1,0);
    layout->addWidget(thinLinesButton,0,1);
    layout->addWidget(outlinesButton,1,1);
    layout->addWidget(onionPrevButton,0,2);
    layout->addWidget(onionNextButton,1,2);
    layout->addWidget(gridAButton,0,3);
    layout->addWidget(gridBButton,1,3);

    displayGroup->setLayout(layout);

    setWidget(displayGroup);
}

void DisplayOptionDockWidget::makeConnectionToEditor(Editor* editor)
{
    connect(thinLinesButton, SIGNAL(clicked()), editor->getScribbleArea(), SLOT(toggleThinLines()));
    connect(outlinesButton, SIGNAL(clicked()), editor->getScribbleArea(), SLOT(toggleOutlines()));
    connect(onionPrevButton, SIGNAL(clicked(bool)), editor, SIGNAL(toggleOnionPrev(bool)));
    connect(onionNextButton, SIGNAL(clicked(bool)), editor, SIGNAL(toggleOnionNext(bool)));
    connect(mirrorButton, SIGNAL(clicked()), editor, SLOT(toggleMirror()));
    connect(mirrorButtonV, SIGNAL(clicked()), editor, SLOT(toggleMirrorV()));
    connect(gridAButton, SIGNAL(clicked(bool)), editor->getScribbleArea(), SLOT(toggleGridA(bool)));
    connect(gridBButton, SIGNAL(clicked(bool)), editor->getScribbleArea(), SLOT(toggleGridB(bool)));

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

void DisplayOptionDockWidget::onionNextChanged(bool checked)
{
    onionNextButton->setChecked(checked);
}
