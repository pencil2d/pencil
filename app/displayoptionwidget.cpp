
#include "displayoptionwidget.h"
#include "ui_displayoption.h"

#include <QFrame>
#include <QToolButton>
#include <QGridLayout>
#include "preferencemanager.h"
#include "scribblearea.h"
#include "editor.h"


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

void DisplayOptionWidget::updateZoomLabel()
{
}

void DisplayOptionWidget::makeConnectionToEditor( Editor* editor )
{
    PreferenceManager* prefs = editor->preference();
    ScribbleArea* pScriArea = editor->getScribbleArea();

	connect( ui->thinLinesButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleThinLines);
	connect( ui->outLinesButton,  &QToolButton::clicked, pScriArea, &ScribbleArea::toggleOutlines);
    connect( ui->onionPrevButton, &QToolButton::clicked, editor, &Editor::toggleOnionPrev);
	connect( ui->onionNextButton, &QToolButton::clicked, editor, &Editor::toggleOnionNext);
	connect( ui->onionBlueButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleOnionBlue );
	connect( ui->onionRedButton,  &QToolButton::clicked, pScriArea, &ScribbleArea::toggleOnionRed );
	connect( ui->mirrorButton,    &QToolButton::clicked, editor, &Editor::toggleMirror);
    connect( ui->mirrorVButton,   &QToolButton::clicked, editor, &Editor::toggleMirrorV);
    //connect( ui->cameraBorderButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleCameraBorder);

    connect( prefs, &PreferenceManager::optionChanged, this, &DisplayOptionWidget::updateUI );

    connect( editor->view(), &ViewManager::viewChanged, this, &DisplayOptionWidget::updateZoomLabel );

    updateUI();

	//connect(gridAButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleGridA);
    //connect(multiLayerOnionSkinButton, &QToolButton::clicked, pScriArea, &ScribbleArea::toggleMultiLayerOnionSkin);
}

void DisplayOptionWidget::updateUI()
{
    PreferenceManager* prefs = editor()->preference();

    QSignalBlocker b( ui->thinLinesButton );
    ui->thinLinesButton->setChecked( prefs->isOn( SETTING::INVISIBLE_LINES ) );
    
    QSignalBlocker b2( ui->outLinesButton );
    ui->outLinesButton->setChecked( prefs->isOn( SETTING::OUTLINES ) );
    
    QSignalBlocker b3( ui->onionPrevButton );
    ui->onionPrevButton->setChecked( prefs->isOn( SETTING::PREV_ONION ) );
    
    QSignalBlocker b4( ui->onionNextButton );
    ui->onionNextButton->setChecked( prefs->isOn( SETTING::NEXT_ONION ) );
    
    QSignalBlocker b5( ui->onionBlueButton );
    ui->onionBlueButton->setChecked( prefs->isOn( SETTING::ONION_BLUE ) );
    
    QSignalBlocker b6( ui->onionRedButton );
    ui->onionRedButton->setChecked( prefs->isOn( SETTING::ONION_RED ) );
    
    QSignalBlocker b7( ui->mirrorButton );
    ui->mirrorButton->setChecked( prefs->isOn( SETTING::MIRROR_H ) );
    
    QSignalBlocker b8( ui->mirrorVButton );
    ui->mirrorVButton->setChecked( prefs->isOn( SETTING::MIRROR_V ) );
}
