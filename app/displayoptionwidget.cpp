
#include "displayoptionwidget.h"
#include "ui_displayoption.h"

#include <QFrame>
#include <QToolButton>
#include <QGridLayout>
#include "editor.h"
#include "scribblearea.h"


DisplayOptionWidget::DisplayOptionWidget(QWidget *parent) : QDockWidget( parent )
{
    QWidget* pCentralWidget = new QWidget( this );
    ui = new Ui::DisplayOption;
    ui->setupUi( pCentralWidget );
    setWidget( pCentralWidget );
    //createUI();
    //setWindowTitle(tr("Display Options"));
    //setMaximumHeight(100);
}

void DisplayOptionWidget::createUI()
{
    // Create Display Option Tool Buttons
    return;
    QFrame* displayGroup = new QFrame();

    multiLayerOnionSkinButton = new QToolButton(displayGroup);
    multiLayerOnionSkinButton->setText(QString("M"));
    multiLayerOnionSkinButton->setToolTip(tr("enable onionskin on multiple layers"));
    multiLayerOnionSkinButton->setIconSize( QSize(21,21) );

    gridAButton = new QToolButton(displayGroup);
    gridAButton->setIcon(QIcon(":icons/grid-a.png"));
    gridAButton->setToolTip(tr("Grid A - composition"));
    gridAButton->setIconSize( QSize(21,21) );

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
    layout->addWidget(onionBlueButton,0,3);
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
	connect( ui->outLinesButton, &QToolButton::clicked,  pScriArea, &ScribbleArea::toggleOutlines);
    connect( ui->onionPrevButton, &QToolButton::clicked, editor, &Editor::toggleOnionPrev);
	connect( ui->onionNextButton, &QToolButton::clicked, editor, &Editor::toggleOnionNext);
	connect( ui->onionBlueButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleOnionBlue );
	connect( ui->onionRedButton, &QToolButton::clicked,  pScriArea, &ScribbleArea::toggleOnionRed );
	connect( ui->mirrorButton, &QToolButton::clicked, editor, &Editor::toggleMirror);
	connect( ui->mirrorVButton, &QToolButton::clicked, editor, &Editor::toggleMirrorV);
    // FIXME
	//connect(gridAButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleGridA);
	//connect(multiLayerOnionSkinButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleMultiLayerOnionSkin);
}


// ### public slots ###

void DisplayOptionWidget::changeMultiLayerOnionSkin(bool bIsChecked)
{
    multiLayerOnionSkinButton->setChecked(bIsChecked);
}

void DisplayOptionWidget::multiLayerOnionSkinButtonChanged(bool checked)
{
    multiLayerOnionSkinButton->setChecked(checked);
}
