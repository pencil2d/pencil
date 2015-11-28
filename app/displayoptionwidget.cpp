
#include "displayoptionwidget.h"
#include "ui_displayoption.h"

#include <QFrame>
#include <QToolButton>
#include <QGridLayout>
#include "preferencemanager.h"
#include "scribblearea.h"


DisplayOptionWidget::DisplayOptionWidget( QWidget *parent ) : BaseDockWidget( parent )
{
    setWindowTitle( tr( "Display", "Window title of display options like ." ) );

    QWidget* innerWidget = new QWidget;
    setWidget( innerWidget );

    ui = new Ui::DisplayOption;
    ui->setupUi( innerWidget );
}

DisplayOptionWidget::~DisplayOptionWidget()
{
}

void DisplayOptionWidget::initUI()
{    
}

void DisplayOptionWidget::makeConnectionToEditor( Editor* editor )
{
    mEditor = editor;
    PreferenceManager* prefs = mEditor->preference();

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

    connect(prefs,            &PreferenceManager::prefsLoaded, this, &DisplayOptionWidget::loadUI);
    connect(prefs,            &PreferenceManager::effectChanged, this, &DisplayOptionWidget::updateUI);

    updateUI();


    // FIXME
	//connect(gridAButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleGridA);
    //connect(multiLayerOnionSkinButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleMultiLayerOnionSkin);
}

void DisplayOptionWidget::loadUI()
{
    updateUI();
}

void DisplayOptionWidget::updateUI()
{
    PreferenceManager* prefs = mEditor->preference();

    ui->thinLinesButton->setChecked(prefs->isOn(EFFECT::INVISIBLE_LINES));
    ui->outLinesButton->setChecked(prefs->isOn(EFFECT::OUTLINES));
    ui->onionPrevButton->setChecked(prefs->isOn(EFFECT::PREV_ONION));
    ui->onionNextButton->setChecked(prefs->isOn(EFFECT::NEXT_ONION));
    ui->onionBlueButton->setChecked(prefs->isOn(EFFECT::ONION_BLUE));
    ui->onionRedButton->setChecked(prefs->isOn(EFFECT::ONION_RED));
    ui->mirrorButton->setChecked(prefs->isOn(EFFECT::MIRROR_H));
    ui->mirrorVButton->setChecked(prefs->isOn(EFFECT::MIRROR_V));
    ui->cameraBorderButton->setChecked(prefs->isOn(EFFECT::CAMERABORDER));

}
