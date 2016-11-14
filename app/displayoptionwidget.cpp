
#include "displayoptionwidget.h"
#include "ui_displayoption.h"

#include <QFrame>
#include <QToolButton>
#include <QGridLayout>
#include "preferencemanager.h"
#include "scribblearea.h"
#include "editor.h"
#include "util.h"


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
    updateUI();
}

void DisplayOptionWidget::makeConnectionToEditor( Editor* editor )
{
    PreferenceManager* prefs = editor->preference();
    ScribbleArea* pScriArea = editor->getScribbleArea();

	connect( ui->thinLinesButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleThinLines);
	connect( ui->outLinesButton,  &QToolButton::clicked, pScriArea, &ScribbleArea::toggleOutlines);
    connect( ui->onionPrevButton, &QToolButton::clicked, this, &DisplayOptionWidget::onionPrevButtonClicked );
	connect( ui->onionNextButton, &QToolButton::clicked, this, &DisplayOptionWidget::onionNextButtonClicked );
    connect( ui->onionBlueButton, &QToolButton::clicked, this, &DisplayOptionWidget::onionBlueButtonClicked );
    connect( ui->onionRedButton,  &QToolButton::clicked, this, &DisplayOptionWidget::onionRedButtonClicked );
	connect( ui->mirrorButton,    &QToolButton::clicked, editor, &Editor::toggleMirror);
    connect( ui->mirrorVButton,   &QToolButton::clicked, editor, &Editor::toggleMirrorV);
    //connect( ui->cameraBorderButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleCameraBorder);

    connect( prefs, &PreferenceManager::optionChanged, this, &DisplayOptionWidget::updateUI );

    updateUI();

	//connect(gridAButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleGridA);
    //connect(multiLayerOnionSkinButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleMultiLayerOnionSkin);
}

void DisplayOptionWidget::updateUI()
{
    PreferenceManager* prefs = editor()->preference();

    SignalBlocker b( ui->thinLinesButton );
    ui->thinLinesButton->setChecked( prefs->isOn( SETTING::INVISIBLE_LINES ) );
    
    SignalBlocker b2( ui->outLinesButton );
    ui->outLinesButton->setChecked( prefs->isOn( SETTING::OUTLINES ) );
    
    SignalBlocker b3( ui->onionPrevButton );
    ui->onionPrevButton->setChecked( prefs->isOn( SETTING::PREV_ONION ) );
    
    SignalBlocker b4( ui->onionNextButton );
    ui->onionNextButton->setChecked( prefs->isOn( SETTING::NEXT_ONION ) );
    
    SignalBlocker b5( ui->onionBlueButton );
    ui->onionBlueButton->setChecked( prefs->isOn( SETTING::ONION_BLUE ) );
    
    SignalBlocker b6( ui->onionRedButton );
    ui->onionRedButton->setChecked( prefs->isOn( SETTING::ONION_RED ) );
    
    SignalBlocker b7( ui->mirrorButton );
    ui->mirrorButton->setChecked( prefs->isOn( SETTING::MIRROR_H ) );
    
    SignalBlocker b8( ui->mirrorVButton );
    ui->mirrorVButton->setChecked( prefs->isOn( SETTING::MIRROR_V ) );
}


void DisplayOptionWidget::onionPrevButtonClicked( bool isOn )
{
    PreferenceManager* prefs = editor()->preference();
    prefs->set( SETTING::PREV_ONION, isOn );
}

void DisplayOptionWidget::onionNextButtonClicked( bool isOn )
{
    PreferenceManager* prefs = editor()->preference();
    prefs->set( SETTING::NEXT_ONION, isOn );
}

void DisplayOptionWidget::onionBlueButtonClicked( bool isOn )
{
    PreferenceManager* prefs = editor()->preference();
    prefs->set( SETTING::ONION_BLUE, isOn );
}

void DisplayOptionWidget::onionRedButtonClicked( bool isOn )
{
    PreferenceManager* prefs = editor()->preference();
    prefs->set( SETTING::ONION_RED, isOn );
}
