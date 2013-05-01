
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

    onionPrev = new QToolButton(displayGroup);
    onionPrev->setIcon(QIcon(":icons/onionPrev.png"));
    onionPrev->setToolTip("Onion skin previous frame");
    onionPrev->setIconSize( QSize(21,21) );

    onionNext = new QToolButton(displayGroup);
    onionNext->setIcon(QIcon(":icons/onionNext.png"));
    onionNext->setToolTip("Onion skin next frame");
    onionNext->setIconSize( QSize(21,21) );

    thinLinesButton->setCheckable(true);
    thinLinesButton->setChecked(false);
    outlinesButton->setCheckable(true);
    outlinesButton->setChecked(false);
    mirrorButton->setCheckable(true);
    mirrorButton->setChecked(false);
    mirrorButtonV->setCheckable(true);
    mirrorButtonV->setChecked(false);
    onionPrev->setCheckable(true);
    onionPrev->setChecked(true);
    onionNext->setCheckable(true);
    onionNext->setChecked(false);

    QGridLayout* layout = new QGridLayout();
    layout->setMargin(4);
    layout->setSpacing(0);
    layout->addWidget(mirrorButton,0,0);
    layout->addWidget(thinLinesButton,0,1);
    layout->addWidget(outlinesButton,1,1);
    layout->addWidget(mirrorButtonV,1,0);
    layout->addWidget(onionPrev,0,2);
    layout->addWidget(onionNext,1,2);

    displayGroup->setLayout(layout);

    setWidget(displayGroup);
}

void makeConnectionToEditor(Editor* editor)
{

}
