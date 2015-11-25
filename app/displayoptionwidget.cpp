
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
    connect(pScriArea,            &ScribbleArea::updateDisplayOption, this, &DisplayOptionWidget::updateDisplayOption);

    ui->cameraBorderButton->setChecked(pScriArea->isEffectOn(EFFECT_CAMERABORDER));

    // FIXME
	//connect(gridAButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleGridA);
    //connect(multiLayerOnionSkinButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleMultiLayerOnionSkin);
}

void DisplayOptionWidget::updateDisplayOption(DisplayEffect effect, bool optionState)
{
    switch (effect) {
    case DisplayEffect::EFFECT_THIN_LINES:
        ui->thinLinesButton->setChecked(optionState);
        break;
    case DisplayEffect::EFFECT_OUTLINES:
        ui->outLinesButton->setChecked(optionState);
        break;
    case DisplayEffect::EFFECT_PREV_ONION:
        ui->onionPrevButton->setChecked(optionState);
        break;
    case DisplayEffect::EFFECT_NEXT_ONION:
        ui->onionNextButton->setChecked(optionState);
        break;
    case DisplayEffect::EFFECT_ONION_BLUE:
        ui->onionBlueButton->setChecked(optionState);
        break;
    case DisplayEffect::EFFECT_ONION_RED:
        ui->onionRedButton->setChecked(optionState);
        break;
    case DisplayEffect::EFFECT_MIRROR_H:
        ui->mirrorButton->setChecked(optionState);
        break;
    case DisplayEffect::EFFECT_MIRROR_V:
        ui->mirrorVButton->setChecked(optionState);
        break;
    case DisplayEffect::EFFECT_CAMERABORDER:
        ui->cameraBorderButton->setChecked(optionState);
        break;
    default:
        break;
    }
}
