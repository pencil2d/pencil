
#include "displayoptionwidget.h"
#include "ui_displayoption.h"

#include <QFrame>
#include <QToolButton>
#include <QGridLayout>
#include "editor.h"
#include "scribblearea.h"


DisplayOptionWidget::DisplayOptionWidget(QWidget *parent) : QDockWidget( parent )
{
    QWidget* innerWidget = new QWidget;
    setWidget( innerWidget );

    ui = new Ui::DisplayOption;
    ui->setupUi( innerWidget );



    setWindowTitle(tr("Display Options"));
}

void DisplayOptionWidget::makeConnectionToEditor(Editor* editor)
{
	ScribbleArea* pScriArea = editor->getScribbleArea();

	connect( ui->thinLinesButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleThinLines);
	connect( ui->outLinesButton,  &QToolButton::clicked, pScriArea, &ScribbleArea::toggleOutlines);
    connect( ui->onionPrevButton, &QToolButton::clicked, editor, &Editor::toggleOnionPrev);
	connect( ui->onionNextButton, &QToolButton::clicked, editor, &Editor::toggleOnionNext);
	connect( ui->onionBlueButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleOnionBlue );
	connect( ui->onionRedButton,  &QToolButton::clicked, pScriArea, &ScribbleArea::toggleOnionRed );
	connect( ui->mirrorButton,    &QToolButton::clicked, editor, &Editor::toggleMirror);
	connect( ui->mirrorVButton,   &QToolButton::clicked, editor, &Editor::toggleMirrorV);
    connect( ui->cameraBorderButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleCameraBorder);

    ui->cameraBorderButton->setChecked(pScriArea->isEffectOn(EFFECT_CAMERABORDER));

    // FIXME
	//connect(gridAButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleGridA);
	//connect(multiLayerOnionSkinButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleMultiLayerOnionSkin);
}
